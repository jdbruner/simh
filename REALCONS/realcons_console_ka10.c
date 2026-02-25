/*  realcons_console_ka10.c: State and function for the PDP10 KA10 console

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

static const struct kx10_control_def ka10_control_defs[] = {
    KX10CDEF(leds_PI_ACTIVE, NULL, "PI_ENB", OUTPUT),
    KX10CDEF(leds_IOB_PI_REQUEST, NULL, "PI_IOB", OUTPUT),
    KX10CDEF(leds_PI_IN_PROGRESS, NULL, "PI_PRO", OUTPUT),
    KX10CDEF(leds_PI_REQUEST, NULL, "PI_REQ", OUTPUT),
    KX10CDEF(led_PI_ON, NULL, "PI_ON", OUTPUT),
    KX10CDEF(led_RUN, NULL, "RUN", OUTPUT),
    KX10CDEF(led_POWER, NULL, "POWER", OUTPUT),
    KX10CDEF(leds_PROGRAM_COUNTER, NULL, "PC", OUTPUT),
    KX10CDEF(leds_INSTRUCTION, NULL, "INSTR", OUTPUT),
    KX10CDEF(led_PROGRAM_DATA, NULL, "PI", OUTPUT),
    KX10CDEF(led_MEMORY_DATA, NULL, "MI", OUTPUT),
    KX10CDEF(leds_DATA, NULL, "MB", OUTPUT),
    KX10CDEF(led_USER_MODE, NULL, "USER_MODE", OUTPUT),
    KX10CDEF(led_PROG_STOP, NULL, "PROG_STOP", OUTPUT),
    KX10CDEF(led_MEM_STOP, NULL, "MEM_STOP", OUTPUT),

    KX10CDEF(buttons_ADDRESS, "MA", NULL, SWITCH),
    KX10CDEF(buttons_DATA, "SR", NULL, SWITCH),
    KX10CDEF(button_SINGLE_INST, "SING_INST", NULL, SWITCH),
    KX10CDEF(button_SINGLE_PULSER, "SING_CYCL", NULL, KEY),
    KX10CDEF(button_STOP_PAR, "PAR_STOP", NULL, SWITCH),
    KX10CDEF(button_STOP_NXM, "NXM_STOP", NULL, SWITCH),
    KX10CDEF(button_REPEAT, "REP", NULL, SWITCH),
    KX10CDEF(button_FETCH_INST, "INST_FETCH", NULL, KEY),
    KX10CDEF(button_FETCH_DATA, "DATA_FETCH", NULL, KEY),
    KX10CDEF(button_WRITE, "WRITE", NULL, KEY),
    KX10CDEF(button_ADDRESS_STOP, "ADR_STOP", NULL, KEY),
    KX10CDEF(button_ADDRESS_BREAK, "ADR_BRK", NULL, KEY),
    KX10CDEF(button_READ_IN, "READ_IN", NULL, KEY),
    KX10CDEF(button_START, "START", NULL, KEY),
    KX10CDEF(button_CONT, "CONT", NULL, KEY),
    KX10CDEF(button_STOP, "STOP", NULL, KEY),
    KX10CDEF(button_RESET, "RESET", NULL, KEY),
    KX10CDEF(button_XCT, "XCT", NULL, KEY),
    KX10CDEF(button_EXAMINE_THIS, "EXAM_THIS", NULL, KEY),
    KX10CDEF(button_EXAMINE_NEXT, "EXAM_NEXT", NULL, KEY),
    KX10CDEF(button_DEPOSIT_THIS, "DEP_THIS", NULL, KEY),
    KX10CDEF(button_DEPOSIT_NEXT, "DEP_NEXT", NULL, KEY),
    { 0 }
};

static t_stat
realcons_console_ka10_reset(realcons_console_logic_kx10_t *_this)
{
    return realcons_console_kx10_reset_internal(_this, ka10_control_defs);
}

// process panel state.
// operates on Blinkenlight_API panel structs,
// but RPC communication is done external
// all input controls are read from panel before call
// all output controls are written back to panel after call
// keep track of any error status, but perform all steps
static t_stat
realcons_console_ka10_service(realcons_console_logic_kx10_t *_this)
{
    t_stat status, substatus;
    
    // call the base class method for the common functionality
    status = realcons_console_kx10_service(_this);

    // service the operator panel
    substatus = realcons_kx10_operpanel_service(_this);
    status = (status == SCPE_OK) ? substatus : status;

    return status;
}

void
realcons_console_ka10_interface_connect(realcons_console_logic_kx10_t *_this,
    realcons_console_controller_interface_t *intf, char *panel_name)
{
    // ka10-specific method overrides
    intf->reset_func = (console_controller_reset_func_t)realcons_console_ka10_reset;
    intf->service_func = (console_controller_service_func_t)realcons_console_ka10_service;
}