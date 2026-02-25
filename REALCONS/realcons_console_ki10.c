/*  realcons_console_ki10.c: State and function for the PDP10 KI10 console

   Copyright (c) 2014-2026, Joerg Hoppe, John D. Bruner
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

   24-Feb-2026  JB      refactored to support KA10 and KI10 consoles
   20-Feb-2016  JH      added PANEL_MODE_POWERLESS and power switch logic
   31-Aug-2014  JH      created

   This is an extension of the base realcons_kx10_console that is specific
   to the KI10.

   Life cycle of state:
    generated on SimH simulator start
    destroyed: never.
 */

#define REALCONS_CONSOLE_KX10_C_ // enable private global definitions in realcons_console_kx10.h
#include    <stddef.h>
#include    "realcons.h"
#include    "realcons_console_kx10.h"
#include    "realcons_kx10_control.h"

static t_stat realcons_console_ki10_reset(realcons_console_logic_kx10_t *_this);
static t_stat realcons_console_ki10_service(realcons_console_logic_kx10_t *_this);

static const struct kx10_control_def ki10_control_defs[] = {
    // maintenance panel
    KX10CDEF(lamp_OVERTEMP, NULL, "OVERTEMP", OUTPUT),
    KX10CDEF(lamp_CKT_BRKR_TRIPPED, NULL, "CKT_BRKR_TRIPPED", OUTPUT),
    KX10CDEF(lamp_DOORS_OPEN, NULL, "DOORS_OPEN", OUTPUT),
    KX10CDEF(knob_MARGIN_SELECT, "MARGIN_SELECT", NULL, INPUT),
    KX10CDEF(knob_IND_SELECT, "IND_SELECT", NULL, INPUT),
    KX10CDEF(knob_SPEED_CONTROL_COARSE, "SPEED_CONTROL_COARSE", NULL, INPUT),
    KX10CDEF(knob_SPEED_CONTROL_FINE, "SPEED_CONTROL_FINE", NULL, INPUT),
    KX10CDEF(knob_MARGIN_VOLTAGE, "MARGIN_VOLTAGE", NULL, INPUT),
    KX10CDEF(VOLTMETER, NULL, "VOLTMETER", OUTPUT),
    KX10CDEF(enable_HOURMETER, NULL, "HOURMETER", OUTPUT),
    KX10CDEF(button_FM_MANUAL, "FM_MANUAL_SW", "FM_MANUAL_FB", KEY),
    KX10CDEF(buttons_FM_BLOCK, "FM_BLOCK_SW", "FM_BLOCK_FB", SWITCH),
    KX10CDEF(buttons_SENSE, "SENSE_SW", "SENSE_FB", SWITCH),
    KX10CDEF(button_MI_PROG_DIS, "MI_PROG_DIS_SW", "MI_PROG_DIS_FB", SWITCH),
    KX10CDEF(button_MEM_OVERLAP_DIS, "MEM_OVERLAP_DIS_SW", "MEM_OVERLAP_DIS_FB", SWITCH),
    KX10CDEF(button_SINGLE_PULSE, "SINGLE_PULSE_SW", "SINGLE_PULSE_FB", SWITCH),
    KX10CDEF(button_MARGIN_ENABLE, "MARGIN_ENABLE_SW", "MARGIN_ENABLE_FB", SWITCH),
    KX10CDEF(buttons_MANUAL_MARGIN_ADDRESS, "MANUAL_MARGIN_ADDRESS_SW", "MANUAL_MARGIN_ADDRESS_FB", KEY),
    KX10CDEF(buttons_READ_IN_DEVICE, "READ_IN_DEVICE_SW", "READ_IN_DEVICE_FB", SWITCH),
    KX10CDEF(button_LAMP_TEST, "LAMP_TEST_SW", NULL, INPUT),
    // From doc: " the console data lock disables the data and sense switches;
    //  the console lock disables all other buttons except those that are mechanical"
    KX10CDEF(button_CONSOLE_LOCK, "CONSOLE_LOCK_SW", NULL, INPUT),
    KX10CDEF(button_CONSOLE_DATALOCK, "CONSOLE_DATALOCK_SW", NULL, INPUT),
    KX10CDEF(button_POWER, "POWERBUTTON_SW", NULL, INPUT),

    // operator panel
    KX10CDEF(leds_PI_ACTIVE, NULL, "PI_ACTIVE", OUTPUT),
    KX10CDEF(leds_IOB_PI_REQUEST, NULL, "IOB_PI_REQUEST", OUTPUT),
    KX10CDEF(leds_PI_IN_PROGRESS, NULL, "PI_IN_PROGRESS", OUTPUT),
    KX10CDEF(leds_PI_REQUEST, NULL, "PI_REQUEST", OUTPUT),
    KX10CDEF(led_PI_ON, NULL, "PI_ON", OUTPUT),
    KX10CDEF(led_PI_OK_8, NULL, "PI_OK_8", OUTPUT),
    // 0x01 = EXEC_MODE_KERNEL, 0x02 = EXEC_MODE_SUPER, 0x04 = USER_MODE_CONCEAL, 0x08 = USER_MODE_PUBLIC
    KX10CDEF(leds_MODE, NULL, "MODE", OUTPUT),
    KX10CDEF(led_KEY_PG_FAIL, NULL, "KEY_PG_FAIL", OUTPUT),
    KX10CDEF(led_KEY_MAINT, NULL, "KEY_MAINT", OUTPUT),
    // 0x01 = STOP_MEM, 0x02 = STOP_PROG, 0x04 = STOP_MAN
    KX10CDEF(leds_STOP, NULL, "STOP", OUTPUT),
    KX10CDEF(led_RUN, NULL, "RUN", OUTPUT),
    KX10CDEF(led_POWER, NULL, "POWER", OUTPUT),
    KX10CDEF(leds_PROGRAM_COUNTER, NULL, "PROGRAM_COUNTER", OUTPUT),
    KX10CDEF(leds_INSTRUCTION, NULL, "INSTRUCTION", OUTPUT),
    KX10CDEF(led_PROGRAM_DATA, NULL, "PROGRAM_DATA", OUTPUT),
    KX10CDEF(led_MEMORY_DATA, NULL, "MEMORY_DATA", OUTPUT),
    KX10CDEF(leds_DATA, NULL, "DATA", OUTPUT),
    // "In the upper half of the operator panel are four rows of indicators, and below them are three
    // rows of two-position keys and switches. Physically both are pushbuttons, but the keys are
    // momentary contact whereas the switches are alternate action.""
    KX10CDEF(button_PAGING_EXEC, "PAGING_EXEC_SW", "PAGING_EXEC_FB", SWITCH),
    KX10CDEF(button_PAGING_USER, "PAGING_USER_SW", "PAGING_USER_FB", SWITCH),
    KX10CDEF(buttons_ADDRESS, "ADDRESS_SW", "ADDRESS_FB", SWITCH),
    KX10CDEF(button_ADDRESS_CLEAR, "ADDRESS_CLEAR_SW", "ADDRESS_CLEAR_FB", KEY),
    KX10CDEF(button_ADDRESS_LOAD, "ADDRESS_LOAD_SW", "ADDRESS_LOAD_FB", KEY),
    KX10CDEF(buttons_DATA, "DATA_SW", "DATA_FB", SWITCH),
    KX10CDEF(button_DATA_CLEAR, "DATA_CLEAR_SW", "DATA_CLEAR_FB", KEY),
    KX10CDEF(button_DATA_LOAD, "DATA_LOAD_SW", "DATA_LOAD_FB", KEY),
    KX10CDEF(button_SINGLE_INST, "SINGLE_INST_SW", "SINGLE_INST_FB", SWITCH),
    KX10CDEF(button_SINGLE_PULSER, "SINGLE_PULSER_SW", "SINGLE_PULSER_FB", KEY),
    KX10CDEF(button_STOP_PAR, "STOP_PAR_SW", "STOP_PAR_FB", SWITCH),
    KX10CDEF(button_STOP_NXM, "STOP_NXM_SW", "STOP_NXM_FB", SWITCH),
    KX10CDEF(button_REPEAT, "REPEAT_SW", "REPEAT_FB", SWITCH),
    KX10CDEF(button_FETCH_INST, "FETCH_INST_SW", "FETCH_INST_FB", KEY),
    KX10CDEF(button_FETCH_DATA, "FETCH_DATA_SW", "FETCH_DATA_FB", KEY),
    KX10CDEF(button_WRITE, "WRITE_SW", "WRITE_FB", KEY),
    KX10CDEF(button_ADDRESS_STOP, "ADDRESS_STOP_SW", "ADDRESS_STOP_FB", KEY),
    KX10CDEF(button_ADDRESS_BREAK, "ADDRESS_BREAK_SW", "ADDRESS_BREAK_FB", KEY),
    KX10CDEF(button_READ_IN, "READ_IN_SW", "READ_IN_FB", KEY),
    KX10CDEF(button_START, "START_SW", "START_FB", KEY),
    KX10CDEF(button_CONT, "CONT_SW", "CONT_FB", KEY),
    KX10CDEF(button_STOP, "STOP_SW", "STOP_FB", KEY),
    KX10CDEF(button_RESET, "RESET_SW", "RESET_FB", KEY),
    KX10CDEF(button_XCT, "XCT_SW", "XCT_FB", KEY),
    KX10CDEF(button_EXAMINE_THIS, "EXAMINE_THIS_SW", "EXAMINE_THIS_FB", KEY),
    KX10CDEF(button_EXAMINE_NEXT, "EXAMINE_NEXT_SW", "EXAMINE_NEXT_FB", KEY),
    KX10CDEF(button_DEPOSIT_THIS, "DEPOSIT_THIS_SW", "DEPOSIT_THIS_FB", KEY),
    KX10CDEF(button_DEPOSIT_NEXT, "DEPOSIT_NEXT_SW", "DEPOSIT_NEXT_FB", KEY),
    { 0 }
};

