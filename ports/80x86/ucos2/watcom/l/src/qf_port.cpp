//////////////////////////////////////////////////////////////////////////////
// Product: QF/C++ port to x86, uC/OS-II, Turbo C++ 1.01, Large model
// Last Updated for Version: 4.3.00
// Date of the Last Update:  Nov 02, 2011
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2011 Quantum Leaps, LLC. All rights reserved.
//
// This software may be distributed and modified under the terms of the GNU
// General Public License version 2 (GPL) as published by the Free Software
// Foundation and appearing in the file GPL.TXT included in the packaging of
// this file. Please note that GPL Section 2[b] requires that all works based
// on this software must also be made publicly available under the terms of
// the GPL ("Copyleft").
//
// Alternatively, this software may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GPL and are specifically designed for licensees interested in
// retaining the proprietary status of their code.
//
// Contact information:
// Quantum Leaps Web site:  http://www.quantum-leaps.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#include "qf_pkg.h"
#include "qassert.h"

#include <dos.h>                          // for _dos_setvect()/_dos_getvect()

#ifdef Q_USE_NAMESPACE
namespace QP {
#endif

Q_DEFINE_THIS_MODULE(qf_port)

// Local objects -------------------------------------------------------------
static void interrupt (*l_dosSpareISR)(void);

//............................................................................
//lint -e970 -e971               ignore MISRA rules 13 and 14 in this function
const char Q_ROM * Q_ROM_VAR QF::getPortVersion(void) {
    static const char Q_ROM Q_ROM_VAR version[] =  "4.3.00";
    return version;
}
//............................................................................
void QF::init(void) {
    OSInit();                                           // initialize uC/OS-II
}
//............................................................................
void QF::run(void) {
                                     // install uC/OS-II context switch vector
    l_dosSpareISR = _dos_getvect(uCOS);
    _dos_setvect(uCOS, (void interrupt (*)(void))&OSCtxSw);

    // NOTE the QF::onStartup() callback must be invoked from the task level
    OSStart();                                  // start uC/OS-II multitasking
}
//............................................................................
void QF::stop(void) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();
    _dos_setvect(uCOS, l_dosSpareISR);      // restore the original DOS vector
    QF_CRIT_EXIT_();
    onCleanup();                                           // cleanup callback
}
//............................................................................
extern "C" void task_function(void *pdata) {        // uC/OS-II task signature
    ((QActive *)pdata)->m_running = (uint8_t)1;// allow the thread-loop to run
    while (((QActive *)pdata)->m_running) {
        QEvent const *e = ((QActive *)pdata)->get_();
        ((QActive *)pdata)->dispatch(e); // dispatch to the active object's SM
        QF::gc(e);      // check if the event is garbage, and collect it if so
    }

    QF::remove_((QActive *)pdata);    // remove this object from the framework
    OSTaskDel(OS_PRIO_SELF);           // make uC/OS-II forget about this task
}
//............................................................................
void QActive::start(uint8_t prio,
                    QEvent const *qSto[], uint32_t qLen,
                    void *stkSto, uint32_t stkSize,
                    QEvent const *ie)
{
    m_eQueue = OSQCreate((void **)qSto, qLen);
    Q_ASSERT(m_eQueue != (OS_EVENT *)0);             // uC/OS-II queue created
    m_prio = prio;                                      // set the QF priority
    QF::add_(this);                     // make QF aware of this active object
    init(ie);                                // execute the initial transition

    QS_FLUSH();                          // flush the trace buffer to the host

    m_thread = QF_MAX_ACTIVE - m_prio;          // map QF priority to uC/OS-II
    INT8U err = OSTaskCreateExt(&task_function,           // the task function
             this,                                    // the 'pdata' parameter
             &(((OS_STK *)stkSto)[(stkSize / sizeof(OS_STK)) - 1]),    // ptos
             m_thread,                               // uC/OS-II task priority
             m_thread,                                              // task id
             (OS_STK *)stkSto,                                         // pbos
             stkSize/sizeof(OS_STK),      // size of the stack in OS_STK units
             (void *)0,                                                // pext
             (INT16U)OS_TASK_OPT_STK_CLR);                              // opt
    Q_ASSERT(err == OS_NO_ERR);                       // uC/OS-II task created
}
//............................................................................
void QActive::stop(void) {
    m_running = (uint8_t)0;  // clear the loop variable used in QActive::run()

    INT8U err;
    OSQDel(m_eQueue, OS_DEL_ALWAYS, &err);       // cleanup the uC/OS-II queue
    Q_ASSERT(err == OS_NO_ERR);
}
//............................................................................
#ifndef Q_SPY
void QActive::postFIFO(QEvent const *e) {
#else
void QActive::postFIFO(QEvent const *e, void const *sender) {
#endif

    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_POST_FIFO, QS::aoObj_, this)
        QS_TIME_();                                               // timestamp
        QS_OBJ_(sender);                                  // the sender object
        QS_SIG_(e->sig);                            // the signal of the event
        QS_OBJ_(this);                       // this active object (recipient)
        QS_U8_(EVT_POOL_ID(e));                    // the pool Id of the event
        QS_U8_(EVT_REF_CTR(e));                  // the ref count of the event
        QS_EQC_(0);                        // number of free entries (unknown)
        QS_EQC_(0);                    // min number of free entries (unknown)
    QS_END_NOCRIT_()

    if (EVT_POOL_ID(e) != (uint8_t)0) {                 // is it a pool event?
        EVT_INC_REF_CTR(e);                 // increment the reference counter
    }
    QF_CRIT_EXIT_();
    Q_ALLEGE(OSQPost((OS_EVENT *)m_eQueue, (void *)e) == OS_NO_ERR);
}
//............................................................................
void QActive::postLIFO(QEvent const *e) {
    QF_CRIT_STAT_
    QF_CRIT_ENTRY_();

    QS_BEGIN_NOCRIT_(QS_QF_ACTIVE_POST_LIFO, QS::aoObj_, this)
        QS_TIME_();                                               // timestamp
        QS_SIG_(e->sig);                           // the signal of this event
        QS_OBJ_(this);                                   // this active object
        QS_U8_(EVT_POOL_ID(e));                    // the pool Id of the event
        QS_U8_(EVT_REF_CTR(e));                  // the ref count of the event
        QS_EQC_(0);                        // number of free entries (unknown)
        QS_EQC_(0);                    // min number of free entries (unknown)
    QS_END_NOCRIT_()

    if (EVT_POOL_ID(e) != (uint8_t)0) {                 // is it a pool event?
        EVT_INC_REF_CTR(e);                 // increment the reference counter
    }
    QF_CRIT_EXIT_();
    Q_ALLEGE(OSQPostFront((OS_EVENT *)m_eQueue, (void *)e) == OS_NO_ERR);
}
//............................................................................
QEvent const *QActive::get_(void) {
    INT8U err;
    QEvent const *e = (QEvent *)OSQPend((OS_EVENT *)m_eQueue, 0, &err);
    QS_CRIT_STAT_

    Q_ASSERT(err == OS_NO_ERR);

    QS_BEGIN_(QS_QF_ACTIVE_GET, QS::aoObj_, this)
        QS_TIME_();                                               // timestamp
        QS_SIG_(e->sig);                           // the signal of this event
        QS_OBJ_(this);                                   // this active object
        QS_U8_(EVT_POOL_ID(e));                    // the pool Id of the event
        QS_U8_(EVT_REF_CTR(e));                  // the ref count of the event
        QS_EQC_(0);                        // number of free entries (unknown)
    QS_END_()

    return e;
}

#ifdef Q_USE_NAMESPACE
}                                                              // namespace QP
#endif

