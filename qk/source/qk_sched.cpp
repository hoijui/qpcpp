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
//#include "qassert.h"

/// \file
/// \ingroup qk
/// \brief QK_sched_() implementation.

#ifdef Q_USE_NAMESPACE
namespace QP {
#endif

//Q_DEFINE_THIS_MODULE(qk_sched)

#ifdef Q_USE_NAMESPACE
}                                                              // namespace QP
#endif

//............................................................................
// NOTE: the QK scheduler is entered and exited with interrupts DISABLED.
// QK_sched_() is extern "C", so it does not belong to the QP namespace.
//
extern "C" {

void QK_sched_(uint8_t p) {

#ifdef Q_USE_NAMESPACE
    using namespace QP;
#endif

    uint8_t pin = QK_currPrio_;                   // save the initial priority
    QActive *a;
#ifdef QK_TLS                                    // thread-local storage used?
    uint8_t pprev = pin;
#endif
    do {
        a = QF::active_[p];                    // obtain the pointer to the AO
        QK_currPrio_ = p;            // this becomes the current task priority

#ifdef QK_TLS                                    // thread-local storage used?
        if (p != pprev) {                          // are we changing threads?
            QK_TLS(a);                      // switch new thread-local storage
            pprev = p;
        }
#endif
        QS_BEGIN_NOCRIT_(QS_QK_SCHEDULE, QS::aoObj_, a)
            QS_TIME_();                                           // timestamp
            QS_U8_(p);                    // the priority of the active object
            QS_U8_(pin);                             // the preempted priority
        QS_END_NOCRIT_()

        QF_INT_ENABLE();                  // unconditionally enable interrupts

        QEvent const *e = a->get_();         // get the next event for this AO
        a->dispatch(e);                               // dispatch e to this AO
        QF::gc(e);                  // garbage collect the event, if necessary

                        // determine the next highest-priority AO ready to run
        QF_INT_DISABLE();                                // disable interrupts
        p = QK_readySet_.findMax();

#ifdef QK_NO_MUTEX
    } while (p > pin);             // is the new priority higher than initial?
#else                                   // QK priority-ceiling mutexes allowed
    } while ((p > pin) && (p > QK_ceilingPrio_));
#endif

    QK_currPrio_ = pin;                        // restore the initial priority

#ifdef QK_TLS                                    // thread-local storage used?
    if (pin != (uint8_t)0) {          // no extended context for the idle loop
        a = QF::active_[pin];               // the pointer to the preempted AO
        QK_TLS(a);                                 // restore the original TLS
    }
#endif
}

}                                                                // extern "C"