#if CAACS
// program has written to address and address condition switches
void
realcons_console_kx10__event_program_write_address_addrcond(
    realcons_console_logic_kx10_t *_this)
{
    // switch displaymode, if not disabled
    if (!realcons_kx10_control_get(&_this->button_MI_PROG_DIS)) {
        uint64_t address_addrcond = SIGNAL_GET(cpusignal_console_address_addrcond);
        // _this->memory_indicator_program = 1 ; // MI PROG? not, if address is written?

        // console_address_addrcond codes the ADDRESS SWITCHES
        // and INT FETCH, DATA FETCH, WRITE, ADDRESS BREAK EXEC PAGING USER, pAGINMHG
        // See bitsavers, pdf/dec/pdp10/TOPS10/1973_Assembly_Language_Handbook/01_1973AsmRef_SysRef.pdf
        // document page "103".

        // INST_FETCH = Bit 0(pdp10)
        realcons_kx10_control_set(&_this->button_FETCH_INST,
            !!(address_addrcond & ((uint64_t)1 << (35 - 0))));
        // DATA_FETCH = Bit 1(pdp10)
        realcons_kx10_control_set(&_this->button_FETCH_DATA,
            !!(address_addrcond & ((uint64_t)1 << (35 - 1))));
        // WRITE = Bit 4(pdp10)
        realcons_kx10_control_set(&_this->button_WRITE,
            !!(address_addrcond & ((uint64_t)1 << (35 - 4))));
        // EXEC PAGING = Bit 5(pdp10)
        realcons_kx10_control_set(&_this->button_PAGING_EXEC,
            !!(address_addrcond & ((uint64_t)1 << (35 - 5))));
        // USER PAGING = Bit 6(pdp10)
        realcons_kx10_control_set(&_this->button_PAGING_USER,
            !!(address_addrcond & ((uint64_t)1 << (35 - 6))));
        // ADDRESS SWITCHES = Bits 7..35(pdp10) = lower 22 bits
        realcons_kx10_control_set(&_this->buttons_ADDRESS, address_addrcond & 0x3fffffL);
    }
}
#endif

