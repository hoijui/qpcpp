#!/bin/bash
# ===========================================================================
# Product: QP/C buld script for ARM Cortex-M3, QK port, GNU compiler
# Last Updated for Version: 4.1.02
# Date of the Last Update:  Feb 15, 2010
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2002-2010 Quantum Leaps, LLC. All rights reserved.
#
# This software may be distributed and modified under the terms of the GNU
# General Public License version 2 (GPL) as published by the Free Software
# Foundation and appearing in the file GPL.TXT included in the packaging of
# this file. Please note that GPL Section 2[b] requires that all works based
# on this software must also be made publicly available under the terms of
# the GPL ("Copyleft").
#
# Alternatively, this software may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GPL and are specifically designed for licensees interested in
# retaining the proprietary status of their code.
#
# Contact information:
# Quantum Leaps Web site:  http://www.quantum-leaps.cppom
# e-mail:                  info@quantum-leaps.cppom
# ===========================================================================

# adjust the following path to the location where you've installed
# the GNU_ARM toolexport...
#

export GNU_ARM=arm-none-eabi

export CC=$GNU_ARM-gcc
export ASM=$GNU_ARM-as
export LIB=$GNU_ARM-ar

export QP_INCDIR=../../../../include
export QP_PRTDIR=.

export ARM_CORE=cortex-m3
export ARM_CORE_NAME=cortex-m3_dk

if [ -z $1 ]; then
    echo "default selected"
    export BINDIR=$QP_PRTDIR/dbg
    export CCFLAGS="-g -c -mcpu=$ARM_CORE -mthumb -Wall -O -fno-rtti -fno-exceptions"
    export ASMFLAGS="-g -mcpu=$ARM_CORE"
fi
if [ "$1" = "rel" ]; then
    echo "rel selected"
    export BINDIR=$QP_PRTDIR/rel
    export CCFLAGS="-c -mcpu=$ARM_CORE -mthumb -Wall -Os -fno-rtti -fno-exceptions -DNDEBUG"
    export ASMFLAGS="-mcpu=$ARM_CORE"
fi
if [ "$1" = "spy" ]; then 
    echo "spy selected"
    export BINDIR=$QP_PRTDIR/spy
    export CCFLAGS="-g -c -mcpu=$ARM_CORE -mthumb -Wall -O -fno-rtti -fno-exceptions -DQ_SPY"
    export ASMFLAGS="-g -mcpu=$ARM_CORE"
fi

export LIBDIR=$BINDIR
export LIBFLAGS=rs

# QEP ----------------------------------------------------------------------
export SRCDIR=../../../../qep/source
export CCINC="-I$QP_PRTDIR -I$QP_INCDIR -I$SRCDIR"

$CC $CCFLAGS $CCINC $SRCDIR/qep.cpp      -o$BINDIR/qep.o
$CC $CCFLAGS $CCINC $SRCDIR/qfsm_ini.cpp -o$BINDIR/qfsm_ini.o
$CC $CCFLAGS $CCINC $SRCDIR/qfsm_dis.cpp -o$BINDIR/qfsm_dis.o
$CC $CCFLAGS $CCINC $SRCDIR/qhsm_ini.cpp -o$BINDIR/qhsm_ini.o
$CC $CCFLAGS $CCINC $SRCDIR/qhsm_dis.cpp -o$BINDIR/qhsm_dis.o
$CC $CCFLAGS $CCINC $SRCDIR/qhsm_top.cpp -o$BINDIR/qhsm_top.o
$CC $CCFLAGS $CCINC $SRCDIR/qhsm_in.cpp  -o$BINDIR/qhsm_in.o

$LIB $LIBFLAGS $LIBDIR/libqep_$ARM_CORE_NAME.a $BINDIR/qep.o $BINDIR/qfsm_ini.o $BINDIR/qfsm_dis.o $BINDIR/qhsm_ini.o $BINDIR/qhsm_dis.o $BINDIR/qhsm_top.o $BINDIR/qhsm_in.o

