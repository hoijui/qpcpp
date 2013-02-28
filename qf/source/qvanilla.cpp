//////////////////////////////////////////////////////////////////////////////
// Product: QF/C++
// Last Updated for Version: 4.3.00
// Date of the Last Update:  Nov 01, 2011
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

/// \file
/// \ingroup qf
/// \brief "vanilla" cooperative kernel,
/// QActive::start(), QActive::stop(), and QF::run() implementation.

#ifdef Q_USE_NAMESPACE
namespace QP {
#endif

Q_DEFINE_THIS_MODULE(qvanilla)

// Package-scope objects -----------------------------------------------------
extern "C" {
#if (QF_MAX_ACTIVE <= 8)
    QPSet8  volatile QF_readySet_;              // ready set of active objects
#else
    QPSet64 volatile QF_readySet_;              // ready set of active objects
#endif

uint8_t volatile QF_currPrio_;            ///< current task/interrupt priority
uint8_t volatile QF_intNest_;                     ///< interrupt nesting level

}                                                                // extern "C"

//............................................................................
char const Q_ROM * Q_ROM_VAR QF::getPortVersion(void) {
    static const char Q_ROM version[] = "4.3.00";
    return version;
}
//............................................................................
void QF::init(void) {
    // nothing to do for the "vanilla" kernel
}
//............................................................................
void QF::stop(void) {
    QF::onCleanup();                                       // cleanup callback
    // nothing else to do for the "vanilla" kernel
}
//............................................................................
void QF::run(void) {
    QF::onStartup();                                       // startup callback

    for (;;) {                                           // the bacground loop
        QF_INT_DISABLE();
        if (QF_readySet_.notEmpty()) {
            uint8_t p = QF_readySet_.findMax();
            QActive *a = active_[p];
            QF_currPrio_ = p;                     // save the current priority
            QF_INT_ENABLE();

            QEvent const *e = a->get_();     // get the next event for this AO
            a->dispatch(e);                         // dispatch evt to the HSM
            gc(e);       // determine if event is garbage and collect it if so
        }
        else {
            QF::onIdle();                                        // see NOTE01
        }
    }
}
//............................................................................
void QActive::start(uint8_t prio,
                    QEvent const *qSto[], uint32_t qLen,
                    void *stkSto, uint32_t /*lint -e1904 stkSize */,
                    QEvent const *ie)
{
    Q_REQUIRE(((uint8_t)0 < prio) && (prio <= (uint8_t)QF_MAX_ACTIVE)
              && (stkSto == (void *)0));      // does not need per-actor stack

    m_eQueue.init(qSto, (QEQueueCtr)qLen);               // initialize QEQueue
    m_prio = prio;                // set the QF priority of this active object
    QF::add_(this);                     // make QF aware of this active object
    init(ie);                                    // execute initial transition

    QS_FLUSH();                          // flush the trace buffer to the host
}
//............................................................................
void QActive::stop(void) {
    QF::remove_(this);
}

#ifdef Q_USE_NAMESPACE
}                                                              // namespace QP
#endif

//////////////////////////////////////////////////////////////////////////////
// NOTE01:
// QF::onIdle() must be called with interrupts DISABLED because the
// determination of the idle condition (no events in the queues) can change
// at any time by an interrupt posting events to a queue. The QF::onIdle()
// MUST enable interrups internally, perhaps at the same time as putting the
// CPU into a power-saving mode.
//