static t_stat
realcons_console_ki10_reset(realcons_console_logic_kx10_t *_this)
{
    return realcons_console_kx10_reset_internal(_this, ki10_control_defs);
}

// process panel state.
// operates on Blinkenlight_API panel structs,
// but RPC communication is done external
// all input controls are read from panel before call
// all output controls are written back to panel after call
// keep track of any error status, but perform all steps
static t_stat
realcons_console_ki10_service(realcons_console_logic_kx10_t *_this)
{
    t_stat status, substatus;
    
    // call the base class method for the common functionality
    status = realcons_console_kx10_service(_this);

    // service the maintenance panel
    substatus = realcons_console_ki10_maintpanel_service(_this);
    status = (status == SCPE_OK) ? substatus : status;

    if (_this->realcons->lamp_test)
        return status; // no more light logic needed

    // service the operator panel
    substatus = realcons_kx10_operpanel_service(_this);
    status = (status == SCPE_OK) ? substatus : status;

    return status;
}

void
realcons_console_ki10_interface_connect(realcons_console_logic_kx10_t *_this,
    realcons_console_controller_interface_t *intf, char *panel_name)
{
    // KI10-specific method overrides
    intf->reset_func = (console_controller_reset_func_t)realcons_console_ki10_reset;
    intf->service_func = (console_controller_service_func_t)realcons_console_ki10_service;

#ifdef CAACS
    {
        extern console_controller_event_func_t  realcons_event_program_write_address_addrcond;
        realcons_event_program_write_address_addrcond =
            (console_controller_event_func_t)realcons_console_kx10__event_program_write_address_addrcond;
    }
#endif
}