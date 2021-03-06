/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-17     Meco Man     first version
 */
/*
*********************************************************************************************************
*                                              uC/OS-II
*                                        The Real-Time Kernel
*
*                    Copyright 1992-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                             ARMv7-M Port
*
* Filename  : os_cpu.h
* Version   : V2.93.00
*********************************************************************************************************
* For       : ARMv7-M Cortex-M
* Mode      : Thumb-2 ISA
* Toolchain : ARM C Compiler
*********************************************************************************************************
* Note(s)   : (1) This port supports the ARM Cortex-M3, Cortex-M4 and Cortex-M7 architectures.
*             (2) It has been tested with the following Hardware Floating Point Unit.
*                 (a) Single-precision: FPv4-SP-D16-M and FPv5-SP-D16-M
*                 (b) Double-precision: FPv5-D16-M
*********************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

#include <rtdef.h>

/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                    /* See Note #1.                                       */
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*
* Note(s) : (1) If using uC/CPU then 'cpu_cfg.h' has already this define in its template. The user will
*               only need to include 'cpu_cfg.h' in 'app_cfg.h'; otherwise, define CPU_CFG_KA_IPL_BOUNDARY
*               and CPU_CFG_NVIC_PRIO_BITS in 'app_cfg.h'
*
*           (2) Since the port is using BASEPRI to separate kernel vs non-kernel aware ISR, please make sure
*               your external interrupt priorities are set accordingly. For example, if CPU_CFG_KA_IPL_BOUNDARY
*               is set to 4 then external interrupt priorities 4-15 will be kernel aware while priorities
*               0-3 will be use as non-kernel aware.
*
*           (3) Determines the interrupt programmable priority levels. This is normally specified in the
*               Microcontroller reference manual. 4-bits gives us 16 programmable priority levels.
*
*                     NVIC_IPRx
*                 7                0
*                +------------------+
*                |       PRIO       |
*                +------------------+
*
*                Bits[7:4] Priority mask bits
*                Bits[3:0] Reserved
*
*                In this example our CPU_CFG_NVIC_PRIO_BITS define should be set to 4 due to the processor
*                implementing only bits[7:4].
*********************************************************************************************************
*/

#ifndef  OS_CPU_EXCEPT_STK_SIZE
#define  OS_CPU_EXCEPT_STK_SIZE      256u        /* Default exception stack size is 256 OS_STK entries */
#endif


/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef rt_bool_t      BOOLEAN;
typedef rt_uint8_t     INT8U;                    /* Unsigned  8 bit quantity                           */
typedef rt_int8_t      INT8S;                    /* Signed    8 bit quantity                           */
typedef rt_uint16_t    INT16U;                   /* Unsigned 16 bit quantity                           */
typedef rt_int16_t     INT16S;                   /* Signed   16 bit quantity                           */
typedef rt_uint32_t    INT32U;                   /* Unsigned 32 bit quantity                           */
typedef rt_int32_t     INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef rt_ubase_t     OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef rt_ubase_t     OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */


/*
*********************************************************************************************************
*                                              Cortex-M
*                                      Critical Section Management
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD   3u                 /* ֻ��,��Ҫ�޸�                                      */

#if OS_CRITICAL_METHOD == 3u
                                                 /* Save current BASEPRI priority lvl for exception... */
                                                 /* .. and set BASEPRI to CPU_CFG_KA_IPL_BOUNDARY      */
#define  OS_ENTER_CRITICAL()  do { cpu_sr = OS_CPU_SR_Save();} while (0)
                                                 /* Restore CPU BASEPRI priority level.                */
#define  OS_EXIT_CRITICAL()   do { OS_CPU_SR_Restore(cpu_sr);} while (0)
#endif


/*
*********************************************************************************************************
*                                        Cortex-M Miscellaneous
*********************************************************************************************************
*/
#if defined(ARCH_CPU_STACK_GROWS_UPWARD)
#define  OS_STK_GROWTH        0u                 /* Stack grows from LOW to HIGH memory on ARM         */
#else
#define  OS_STK_GROWTH        1u                 /* Stack grows from HIGH to LOW memory on ARM         */
#endif

#define  OS_TASK_SW()         OSCtxSw()


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

OS_CPU_EXT  OS_STK   OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
OS_CPU_EXT  OS_STK  *OS_CPU_ExceptStkBase;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3u                     /* See OS_CPU_A.ASM                                   */
OS_CPU_SR  OS_CPU_SR_Save         (void);
void       OS_CPU_SR_Restore      (OS_CPU_SR  cpu_sr);
#endif

/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                /* End of 'extern'al C lang linkage.                  */
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif
