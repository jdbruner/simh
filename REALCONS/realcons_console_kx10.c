/*  realcons_console_kx10.c: State and function identical for the PDP10 KA10 and KI10 console

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

   20-Feb-2016  JH      added PANEL_MODE_POWERLESS and power switch logic
   31-Aug-2014  JH      created


   Signals into simulated CPU.
   extensions to the simulated cpu.

   Life cycle of state:
    generated on SimH simulator start
    destroyed: never.
 */

#include <stddef.h>
#include <string.h>

#define REALCONS_CONSOLE_KX10_C_ // enable private global definitions in realcons_console_kx10.h
#include    "realcons.h"
#include    "realcons_console_kx10.h"
#include    "realcons_kx10_control.h"

 /*
  * constructor / destructor
  */
realcons_console_logic_kx10_t *realcons_console_kx10_constructor(realcons_t *realcons)
{
    realcons_console_logic_kx10_t *_this;

    _this = (realcons_console_logic_kx10_t *)malloc(sizeof(realcons_console_logic_kx10_t));
    memset(_this, 0, sizeof *_this);
    _this->realcons = realcons; // connect to parent
    _this->run_state = RUN_STATE_HALT_MAN;
    return _this;
}

void realcons_console_kx10_destructor(realcons_console_logic_kx10_t *_this)
{
    free(_this);
}

void realcons_console_kx10_event_connect(realcons_console_logic_kx10_t *_this)
{
    // everything off
    (*_this->realcons->console_controller_interface.reset_func)(_this);

    // panel connected to SimH: "POWER" Led, hour meter
    realcons_kx10_control_set(&_this->led_POWER, 1);
    realcons_kx10_control_set(&_this->enable_HOURMETER, 1);

    // Set panel mode to normal
    // On Java panels the powerbutton flips to the ON position.
    realcons_power_mode(_this->realcons, 1);
}

void realcons_console_kx10_event_disconnect(realcons_console_logic_kx10_t *_this)
{
    // everything off
    (*_this->realcons->console_controller_interface.reset_func)(_this);

    // panel disconnected from SimH: "POWER" Led
    realcons_kx10_control_set(&_this->led_POWER, 0);
    realcons_kx10_control_set(&_this->enable_HOURMETER, 0);

    // Set panel mode to normal
    // On Java panels the powerbutton flips to the ON position.
    realcons_power_mode(_this->realcons, 0);
}

void realcons_console_kx10__event_opcode_any(realcons_console_logic_kx10_t *_this)
{
    // other opcodes executed by processor
    if (_this->realcons->debug)
        printf("realcons_console_kx10__event_opcode_any\n");

    // set PC and instruction LEDS
    realcons_kx10_control_set(&_this->leds_PROGRAM_COUNTER, SIGNAL_GET(cpusignal_PC));
    realcons_kx10_control_set(&_this->leds_INSTRUCTION, SIGNAL_GET(cpusignal_instruction));

     // _this->run_state = RUN_STATE_RUN; // single step?
}

void realcons_console_kx10__event_opcode_halt(realcons_console_logic_kx10_t *_this)
{
    if (_this->realcons->debug)
        printf("realcons_console_kx10__event_opcode_halt\n");
    SIGNAL_SET(cpusignal_console_halt, 0);
    _this->run_state = RUN_STATE_HALT_PROG;
}

void realcons_console_kx10__event_run_start(realcons_console_logic_kx10_t *_this)
{
    if (_this->realcons->debug)
        printf("realcons_console_kx10__event_run_start\n");
    _this->run_state = RUN_STATE_RUN;
}

void realcons_console_kx10__event_step_start(realcons_console_logic_kx10_t *_this)
{
    if (_this->realcons->debug)
        printf("realcons_console_kx10__event_step_start\n");
    SIGNAL_SET(cpusignal_console_halt, 0); // running
}

void realcons_console_kx10__event_operator_halt(realcons_console_logic_kx10_t *_this)
{
    if (_this->realcons->debug)
        printf("realcons_console_kx10__event_operator_halt\n");
    //STOP button is "momentary action". Remove Stop condition after scp acknowledges it
    SIGNAL_SET(cpusignal_console_halt, 0);
    _this->run_state = RUN_STATE_HALT_MAN;

    // stop caused by RESET button? do reset
    if (_this->button_RESET.pendingbuttons) {
        sprintf(_this->realcons->simh_cmd_buffer, "reset all\n");
        _this->button_RESET.pendingbuttons = 0;
    }

}

void realcons_console_kx10__event_step_halt(realcons_console_logic_kx10_t *_this)
{
    if (_this->realcons->debug)
        printf("realcons_console_kx10__event_step_halt\n");
    SIGNAL_SET(cpusignal_console_halt, 1);
    _this->run_state = RUN_STATE_HALT_MAN;
}

// exam and deposit
// addr in cpusignal_memory_address_phys_register
// value in cpusignal_memory_data_register
void realcons_console_kx10__event_operator_exam_deposit(realcons_console_logic_kx10_t *_this)
{
    if (_this->realcons->debug)
        printf("realcons_console_kx10__event_operator_exam\n");

    // set data LEDs to value. data buttons unchanged
    // set memory indicator.
    // if there are address button lights (KI10 panel), show the address there
    // otherwise (KA10 panel), show it in the address LEDs
    _this->memory_indicator_program = 0;
#ifndef VM_PDP10
    extern uint8  MI_flag;    /* Memory indicator mode */
    MI_flag = 0;    // keep kx10_cpu flag in sync with memory_indicator_program
#endif
    uint64_t addr = SIGNAL_GET(cpusignal_memory_address_phys_register) & 0777777uL;
    if (_this->buttons_ADDRESS.lamps)
        realcons_kx10_control_set(&_this->buttons_ADDRESS, addr);
    else
        realcons_kx10_control_set(&_this->leds_INSTRUCTION,
            (realcons_kx10_control_get(&_this->leds_INSTRUCTION) & ~0777777uL) | addr);
    realcons_kx10_control_set(&_this->leds_DATA,
        SIGNAL_GET(cpusignal_memory_data_register));
    realcons_kx10_control_set(&_this->led_MEMORY_DATA, 1);
    realcons_kx10_control_set(&_this->led_PROGRAM_DATA, 0);
}

// if the program writes to console data or address registers,
// the MI indicator changes from  "MEMORY DATA" to "PROGRAM DATA"
// The new data value may not used with DATA LOAD or ADDR LOAD,
// until MI PROG DISABLE is set
// (to prevent fast changing values copied into the DATA/ADDR buttons ?)
// See bitsavers, pdf/dec/pdp10/TOPS10/1973_Assembly_Language_Handbook/01_1973AsmRef_SysRef.pdf
// document page "102".
void realcons_console_kx10__event_program_write_memory_indicator(
    realcons_console_logic_kx10_t *_this)
{
    // switch displaymode, if not disabled
    if (!realcons_kx10_control_get(&_this->button_MI_PROG_DIS)) {
        _this->memory_indicator_program = 1; // MI PROG

        // the small triangle memory indicators are set in service()

        // update DATA from CPU register, access under program control
        realcons_kx10_control_set(&_this->leds_DATA,
            SIGNAL_GET(cpusignal_console_memory_indicator));
    }
}


#ifndef VM_PDP10
// program (actually SimH user) has written to READ IN device switches
void realcons_console_kx10__event_program_write_readin_device(
    realcons_console_logic_kx10_t *_this)
{
    uint32 value = SIGNAL_GET(cpusignal_console_readin_device) & 0774;
    SIGNAL_SET(cpusignal_console_readin_device, value);
    realcons_kx10_control_set(&_this->buttons_READ_IN_DEVICE, value >> 2);
}
#endif

/* There was a deposit over simh console.
 * Convert to an event, if necessary.
 * reg is any simh register, not just one of ours.
 * Here events for access to the output registers "cim" and "caacs" are generated.
 * */
void realcons_console_kx10_event_simh_deposit(realcons_console_logic_kx10_t *_this,
struct REG *reg)
{
    // user made a "deposit CMI xxxx": flip "program data" ON
    if (reg->loc == _this->cpusignal_console_memory_indicator)  // cmi
        realcons_console_kx10__event_program_write_memory_indicator(_this);
#ifdef CAACS
    if (reg->loc == _this->cpusignal_console_address_addrcond) // caacs
        realcons_console_kx10__event_program_write_address_addrcond(_this);
#endif
#ifndef VM_PDP10
    if (reg->loc == _this->cpusignal_console_readin_device) // rdrin_dev
        realcons_console_kx10__event_program_write_readin_device(_this);
#endif
}