rm $BINDIR/*.o

# QF -----------------------------------------------------------------------
export SRCDIR=../../../../qf/source
export CCINC="-I$QP_PRTDIR -I$QP_INCDIR -I$SRCDIR"


$CC $CCFLAGS $CCINC $SRCDIR/qa_defer.cpp -o$BINDIR/qa_defer.o
$CC $CCFLAGS $CCINC $SRCDIR/qa_fifo.cpp  -o$BINDIR/qa_fifo.o 
$CC $CCFLAGS $CCINC $SRCDIR/qa_lifo.cpp  -o$BINDIR/qa_lifo.o 
$CC $CCFLAGS $CCINC $SRCDIR/qa_get_.cpp  -o$BINDIR/qa_get_.o 
$CC $CCFLAGS $CCINC $SRCDIR/qa_sub.cpp   -o$BINDIR/qa_sub.o  
$CC $CCFLAGS $CCINC $SRCDIR/qa_usub.cpp  -o$BINDIR/qa_usub.o 
$CC $CCFLAGS $CCINC $SRCDIR/qa_usuba.cpp -o$BINDIR/qa_usuba.o
$CC $CCFLAGS $CCINC $SRCDIR/qeq_fifo.cpp -o$BINDIR/qeq_fifo.o
$CC $CCFLAGS $CCINC $SRCDIR/qeq_get.cpp  -o$BINDIR/qeq_get.o 
$CC $CCFLAGS $CCINC $SRCDIR/qeq_init.cpp -o$BINDIR/qeq_init.o
$CC $CCFLAGS $CCINC $SRCDIR/qeq_lifo.cpp -o$BINDIR/qeq_lifo.o
$CC $CCFLAGS $CCINC $SRCDIR/qf_act.cpp   -o$BINDIR/qf_act.o  
$CC $CCFLAGS $CCINC $SRCDIR/qf_gc.cpp    -o$BINDIR/qf_gc.o      
$CC $CCFLAGS $CCINC $SRCDIR/qf_log2.cpp  -o$BINDIR/qf_log2.o 
$CC $CCFLAGS $CCINC $SRCDIR/qf_new.cpp   -o$BINDIR/qf_new.o  
$CC $CCFLAGS $CCINC $SRCDIR/qf_pool.cpp  -o$BINDIR/qf_pool.o 
$CC $CCFLAGS $CCINC $SRCDIR/qf_psini.cpp -o$BINDIR/qf_psini.o
$CC $CCFLAGS $CCINC $SRCDIR/qf_pspub.cpp -o$BINDIR/qf_pspub.o
$CC $CCFLAGS $CCINC $SRCDIR/qf_pwr2.cpp  -o$BINDIR/qf_pwr2.o 
$CC $CCFLAGS $CCINC $SRCDIR/qf_tick.cpp  -o$BINDIR/qf_tick.o 
$CC $CCFLAGS $CCINC $SRCDIR/qmp_get.cpp  -o$BINDIR/qmp_get.o 
$CC $CCFLAGS $CCINC $SRCDIR/qmp_init.cpp -o$BINDIR/qmp_init.o
$CC $CCFLAGS $CCINC $SRCDIR/qmp_put.cpp  -o$BINDIR/qmp_put.o 
$CC $CCFLAGS $CCINC $SRCDIR/qte_ctor.cpp -o$BINDIR/qte_ctor.o
$CC $CCFLAGS $CCINC $SRCDIR/qte_arm.cpp  -o$BINDIR/qte_arm.o 
$CC $CCFLAGS $CCINC $SRCDIR/qte_darm.cpp -o$BINDIR/qte_darm.o
$CC $CCFLAGS $CCINC $SRCDIR/qte_rarm.cpp -o$BINDIR/qte_rarm.o
$ASM src/qk_port.s -o $BINDIR/qk_port.o $ASMFLAGS

$LIB $LIBFLAGS $LIBDIR/libqf_$ARM_CORE_NAME.a $BINDIR/qa_defer.o $BINDIR/qa_fifo.o $BINDIR/qa_lifo.o $BINDIR/qa_get_.o $BINDIR/qa_sub.o $BINDIR/qa_usub.o $BINDIR/qa_usuba.o $BINDIR/qeq_fifo.o $BINDIR/qeq_get.o $BINDIR/qeq_init.o $BINDIR/qeq_lifo.o $BINDIR/qf_act.o $BINDIR/qf_gc.o $BINDIR/qf_log2.o $BINDIR/qf_new.o $BINDIR/qf_pool.o $BINDIR/qf_psini.o $BINDIR/qf_pspub.o $BINDIR/qf_pwr2.o $BINDIR/qf_tick.o $BINDIR/qmp_get.o $BINDIR/qmp_init.o $BINDIR/qmp_put.o $BINDIR/qte_ctor.o $BINDIR/qte_arm.o $BINDIR/qte_darm.o $BINDIR/qte_rarm.o $BINDIR/qk_port.o

rm $BINDIR/*.o

# QK -----------------------------------------------------------------------
export SRCDIR=../../../../qk/source
export CCINC="-I$QP_PRTDIR -I$QP_INCDIR -I$SRCDIR"


$CC $CCFLAGS $CCINC $SRCDIR/qk.cpp       -o$BINDIR/qk.o
$CC $CCFLAGS $CCINC $SRCDIR/qk_sched.cpp -o$BINDIR/qk_sched.o
$CC $CCFLAGS $CCINC $SRCDIR/qk_mutex.cpp -o$BINDIR/qk_mutex.o
$ASM src/qk_port.s -o $BINDIR/qk_port.o $ASMFLAGS

$LIB $LIBFLAGS $LIBDIR/libqk_$ARM_CORE_NAME.a $BINDIR/qk.o $BINDIR/qk_sched.o $BINDIR/qk_mutex.o $BINDIR/qk_port.o

rm $BINDIR/*.o

# QS -----------------------------------------------------------------------
if [ "$1" = "spy" ]; then
	export SRCDIR=../../../../qs/source
	export CCINC=-I$QP_PRTDIR -I$QP_INCDIR -I$SRCDIR

	$CC $CCFLAGS $CCINC $SRCDIR/qs.cpp      -o$BINDIR/qs.o     
	$CC $CCFLAGS $CCINC $SRCDIR/qs_.cpp     -o$BINDIR/qs_.o     
	$CC $CCFLAGS $CCINC $SRCDIR/qs_blk.cpp  -o$BINDIR/qs_blk.o 
	$CC $CCFLAGS $CCINC $SRCDIR/qs_byte.cpp -o$BINDIR/qs_byte.o
	$CC $CCFLAGS $CCINC $SRCDIR/qs_f32.cpp  -o$BINDIR/qs_f32.o 
	$CC $CCFLAGS $CCINC $SRCDIR/qs_f64.cpp  -o$BINDIR/qs_f64.o 
	$CC $CCFLAGS $CCINC $SRCDIR/qs_mem.cpp  -o$BINDIR/qs_mem.o 
	$CC $CCFLAGS $CCINC $SRCDIR/qs_str.cpp  -o$BINDIR/qs_str.o 

	$LIB $LIBFLAGS $LIBDIR/libqs_$ARM_CORE_NAME.a $BINDIR/qs.o $BINDIR/qs_.o $BINDIR/qs_blk.o $BINDIR/qs_byte.o $BINDIR/qs_f32.o $BINDIR/qs_f64.o $BINDIR/qs_mem.o $BINDIR/qs_str.o

	rm $BINDIR/*.o
fi

# --------------------------------------------------------------------------



