//////////////////////////////////////////////////////////////////////////////
// Product: QK/C++
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
#include "qk_pkg.h"
#include "qassert.h"

/// \file
/// \ingroup qk
/// \brief QK_readySet_, QK_currPrio_, and QK_intNest_ definitions and
/// QK::getVersion(),  QK_schedPrio_(), QF::init(), QF::run(), QF::stop(),
/// QActive::start(), QActive::stop() implementations.

// Public-scope objects ------------------------------------------------------
extern "C" {
#if (QF_MAX_ACTIVE <= 8U)
#ifdef Q_USE_NAMESPACE
    QP::QPSet8  volatile QK_readySet_;                      // ready set of QK
#else
    QPSet8  volatile QK_readySet_;                          // ready set of QK
#endif
#else
#ifdef Q_USE_NAMESPACE
    QP::QPSet64 volatile QK_readySet_;                      // ready set of QK
#else
    QPSet64 volatile QK_readySet_;                          // ready set of QK
#endif
#endif
                                         // start with the QK scheduler locked
uint8_t volatile QK_currPrio_ = (uint8_t)(QF_MAX_ACTIVE + 1);
uint8_t volatile QK_intNest_;                 // start with nesting level of 0

}                                                                // extern "C"

#ifdef Q_USE_NAMESPACE
namespace QP {
#endif

Q_DEFINE_THIS_MODULE(qk)

//............................................................................
//lint -e970 -e971               ignore MISRA rules 13 and 14 in this function
char const Q_ROM * Q_ROM_VAR QK::getVersion(void) {
    static char const Q_ROM Q_ROM_VAR version[] = {
        (char)(((QP_VERSION >> 12U) & 0xFU) + (uint8_t)'0'),
        '.',
        (char)(((QP_VERSION >>  8U) & 0xFU) + (uint8_t)'0'),
        '.',
        (char)(((QP_VERSION >>  4U) & 0xFU) + (uint8_t)'0'),
        (char)((QP_VERSION          & 0xFU) + (uint8_t)'0'),
        '\0'
    };
    return version;
}
//............................................................................
void QF::init(void) {
    QK_init();           // QK initialization ("C" linkage, might be assembly)
}
//............................................................................
void QF::stop(void) {
    QF::onCleanup();                                       // cleanup callback
    // nothing else to do for the QK preemptive kernel
}
//............................................................................
static void initialize(void) {
    QK_currPrio_ = (uint8_t)0;        // set the priority for the QK idle loop
    uint8_t p = QK_schedPrio_();
    if (p != (uint8_t)0) {
        QK_sched_(p);                    // process all events produced so far
    }
}
//............................................................................
void QF::run(void) {
    QF_INT_DISABLE();
    initialize();
    onStartup();                                           // startup callback
    QF_INT_ENABLE();

    for (;;) {                                             // the QK idle loop
        QK::onIdle();                        // invoke the QK on-idle callback
    }
}
//............................................................................
void QActive::start(uint8_t prio,
                    QEvent const *qSto[], uint32_t qLen,
                    void *tls, uint32_t flags,
                    QEvent const *ie)
{
    Q_REQUIRE(((uint8_t)0 < prio) && (prio <= (uint8_t)QF_MAX_ACTIVE));

    m_eQueue.init(qSto, (QEQueueCtr)qLen);       // initialize the event queue
    m_prio = prio;
    QF::add_(this);                     // make QF aware of this active object

#if defined(QK_TLS) || defined(QK_EXT_SAVE)
    m_osObject = (uint8_t)flags;       // m_osObject contains the thread flags
    m_thread   = tls;      // contains the pointer to the thread-local-storage
#else
    Q_ASSERT((tls == (void *)0) && (flags == (uint32_t)0));
#endif

    init(ie);                                    // execute initial transition

    QS_FLUSH();                          // flush the trace buffer to the host
}
//............................................................................
void QActive::stop(void) {
    QF::remove_(this);                // remove this active object from the QF
}

#ifdef Q_USE_NAMESPACE
}                                                              // namespace QP
#endif

//............................................................................
extern "C" {

// NOTE: QK schedPrio_() is entered and exited with interrupts DISABLED
uint8_t QK_schedPrio_(void) {

#ifdef Q_USE_NAMESPACE
    using namespace QP;
#endif

    uint8_t p = QK_readySet_.findMax();

#ifdef QK_NO_MUTEX
    if (p > QK_currPrio_) {                        // do we have a preemption?
#else                                   // QK priority-ceiling mutexes allowed
    if ((p > QK_currPrio_) && (p > QK_ceilingPrio_)) {
#endif
        return p;
    }
    else {
        return (uint8_t)0;
    }
}

}                                                                // extern "C"