void realcons_console_kx10_interface_connect(realcons_console_logic_kx10_t *_this,
    realcons_console_controller_interface_t *intf, char *panel_name)
{
    intf->name = panel_name;
    intf->constructor_func =
        (console_controller_constructor_func_t)realcons_console_kx10_constructor;
    intf->destructor_func =
        (console_controller_destructor_func_t)realcons_console_kx10_destructor;
    intf->reset_func = (console_controller_reset_func_t)realcons_console_kx10_reset;
    intf->service_func = (console_controller_service_func_t)realcons_console_kx10_service;
    intf->test_func = (console_controller_test_func_t)realcons_console_kx10_test;

    intf->event_connect = (console_controller_event_func_t)realcons_console_kx10_event_connect;
    intf->event_disconnect = (console_controller_event_func_t)realcons_console_kx10_event_disconnect;

    // connect pdp10 cpu signals end events to simulator and
    // realcons state variables
    {
        // REALCONS extension in scp.c
        extern t_addr realcons_memory_address_phys_register; // REALCONS extension in scp.c
        extern char *realcons_register_name; // pseudo: name of last accessed register
        extern t_value realcons_memory_data_register; // REALCONS extension in scp.c
        extern  int realcons_console_halt; // 1: CPU halted by realcons console
        extern volatile t_bool sim_is_running; // global in scp.c
#ifdef VM_PDP10
        // from pdp10_cpu.c
        extern int32 pi_enb, pi_act, pi_on, pi_prq, pi_ioq;
        extern uint64_t cds, cmi, caacs;
        extern  int32           realcons_PC; // own buffer!
        extern uint64_t     realcons_instruction; // current instr
        extern int32            flags; // CPU state flags
#else
        // from kx10_cpu.c
        extern uint16 IOB_PI;     /* Pending Interrupt requests */
        extern uint8  PIR;        /* Current Interrupt requests */
        extern uint8  PIH;        /* Currently held interrupts */
        extern uint8  PIE;        /* Currently enabled interrupts */
        extern int    pi_enable;  /* Interrupt system enabled */
        extern uint64 SW;         /* Switch register */
        extern t_addr AS;         /* Address switches */
        extern uint64 MI;         /* Memory indicator register */
        extern uint32 rdrin_dev;  /* READ IN device */
        extern t_addr PC_Global;  /* Program counter register */
        extern uint64_t realcons_instruction; /* current instruction */
	extern uint8 realcons_xct;/* XCT in progress */
        extern uint32 FLAGS;      /* CPU state flags */
#endif

        realcons_console_halt = 0;

        // from scp.c
        _this->cpusignal_memory_address_phys_register = &realcons_memory_address_phys_register;
        _this->cpusignal_register_name = &realcons_register_name; // pseudo: name of last accessed register
        _this->cpusignal_memory_data_register = &realcons_memory_data_register;
        _this->cpusignal_console_halt = &realcons_console_halt;
        // is "sim_is_running" indeed identical with our "cpu_is_running" ?
        // may cpu stops, but some device are still serviced?
        _this->cpusignal_run = &(sim_is_running);
#ifdef VM_PDP10
        // from pdp10_cpu.c
        _this->cpusignal_PC = &realcons_PC; // own buffer!
        _this->cpusignal_flags = &flags; // CPU flags
        _this->cpusignal_instruction = &realcons_instruction; // current instruction
        _this->cpusignal_console_data_switches = &cds;
        _this->cpusignal_console_memory_indicator = &cmi;
#ifdef CAACS
        _this->cpusignal_console_address_addrcond = &caacs;
#endif
        _this->cpusignal_pi_on = &pi_on;
        _this->cpusignal_pi_enb = &pi_enb; // simh pdp10 cpu reg "PIENB"
        _this->cpusignal_pi_act = &pi_act; // simh  pdp10 cpu reg "PIACT"
        _this->cpusignal_pi_prq = &pi_prq; // simh  pdp10 cpu reg "PIPRQ"
        _this->cpusignal_pi_ioq = &pi_ioq; // simh  pdp10 cpu reg "PIIOQ"
#else
        // from kx10_cpu.c
        _this->cpusignal_PC = &PC_Global;                       /* program counter */
        _this->cpusignal_flags = &FLAGS;                        /* CPU state flags */
        _this->cpusignal_instruction = &realcons_instruction;   /* current instruction */
        _this->cpusignal_console_data_switches = &SW;           /* (data) switch register */
        _this->cpusignal_console_address_switches = &AS;        /* address switch register */
        _this->cpusignal_console_memory_indicator = &MI;        /* memory indicator register */
        _this->cpusignal_console_readin_device = &rdrin_dev;    /* READ IN device */
	    _this->cpusignal_xct = &realcons_xct;			/* XCT in progress */
#ifdef CAACS
        // _this->cpusignal_console_address_addrcond = 
#endif
        _this->cpusignal_pi_on = &pi_enable;                    /* interrupt system enabled */
        _this->cpusignal_pi_enb = &PIE;                         /* currently enabled interrupts */
        _this->cpusignal_pi_act = &PIH;                         /* currently held interrupts */
        _this->cpusignal_pi_prq = &PIR;                         /* current interrupt requests */
        _this->cpusignal_pi_ioq = &IOB_PI;                      /* pending interrupt requests */
#endif
    }

    /*** link handler to cpu/device events ***/
    {
        // scp.c
        extern console_controller_event_func_t realcons_event_run_start;
        extern console_controller_event_func_t realcons_event_operator_halt;
        extern console_controller_event_func_t realcons_event_step_start;
        extern console_controller_event_func_t realcons_event_step_halt;
        extern console_controller_event_func_t realcons_event_operator_exam;
        extern console_controller_event_func_t realcons_event_operator_deposit;
        extern console_controller_event_func_t realcons_event_operator_reg_exam;
        extern console_controller_event_func_t realcons_event_operator_reg_deposit;
        extern console_controller_event_func_t realcons_event_cpu_reset;
        // pdp10_cpu.c
        extern console_controller_event_func_t realcons_event_opcode_any; // triggered after any opcode execution
        extern console_controller_event_func_t realcons_event_opcode_halt;
        extern console_controller_event_func_t  realcons_event_program_write_memory_indicator;

        realcons_event_run_start =
            (console_controller_event_func_t)realcons_console_kx10__event_run_start;
        realcons_event_step_start =
            (console_controller_event_func_t)realcons_console_kx10__event_step_start;
        realcons_event_operator_halt =
            (console_controller_event_func_t)realcons_console_kx10__event_operator_halt;
        realcons_event_step_halt =
            (console_controller_event_func_t)realcons_console_kx10__event_step_halt;
        //exam, deposit handled by same routine
        realcons_event_operator_exam =
            (console_controller_event_func_t)realcons_console_kx10__event_operator_exam_deposit;
        realcons_event_operator_deposit =
            (console_controller_event_func_t)realcons_console_kx10__event_operator_exam_deposit;
            realcons_event_operator_reg_exam = realcons_event_operator_reg_deposit = NULL ;
            realcons_event_cpu_reset = NULL ;
        realcons_event_opcode_any =
            (console_controller_event_func_t)realcons_console_kx10__event_opcode_any;
        realcons_event_opcode_halt =
            (console_controller_event_func_t)realcons_console_kx10__event_opcode_halt;
        realcons_event_program_write_memory_indicator =
            (console_controller_event_func_t)realcons_console_kx10__event_program_write_memory_indicator;
    }

    if (!strcmp(panel_name, "PDP10-KA10"))
        realcons_console_ka10_interface_connect(_this, intf, panel_name);
    else if (!strcmp(panel_name, "PDP10-KI10"))
        realcons_console_ki10_interface_connect(_this, intf, panel_name);
}

// setup first state - provided by derived class
t_stat realcons_console_kx10_reset(realcons_console_logic_kx10_t *_this)
{
    abort();
    return SCPE_NOATT;  // if not overridden, then panel is unknown
}

// setup first state - internal helper, KX10
t_stat realcons_console_kx10_reset_internal(realcons_console_logic_kx10_t *_this,
        const struct kx10_control_def control_defs[])
{
    const struct kx10_control_def *cdef;

    _this->realcons->simh_cmd_buffer[0] = '\0';

    _this->memory_indicator_program = 0;

    /*
     * Create all controls according to the panel-specific definition
     */
    realcons_kx10_controls_count = 0; // empty list of controls

    for (cdef = control_defs; cdef->member_offset; cdef++)
        if (realcons_kx10_control_init((realcons_kx10_control_t *)((char *)_this + cdef->member_offset),
            _this->realcons, cdef->button_name, cdef->lamp_name, cdef->control_mode) != SCPE_OK)
            return SCPE_NOATT;

    return SCPE_OK;
}

// process panel state.
// operates on Blinkenlight_API panel structs,
// but RPC communication is done external
// this should be overridden to include panel-specific service
t_stat realcons_console_kx10_service(realcons_console_logic_kx10_t *_this)
{
    unsigned i;

#ifndef VM_PDP10
    // hack - ensure these parallel condition variables stay in sync
    extern uint8  MI_flag;    /* Memory indicator mode */
    _this->memory_indicator_program = MI_flag;
#endif

    // call the service function for all controls
    for (i = 0; realcons_kx10_controls[i]; i++)
        realcons_kx10_control_service(realcons_kx10_controls[i]);

    return SCPE_OK;
}

/*
 * start 1 sec test sequence.
 * - lamps ON for 1 sec
 * - print state of all switches
 */
int realcons_console_kx10_test(realcons_console_logic_kx10_t *_this, int arg)
{
    unsigned i;
    realcons_kx10_control_t *c;

    // send end time for test: 1 second = curtime + 1000
    // lamp test is set in service()
    _this->realcons->timer_running_msec[TIMER_TEST] = _this->realcons->service_cur_time_msec
        + TIME_TEST_MS;

    // lamps are set ON in _service() by monitoring the timer
    realcons_printf(_this->realcons, stdout, "Verify lamp test!\n");

    // print state of all buttons
    for (i = 0; (c = realcons_kx10_controls[i]); i++)
        if (c->buttons) {
            realcons_printf(_this->realcons, stdout, "Switch %-20s = %llo\n", c->buttons->name,
                c->buttons->value);
        }

    return 0; // OK

}
