/*  realcons_console_ki10.h: State and function definitions for the PDP-10 KI10 panel

   Copyright (c) 2014-2016, Joerg Hoppe
   j_hoppe@t-online.de, www.retrocmp.com

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   JOERG HOPPE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


   31-Aug-2014  JH      created
*/


#ifndef REALCONS_CONSOLE_KI10_H_
#define REALCONS_CONSOLE_KI10_H_

typedef t_uint64 uint64;        // for consistency with kx10_cpu

#include "realcons_ki10_control.h"

// Logic of the panel ... which is different electronic than the CPU.
typedef struct
{
    /* base class members, identical for all panels */
    struct realcons_struct *realcons; // parent
    // parent is used to link to other data structs:
    //  example: Blinkenlight API panel struct = realcons->blinkenlight_panel

    /* Signals for SimH's generic console interface
     * (halt, start, singlestep, exam deposit).
     * Signals in VHDL are variables holding values of physical wires
     * Signals in REALCONS are pointers to variables either in the SimH simulation code, or
     * in panel logic. They connect simulation and panel. A certain signal is can be unidirectional
     * (either set by simulator and read by panel logic, ore vice verse) or be bidirectional.
     */
        t_addr *cpusignal_memory_address_phys_register; // address of last bus cycle (EXAM/DEPOSIT)
        char **cpusignal_register_name; // name of last accessed exam/deposit register
        t_value *cpusignal_memory_data_register; // data of last bus cycle
        int *cpusignal_console_halt; // 1, if a real console halts program execution
        volatile t_bool *cpusignal_run; // 1, if simulated cpu is running

#ifdef VM_PDP10
         /* Signals for SimH's PDP10 cpu. */
         int32 *cpusignal_PC; // programm counter
         int32 *cpusignal_flags; // CPU state flags
         uint64_t   *cpusignal_instruction ; // current instruction

         int32      *cpusignal_pi_on ; // PI system enable
         int32_t    *cpusignal_pi_enb ; // PI levels enabled. simh pdp10 cpu reg "PIENB"
         int32_t    *cpusignal_pi_act ; // PI levels active. simh  pdp10 cpu reg "PIACT"
         int32_t    *cpusignal_pi_prq ; // PI levels with program requests . simh  pdp10 cpu reg "PIPRQ"
         int32_t    *cpusignal_pi_ioq ; // PI levels with IO requests. simh  pdp10 cpu reg "PIIOQ"
        // KI10 console registers in SimH
    	uint64_t  *cpusignal_console_data_switches ;
	uint64_t  *cpusignal_console_memory_indicator ;
        uint64_t  *cpusignal_console_address_addrcond ;
#else
        /* Signals for SimH's KX10 cpus */
        t_addr *cpusignal_PC;           /* program counter */
        uint32 *cpusignal_flags;        /* CPU state flags */
        uint64_t *cpusignal_instruction;/* current instruction */

        int *cpusignal_pi_on;           /* PI system enable */
        uint8 *cpusignal_pi_enb;        /* PI levels enabled (PIE)*/
        uint8 *cpusignal_pi_act;        /* PI levels active (PIH) */
        uint8 *cpusignal_pi_prq;        /* PI levels with program requests (PIR) */
        uint16 *cpusignal_pi_ioq;       /* PI levels with IOB requests (IOB_PI) */
        uint32 *cpusignal_console_readin_device; /* READ IN device */
        uint8 *cpusignal_xct;           /* XCT in progress */

        /* KI10 console registers in SimH */
        uint64 *cpusignal_console_data_switches;
        uint64 *cpusignal_console_memory_indicator;
#endif


    /* KI10 specific Blinkenlight API controls */
    realcons_ki10_control_t lamp_OVERTEMP ;
    realcons_ki10_control_t lamp_CKT_BRKR_TRIPPED ;
    realcons_ki10_control_t lamp_DOORS_OPEN ;
    realcons_ki10_control_t knob_MARGIN_SELECT ;
    realcons_ki10_control_t knob_IND_SELECT ;
    realcons_ki10_control_t knob_SPEED_CONTROL_COARSE ;
    realcons_ki10_control_t knob_SPEED_CONTROL_FINE ;
    realcons_ki10_control_t knob_MARGIN_VOLTAGE ;
    realcons_ki10_control_t VOLTMETER ;
    realcons_ki10_control_t enable_HOURMETER ;
    realcons_ki10_control_t button_FM_MANUAL ;
    realcons_ki10_control_t buttons_FM_BLOCK ;
    realcons_ki10_control_t buttons_SENSE ;
    realcons_ki10_control_t button_MI_PROG_DIS ;
    realcons_ki10_control_t button_MEM_OVERLAP_DIS ;
    realcons_ki10_control_t button_SINGLE_PULSE ;
    realcons_ki10_control_t button_MARGIN_ENABLE ;
    realcons_ki10_control_t buttons_READ_IN_DEVICE ;
    realcons_ki10_control_t buttons_MANUAL_MARGIN_ADDRESS ;
    realcons_ki10_control_t button_LAMP_TEST ;
    realcons_ki10_control_t button_CONSOLE_LOCK ;
    realcons_ki10_control_t button_CONSOLE_DATALOCK ;
    realcons_ki10_control_t button_POWER;

    realcons_ki10_control_t leds_PI_ACTIVE ;
    realcons_ki10_control_t leds_IOB_PI_REQUEST ;
    realcons_ki10_control_t leds_PI_IN_PROGRESS ;
    realcons_ki10_control_t leds_PI_REQUEST ;
    realcons_ki10_control_t led_PI_ON ;
    realcons_ki10_control_t led_PI_OK_8 ;
    realcons_ki10_control_t leds_MODE ;
    realcons_ki10_control_t led_KEY_PG_FAIL ;
    realcons_ki10_control_t led_KEY_MAINT ;
    realcons_ki10_control_t leds_STOP ;
    realcons_ki10_control_t led_RUN ;
    realcons_ki10_control_t led_POWER ;
    realcons_ki10_control_t leds_PROGRAM_COUNTER ;
    realcons_ki10_control_t leds_INSTRUCTION ;
    realcons_ki10_control_t led_PROGRAM_DATA ;
    realcons_ki10_control_t led_MEMORY_DATA ;
    realcons_ki10_control_t leds_DATA ;

    realcons_ki10_control_t button_PAGING_EXEC ;
    realcons_ki10_control_t button_PAGING_USER ;

    realcons_ki10_control_t buttons_ADDRESS ;  // switches 14..35
    realcons_ki10_control_t button_ADDRESS_CLEAR ;
    realcons_ki10_control_t button_ADDRESS_LOAD ;

    realcons_ki10_control_t buttons_DATA ;
    realcons_ki10_control_t button_DATA_CLEAR ;
    realcons_ki10_control_t button_DATA_LOAD ;

    realcons_ki10_control_t button_SINGLE_INST ;
    realcons_ki10_control_t button_SINGLE_PULSER ;
    realcons_ki10_control_t button_STOP_PAR ;
    realcons_ki10_control_t button_STOP_NXM ;
    realcons_ki10_control_t button_REPEAT ;
    realcons_ki10_control_t button_FETCH_INST ;
    realcons_ki10_control_t button_FETCH_DATA ;
    realcons_ki10_control_t button_WRITE ;
    realcons_ki10_control_t button_ADDRESS_STOP ;
    realcons_ki10_control_t button_ADDRESS_BREAK ;
    realcons_ki10_control_t button_READ_IN ;
    realcons_ki10_control_t button_START ;
    realcons_ki10_control_t button_CONT ;
    realcons_ki10_control_t button_STOP ;
    realcons_ki10_control_t button_RESET ;
    realcons_ki10_control_t button_XCT ;
    realcons_ki10_control_t button_EXAMINE_THIS ;
    realcons_ki10_control_t button_EXAMINE_NEXT ;
    realcons_ki10_control_t button_DEPOSIT_THIS ;
    realcons_ki10_control_t button_DEPOSIT_NEXT ;

    // these two flags are referenced by all buttons as "disable"
    // "1" =
    // From doc: " the console data lock disables the data and sense switches;
    //  the console lock disables all other buttons except those that are mechanical"
    unsigned    console_lock ;
    unsigned    console_datalock ;

    // bool. Source of value in memory indicator (lower pabel, row 4)
    // 0: display caused by console exam/deposit
    // 1, if CPU has written the value
    unsigned    memory_indicator_program  ; // MI PROG

    // intern state registers of console panel
    unsigned run_state; // cpu can be: reset, halt, running.
} realcons_console_logic_ki10_t;




struct realcons_struct;

realcons_console_logic_ki10_t *realcons_console_ki10_constructor(
        struct realcons_struct *realcons);
void realcons_console_ki10_destructor(realcons_console_logic_ki10_t *_this);

void realcons_console_ki10_interface_connect(realcons_console_logic_ki10_t *_this,
        realcons_console_controller_interface_t *intf, char *panel_name);

t_stat realcons_console_ki10_reset(realcons_console_logic_ki10_t *_this);

t_stat realcons_console_ki10_service(realcons_console_logic_ki10_t *_this);

int realcons_console_ki10_test(realcons_console_logic_ki10_t *_this, int arg);


void realcons_console_ki10_event_connect(realcons_console_logic_ki10_t *_this) ;
void realcons_console_ki10_event_disconnect(realcons_console_logic_ki10_t *_this) ;


void realcons_console_ki10_event_simh_deposit(realcons_console_logic_ki10_t *_this, struct REG *reg) ;


// realcons_cpu_ki10_maintpanel.c
t_stat realcons_console_ki10_maintpanel_service(realcons_console_logic_ki10_t *_this) ;

t_stat realcons_ki10_operpanel_service(realcons_console_logic_ki10_t *_this) ;


/*************************************************************************
 * Definitions private to realcons_ki10* modules
 * */
#ifdef REALCONS_CONSOLE_KI10_C_


// state of the cpu
#define RUN_STATE_HALT_MAN  0   // halt by panel
#define RUN_STATE_HALT_PROG 1   // halt by opcode
//#define RUN_STATE_RESET   1
//#define RUN_STATE_WAIT    2
#define RUN_STATE_RUN       3


// indexes of used general timers
#define TIMER_TEST  0
// #define TIMER_DATA_FLASH 1
#define TIME_TEST_MS        3000    // self test terminates after 3 seconds
//#define TIME_DATA_FLASH_MS    50      // if DATA LEDs flash, they are ON for 1/20 sec

 // SHORTER macros for signal access
 // assume "realcons_console_logic_ki10_t *_this" in context
#define SIGNAL_SET(signal,value) REALCONS_SIGNAL_SET(_this,signal,value)
#define SIGNAL_GET(signal) REALCONS_SIGNAL_GET(_this,signal)


#endif






#endif /* REALCONS_CONSOLE_KI10_H_ */
