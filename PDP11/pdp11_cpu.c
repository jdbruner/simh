/* pdp11_cpu.c: PDP-11 CPU simulator

   Copyright (c) 1993-2022, Robert M Supnik

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
   ROBERT M SUPNIK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Robert M Supnik shall not be
   used in advertising or otherwise to promote the sale, use or other dealings
   in this Software without prior written authorization from Robert M Supnik.

   cpu          PDP-11 CPU

   30-Nov-22    RMS     More 11/45,11/70 trap hackery (Walter Mueller)
   29-Nov-22    RMS     Trap stack abort must clear other traps/aborts (Walter Mueller)
   23-Oct-22    RMS     Fixed priority of MME traps (Walter Mueller)
   02-Sep-22    RMS     Fixed handling of PDR<A> (Walter Mueller)
   31-Aug-22    RMS     MMR0<15:13> != 0 locks bits<15:13> (Walter Mueller)
                        MMR0<12> = 1 disables further traps (Walter Mueller)
   25-Aug-22    RMS     11/45,70 clear MMR1 in trap sequence (Walter Mueller)
   23-Aug-22    RMS     11/45,70 detect red stack abort before memory write
                        in JSR, MFPx (Walter Mueller)
   20-Aug-22    RMS     MMR1 reads as 0 on subset memory mmgt systems
                        11/44, 45, 70 track PC changes (Walter Mueller)
                        J11 tracks PC changes on -(PC) and @-(PC)
   25-Jul-22    RMS     Removed deprecated CPU models (Q22, UHR11, URH70)
   04-Jun-18    RMS     Removed CPU model entries for UC15 (Mark Pizzolato)
   04-Dec-16    RMS     Removed duplicate IDLE entries in MTAB
   30-Aug-16    RMS     Fixed overloading of -d in ex/mod
   14-Mar-16    RMS     Added UC15 support
   06-Mar-16    RMS     Fixed bug in history virtual addressing
   30-Dec-15    RMS     Added NOBEVENT option for 11/03, 11/23
   29-Dec-15    RMS     Call build_dib_tab during reset (Mark Pizzolato)
   05-Dec-13    RMS     Fixed bug in CSM (John Dundas)
   23-Oct-13    RMS     Fixed PS behavior on initialization and boot
   10-Apr-13    RMS     MMR1 does not track PC changes (Johnny Billquist)
   29-Apr-12    RMS     Fixed compiler warning (Mark Pizzolato)
   19-Mar-12    RMS     Fixed declaration of sim_switches (Mark Pizzolato)
   29-Dec-08    RMS     Fixed failure to clear cpu_bme on RESET (Walter Mueller)
   22-Apr-08    RMS     Fixed MMR0 treatment in RESET (Walter Mueller)
   02-Feb-08    RMS     Fixed DMA memory address limit test (John Dundas)
   28-Apr-07    RMS     Removed clock initialization
   27-Oct-06    RMS     Added idle support
   18-Oct-06    RMS     Fixed bug in ASH -32 C value
   24-May-06    RMS     Added instruction history
   03-May-06    RMS     Fixed XOR operand fetch order for 11/70-style systems
   22-Sep-05    RMS     Fixed declarations (Sterling Garwood)
   16-Aug-05    RMS     Fixed C++ declaration and cast problems
   19-May-05    RMS     Replaced WAIT clock queue check with API call
   19-Jan-05    RMS     Fixed bug(s) in RESET for 11/70 (Tim Chapman)
   22-Dec-04    RMS     Fixed WAIT to work in all modes (John Dundas)
   02-Oct-04    RMS     Added model emulation
   25-Jan-04    RMS     Removed local debug logging support
   29-Dec-03    RMS     Formalized 18b Qbus support
   21-Dec-03    RMS     Added autoconfiguration controls
   05-Jun-03    RMS     Fixed bugs in memory size table
   12-Mar-03    RMS     Added logical name support
   01-Feb-03    RMS     Changed R display to follow PSW<rs>, added SP display
   19-Jan-03    RMS     Changed mode definitions for Apple Dev Kit conflict
   05-Jan-03    RMS     Added memory size restore support
   17-Oct-02    RMS     Fixed bug in examine/deposit (Hans Pufal)
   08-Oct-02    RMS     Revised to build dib_tab dynamically
                        Added SHOW IOSPACE
   09-Sep-02    RMS     Added KW11P support
   14-Jul-02    RMS     Fixed bug in MMR0 error status load
   03-Jun-02    RMS     Fixed relocation add overflow, added PS<15:12> = 1111
                        special case logic to MFPI and removed it from MTPI
                        (John Dundas)
   29-Apr-02    RMS     More fixes to DIV and ASH/ASHC (John Dundas)
   28-Apr-02    RMS     Fixed bugs in illegal instruction 000010 and in
                        write-only memory pages (Wolfgang Helbig)
   21-Apr-02    RMS     Fixed bugs in DIV by zero, DIV overflow, TSTSET, RTS,
                        ASHC -32, and red zone trap (John Dundas)
   04-Mar-02    RMS     Changed double operand evaluation order for M+
   23-Feb-02    RMS     Fixed bug in MAINT, CPUERR, MEMERR read
   28-Jan-02    RMS     Revised for multiple timers; fixed calc_MMR1 macros
   06-Jan-02    RMS     Revised enable/disable support
   30-Dec-01    RMS     Added old PC queue
   25-Dec-01    RMS     Cleaned up sim_inst declarations
   11-Dec-01    RMS     Moved interrupt debug code
   07-Dec-01    RMS     Revised to use new breakpoint package
   08-Nov-01    RMS     Moved I/O to external module
   26-Oct-01    RMS     Revised to use symbolic definitions for IO page
   15-Oct-01    RMS     Added debug logging
   08-Oct-01    RMS     Fixed bug in revised interrupt logic
   07-Sep-01    RMS     Revised device disable and interrupt mechanisms
   26-Aug-01    RMS     Added DZ11 support
   10-Aug-01    RMS     Removed register from declarations
   17-Jul-01    RMS     Fixed warning from VC++ 6.0
   01-Jun-01    RMS     Added DZ11 interrupts
   23-Apr-01    RMS     Added RK611 support
   05-Apr-01    RMS     Added TS11/TSV05 support
   05-Mar-01    RMS     Added clock calibration support
   11-Feb-01    RMS     Added DECtape support
   25-Jan-01    RMS     Fixed 4M memory definition (Eric Smith)
   14-Apr-99    RMS     Changed t_addr to unsigned
   18-Aug-98    RMS     Added CIS support
   09-May-98    RMS     Fixed bug in DIV overflow test
   19-Jan-97    RMS     Added RP/RM support
   06-Apr-96    RMS     Added dynamic memory sizing
   29-Feb-96    RMS     Added TM11 support
   17-Jul-94    RMS     Corrected updating of MMR1 if MMR0 locked

   The register state for the PDP-11 is:

   REGFILE[0:5][0]      general register set
   REGFILE[0:5][1]      alternate general register set
   STACKFILE[4]         stack pointers for kernel, supervisor, unused, user
   PC                   program counter
   PSW                  processor status word
    <15:14> = CM         current processor mode
    <13:12> = PM         previous processor mode
    <11> = RS            register set select
    <8> = FPD            first part done (CIS)
    <7:5> = IPL          interrupt priority level
    <4> = TBIT           trace trap enable
    <3:0> = NZVC         condition codes
   FR[0:5]              floating point accumulators
   FPS                  floating point status register
   FEC                  floating exception code
   FEA                  floating exception address
   MMR0,1,2,3           memory management control registers
   APRFILE[0:63]        memory management relocation registers for
                         kernel, supervisor, unused, user
    <31:16> = PAR        processor address registers
    <15:0> = PDR         processor data registers
   PIRQ                 processor interrupt request register
   CPUERR               CPU error register
   MEMERR               memory system error register
   CCR                  cache control register
   MAINT                maintenance register
   HITMISS              cache status register
   SR                   switch register
   DR                   display register

   The PDP-11 has many instruction formats:

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    double operand
   |  opcode   |   source spec   |     dest spec   |    010000:067777
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    110000:167777

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    register + operand
   |        opcode      | src reg|     dest spec   |    004000:004777
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    070000:077777

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    single operand
   |           opcode            |     dest spec   |    000100:000177
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    000300:000377
                                                        005000:007777
                                                        105000:107777

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    single register
   |                opcode                |dest reg|    000200:000207
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    000230:000237

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    no operand
   |                     opcode                    |    000000:000007
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    branch
   |       opcode          |  branch displacement  |    000400:003477
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    100000:103477

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    EMT/TRAP
   |       opcode          |       trap code       |    104000:104777
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    cond code operator
   |                opcode             | immediate |    000240:000277
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

   An operand specifier consists of an addressing mode and a register.
   The addressing modes are:

   0    register direct         R               op = R
   1    register deferred       (R)             op = M[R]
   2    autoincrement           (R)+            op = M[R]; R = R + length
   3    autoincrement deferred  @(R)+           op = M[M[R]]; R = R + 2
   4    autodecrement           -(R)            R = R - length; op = M[R]
   5    autodecrement deferred  @-(R)           R = R - 2; op = M[M[R]]
   6    displacement            d(R)            op = M[R + disp]
   7    displacement deferred   @d(R)           op = M[M[R + disp]]

   There are eight general registers, R0-R7.  R6 is the stack pointer,
   R7 the PC.  The combination of addressing modes with R7 yields:

   27   immediate               #n              op = M[PC]; PC = PC + 2
   37   absolute                @#n             op = M[M[PC]]; PC = PC + 2
   67   relative                d(PC)           op = M[PC + disp]
   77   relative deferred       @d(PC)          op = M[M[PC + disp]]

   This routine is the instruction decode routine for the PDP-11.  It
   is called from the simulator control program to execute instructions
   in simulated memory, starting at the simulated PC.  It runs until an
   enabled exception is encountered.

   General notes:

   1. Virtual address format.  PDP-11 memory management uses the 16b
      virtual address, the type of reference (instruction or data), and
      the current mode, to construct the 22b physical address.  To
      package this conveniently, the simulator uses a 19b pseudo virtual
      address, consisting of the 16b virtual address prefixed with the
      current mode and ispace/dspace indicator.  These are precalculated
      as isenable and dsenable for ispace and dspace, respectively, and
      must be recalculated whenever MMR0, MMR3, or PSW<cm> changes.

   2. Traps and interrupts.  Variable trap_req bit-encodes all possible
      traps.  In addition, an interrupt pending bit is encoded as the
      lowest priority trap.  Traps are processed by trap_vec and trap_clear,
      which provide the vector and subordinate traps to clear, respectively.

      Array int_req[0:7] bit encodes all possible interrupts.  It is masked
      under the interrupt priority level, ipl.  If any interrupt request
      is not masked, the interrupt bit is set in trap_req.  While most
      interrupts are handled centrally, a device can supply an interrupt
      acknowledge routine.

   3. PSW handling.  The PSW is kept as components, for easier access.
      Because the PSW can be explicitly written as address 17777776,
      all instructions must update PSW before executing their last write.

   4. Adding I/O devices.  These modules must be modified:

        pdp11_defs.h    add device address and interrupt definitions
        pdp11_sys.c     add to sim_devices table entry
*/

/* Definitions */

#ifdef USE_REALCONS
#include "realcons.h"	/* REAL-CONSOLE */
// !!! must be included before pd11_defs.h, because  duplicate symbol INT_PTR
// #1: realcons.h  -> .... -> rpc_blinkenlight_api.h -> rpc.h -> windows.h --> basetsd.h
// #2: pdp11_defs.h
#endif

#include "pdp11_defs.h"
#include "pdp11_cpumod.h"

#define PCQ_SIZE        64                              /* must be 2**n */
#define PCQ_MASK        (PCQ_SIZE - 1)
#define PCQ_ENTRY       pcq[pcq_p = (pcq_p - 1) & PCQ_MASK] = PC
#define calc_is(md)     ((md) << VA_V_MODE)
#define calc_ds(md)     (calc_is((md)) | ((MMR3 & dsmask[(md)])? VA_DS: 0))
/* Register change tracking actually goes into variable reg_mods; from there
   it is copied into MMR1 if that register is not currently locked.  */
#define calc_MMR1(val)  ((reg_mods)? (((val) << 8) | reg_mods): (val))
#define GET_SIGN_W(v)   (((v) >> 15) & 1)
#define GET_SIGN_B(v)   (((v) >> 7) & 1)
#define GET_Z(v)        ((v) == 0)
#define JMP_PC(x)       PCQ_ENTRY; PC = (x)
#define BRANCH_F(x)     PCQ_ENTRY; PC = (PC + (((x) + (x)) & 0377)) & 0177777
#define BRANCH_B(x)     PCQ_ENTRY; PC = (PC + (((x) + (x)) | 0177400)) & 0177777
#define UNIT_V_MSIZE    (UNIT_V_UF + 0)                 /* dummy */
#define UNIT_MSIZE      (1u << UNIT_V_MSIZE)

#define HIST_MIN        64
#define HIST_MAX        (1u << 18)
#define HIST_VLD        1                               /* make PC odd */
#define HIST_ILNT       4                               /* max inst length */

typedef struct {
    uint16              pc;
    uint16              psw;
    uint16              src;
    uint16              dst;
    uint16              sp;
    uint16              pad;
    uint16              inst[HIST_ILNT];
    } InstHistory;

/* Global state */

uint16 *M = NULL;                                       /* memory */
int32 REGFILE[6][2] = { {0} };                          /* R0-R5, two sets */
int32 STACKFILE[4] = { 0 };                             /* SP, four modes */
int32 saved_PC = 0;                                     /* program counter */
int32 R[8] = { 0 };                                     /* working registers */
int32 PSW = 0;                                          /* PSW */
  int32 cm = 0;                                         /*   current mode */
  int32 pm = 0;                                         /*   previous mode */
  int32 rs = 0;                                         /*   register set */
  int32 fpd = 0;                                        /*   first part done */
  int32 ipl = 0;                                        /*   int pri level */
  int32 tbit = 0;                                       /*   trace flag */
  int32 N = 0, Z = 0, V = 0, C = 0;                     /*   condition codes */
int32 wait_state = 0;                                   /* wait state */
int32 trap_req = 0;                                     /* trap requests */
int32 int_req[IPL_HLVL] = { 0 };                        /* interrupt requests */
int32 PIRQ = 0;                                         /* programmed int req */
int32 STKLIM = 0;                                       /* stack limit */
fpac_t FR[6] = { {0} };                                 /* fp accumulators */
int32 FPS = 0;                                          /* fp status */
int32 FEC = 0;                                          /* fp exception code */
int32 FEA = 0;                                          /* fp exception addr */
int32 APRFILE[64] = { 0 };                              /* PARs/PDRs */
int32 MMR0 = 0;                                         /* MMR0 - status */
int32 MMR1 = 0;                                         /* MMR1 - R+/-R */
int32 MMR2 = 0;                                         /* MMR2 - saved PC */
int32 MMR3 = 0;                                         /* MMR3 - 22b status */
int32 cpu_bme = 0;                                      /* bus map enable */
int32 cpu_astop = 0;                                    /* address stop */
int32 isenable = 0, dsenable = 0;                       /* i, d space flags */
int32 stop_trap = 1;                                    /* stop on trap */
int32 stop_vecabort = 1;                                /* stop on vec abort */
int32 stop_spabort = 1;                                 /* stop on SP abort */
int32 autcon_enb = 1;                                   /* autoconfig enable */
uint32 cpu_model = INIMODEL;                            /* CPU model */
uint32 cpu_type = 1u << INIMODEL;                       /* model as bit mask */
uint32 cpu_opt = INIOPTNS;                              /* CPU options */
uint16 pcq[PCQ_SIZE] = { 0 };                           /* PC queue */
int32 pcq_p = 0;                                        /* PC queue ptr */
REG *pcq_r = NULL;                                      /* PC queue reg ptr */
jmp_buf save_env;                                       /* abort handler */
int32 hst_p = 0;                                        /* history pointer */
int32 hst_lnt = 0;                                      /* history length */
InstHistory *hst = NULL;                                /* instruction history */
int32 dsmask[4] = { MMR3_KDS, MMR3_SDS, 0, MMR3_UDS };  /* dspace enables */
int16 inst_pc;                                          /* PC of current instr */
int32 inst_psw;                                         /* PSW at instr. start */
int16 reg_mods;                                         /* reg deltas */
int32 last_pa;                                          /* pa from ReadMW/ReadMB */
int32 saved_sim_interval;                               /* saved at inst start */
t_stat reason;                                          /* stop reason */

extern int32 CPUERR, MAINT;
extern CPUTAB cpu_tab[];

/* Function declarations */

t_stat cpu_ex (t_value *vptr, t_addr addr, UNIT *uptr, int32 sw);
t_stat cpu_dep (t_value val, t_addr addr, UNIT *uptr, int32 sw);
t_stat cpu_reset (DEVICE *dptr);
t_stat cpu_boot (int32 unitno, DEVICE *dptr);
t_bool cpu_is_pc_a_subroutine_call (t_addr **ret_addrs);
t_stat cpu_set_hist (UNIT *uptr, int32 val, CONST char *cptr, void *desc);
t_stat cpu_show_hist (FILE *st, UNIT *uptr, int32 val, CONST void *desc);
t_stat cpu_show_virt (FILE *st, UNIT *uptr, int32 val, CONST void *desc);
t_stat cpu_help (FILE *st, DEVICE *dptr, UNIT *uptr, int32 flag, const char *cptr);
const char *cpu_description (DEVICE *dptr);
int32 GeteaB (int32 spec);
int32 GeteaW (int32 spec);
int32 relocR (int32 addr);
int32 relocW (int32 addr);
void relocR_test (int32 va, int32 apridx);
void relocW_test (int32 va, int32 apridx);
int32 clean_MMR1 (int32 mmr1);
int32 relocC (int32 va, int32 sw);
t_bool PLF_test (int32 va, int32 apr);
void reloc_abort (int32 err, int32 apridx);
int32 ReadE (int32 addr);
int32 ReadW (int32 addr);
int32 ReadB (int32 addr);
int32 ReadCW (int32 addr);
int32 ReadMW (int32 addr);
int32 ReadMB (int32 addr);
int32 PReadW (int32 addr);
int32 PReadB (int32 addr);
void WriteW (int32 data, int32 addr);
void WriteB (int32 data, int32 addr);
void WriteCW (int32 data, int32 addr);
void PWriteW (int32 data, int32 addr);
void PWriteB (int32 data, int32 addr);
void set_r_display (int32 rs, int32 cm);
t_stat CPU_wr (int32 data, int32 addr, int32 access);
void set_stack_trap (int32 adr);
int32 get_PSW (void);
void put_PSW (int32 val, t_bool prot);
void put_PIRQ (int32 val);

extern void fp11 (int32 IR);
extern t_stat cis11 (int32 IR);
extern t_stat fis11 (int32 IR);
extern t_stat build_dib_tab (void);
extern t_stat iopageR (int32 *data, uint32 addr, int32 access);
extern t_stat iopageW (int32 data, uint32 addr, int32 access);
extern int32 calc_ints (int32 nipl, int32 trq);
extern int32 get_vector (int32 nipl);

/* Trap data structures */

int32 trap_vec[TRAP_V_MAX] = {                          /* trap req to vector */
    VEC_RED, VEC_ODD, VEC_NXM, VEC_MME,
    VEC_PAR, VEC_PRV, VEC_ILL, VEC_BPT,
    VEC_IOT, VEC_EMT, VEC_TRAP, VEC_TRC,
    VEC_YEL, VEC_PWRFL, VEC_FPE
    };

t_bool trap_load_mmr2[TRAP_V_MAX + 1] = {               /* do trap requests load MMR2? */
    TRUE, TRUE, TRUE, TRUE,
    TRUE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, TRUE,
    TRUE, TRUE, TRUE, TRUE                              /* last is interrupt */
    };

int32 trap_clear[TRAP_V_MAX] = {                        /* trap clears */
    TRAP_RED+TRAP_ODD+TRAP_NXM+TRAP_PAR+TRAP_YEL+TRAP_TRC+TRAP_MME, /* red stack abort */
    TRAP_ODD+TRAP_NXM+TRAP_PAR+TRAP_YEL+TRAP_TRC+TRAP_MME, /* odd address abort */
    TRAP_NXM+TRAP_PAR+TRAP_YEL+TRAP_TRC+TRAP_MME,       /* nxm abort */
    TRAP_MME+TRAP_PAR+TRAP_YEL+TRAP_TRC,                /* mme abort or trap */
    TRAP_PAR+TRAP_YEL+TRAP_TRC,
    TRAP_PRV+TRAP_TRC,                                  /* instruction traps */
    TRAP_ILL+TRAP_TRC,                                  /* occur in fetch or */
    TRAP_BPT+TRAP_TRC,                                  /* initial decode */
    TRAP_IOT+TRAP_TRC,                                  /* no yelstk possible */
    TRAP_EMT+TRAP_TRC,
    TRAP_TRAP+TRAP_TRC,
    TRAP_TRC,
    TRAP_YEL,
    TRAP_PWRFL,
    TRAP_FPE
    };

/* CPU data structures

   cpu_dev      CPU device descriptor
   cpu_unit     CPU unit descriptor
   cpu_reg      CPU register list
   cpu_mod      CPU modifier list
*/

UNIT cpu_unit = { UDATA (NULL, UNIT_FIX|UNIT_BINK, INIMEMSIZE) };

const char *psw_modes[] = {"K", "S", "E", "U"};


BITFIELD psw_bits[] = {
    BIT(C),                                 /* Carry */
    BIT(V),                                 /* Overflow */
    BIT(Z),                                 /* Zero */
    BIT(N),                                 /* Negative */
    BIT(TBIT),                              /* trace trap */
    BITFFMT(IPL,3,%d),                      /* IPL */
    BIT(FPD),                               /* First Part Done */
    BITNCF(2),                              /* MBZ */
    BIT(RS),                                /* Register Set */
    BITFNAM(PM,2,psw_modes),                /* Previous Access Mode */
    BITFNAM(CM,2,psw_modes),                /* Current Access Mode */
    ENDBITS
};

REG cpu_reg[] = {
    { ORDATAD (PC, saved_PC, 16,            "Program Counter") },
    { ORDATAD (R0, REGFILE[0][0], 16,       "General Purpose R0") },
    { ORDATAD (R1, REGFILE[1][0], 16,       "General Purpose R1") },
    { ORDATAD (R2, REGFILE[2][0], 16,       "General Purpose R2") },
    { ORDATAD (R3, REGFILE[3][0], 16,       "General Purpose R3") },
    { ORDATAD (R4, REGFILE[4][0], 16,       "General Purpose R4") },
    { ORDATAD (R5, REGFILE[5][0], 16,       "General Purpose R5") },
    { ORDATAD (SP, STACKFILE[MD_KER], 16,   "Stack Pointer"), },
    { ORDATAD (R00, REGFILE[0][0], 16,      "Register File R00") },
    { ORDATAD (R01, REGFILE[1][0], 16,      "Register File R01") },
    { ORDATAD (R02, REGFILE[2][0], 16,      "Register File R02") },
    { ORDATAD (R03, REGFILE[3][0], 16,      "Register File R03") },
    { ORDATAD (R04, REGFILE[4][0], 16,      "Register File R04") },
    { ORDATAD (R05, REGFILE[5][0], 16,      "Register File R05") },
    { ORDATAD (R10, REGFILE[0][1], 16,      "Register File R10") },
    { ORDATAD (R11, REGFILE[1][1], 16,      "Register File R11") },
    { ORDATAD (R12, REGFILE[2][1], 16,      "Register File R12") },
    { ORDATAD (R13, REGFILE[3][1], 16,      "Register File R13") },
    { ORDATAD (R14, REGFILE[4][1], 16,      "Register File R14") },
    { ORDATAD (R15, REGFILE[5][1], 16,      "Register File R15") },
    { ORDATAD (KSP, STACKFILE[MD_KER], 16,  "Kernel Stack Pointer" ) },
    { ORDATAD (SSP, STACKFILE[MD_SUP], 16,  "Supervisor Stack Pointer" ) },
    { ORDATAD (USP, STACKFILE[MD_USR], 16,  "User Stack Pointer" ) },
    { ORDATADF(PSW, PSW, 16,                "Processor Status Word", psw_bits) },
    { GRDATAD (CM, PSW, 8, 2, PSW_V_CM,     "Current Mode") },
    { GRDATAD (PM, PSW, 8, 2, PSW_V_PM,     "Previous Mode") },
    { FLDATAD (RS, PSW, PSW_V_RS,           "Register Set") },
    { FLDATAD (FPD, PSW, PSW_V_FPD,         "First Part Done") },
    { GRDATAD (IPL, PSW, 8, 3, PSW_V_IPL,   "Interrupt Priority Level") },
    { FLDATAD (T, PSW, PSW_V_TBIT,          "Trace Trap") },
    { FLDATAD (N, PSW, PSW_V_N,             "Condition Code: Negative") },
    { FLDATAD (Z, PSW, PSW_V_Z,             "Condition Code: Zero") },
    { FLDATAD (V, PSW, PSW_V_V,             "Condition Code: Overflow") },
    { FLDATAD (C, PSW, PSW_V_C,             "Condition Code: Carry") },
    { ORDATAD (PIRQ, PIRQ, 16,              "programmed interrupt requests") },
    { ORDATAD (STKLIM, STKLIM, 16,          "stack limit") },
    { ORDATAD (FAC0H, FR[0].h, 32,          "Floating Point: R0 High") },
    { ORDATAD (FAC0L, FR[0].l, 32,          "Floating Point: R0 Low") },
    { ORDATAD (FAC1H, FR[1].h, 32,          "Floating Point: R1 High") },
    { ORDATAD (FAC1L, FR[1].l, 32,          "Floating Point: R1 Low") },
    { ORDATAD (FAC2H, FR[2].h, 32,          "Floating Point: R2 High") },
    { ORDATAD (FAC2L, FR[2].l, 32,          "Floating Point: R2 Low") },
    { ORDATAD (FAC3H, FR[3].h, 32,          "Floating Point: R3 High") },
    { ORDATAD (FAC3L, FR[3].l, 32,          "Floating Point: R3 Low") },
    { ORDATAD (FAC4H, FR[4].h, 32,          "Floating Point: R4 High") },
    { ORDATAD (FAC4L, FR[4].l, 32,          "Floating Point: R4 Low") },
    { ORDATAD (FAC5H, FR[5].h, 32,          "Floating Point: R5 High") },
    { ORDATAD (FAC5L, FR[5].l, 32,          "Floating Point: R5 Low") },
    { ORDATAD (FPS, FPS, 16,                "floating point status") },
    { ORDATAD (FEA, FEA, 16,                "floating exception address") },
    { ORDATAD (FEC, FEC, 4,                 "floating exception code") },
    { ORDATAD (MMR0, MMR0, 16,              "MMR0 - Status") },
    { ORDATAD (MMR1, MMR1, 16,              "MMR1 - R+/-R") },
    { ORDATAD (MMR2, MMR2, 16,              "MMR2 - saved PC") },
    { ORDATAD (MMR3, MMR3, 16,              "MMR3 - 22b status") },
    { GRDATA (KIPAR0, APRFILE[000], 8, 16, 16) },
    { GRDATA (KIPDR0, APRFILE[000], 8, 16, 0) },
    { GRDATA (KIPAR1, APRFILE[001], 8, 16, 16) },
    { GRDATA (KIPDR1, APRFILE[001], 8, 16, 0) },
    { GRDATA (KIPAR2, APRFILE[002], 8, 16, 16) },
    { GRDATA (KIPDR2, APRFILE[002], 8, 16, 0) },
    { GRDATA (KIPAR3, APRFILE[003], 8, 16, 16) },
    { GRDATA (KIPDR3, APRFILE[003], 8, 16, 0) },
    { GRDATA (KIPAR4, APRFILE[004], 8, 16, 16) },
    { GRDATA (KIPDR4, APRFILE[004], 8, 16, 0) },
    { GRDATA (KIPAR5, APRFILE[005], 8, 16, 16) },
    { GRDATA (KIPDR5, APRFILE[005], 8, 16, 0) },
    { GRDATA (KIPAR6, APRFILE[006], 8, 16, 16) },
    { GRDATA (KIPDR6, APRFILE[006], 8, 16, 0) },
    { GRDATA (KIPAR7, APRFILE[007], 8, 16, 16) },
    { GRDATA (KIPDR7, APRFILE[007], 8, 16, 0) },
    { GRDATA (KDPAR0, APRFILE[010], 8, 16, 16) },
    { GRDATA (KDPDR0, APRFILE[010], 8, 16, 0) },
    { GRDATA (KDPAR1, APRFILE[011], 8, 16, 16) },
    { GRDATA (KDPDR1, APRFILE[011], 8, 16, 0) },
    { GRDATA (KDPAR2, APRFILE[012], 8, 16, 16) },
    { GRDATA (KDPDR2, APRFILE[012], 8, 16, 0) },
    { GRDATA (KDPAR3, APRFILE[013], 8, 16, 16) },
    { GRDATA (KDPDR3, APRFILE[013], 8, 16, 0) },
    { GRDATA (KDPAR4, APRFILE[014], 8, 16, 16) },
    { GRDATA (KDPDR4, APRFILE[014], 8, 16, 0) },
    { GRDATA (KDPAR5, APRFILE[015], 8, 16, 16) },
    { GRDATA (KDPDR5, APRFILE[015], 8, 16, 0) },
    { GRDATA (KDPAR6, APRFILE[016], 8, 16, 16) },
    { GRDATA (KDPDR6, APRFILE[016], 8, 16, 0) },
    { GRDATA (KDPAR7, APRFILE[017], 8, 16, 16) },
    { GRDATA (KDPDR7, APRFILE[017], 8, 16, 0) },
    { GRDATA (SIPAR0, APRFILE[020], 8, 16, 16) },
    { GRDATA (SIPDR0, APRFILE[020], 8, 16, 0) },
    { GRDATA (SIPAR1, APRFILE[021], 8, 16, 16) },
    { GRDATA (SIPDR1, APRFILE[021], 8, 16, 0) },
    { GRDATA (SIPAR2, APRFILE[022], 8, 16, 16) },
    { GRDATA (SIPDR2, APRFILE[022], 8, 16, 0) },
    { GRDATA (SIPAR3, APRFILE[023], 8, 16, 16) },
    { GRDATA (SIPDR3, APRFILE[023], 8, 16, 0) },
    { GRDATA (SIPAR4, APRFILE[024], 8, 16, 16) },
    { GRDATA (SIPDR4, APRFILE[024], 8, 16, 0) },
    { GRDATA (SIPAR5, APRFILE[025], 8, 16, 16) },
    { GRDATA (SIPDR5, APRFILE[025], 8, 16, 0) },
    { GRDATA (SIPAR6, APRFILE[026], 8, 16, 16) },
    { GRDATA (SIPDR6, APRFILE[026], 8, 16, 0) },
    { GRDATA (SIPAR7, APRFILE[027], 8, 16, 16) },
    { GRDATA (SIPDR7, APRFILE[027], 8, 16, 0) },
    { GRDATA (SDPAR0, APRFILE[030], 8, 16, 16) },
    { GRDATA (SDPDR0, APRFILE[030], 8, 16, 0) },
    { GRDATA (SDPAR1, APRFILE[031], 8, 16, 16) },
    { GRDATA (SDPDR1, APRFILE[031], 8, 16, 0) },
    { GRDATA (SDPAR2, APRFILE[032], 8, 16, 16) },
    { GRDATA (SDPDR2, APRFILE[032], 8, 16, 0) },
    { GRDATA (SDPAR3, APRFILE[033], 8, 16, 16) },
    { GRDATA (SDPDR3, APRFILE[033], 8, 16, 0) },
    { GRDATA (SDPAR4, APRFILE[034], 8, 16, 16) },
    { GRDATA (SDPDR4, APRFILE[034], 8, 16, 0) },
    { GRDATA (SDPAR5, APRFILE[035], 8, 16, 16) },
    { GRDATA (SDPDR5, APRFILE[035], 8, 16, 0) },
    { GRDATA (SDPAR6, APRFILE[036], 8, 16, 16) },
    { GRDATA (SDPDR6, APRFILE[036], 8, 16, 0) },
    { GRDATA (SDPAR7, APRFILE[037], 8, 16, 16) },
    { GRDATA (SDPDR7, APRFILE[037], 8, 16, 0) },
    { GRDATA (UIPAR0, APRFILE[060], 8, 16, 16) },
    { GRDATA (UIPDR0, APRFILE[060], 8, 16, 0) },
    { GRDATA (UIPAR1, APRFILE[061], 8, 16, 16) },
    { GRDATA (UIPDR1, APRFILE[061], 8, 16, 0) },
    { GRDATA (UIPAR2, APRFILE[062], 8, 16, 16) },
    { GRDATA (UIPDR2, APRFILE[062], 8, 16, 0) },
    { GRDATA (UIPAR3, APRFILE[063], 8, 16, 16) },
    { GRDATA (UIPDR3, APRFILE[063], 8, 16, 0) },
    { GRDATA (UIPAR4, APRFILE[064], 8, 16, 16) },
    { GRDATA (UIPDR4, APRFILE[064], 8, 16, 0) },
    { GRDATA (UIPAR5, APRFILE[065], 8, 16, 16) },
    { GRDATA (UIPDR5, APRFILE[065], 8, 16, 0) },
    { GRDATA (UIPAR6, APRFILE[066], 8, 16, 16) },
    { GRDATA (UIPDR6, APRFILE[066], 8, 16, 0) },
    { GRDATA (UIPAR7, APRFILE[067], 8, 16, 16) },
    { GRDATA (UIPDR7, APRFILE[067], 8, 16, 0) },
    { GRDATA (UDPAR0, APRFILE[070], 8, 16, 16) },
    { GRDATA (UDPDR0, APRFILE[070], 8, 16, 0) },
    { GRDATA (UDPAR1, APRFILE[071], 8, 16, 16) },
    { GRDATA (UDPDR1, APRFILE[071], 8, 16, 0) },
    { GRDATA (UDPAR2, APRFILE[072], 8, 16, 16) },
    { GRDATA (UDPDR2, APRFILE[072], 8, 16, 0) },
    { GRDATA (UDPAR3, APRFILE[073], 8, 16, 16) },
    { GRDATA (UDPDR3, APRFILE[073], 8, 16, 0) },
    { GRDATA (UDPAR4, APRFILE[074], 8, 16, 16) },
    { GRDATA (UDPDR4, APRFILE[074], 8, 16, 0) },
    { GRDATA (UDPAR5, APRFILE[075], 8, 16, 16) },
    { GRDATA (UDPDR5, APRFILE[075], 8, 16, 0) },
    { GRDATA (UDPAR6, APRFILE[076], 8, 16, 16) },
    { GRDATA (UDPDR6, APRFILE[076], 8, 16, 0) },
    { GRDATA (UDPAR7, APRFILE[077], 8, 16, 16) },
    { GRDATA (UDPDR7, APRFILE[077], 8, 16, 0) },
    { BRDATAD (IREQ, int_req, 8, 32, IPL_HLVL, "interrupt pending flags, IPL 0 to 7"), REG_RO },
    { ORDATAD (TRAPS, trap_req, TRAP_V_MAX, "trap pending flags") },
    { FLDATAD (WAIT, wait_state, 0,         "wait state enable flag") },
    { ORDATAD (STOP_TRAPS, stop_trap, TRAP_V_MAX, "stop on trap flags") },
    { FLDATAD (STOP_VECA, stop_vecabort, 0, "stop on read abort in trap or interrupt") },
    { FLDATAD (STOP_SPA, stop_spabort, 0,   "stop on stack abort in trap or interrupt") },
    { FLDATA (AUTOCON, autcon_enb, 0), REG_HRO },
    { BRDATAD (PCQ, pcq, 8, 16, PCQ_SIZE, "PC prior to last jump, branch, or interrupt; Most recent PC change first"), REG_RO+REG_CIRC },
    { ORDATA (PCQP, pcq_p, 6), REG_HRO },
    { ORDATAD (WRU, sim_int_char, 8, "interrupt character") },
    { ORDATA (MODEL, cpu_model, 16), REG_HRO },
    { ORDATA (OPTIONS, cpu_opt, 32), REG_HRO },
    { NULL}
    };

MTAB cpu_mod[] = {
    { MTAB_XTD|MTAB_VDV, 0, "TYPE", NULL,
      NULL, &cpu_show_model, NULL, "Display current model features" },
#if !defined (UC15)
    { MTAB_XTD|MTAB_VDV, MOD_1103, NULL, "11/03", &cpu_set_model, NULL, NULL, "Set CPU type to 11/03" },
    { MTAB_XTD|MTAB_VDV, MOD_1104, NULL, "11/04", &cpu_set_model, NULL, NULL, "Set CPU type to 11/04" },
    { MTAB_XTD|MTAB_VDV, MOD_1105, NULL, "11/05", &cpu_set_model, NULL, NULL, "Set CPU type to 11/05" },
    { MTAB_XTD|MTAB_VDV, MOD_1120, NULL, "11/20", &cpu_set_model, NULL, NULL, "Set CPU type to 11/20" },
    { MTAB_XTD|MTAB_VDV, MOD_1123, NULL, "11/23", &cpu_set_model, NULL, NULL, "Set CPU type to 11/23" },
    { MTAB_XTD|MTAB_VDV, MOD_1123P, NULL, "11/23+", &cpu_set_model, NULL, NULL, "Set CPU type to 11/23+" },
    { MTAB_XTD|MTAB_VDV, MOD_1124, NULL, "11/24", &cpu_set_model, NULL, NULL, "Set CPU type to 11/24" },
    { MTAB_XTD|MTAB_VDV, MOD_1134, NULL, "11/34", &cpu_set_model, NULL, NULL, "Set CPU type to 11/34" },
    { MTAB_XTD|MTAB_VDV, MOD_1140, NULL, "11/40", &cpu_set_model, NULL, NULL, "Set CPU type to 11/40" },
    { MTAB_XTD|MTAB_VDV, MOD_1144, NULL, "11/44", &cpu_set_model, NULL, NULL, "Set CPU type to 11/44" },
    { MTAB_XTD|MTAB_VDV, MOD_1145, NULL, "11/45", &cpu_set_model, NULL, NULL, "Set CPU type to 11/45" },
    { MTAB_XTD|MTAB_VDV, MOD_1153, NULL, "11/53", &cpu_set_model, NULL, NULL, "Set CPU type to 11/53" },
    { MTAB_XTD|MTAB_VDV, MOD_1160, NULL, "11/60", &cpu_set_model, NULL, NULL, "Set CPU type to 11/60" },
    { MTAB_XTD|MTAB_VDV, MOD_1170, NULL, "11/70", &cpu_set_model, NULL, NULL, "Set CPU type to 11/70" },
    { MTAB_XTD|MTAB_VDV, MOD_1173, NULL, "11/73", &cpu_set_model, NULL, NULL, "Set CPU type to 11/73" },
    { MTAB_XTD|MTAB_VDV, MOD_1173B, NULL, "11/73B", &cpu_set_model, NULL, NULL, "Set CPU type to 11/73B" },
    { MTAB_XTD|MTAB_VDV, MOD_1183, NULL, "11/83", &cpu_set_model, NULL, NULL, "Set CPU type to 11/83" },
    { MTAB_XTD|MTAB_VDV, MOD_1184, NULL, "11/84", &cpu_set_model, NULL, NULL, "Set CPU type to 11/84" },
    { MTAB_XTD|MTAB_VDV, MOD_1193, NULL, "11/93", &cpu_set_model, NULL, NULL, "Set CPU type to 11/93" },
    { MTAB_XTD|MTAB_VDV, MOD_1194, NULL, "11/94", &cpu_set_model, NULL, NULL, "Set CPU type to 11/94" },
//    { MTAB_XTD|MTAB_VDV, MOD_1173, NULL, "Q22", &cpu_set_model, NULL, NULL, "deprecated: same as 11/73" },
//    { MTAB_XTD|MTAB_VDV, MOD_1184, NULL, "URH11", &cpu_set_model, NULL, NULL, "deprecated: same as 11/84" },
//    { MTAB_XTD|MTAB_VDV, MOD_1170, NULL, "URH70", &cpu_set_model, NULL, NULL, "deprecated: same as 11/70" },
    { MTAB_XTD|MTAB_VDV, MOD_1145, NULL, "U18", &cpu_set_model, NULL, NULL, "deprecated: same as 11/45" },
    { MTAB_XTD|MTAB_VDV, OPT_EIS, NULL, "EIS", &cpu_set_opt, NULL, NULL, "enable EIS instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_EIS, NULL, "NOEIS", &cpu_clr_opt, NULL, NULL, "disable EIS instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_FIS, NULL, "FIS", &cpu_set_opt, NULL, NULL, "enable FIS instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_FIS, NULL, "NOFIS", &cpu_clr_opt, NULL, NULL, "disable FIS instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_FPP, NULL, "FPP", &cpu_set_opt, NULL, NULL, "enable FPP instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_FPP, NULL, "NOFPP", &cpu_clr_opt, NULL, NULL, "disable FPP instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_CIS, NULL, "CIS", &cpu_set_opt, NULL, NULL, "enable CIS instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_CIS, NULL, "NOCIS", &cpu_clr_opt, NULL, NULL, "disable CIS instructions" },
    { MTAB_XTD|MTAB_VDV, OPT_MMU, NULL, "MMU", &cpu_set_opt, NULL, NULL, "enable MMU functionality" },
    { MTAB_XTD|MTAB_VDV, OPT_MMU, NULL, "NOMMU", &cpu_clr_opt, NULL, NULL, "disable MMU functionality" },
    { MTAB_XTD|MTAB_VDV, OPT_BVT, NULL, "BEVENT", &cpu_set_opt, NULL, NULL, "Enable BEVENT line (11/03, 11/23 only)" },
    { MTAB_XTD|MTAB_VDV, OPT_BVT, NULL, "NOBEVENT", &cpu_clr_opt, NULL, NULL, "Disable BEVENT line (11/03, 11/23 only)" },
    { UNIT_MSIZE, 8192, NULL, "8K", &cpu_set_size, NULL, NULL, "Set memory size to 8Kb"},
    { UNIT_MSIZE, 16384, NULL, "16K", &cpu_set_size, NULL, NULL, "Set memory size to 16Kb"},
    { UNIT_MSIZE, 24576, NULL, "24K", &cpu_set_size, NULL, NULL, "Set memory size to 24Kb"},
    { UNIT_MSIZE, 32768, NULL, "32K", &cpu_set_size, NULL, NULL, "Set memory size to 32Kb"},
    { UNIT_MSIZE, 40960, NULL, "40K", &cpu_set_size, NULL, NULL, "Set memory size to 40Kb"},
    { UNIT_MSIZE, 49152, NULL, "48K", &cpu_set_size, NULL, NULL, "Set memory size to 48Kb"},
    { UNIT_MSIZE, 57344, NULL, "56K", &cpu_set_size, NULL, NULL, "Set memory size to 56Kb"},
    { UNIT_MSIZE, 65536, NULL, "64K", &cpu_set_size, NULL, NULL, "Set memory size to 64Kb"},
    { UNIT_MSIZE, 98304, NULL, "96K", &cpu_set_size, NULL, NULL, "Set memory size to 96Kb"},
    { UNIT_MSIZE, 131072, NULL, "128K", &cpu_set_size, NULL, NULL, "Set memory size to 128Kb"},
    { UNIT_MSIZE, 196608, NULL, "192K", &cpu_set_size, NULL, NULL, "Set memory size to 192Kb"},
    { UNIT_MSIZE, 221184, NULL, "216K", &cpu_set_size, NULL, NULL, "Set memory size to 216Kb"},
    { UNIT_MSIZE, 262144, NULL, "256K", &cpu_set_size, NULL, NULL, "Set memory size to 256Kb"},
    { UNIT_MSIZE, 393216, NULL, "384K", &cpu_set_size, NULL, NULL, "Set memory size to 384Kb"},
    { UNIT_MSIZE, 524288, NULL, "512K", &cpu_set_size, NULL, NULL, "Set memory size to 512Kb"},
    { UNIT_MSIZE, 786432, NULL, "768K", &cpu_set_size, NULL, NULL, "Set memory size to 768Kb"},
    { UNIT_MSIZE, 1048576, NULL, "1024K", &cpu_set_size, NULL, NULL, "Set memory size to 1Mb"},
    { UNIT_MSIZE, 1572864, NULL, "1536K", &cpu_set_size, NULL, NULL, "Set memory size to 1536Kb"},
    { UNIT_MSIZE, 2097152, NULL, "2048K", &cpu_set_size, NULL, NULL, "Set memory size to 2Mb"},
    { UNIT_MSIZE, 3145728, NULL, "3072K", &cpu_set_size, NULL, NULL, "Set memory size to 3Mb"},
    { UNIT_MSIZE, 4186112, NULL, "4096K", &cpu_set_size, NULL, NULL, "Set memory size to 4Mb"},
    { UNIT_MSIZE, 1048576, NULL, "1M", &cpu_set_size, NULL, NULL, "Set memory size to 1Mb"},
    { UNIT_MSIZE, 2097152, NULL, "2M", &cpu_set_size, NULL, NULL, "Set memory size to 2Mb"},
    { UNIT_MSIZE, 3145728, NULL, "3M", &cpu_set_size, NULL, NULL, "Set memory size to 3Mb"},
    { UNIT_MSIZE, 4186112, NULL, "4M", &cpu_set_size, NULL, NULL, "Set memory size to 4Mb"},
    { MTAB_XTD|MTAB_VDV, 1, "AUTOCONFIG", "AUTOCONFIG",
      &set_autocon, &show_autocon, NULL, "Enable/Display Auto Configuration" },
    { MTAB_XTD|MTAB_VDV, 0, NULL, "NOAUTOCONFIG",
      &set_autocon, NULL, NULL, "Disable Auto Configuration" },
#else
    { UNIT_MSIZE, 16384, NULL, "16K", &cpu_set_size, NULL, NULL, "Set memory size to 16Kb"},
    { UNIT_MSIZE, 24576, NULL, "24K", &cpu_set_size, NULL, NULL, "Set memory size to 24Kb"},
#endif
    { MTAB_XTD|MTAB_VDV|MTAB_NMO, 0, "IOSPACE", NULL,
      NULL, &show_iospace, NULL, "Show I/O space address assignments" },
    { MTAB_XTD|MTAB_VDV, 0, "IDLE", "IDLE", &sim_set_idle, &sim_show_idle, NULL, "Enable/Display idle detection" },
    { MTAB_XTD|MTAB_VDV, 0, NULL, "NOIDLE", &sim_clr_idle, NULL, NULL, "Disable idle detection" },
    { MTAB_XTD|MTAB_VDV|MTAB_NMO|MTAB_SHP|MTAB_NC, 0, "HISTORY", "HISTORY=n",
      &cpu_set_hist, &cpu_show_hist, NULL, "Enable/Display instruction history" },
    { MTAB_XTD|MTAB_VDV|MTAB_NMO|MTAB_SHP, 0, "VIRTUAL", NULL,
      NULL, &cpu_show_virt, NULL, "Display address translation" },
    { 0 }
    };

BRKTYPTAB cpu_breakpoints [] = {
    BRKTYPE('E',"Execute Instruction at Virtual Address"),
    BRKTYPE('P',"Execute Instruction at Physical Address"),
    BRKTYPE('R',"Read from Virtual Address"),
    BRKTYPE('S',"Read from Physical Address"),
    BRKTYPE('W',"Write to Virtual Address"),
    BRKTYPE('X',"Write to Physical Address"),
    { 0 }
    };

DEVICE cpu_dev = {
    "CPU", &cpu_unit, cpu_reg, cpu_mod,
    1, 8, 22, 2, 8, 16,
    &cpu_ex, &cpu_dep, &cpu_reset,
    &cpu_boot, NULL, NULL,
    NULL, DEV_DYNM, 0,
    NULL, &cpu_set_size, NULL,
    &cpu_help, NULL, NULL, &cpu_description,
    cpu_breakpoints
    };

#ifdef USE_REALCONS
// extended cpu state for panel logic
// 1. state for all cpu's in scp.c
extern	t_addr realcons_memory_address_phys_register; // memory address
extern	t_addr realcons_memory_address_virt_register; // memory address
extern 	t_value realcons_memory_data_register; // memory data
extern 	int realcons_memory_write_access;
extern 	int realcons_console_halt; // 1: CPU halted by realcons console
  // 2. state extension for PDP11
  //    Conceptual problem:
  //    For panel logic PDP11 cpu signals are required, which depend on cpu model.
  //	(PDP11/40 DMUX, etc.)
  //	Initialize in cpu_reset()

int		realcons_bus_ID_mode; // 1 = DATA space access, 0 = instruction space access
t_value realcons_DATAPATH_shifter;  // value of shifter in PDP-11 processor data paths
t_value realcons_IR; // buffer for instruction register (opcode)
t_value realcons_PSW; // buffer for program status word

								   // Pointers to event handlers
								   // Events are called in SimH-code as pointers to functions in panel logic
extern console_controller_event_func_t	realcons_event_operator_halt; // scp.c, needed here
extern console_controller_event_func_t	realcons_event_step_halt; // scp.c, needed here
extern console_controller_event_func_t	realcons_event_cpu_reset;
console_controller_event_func_t	realcons_event_opcode_any; // triggered after any opcode execution
console_controller_event_func_t realcons_event_opcode_halt;
console_controller_event_func_t	realcons_event_opcode_reset; // triggered after execution of RESET
console_controller_event_func_t	realcons_event_opcode_wait; // triggered after execution of WAIT



  /*** register memory accesses
   * SimH codes access memory in many code paths,
   * for each virtual, physical address, data value etc must be registered.
   * Because of speed, macros instead of functions are used.
   * To make code more reliable, telling names for diffrenet situtaions are used.
   */

   // R/W access, virtual and physical address given. va can be invalid
//#define REALCONS_CPU_PDP11_MEMACCESS_INTERN(realcons,va,pa,data_expr,write)	 (data_expr)
//#define RETURN_REALCONS_CPU_PDP11_MEMACCESS_INTERN(realcons,va,pa,data_expr,write)	 return (data_expr)


#define REALCONS_CPU_PDP11_MEMACCESS_INTERN(realcons,va,pa,data_expr,write)	 do { \
 				  realcons_bus_ID_mode = ((va) & 0x10000)? 1 : 0 ; \
				  realcons_memory_address_phys_register = (pa) ; \
                  if ((va) != 0xffffffff) /* only pa given ?) */ \
				    realcons_memory_address_virt_register = (va) & 0xffff ; \
				  realcons_memory_data_register = (data_expr) ; \
				  realcons_memory_write_access = (write) ; \
/*printf("%s M[va=%o, pa=%o] = %o, line #%d\n", realcons_memory_write_access?"WRITE":"READ", realcons_memory_address_virt_register, realcons_memory_address_phys_register, realcons_memory_data_register, __LINE__) ;/**/ \
			  } while(0)

      //dto, terminate current procedure with data_expr as result
#define RETURN_REALCONS_CPU_PDP11_MEMACCESS_INTERN(realcons,va,pa,data_expr,write)	 do { \
				  realcons_bus_ID_mode = ((va) & 0x10000)? 1 : 0 ; \
				  realcons_memory_address_phys_register = (pa) ; \
                  if ((va) != 0xffffffff) /* only pa given ?) */ \
				    realcons_memory_address_virt_register = (va) & 0xffff ; \
				  realcons_memory_write_access = (write) ; \
				  realcons_memory_data_register = (data_expr) ; \
/*printf("RETURN %s M[va=%o, pa=%o] = %o, line #%d\n", realcons_memory_write_access?"WRITE":"READ", realcons_memory_address_virt_register, realcons_memory_address_phys_register, realcons_memory_data_register, __LINE__) ;/**/ \
  				  return realcons_memory_data_register ; /* eval data_expr only once!!*/ \
			  } while(0)

/*** tailored to situations ***/
// Read access, only physical address given
#define REALCONS_CPU_PDP11_MEMACCESS_PA_READ(realcons,pa,data_expr)\
	REALCONS_CPU_PDP11_MEMACCESS_INTERN((realcons),0xffffffff,(pa),(data_expr),FALSE)
// Read access physical address, terminate current procedure with data_expr as result
#define RETURN_REALCONS_CPU_PDP11_MEMACCESS_PA_READ(realcons,pa,data_expr)\
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_INTERN((realcons),0xffffffff,(pa),(data_expr),FALSE)
// READ access virtual and physical
#define REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(realcons,va,pa,data_expr)	\
	REALCONS_CPU_PDP11_MEMACCESS_INTERN((realcons),(va),(pa),(data_expr),FALSE)
// READ access, terminate current procedure with data_expr as result
#define RETURN_REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(realcons,va,pa,data_expr)	\
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_INTERN((realcons),(va),(pa),(data_expr),FALSE)
// WRITE access, only physical address given
#define REALCONS_CPU_PDP11_MEMACCESS_PA_WRITE(realcons,pa,data_expr)\
		REALCONS_CPU_PDP11_MEMACCESS_INTERN((realcons),0xffffffff,(pa),(data_expr),TRUE)
// WRITE access virtual and physical
#define REALCONS_CPU_PDP11_MEMACCESS_VA_PA_WRITE(realcons,va,pa,data_expr)	\
		REALCONS_CPU_PDP11_MEMACCESS_INTERN((realcons),(va),(pa),(data_expr),TRUE)


/* relocate virtual addresses.
 * like relocR(), but without change of CPU state
 * uses relocC()
 *
 * evaluate extended virtual address 'va'.
 * pa = physical address
 * bit 18,17 = cm = mode: MD_SUP, MD_KER,MD_USR,MD_UND, see calc_is()
 * bit 16 = I/D space flag, see calc_ds(). 1 = data, 0 = instruction
 */
int32 realcons_reloc(int32 va) {
    int32  sw;
    int32 relocC(int32 va, int32 sw) ;

    // input to reloC(): switches from "sim>exam" cmd as bitmask
    sw = SWMASK('V') ; // virtual
    switch (va & 0x60000) { // decode mode from bits 18:17
    case 0: /*mode = 0: Kernel*/
        sw |= SWMASK('K');
        break;
    case 0x020000: /*mode = 1: Super*/
        sw |= SWMASK('S') ;
        break;
    case 0x060000: /*mode = 3 User */
        sw |= SWMASK('U') ;
        break;
    }

    if (va & 0x10000) // decode D-space from bit 16
        sw |= SWMASK('D') ;

    return relocC(va, sw); // now these switches are reverse decoded
}
#endif

t_value pdp11_pc_value (void)
{
return (t_value)PC;
}

t_stat sim_instr (void)
{
int abortval, i;
volatile int32 trapea;                                  /* used by setjmp */
InstHistory *hst_ent = NULL;

sim_vm_pc_value = &pdp11_pc_value;

/* Restore register state

        1. PSW components
        2. Active register file based on PSW<rs>
        3. Active stack pointer based on PSW<cm>
        4. Memory management control flags
        5. Interrupt system
*/

reason = build_dib_tab ();                              /* build, chk dib_tab */
if (reason != SCPE_OK)
    return reason;
if (MEMSIZE >= (cpu_tab[cpu_model].maxm - IOPAGESIZE))  /* mem size >= max - io page? */
    MEMSIZE = cpu_tab[cpu_model].maxm - IOPAGESIZE;     /* max - io page */
cpu_type = 1u << cpu_model;                             /* reset type mask */
cpu_bme = (MMR3 & MMR3_BME) && (cpu_opt & OPT_UBM);     /* map enabled? */
PC = saved_PC;
put_PSW (PSW, 0);                                       /* set PSW, call calc_xs */
for (i = 0; i < 6; i++)
    R[i] = REGFILE[i][rs];
SP = STACKFILE[cm];
isenable = calc_is (cm);
dsenable = calc_ds (cm);
put_PIRQ (PIRQ);                                        /* rewrite PIRQ */
STKLIM = STKLIM & STKLIM_RW;                            /* clean up STKLIM */
MMR0 = MMR0 & ~MMR0_IC;                                 /* usually off */

trap_req = calc_ints (ipl, trap_req);                   /* upd int req */
trapea = 0;
reason = 0;

/* Abort handling

   If an abort occurs in memory management or memory access, the lower
   level routine executes a longjmp to this area OUTSIDE the main
   simulation loop.  The longjmp specifies a trap mask which is OR'd
   into the trap_req register.  Simulation then resumes at the fetch
   phase, and the trap is sprung.

   Aborts which occur within a trap sequence (trapea != 0) require
   special handling.  If the abort occured on the stack pushes, and
   the mode (encoded in trapea) is kernel, an "emergency" kernel
   stack is created at 4, and a red zone stack trap taken.

   All variables used in setjmp processing, or assumed to be valid
   after setjmp, must be volatile or global.
*/

abortval = setjmp (save_env);                           /* set abort hdlr */
if (abortval == ABRT_BKPT) {
    /* Breakpoint encountered.  */
    reason = STOP_IBKPT;
    /* Print a message reporting the type and address if it is not a 
       plain virtual PC (instruction execution) breakpoint.  */
    if (sim_brk_match_type != BPT_PCVIR)
        sim_messagef (reason, "\r\n%s", sim_brk_message());
    /* Restore the PC and sim_interval. */
    PC = inst_pc;
    sim_interval = saved_sim_interval;
    /* Restore PSW and the broken-out condition code values, provided
       FPD is not currently set.  If it is, that means the instruction
       is interruptible and breakpoints are treated as continuation
       rather than replay.  */
    if (!fpd) {
        PSW = inst_psw;
        put_PSW (inst_psw, 0);
        }
    /* Undo register changes. */
    while (reg_mods) {
        int rnum = reg_mods & 7;
        int delta = (reg_mods >> 3) & 037;
        reg_mods >>= 8;
        if (delta & 020)                                /* negative delta */
            delta = -(-delta & 037);                    /* get signed value */
        if (rnum != 7)
            R[rnum] -= delta;
        }
    }
else {
    if (abortval != 0) {
        trap_req = trap_req | abortval;                 /* or in trap flag */
        if ((trapea > 0) && stop_vecabort)
            reason = STOP_VECABORT;
        if ((trapea < 0) &&                             /* stack push abort? */
            (CPUT (STOP_STKA) || stop_spabort))
            reason = STOP_SPABORT;
        if (trapea == ~MD_KER) {                        /* kernel stk abort? */
            trap_req = trap_req & ~trap_clear[TRAP_RED];/* clear all traps */
            setTRAP (TRAP_RED);                         /* set red stack trap */
            setCPUERR (CPUE_RED);
            STACKFILE[MD_KER] = 4;
            if (cm == MD_KER)
                SP = 4;
            }
        }
    }

/* Main instruction fetch/decode loop

   Check for traps or interrupts.  If trap, locate the vector and check
   for stop condition.  If interrupt, locate the vector.
*/ 

while (reason == 0)  {

    int32 IR, srcspec, srcreg, dstspec, dstreg;
    int32 src, src2, dst, ea;
    int32 i, t, sign, oldrs, trapnum;

#ifdef USE_REALCONS
    realcons_service(cpu_realcons, 1); // high speed call
#endif

    if (cpu_astop) {
        cpu_astop = 0;
        reason = SCPE_STOP;
        break;
        }

    AIO_CHECK_EVENT;
    if (sim_interval <= 0) {                            /* intv cnt expired? */
        /* Make sure all intermediate state is visible in simh registers */
        PSW = get_PSW ();
        for (i = 0; i < 6; i++)
            REGFILE[i][rs] = R[i];
        STACKFILE[cm] = SP;
        saved_PC = PC & 0177777;
        pcq_r->qptr = pcq_p;                            /* update pc q ptr */
        set_r_display (rs, cm);

        reason = sim_process_event ();                  /* process events */

        /* restore simh register contents into running variables */
        PC = saved_PC;
        put_PSW (PSW, 0);                               /* set PSW, call calc_xs */
        for (i = 0; i < 6; i++)
            R[i] = REGFILE[i][rs];
        SP = STACKFILE[cm];
        isenable = calc_is (cm);
        dsenable = calc_ds (cm);
        put_PIRQ (PIRQ);                                /* rewrite PIRQ */
        STKLIM = STKLIM & STKLIM_RW;                    /* clean up STKLIM */
        MMR0 = MMR0 | MMR0_IC;                          /* usually on */

        trap_req = calc_ints (ipl, trap_req);           /* recalc int req */
        continue;
        }                                               /* end if sim_interval */

    if (trap_req) {                                     /* check traps, ints */
        trapea = 0;                                     /* assume srch fails */
        if ((t = trap_req & TRAP_ALL)) {                /* if a trap */
            for (trapnum = 0; trapnum < TRAP_V_MAX; trapnum++) {
                if ((t >> trapnum) & 1) {               /* trap set? */
                    trapea = trap_vec[trapnum];         /* get vec, clr */
                    trap_req = trap_req & ~trap_clear[trapnum];
                    if ((stop_trap >> trapnum) & 1)     /* stop on trap? */
                        reason = trapnum + 1;
                    break;
                    }                                   /* end if t & 1 */
                }                                       /* end for */
            }                                           /* end if t */
        else {
            trapea = get_vector (ipl);                  /* get int vector */
            trapnum = TRAP_V_MAX;                       /* defang stk trap */
            }                                           /* end else t */
        if (trapea == 0) {                              /* nothing to do? */
            trap_req = calc_ints (ipl, 0);              /* recalculate */
            continue;                                   /* back to fetch */
            }                                           /* end if trapea */

/* Process a trap or interrupt

   1. Exit wait state
   2. Save the current SP and PSW
   3. Read the new PC, new PSW from trapea, kernel data space
   4. Get the mode and stack selected by the new PSW
   5. Push the old PC and PSW on the new stack
   6. Update SP, PSW, and PC
   7. If not stack overflow, check for stack overflow

   If the MMU registers are not frozen, the 11/45 and 11/70 will
   also clear MMR1 and store the trap vector in MMR2, <except>
   for the four instruction traps (EMT, TRAP, IOT, BPT).

   If the reads in step 3, or the writes in step 5, match a data breakpoint,
   the breakpoint status will be set but the interrupt actions will continue.
   The breakpoint stop will occur at the beginning of the next instruction 
   cycle.
*/

        wait_state = 0;                                 /* exit wait state */
        STACKFILE[cm] = SP;
        PSW = get_PSW ();                               /* assemble PSW */
        oldrs = rs;
        if ((CPUT (HAS_MMTR)) && (update_MM)) {         /* 45,70, not frozen? */
            MMR1 = 0;                                   /* clear MMR1 */
            if (trap_load_mmr2[trapnum])                /* load MMR2? */
                MMR2 = trapea;                          /* save vector */
            }
        src = ReadCW (trapea | calc_ds (MD_KER));       /* new PC */
        src2 = ReadCW ((trapea + 2) | calc_ds (MD_KER));/* new PSW */
        t = (src2 >> PSW_V_CM) & 03;                    /* new cm */
        trapea = ~t;                                    /* flag pushes */
        WriteCW (PSW, ((STACKFILE[t] - 2) & 0177777) | calc_ds (t));
        WriteCW (PC, ((STACKFILE[t] - 4) & 0177777) | calc_ds (t));
        trapea = 0;                                     /* clear trap flag */
        src2 = (src2 & ~PSW_PM) | (cm << PSW_V_PM);     /* insert prv mode */
        put_PSW (src2, 0);                              /* call calc_is,ds */
        if (rs != oldrs) {                              /* if rs chg, swap */
            for (i = 0; i < 6; i++) {
                REGFILE[i][oldrs] = R[i];
                R[i] = REGFILE[i][rs];
                }
            }
        SP = (STACKFILE[cm] - 4) & 0177777;             /* update SP, PC */
        isenable = calc_is (cm);
        dsenable = calc_ds (cm);
        trap_req = calc_ints (ipl, trap_req);
        JMP_PC (src);
        if ((cm == MD_KER) && (SP < (STKLIM + STKL_Y)) &&
            (trapnum != TRAP_V_RED) && (trapnum != TRAP_V_YEL))
            set_stack_trap (SP);
        continue;                                       /* end if traps */
        }

/* Fetch and decode next instruction */

    if (tbit)
        setTRAP (TRAP_TRC);
    if (wait_state) {                                   /* wait state? */
        sim_idle (TMR_CLK, TRUE);
        continue;
        }

    reg_mods = 0;
    inst_pc = PC;
    /* Save PSW also because condition codes need to be preserved.  We
       just save the whole PSW because that is sufficient.  If
       restoring is needed, both the PSW and the components that need
       to be restored are handled explicitly.  */
    inst_psw = get_PSW ();
    saved_sim_interval = sim_interval;
    if (BPT_SUMM_PC) {                                  /* possible breakpoint */
        t_addr pa = relocC (PC, 0);                     /* relocate PC */
        if (sim_brk_test (PC, BPT_PCVIR) ||             /* Normal PC breakpoint? */
            sim_brk_test (pa, BPT_PCPHY))               /* Physical Address breakpoint? */
            ABORT (ABRT_BKPT);                          /* stop simulation */
        }

    if (update_MM) {                                    /* if mm not frozen */
        MMR1 = 0;
        MMR2 = PC;
        }
    IR = ReadE (PC | isenable);                         /* fetch instruction */
    sim_interval = sim_interval - 1;
    srcspec = (IR >> 6) & 077;                          /* src, dst specs */
    dstspec = IR & 077;
    srcreg = (srcspec <= 07);                           /* src, dst = rmode? */
    dstreg = (dstspec <= 07);
    if (hst_lnt) {                                      /* record history? */
        t_value val;
        uint32 i;
        static int32 swmap[4] = {
            SWMASK ('K') | SWMASK ('V'), SWMASK ('S') | SWMASK ('V'),
            SWMASK ('U') | SWMASK ('V'), SWMASK ('U') | SWMASK ('V')
            };
        hst_ent = &hst[hst_p];
        hst_ent->pc = PC | HIST_VLD;
        hst_ent->sp = SP;
        hst_ent->psw = get_PSW ();
        hst_ent->src = 0;
        hst_ent->dst = 0;
        hst_ent->inst[0] = IR;
        for (i = 1; i < HIST_ILNT; i++) {
            if (cpu_ex (&val, (PC + (i << 1)) & 0177777, &cpu_unit, swmap[cm & 03]))
                hst_ent->inst[i] = 0;
            else hst_ent->inst[i] = (uint16) val;
            }
        hst_p = (hst_p + 1);
        if (hst_p >= hst_lnt)
            hst_p = 0;
        }
    PC = (PC + 2) & 0177777;                            /* incr PC, mod 65k */
#ifdef USE_REALCONS
    saved_PC = PC ; // saved_PC used in panel
#endif
    switch ((IR >> 12) & 017) {                         /* decode IR<15:12> */

/* Opcode 0: no operands, specials, branches, JSR, SOPs */

    case 000:
        switch ((IR >> 6) & 077) {                      /* decode IR<11:6> */
        case 000:                                       /* no operand */
            if (IR >= 000010) {                         /* 000010 - 000077 */
                setTRAP (TRAP_ILL);                     /* illegal */
                break;
                }
            switch (IR) {                               /* decode IR<2:0> */
            case 0:                                     /* HALT */
                if ((cm == MD_KER) &&
                    (!CPUT (CPUT_J) || ((MAINT & MAINT_HTRAP) == 0)))
                    reason = STOP_HALT;
                else if (CPUT (HAS_HALT4)) {            /* priv trap? */
                    setTRAP (TRAP_PRV);
                    setCPUERR (CPUE_HALT);
                    }
                else setTRAP (TRAP_ILL);                /* no, ill inst */
                break;
            case 1:                                     /* WAIT */
                wait_state = 1;
#if USE_REALCONS
					REALCONS_EVENT(cpu_realcons, realcons_event_opcode_wait);
#endif
                break;
            case 3:                                     /* BPT */
                setTRAP (TRAP_BPT);
                break;
            case 4:                                     /* IOT */
                setTRAP (TRAP_IOT);
                break;
            case 5:                                     /* RESET */
                if (cm == MD_KER) {
                    reset_all (2);                      /* skip CPU, sys reg */
                    PIRQ = 0;                           /* clear PIRQ */
                    STKLIM = 0;                         /* clear STKLIM */
                    MMR0 = 0;                           /* clear MMR0 */
                    MMR3 = 0;                           /* clear MMR3 */
                    cpu_bme = 0;                        /* (also clear bme) */
                    for (i = 0; i < IPL_HLVL; i++)
                        int_req[i] = 0;
                    trap_req = trap_req & ~TRAP_INT;
                    dsenable = calc_ds (cm);
#if USE_REALCONS
						REALCONS_EVENT(cpu_realcons, realcons_event_opcode_reset);
						// The realcons event handler must handle the RESET-delay (70ms for 11/40, 10ms for 11/70, etc)
#endif
                    }
                break;
            case 6:                                     /* RTT */
                if (!CPUT (HAS_RTT)) {
                    setTRAP (TRAP_ILL);
                    break;
                    }
            case 2:                                     /* RTI */
                src = ReadW (SP | dsenable);
                src2 = ReadW (((SP + 2) & 0177777) | dsenable);
                STACKFILE[cm] = SP = (SP + 4) & 0177777;
                oldrs = rs;
                put_PSW (src2, (cm != MD_KER));         /* store PSW, prot */
                if (rs != oldrs) {
                    for (i = 0; i < 6; i++) {
                        REGFILE[i][oldrs] = R[i];
                        R[i] = REGFILE[i][rs];
                        }
                    }
                SP = STACKFILE[cm];
                isenable = calc_is (cm);
                dsenable = calc_ds (cm);
                trap_req = calc_ints (ipl, trap_req);
                JMP_PC (src);
                if (CPUT (HAS_RTT) && tbit &&           /* RTT impl? */
                    (IR == 000002))
                    setTRAP (TRAP_TRC);                 /* RTI immed trap */
                break;
            case 7:                                     /* MFPT */
                if (CPUT (HAS_MFPT))                    /* implemented? */
                    R[0] = cpu_tab[cpu_model].mfpt;     /* get type */
                else setTRAP (TRAP_ILL);
                break;
                }                                       /* end switch no ops */
            break;                                      /* end case no ops */

        case 001:                                       /* JMP */
            if (dstreg)
                setTRAP (CPUT (HAS_JREG4)? TRAP_PRV: TRAP_ILL);
            else {
                dst = GeteaW (dstspec) & 0177777;       /* get eff addr */
                if (CPUT (CPUT_05|CPUT_20) &&           /* 11/05, 11/20 */
                    ((dstspec & 070) == 020))           /* JMP (R)+? */
                    dst = R[dstspec & 07];              /* use post incr */
                if (hst_ent)
                    hst_ent->dst = dst;
                JMP_PC (dst);
                }
            break;                                      /* end JMP */

        case 002:                                       /* RTS et al*/
            if (IR < 000210) {                          /* RTS */
                dstspec = dstspec & 07;
                if (hst_ent)
                    hst_ent->dst = R[dstspec];
                JMP_PC (R[dstspec]);
                R[dstspec] = ReadW (SP | dsenable);
                if (dstspec != 6)
                    SP = (SP + 2) & 0177777;
                break;
                }                                       /* end if RTS */
            if (IR < 000230) {
                setTRAP (TRAP_ILL);
                break;
                }
            if (IR < 000240) {                          /* SPL */
                if (CPUT (HAS_SPL)) {
                    if (cm == MD_KER)
                        ipl = IR & 07;
                    trap_req = calc_ints (ipl, trap_req);
                    }
                else setTRAP (TRAP_ILL);
                break;
                }                                       /* end if SPL */
            if (IR < 000260) {                          /* clear CC */
                if (IR & 010)
                    N = 0;
                if (IR & 004)
                    Z = 0;
                if (IR & 002)
                    V = 0;
                if (IR & 001)
                    C = 0;
                break;
                }                                       /* end if clear CCs */
            if (IR & 010)                               /* set CC */
                N = 1;
            if (IR & 004)
                Z = 1;
            if (IR & 002)
                V = 1;
            if (IR & 001)
                C = 1;
            break;                                      /* end case RTS et al */

        case 003:                                       /* SWAB */
            dst = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = ((dst & 0377) << 8) | ((dst >> 8) & 0377);
            N = GET_SIGN_B (dst & 0377);
            Z = GET_Z (dst & 0377);
            if (!CPUT (CPUT_20))
                V = 0;
            C = 0;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;                                      /* end SWAB */

        case 004: case 005:                             /* BR */
            BRANCH_F (IR);
            break;

        case 006: case 007:                             /* BR */
            BRANCH_B (IR);
            break;

        case 010: case 011:                             /* BNE */
            if (Z == 0) {
                BRANCH_F (IR);
                } 
            break;

        case 012: case 013:                             /* BNE */
            if (Z == 0) {
                BRANCH_B (IR);
                }
            break;

        case 014: case 015:                             /* BEQ */
            if (Z) {
                BRANCH_F (IR);
                } 
            break;

        case 016: case 017:                             /* BEQ */
            if (Z) {
                BRANCH_B (IR);
                }
            break;

        case 020: case 021:                             /* BGE */
            if ((N ^ V) == 0) {
                BRANCH_F (IR);
                } 
            break;

        case 022: case 023:                             /* BGE */
            if ((N ^ V) == 0) {
                BRANCH_B (IR);
                }
            break;

        case 024: case 025:                             /* BLT */
            if (N ^ V) {
                BRANCH_F (IR);
                }
            break;

        case 026: case 027:                             /* BLT */
            if (N ^ V) {
                BRANCH_B (IR);
                }
            break;

        case 030: case 031:                             /* BGT */
            if ((Z | (N ^ V)) == 0) {
                BRANCH_F (IR);
                } 
            break;

        case 032: case 033:                             /* BGT */
            if ((Z | (N ^ V)) == 0) { BRANCH_B (IR); }
            break;

        case 034: case 035:                             /* BLE */
            if (Z | (N ^ V)) {
                BRANCH_F (IR);
                } 
            break;

        case 036: case 037:                             /* BLE */
            if (Z | (N ^ V)) {
                BRANCH_B (IR);
                }
            break;

        case 040: case 041: case 042: case 043:         /* JSR */
        case 044: case 045: case 046: case 047:
            if (dstreg)
                setTRAP (CPUT (HAS_JREG4)? TRAP_PRV: TRAP_ILL);
            else {
                srcspec = srcspec & 07;
                dst = GeteaW (dstspec);
                if (CPUT (CPUT_05|CPUT_20) &&           /* 11/05, 11/20 */
                    ((dstspec & 070) == 020))           /* JSR (R)+? */
                    dst = R[dstspec & 07];              /* use post incr */
                SP = (SP - 2) & 0177777;
                reg_mods = calc_MMR1 (0366);
                if (update_MM)
                    MMR1 = reg_mods;
                if ((cm == MD_KER) && (SP < (STKLIM + STKL_Y)))
                    set_stack_trap (SP);
                WriteW (R[srcspec], SP | dsenable);
                R[srcspec] = PC;
                if (hst_ent)
                    hst_ent->dst = dst;
                JMP_PC (dst & 0177777);
                }
            break;                                      /* end JSR */

        case 050:                                       /* CLR */
            N = V = C = 0;
            Z = 1;
            if (hst_ent)
                hst_ent->dst = 0;
            if (dstreg)
                R[dstspec] = 0;
            else WriteW (0, GeteaW (dstspec));
            break;

        case 051:                                       /* COM */
            dst = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = dst ^ 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            V = 0;
            C = 1;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 052:                                       /* INC */
            dst = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (dst + 1) & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            V = (dst == 0100000);
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 053:                                       /* DEC */
            dst = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (dst - 1) & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            V = (dst == 077777);
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 054:                                       /* NEG */
            dst = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (-dst) & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            V = (dst == 0100000);
            C = Z ^ 1;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 055:                                       /* ADC */
            dst = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (dst + C) & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            V = (C && (dst == 0100000));
            C = C & Z;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 056:                                       /* SBC */
            dst = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (dst - C) & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            V = (C && (dst == 077777));
            C = (C && (dst == 0177777));
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 057:                                       /* TST */
            dst = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
            if (hst_ent)
                hst_ent->dst = dst;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            V = C = 0;
            break;

        case 060:                                       /* ROR */
            src = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (src >> 1) | (C << 15);
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            C = (src & 1);
            V = N ^ C;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 061:                                       /* ROL */
            src = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = ((src << 1) | C) & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            C = GET_SIGN_W (src);
            V = N ^ C;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 062:                                       /* ASR */
            src = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (src >> 1) | (src & 0100000);
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            C = (src & 1);
            V = N ^ C;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

        case 063:                                       /* ASL */
            src = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            dst = (src << 1) & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            C = GET_SIGN_W (src);
            V = N ^ C;
            if (hst_ent)
                hst_ent->dst = dst;
            if (dstreg)
                R[dstspec] = dst;
            else PWriteW (dst, last_pa);
            break;

/* Notes:
   - MxPI must mask GeteaW returned address to force ispace
   - MxPI must set MMR1 for SP recovery in case of fault
*/

        case 064:                                       /* MARK */
            if (CPUT (HAS_MARK)) {
                i = (PC + dstspec + dstspec) & 0177777;
                JMP_PC (R[5]);
                R[5] = ReadW (i | dsenable);
                SP = (i + 2) & 0177777;
                }
            else setTRAP (TRAP_ILL);
            break;

        case 065:                                       /* MFPI */
            if (CPUT (HAS_MXPY)) {
                if (dstreg) {
                    if ((dstspec == 6) && (cm != pm))
                        dst = STACKFILE[pm];
                    else dst = R[dstspec];
                    }
                else {
                    i = ((cm == pm) && (cm == MD_USR))? (int32)calc_ds (pm): (int32)calc_is (pm);
                    dst = ReadW ((GeteaW (dstspec) & 0177777) | i);
                    }
                N = GET_SIGN_W (dst);
                Z = GET_Z (dst);
                V = 0;
                SP = (SP - 2) & 0177777;
                reg_mods = calc_MMR1 (0366);
                if (update_MM)
                    MMR1 = reg_mods;
                if (hst_ent)
                    hst_ent->dst = dst;
                if ((cm == MD_KER) && (SP < (STKLIM + STKL_Y)))
                    set_stack_trap (SP);
                WriteW (dst, SP | dsenable);
                }
            else setTRAP (TRAP_ILL);
            break;

        case 066:                                       /* MTPI */
            if (CPUT (HAS_MXPY)) {
                dst = ReadW (SP | dsenable);
                N = GET_SIGN_W (dst);
                Z = GET_Z (dst);
                V = 0;
                SP = (SP + 2) & 0177777;
                reg_mods = 026;
                if (update_MM) MMR1 = reg_mods;
                if (hst_ent)
                    hst_ent->dst = dst;
                if (dstreg) {
                    if ((dstspec == 6) && (cm != pm))
                        STACKFILE[pm] = dst;
                    else R[dstspec] = dst;
                    }
                else WriteW (dst, (GeteaW (dstspec) & 0177777) | calc_is (pm));
                }
            else setTRAP (TRAP_ILL);
            break;

        case 067:                                       /* SXT */
            if (CPUT (HAS_SXS)) {
                dst = N? 0177777: 0;
                Z = N ^ 1;
                V = 0;
                if (hst_ent)
                    hst_ent->dst = dst;
                if (dstreg)
                    R[dstspec] = dst;
                else WriteW (dst, GeteaW (dstspec));
                }
            else setTRAP (TRAP_ILL);
            break;

        case 070:                                       /* CSM */
            if (CPUT (HAS_CSM) && (MMR3 & MMR3_CSM) && (cm != MD_KER)) {
                dst = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
                PSW = get_PSW () & ~PSW_CC;             /* PSW, cc = 0 */
                STACKFILE[cm] = SP;
                WriteW (PSW, ((SP - 2) & 0177777) | calc_ds (MD_SUP));
                WriteW (PC, ((SP - 4) & 0177777) | calc_ds (MD_SUP));
                WriteW (dst, ((SP - 6) & 0177777) | calc_ds (MD_SUP));
                SP = (SP - 6) & 0177777;
                pm = cm;
                cm = MD_SUP;
                tbit = 0;
                isenable = calc_is (cm);
                dsenable = calc_ds (cm);
                PC = ReadW (010 | isenable);
                }
            else setTRAP (TRAP_ILL);
            break;

        case 072:                                       /* TSTSET */
            if (CPUT (HAS_TSWLK) && !dstreg) {
                dst = ReadMW (GeteaW (dstspec));
                N = GET_SIGN_W (dst);
                Z = GET_Z (dst);
                V = 0;
                C = (dst & 1);
                R[0] = dst;                             /* R[0] <- dst */
                if (hst_ent)
                    hst_ent->dst = dst | 1;
                PWriteW (R[0] | 1, last_pa);            /* dst <- R[0] | 1 */
                }
            else setTRAP (TRAP_ILL);
            break;

        case 073:                                       /* WRTLCK */
            if (CPUT (HAS_TSWLK) && !dstreg) {
                N = GET_SIGN_W (R[0]);
                Z = GET_Z (R[0]);
                V = 0;
                WriteW (R[0], GeteaW (dstspec));
                if (hst_ent)
                    hst_ent->dst = R[0];
                }
            else setTRAP (TRAP_ILL);
            break;

        default:
            setTRAP (TRAP_ILL);
            break;
            }                                           /* end switch SOPs */
        break;                                          /* end case 000 */

/* Opcodes 01 - 06: double operand word instructions

   J-11 (and F-11) optimize away register source operand decoding.
   As a result, dop R,+/-(R) use the modified version of R as source.
   Most (but not all) other PDP-11's fetch the source operand before
   any destination operand decoding.

   Add: v = [sign (src) = sign (src2)] and [sign (src) != sign (result)]
   Cmp: v = [sign (src) != sign (src2)] and [sign (src2) = sign (result)]
*/

    case 001:                                           /* MOV */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            ea = GeteaW (dstspec);
            dst = R[srcspec];
            }
        else {
            dst = srcreg? R[srcspec]: ReadW (GeteaW (srcspec));
            if (!dstreg)
                ea = GeteaW (dstspec);
            }
        N = GET_SIGN_W (dst);
        Z = GET_Z (dst);
        V = 0;
        if (hst_ent) {
            hst_ent->src = dst;
            hst_ent->dst = dst;
            }
        if (dstreg)
            R[dstspec] = dst;
        else WriteW (dst, ea);
        break;

    case 002:                                           /* CMP */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadW (GeteaW (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadW (GeteaW (srcspec));
            src2 = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
            }
        dst = (src - src2) & 0177777;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = src2;
            }
        N = GET_SIGN_W (dst);
        Z = GET_Z (dst);
        V = GET_SIGN_W ((src ^ src2) & (~src2 ^ dst));
        C = (src < src2);
        break;

    case 003:                                           /* BIT */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadW (GeteaW (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadW (GeteaW (srcspec));
            src2 = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
            }
        dst = src2 & src;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_W (dst);
        Z = GET_Z (dst);
        V = 0;
        break;

    case 004:                                           /* BIC */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadMW (GeteaW (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadW (GeteaW (srcspec));
            src2 = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            }
        dst = src2 & ~src;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_W (dst);
        Z = GET_Z (dst);
        V = 0;
        if (dstreg)
            R[dstspec] = dst;
        else PWriteW (dst, last_pa);
        break;

    case 005:                                           /* BIS */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadMW (GeteaW (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadW (GeteaW (srcspec));
            src2 = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            }
        dst = src2 | src;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_W (dst);
        Z = GET_Z (dst);
        V = 0;
        if (dstreg)
            R[dstspec] = dst;
        else PWriteW (dst, last_pa);
        break;

    case 006:                                           /* ADD */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadMW (GeteaW (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadW (GeteaW (srcspec));
            src2 = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            }
        dst = (src2 + src) & 0177777;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_W (dst);
        Z = GET_Z (dst);
        V = GET_SIGN_W ((~src ^ src2) & (src ^ dst));
        C = (dst < src);
        if (dstreg)
            R[dstspec] = dst;
        else PWriteW (dst, last_pa);
        break;

/* Opcode 07: EIS, FIS, CIS

   Notes:
   - The code assumes that the host int length is at least 32 bits.
   - MUL carry: C is set if the (signed) result doesn't fit in 16 bits.
   - Divide has three error cases:
        1. Divide by zero.
        2. Divide largest negative number by -1.
        3. (Signed) quotient doesn't fit in 16 bits.
     Cases 1 and 2 must be tested in advance, to avoid C runtime errors.
   - ASHx left: overflow if the bits shifted out do not equal the sign
     of the result (convert shift out to 1/0, xor against sign).
   - ASHx right: if right shift sign extends, then the shift and
     conditional or of shifted -1 is redundant.  If right shift zero
     extends, then the shift and conditional or does sign extension.
*/

    case 007:
        srcspec = srcspec & 07;
        switch ((IR >> 9) & 07)  {                      /* decode IR<11:9> */

        case 0:                                         /* MUL */
            if (!CPUO (OPT_EIS)) {
                setTRAP (TRAP_ILL);
                break;
                }
            src2 = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
            src = R[srcspec];
            if (GET_SIGN_W (src2))
                src2 = src2 | ~077777;
            if (GET_SIGN_W (src))
                src = src | ~077777;
            dst = src * src2;
            if (hst_ent) {
                hst_ent->src = src;
                hst_ent->dst = dst;
                }
            R[srcspec] = (dst >> 16) & 0177777;
            R[srcspec | 1] = dst & 0177777;
            N = (dst < 0);
            Z = GET_Z (dst);
            V = 0;
            C = ((dst > 077777) || (dst < -0100000));
            break;

        case 1:                                         /* DIV */
            if (!CPUO (OPT_EIS)) {
                setTRAP (TRAP_ILL);
                break;
                }
            src2 = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
            src = (((uint32) R[srcspec]) << 16) | R[srcspec | 1];
            if (src2 == 0) {
                N = 0;                                  /* J11,11/70 compat */
                Z = V = C = 1;                          /* N = 0, Z = 1 */
                break;
                }
            if ((((uint32)src) == 020000000000) && (src2 == 0177777)) {
                V = 1;                                  /* V = 1 */
                N = Z = C = 0;                          /* N = Z = 0 */
                break;
                }
            if (GET_SIGN_W (src2))
                src2 = src2 | ~077777;
            if (GET_SIGN_W (R[srcspec]))
                src = src | ~017777777777;
            dst = src / src2;
            if (hst_ent) {
                hst_ent->src = src;
                hst_ent->dst = dst;
                }
            N = (dst < 0);                              /* N set on 32b result */
            if ((dst > 077777) || (dst < -0100000)) {
                V = 1;                                  /* V = 1 */
                Z = C = 0;                              /* Z = C = 0 */
                break;
                }
            R[srcspec] = dst & 0177777;
            R[srcspec | 1] = (src - (src2 * dst)) & 0177777;
            Z = GET_Z (dst);
            V = C = 0;
            break;

        case 2:                                         /* ASH */
            if (!CPUO (OPT_EIS)) {
                setTRAP (TRAP_ILL);
                break;
                }
            src2 = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
            src2 = src2 & 077;
            sign = GET_SIGN_W (R[srcspec]);
            src = sign? R[srcspec] | ~077777: R[srcspec];
            if (src2 == 0) {                            /* [0] */
                dst = src;
                V = C = 0;
                }
            else if (src2 <= 15) {                      /* [1,15] */
                dst = src << src2;
                i = (src >> (16 - src2)) & 0177777;
                V = (i != ((dst & 0100000)? 0177777: 0));
                C = (i & 1);
                }
            else if (src2 <= 31) {                      /* [16,31] */
                dst = 0;
                V = (src != 0);
                C = (src << (src2 - 16)) & 1;
                }
            else if (src2 == 32) {                      /* [32] = -32 */
                dst = -sign;
                V = 0;
                C = sign;
                }
            else {                                      /* [33,63] = -31,-1 */
                dst = (src >> (64 - src2)) | (-sign << (src2 - 32));
                V = 0;
                C = ((src >> (63 - src2)) & 1);
                }
            if (hst_ent) {
                hst_ent->src = src;
                hst_ent->dst = dst;
                }
            dst = R[srcspec] = dst & 0177777;
            N = GET_SIGN_W (dst);
            Z = GET_Z (dst);
            break;

        case 3:                                         /* ASHC */
            if (!CPUO (OPT_EIS)) {
                setTRAP (TRAP_ILL);
                break;
                }
            src2 = dstreg? R[dstspec]: ReadW (GeteaW (dstspec));
            src2 = src2 & 077;
            sign = GET_SIGN_W (R[srcspec]);
            src = (((uint32) R[srcspec]) << 16) | R[srcspec | 1];
            if (src2 == 0) {                            /* [0] */
                dst = src;
                V = C = 0;
                }
            else if (src2 <= 31) {                      /* [1,31] */
                dst = ((uint32) src) << src2;
                i = (src >> (32 - src2)) | (-sign << src2);
                V = (i != ((dst & 020000000000)? -1: 0));
                C = (i & 1);
                }
            else if (src2 == 32) {                      /* [32] = -32 */
                dst = -sign;
                V = 0;
                C = sign;
                }
            else {                                      /* [33,63] = -31,-1 */
                dst = (src >> (64 - src2)) | (-sign << (src2 - 32));
                V = 0;
                C = ((src >> (63 - src2)) & 1);
                }
            i = R[srcspec] = (dst >> 16) & 0177777;
            if (hst_ent) {
                hst_ent->src = src;
                hst_ent->dst = dst;
                }
            dst = R[srcspec | 1] = dst & 0177777;
            N = GET_SIGN_W (i);
            Z = GET_Z (dst | i);
            break;

        case 4:                                         /* XOR */
            if (CPUT (HAS_SXS)) {
                if (CPUT (IS_SDSD) && !dstreg) {        /* R,not R */
                    src2 = ReadMW (GeteaW (dstspec));
                    src = R[srcspec];
                    }
                else {
                    src = R[srcspec];
                    src2 = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
                    }
                dst = src ^ src2;
                if (hst_ent) {
                    hst_ent->src = src;
                    hst_ent->dst = dst;
                    }
                N = GET_SIGN_W (dst);
                Z = GET_Z (dst);
                V = 0;
                if (dstreg)
                    R[dstspec] = dst;
                else PWriteW (dst, last_pa);
                }
            else setTRAP (TRAP_ILL);
            break;

        case 5:                                         /* FIS */
            if (CPUO (OPT_FIS))
                fis11 (IR);
            else setTRAP (TRAP_ILL);
            break;

        case 6:                                         /* CIS */
            if (CPUT (CPUT_60) && (cm == MD_KER) &&     /* 11/60 MED? */
                (IR == 076600)) {
                ReadE (PC | isenable);                  /* read immediate */
                PC = (PC + 2) & 0177777;
                }
            else if (CPUO (OPT_CIS))                    /* CIS option? */
                reason = cis11 (IR);
            else setTRAP (TRAP_ILL);
            break;

        case 7:                                         /* SOB */
            if (CPUT (HAS_SXS)) {
                R[srcspec] = (R[srcspec] - 1) & 0177777;
                if (hst_ent)
                    hst_ent->dst = R[srcspec];
                if (R[srcspec]) {
                    JMP_PC ((PC - dstspec - dstspec) & 0177777);
                    }
                }
            else setTRAP (TRAP_ILL);
            break;
            }                                           /* end switch EIS */
        break;                                          /* end case 007 */

/* Opcode 10: branches, traps, SOPs */

    case 010:
        switch ((IR >> 6) & 077) {                      /* decode IR<11:6> */

        case 000: case 001:                             /* BPL */
            if (N == 0) {
                BRANCH_F (IR);
                } 
            break;

        case 002: case 003:                             /* BPL */
            if (N == 0) {
                BRANCH_B (IR);
                }
            break;

        case 004: case 005:                             /* BMI */
            if (N) {
                BRANCH_F (IR);
                } 
            break;

        case 006: case 007:                             /* BMI */
            if (N) {
                BRANCH_B (IR);
                }
            break;

        case 010: case 011:                             /* BHI */
            if ((C | Z) == 0) {
                BRANCH_F (IR);
                } 
            break;

        case 012: case 013:                             /* BHI */
            if ((C | Z) == 0) {
                BRANCH_B (IR);
                }
            break;

        case 014: case 015:                             /* BLOS */
            if (C | Z) {
                BRANCH_F (IR);
                } 
            break;

        case 016: case 017:                             /* BLOS */
            if (C | Z) {
                BRANCH_B (IR);
                }
            break;

        case 020: case 021:                             /* BVC */
            if (V == 0) {
                BRANCH_F (IR);
                } 
            break;

        case 022: case 023:                             /* BVC */
            if (V == 0) {
                BRANCH_B (IR);
                }
            break;

        case 024: case 025:                             /* BVS */
            if (V) {
                BRANCH_F (IR);
                } 
            break;

        case 026: case 027:                             /* BVS */
            if (V) {
                BRANCH_B (IR);
                }
            break;

        case 030: case 031:                             /* BCC */
            if (C == 0) {
                BRANCH_F (IR);
                } 
            break;

        case 032: case 033:                             /* BCC */
            if (C == 0) {
                BRANCH_B (IR);
                }
            break;

        case 034: case 035:                             /* BCS */
            if (C) {
                BRANCH_F (IR);
                } 
            break;

        case 036: case 037:                             /* BCS */
            if (C) {
                BRANCH_B (IR);
                }
            break;

        case 040: case 041: case 042: case 043:         /* EMT */
            setTRAP (TRAP_EMT);
            break;

        case 044: case 045: case 046: case 047:         /* TRAP */
            setTRAP (TRAP_TRAP);
            break;

        case 050:                                       /* CLRB */
            N = V = C = 0;
            Z = 1;
            if (dstreg)
                R[dstspec] = R[dstspec] & 0177400;
            else WriteB (0, GeteaB (dstspec));
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = 0;
            }
            break;

        case 051:                                       /* COMB */
            dst = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = (dst ^ 0377) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            V = 0;
            C = 1;
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 052:                                       /* INCB */
            dst = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = (dst + 1) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            V = (dst == 0200);
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 053:                                       /* DECB */
            dst = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = (dst - 1) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            V = (dst == 0177);
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 054:                                       /* NEGB */
            dst = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = (-dst) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            V = (dst == 0200);
            C = (Z ^ 1);
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 055:                                       /* ADCB */
            dst = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = (dst + C) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            V = (C && (dst == 0200));
            C = C & Z;
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 056:                                       /* SBCB */
            dst = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = (dst - C) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            V = (C && (dst == 0177));
            C = (C && (dst == 0377));
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 057:                                       /* TSTB */
            dst = dstreg? R[dstspec] & 0377: ReadB (GeteaB (dstspec));
            if (hst_ent)
                hst_ent->dst = dst;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            V = C = 0;
            break;

        case 060:                                       /* RORB */
            src = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = ((src & 0377) >> 1) | (C << 7);
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            C = (src & 1);
            V = N ^ C;
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 061:                                       /* ROLB */
            src = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = ((src << 1) | C) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            C = GET_SIGN_B (src & 0377);
            V = N ^ C;
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 062:                                       /* ASRB */
            src = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = ((src & 0377) >> 1) | (src & 0200);
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            C = (src & 1);
            V = N ^ C;
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

        case 063:                                       /* ASLB */
            src = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            dst = (src << 1) & 0377;
            N = GET_SIGN_B (dst);
            Z = GET_Z (dst);
            C = GET_SIGN_B (src & 0377);
            V = N ^ C;
            if (dstreg)
                R[dstspec] = (R[dstspec] & 0177400) | dst;
            else PWriteB (dst, last_pa);
            if (hst_ent) {
                if (dstreg)
                    hst_ent->dst = R[dstspec];
                else hst_ent->dst = dst;
            }
            break;

/* Notes:
   - MTPS cannot alter the T bit
   - MxPD must mask GeteaW returned address, dspace is from cm not pm
   - MxPD must set MMR1 for SP recovery in case of fault
*/

        case 064:                                       /* MTPS */
            if (CPUT (HAS_MXPS)) {
                dst = dstreg? R[dstspec]: ReadB (GeteaB (dstspec));
                if (cm == MD_KER) {
                    ipl = (dst >> PSW_V_IPL) & 07;
                    trap_req = calc_ints (ipl, trap_req);
                    }
                N = (dst >> PSW_V_N) & 01;
                Z = (dst >> PSW_V_Z) & 01;
                V = (dst >> PSW_V_V) & 01;
                C = (dst >> PSW_V_C) & 01;
                }
            else setTRAP (TRAP_ILL);
            break;

        case 065:                                       /* MFPD */
            if (CPUT (HAS_MXPY)) {
                if (dstreg) {
                    if ((dstspec == 6) && (cm != pm))
                        dst = STACKFILE[pm];
                    else dst = R[dstspec];
                    }
                else dst = ReadW ((GeteaW (dstspec) & 0177777) | calc_ds (pm));
                N = GET_SIGN_W (dst);
                Z = GET_Z (dst);
                V = 0;
                SP = (SP - 2) & 0177777;
                reg_mods = calc_MMR1 (0366);
                if (update_MM)
                    MMR1 = reg_mods;
                if (hst_ent)
                    hst_ent->dst = dst;
                if ((cm == MD_KER) && (SP < (STKLIM + STKL_Y)))
                    set_stack_trap (SP);
                WriteW (dst, SP | dsenable);
                }
            else setTRAP (TRAP_ILL);
            break;

        case 066:                                       /* MTPD */
            if (CPUT (HAS_MXPY)) {
                dst = ReadW (SP | dsenable);
                N = GET_SIGN_W (dst);
                Z = GET_Z (dst);
                V = 0;
                SP = (SP + 2) & 0177777;
                reg_mods = 026;
                if (update_MM)
                    MMR1 = reg_mods;
                if (hst_ent)
                    hst_ent->dst = dst;
                if (dstreg) {
                    if ((dstspec == 6) && (cm != pm))
                        STACKFILE[pm] = dst;
                    else R[dstspec] = dst;
                    }
                else WriteW (dst, (GeteaW (dstspec) & 0177777) | calc_ds (pm));
                }
            else setTRAP (TRAP_ILL);
            break;

        case 067:                                       /* MFPS */
            if (CPUT (HAS_MXPS)) {
                dst = get_PSW () & 0377;
                N = GET_SIGN_B (dst);
                Z = GET_Z (dst);
                V = 0;
                if (dstreg)
                    R[dstspec] = (dst & 0200)? 0177400 | dst: dst;
                else WriteB (dst, GeteaB (dstspec));
                }
            else setTRAP (TRAP_ILL);
            break;

        default:
            setTRAP (TRAP_ILL);
            break;
            }                                           /* end switch SOPs */
        break;                                          /* end case 010 */

/* Opcodes 11 - 16: double operand byte instructions

   Cmp: v = [sign (src) != sign (src2)] and [sign (src2) = sign (result)]
   Sub: v = [sign (src) != sign (src2)] and [sign (src) = sign (result)]
*/

    case 011:                                           /* MOVB */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            ea = GeteaB (dstspec);
            dst = R[srcspec] & 0377;
            }
        else {
            dst = srcreg? R[srcspec] & 0377: ReadB (GeteaB (srcspec));
            if (!dstreg)
                ea = GeteaB (dstspec);
            }
        N = GET_SIGN_B (dst);
        Z = GET_Z (dst);
        V = 0;
        if (dstreg)
            R[dstspec] = (dst & 0200)? 0177400 | dst: dst;
        else WriteB (dst, ea);
        if (hst_ent) {
            hst_ent->src = srcreg? R[srcspec]: dst;
            hst_ent->dst = dstreg? R[dstspec]: dst;
            }
        break;

    case 012:                                           /* CMPB */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadB (GeteaB (dstspec));
            src = R[srcspec] & 0377;
            }
        else {
            src = srcreg? R[srcspec] & 0377: ReadB (GeteaB (srcspec));
            src2 = dstreg? R[dstspec] & 0377: ReadB (GeteaB (dstspec));
            }
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = src2;
            }
        dst = (src - src2) & 0377;
        N = GET_SIGN_B (dst);
        Z = GET_Z (dst);
        V = GET_SIGN_B ((src ^ src2) & (~src2 ^ dst));
        C = (src < src2);
        break;

    case 013:                                           /* BITB */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadB (GeteaB (dstspec));
            src = R[srcspec] & 0377;
            }
        else {
            src = srcreg? R[srcspec] & 0377: ReadB (GeteaB (srcspec));
            src2 = dstreg? R[dstspec] & 0377: ReadB (GeteaB (dstspec));
            }
        dst = (src2 & src) & 0377;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_B (dst);
        Z = GET_Z (dst);
        V = 0;
        break;

    case 014:                                           /* BICB */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadMB (GeteaB (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadB (GeteaB (srcspec));
            src2 = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            }
        dst = (src2 & ~src) & 0377;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_B (dst);
        Z = GET_Z (dst);
        V = 0;
        if (dstreg)
            R[dstspec] = (R[dstspec] & 0177400) | dst;
        else PWriteB (dst, last_pa);
        break;

    case 015:                                           /* BISB */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadMB (GeteaB (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadB (GeteaB (srcspec));
            src2 = dstreg? R[dstspec]: ReadMB (GeteaB (dstspec));
            }
        dst = (src2 | src) & 0377;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_B (dst);
        Z = GET_Z (dst);
        V = 0;
        if (dstreg)
            R[dstspec] = (R[dstspec] & 0177400) | dst;
        else PWriteB (dst, last_pa);
        break;

    case 016:                                           /* SUB */
        if (CPUT (IS_SDSD) && srcreg && !dstreg) {      /* R,not R */
            src2 = ReadMW (GeteaW (dstspec));
            src = R[srcspec];
            }
        else {
            src = srcreg? R[srcspec]: ReadW (GeteaW (srcspec));
            src2 = dstreg? R[dstspec]: ReadMW (GeteaW (dstspec));
            }
        dst = (src2 - src) & 0177777;
        if (hst_ent) {
            hst_ent->src = src;
            hst_ent->dst = dst;
            }
        N = GET_SIGN_W (dst);
        Z = GET_Z (dst);
        V = GET_SIGN_W ((src ^ src2) & (~src ^ dst));
        C = (src2 < src);
        if (dstreg)
            R[dstspec] = dst;
        else PWriteW (dst, last_pa);
        break;

/* Opcode 17: floating point */

    case 017:
        if (CPUO (OPT_FPP))
            fp11 (IR);                  /* call fpp */
        else setTRAP (TRAP_ILL);
        break;                                          /* end case 017 */
        }                                               /* end switch op */
#ifdef USE_REALCONS
        // assume tmp var "dst" is holding the data path shifter output
        // It is used on PDP-11/70 for DATA PATH knob position.
        // Other PDP-11's may show different signals, or implement "shifter" in another way.
        // The shifter usage on 11/70 is implemneted ad hoc, so the known "idle patterns" appear right
        // Tested for RSX11M, 2.11BSD, IAS
        {
            unsigned ir15_06 = IR & 0177700; // mask bits 15:6
            unsigned ir15_09 = IR & 0177000; // mask bits 15:9
            unsigned ir15_12 = IR & 0170000; // mask bits 15:9
            if (   ir15_09 == 0072000 // ASH
                || ir15_09 == 0073000 // ASHC
                || ir15_06 == 0063000 // ASL
                || ir15_06 == 0163000 // ASLB
                || ir15_06 == 0062000 // ASR
                || ir15_06 == 0162000 // ASRB
                || ir15_12 == 0010000 // MOV
                || ir15_12 == 0110000 // MOVB
                )
            realcons_DATAPATH_shifter = dst;
        }

		// fetch CPU state after opcode processing.
		realcons_IR = IR; // copy: IR only local var
		realcons_PSW = get_PSW(); // copy: PSW not atomic
		// check if the ENABLE/HALT switch was set to HALT
		if (cpu_realcons->connected && realcons_console_halt) {
			reason = SCPE_STOP; // transition is triggered at end of instr loop
		}

		if (reason == 0)
			// if HALT, a more specific transition is done above
			REALCONS_EVENT(cpu_realcons, realcons_event_opcode_any);
#endif
    }                                                   /* end main loop */

/* Simulation halted */

PSW = get_PSW ();
for (i = 0; i < 6; i++)
    REGFILE[i][rs] = R[i];
STACKFILE[cm] = SP;
saved_PC = PC & 0177777;
MMR1 = clean_MMR1 (MMR1);                               /* clean up MMR1 */
pcq_r->qptr = pcq_p;                                    /* update pc q ptr */
set_r_display (rs, cm);
#ifdef USE_REALCONS
	//	if (cm == MD_KER)
	//		realcons_machine_set(cpu_realcons, REALCONS_MP_PDP11_CPU_MEMMODE,REALCONS_MP_PDP11_CPU_MEMMODE_KERNEL) ;
	//	else if (cm == MD_SUP)
	//		realcons_machine_set(cpu_realcons, REALCONS_MP_PDP11_CPU_MEMMODE,REALCONS_MP_PDP11_CPU_MEMMODE_SUPERVISOR) ;
	//	else realcons_machine_set(cpu_realcons, REALCONS_MP_PDP11_CPU_MEMMODE,REALCONS_MP_PDP11_CPU_MEMMODE_USER) ;

	if ((reason == STOP_HALT) || (reason == STOP_WAIT) || (reason == SCPE_STOP)
		|| (reason == STOP_VECABORT) || (reason == STOP_SPABORT))
	{ // during HALT, general register R0 contents are displayed.
	  // trigger HALT transition after fetch of processor state
		if (reason == SCPE_STOP) // address halt, cpu_realcons halt:
			REALCONS_EVENT(cpu_realcons, realcons_event_operator_halt);
		else  // STOP_HALT is opcode , STOP_SPABORT, STOP_VECABORT others are traps, STOP_WAIT  not used
			REALCONS_EVENT(cpu_realcons, realcons_event_opcode_halt);
	}
	else
	{ // during Single Instruction operation, the Processor Status Word is displayed.
		realcons_PSW = (uint16)PSW; // not get_PSW()?
		REALCONS_EVENT(cpu_realcons, realcons_event_step_halt);
	}
#endif /* USE_REALCONS */
return reason;
}

/* Effective address calculations

   Inputs:
        spec    =       specifier <5:0>
   Outputs:
        ea      =       effective address
                        <15:0> =  virtual address
                        <16> =    instruction/data data space
                        <18:17> = mode

   Data space calculation: the PDP-11 features both instruction and data
   spaces.  Instruction space contains the instruction and any sequential
   add ons (eg, immediates, absolute addresses).  Data space contains all
   data operands and indirect addresses.  If data space is enabled, then
   memory references are directed according to these rules:

        Mode    Index ref       Indirect ref            Direct ref
        10..16  na              na                      data
        17      na              na                      instruction
        20..26  na              na                      data
        27      na              na                      instruction
        30..36  na              data                    data
        37      na              instruction (absolute)  data
        40..46  na              na                      data
        47      na              na                      instruction
        50..56  na              data                    data
        57      na              instruction             data
        60..67  instruction     na                      data
        70..77  instruction     data                    data

   According to the PDP-11 Architecture Handbook, MMR1 records all
   autoincrement and autodecrement operations, including those which
   explicitly reference the PC.  For the J-11, this is only true for
   autodecrement operands, autodecrement deferred operands, and
   autoincrement destination operands that involve a write to memory.
   This is cleaned up at simulator exit or MMR1 read.

   Notes:

   - dsenable will direct a reference to data space if data space is enabled
   - ds will direct a reference to data space if data space is enabled AND if
        the specifier register is not PC; this is used for 17, 27, 37, 47, 57
   - Modes 2x, 3x, 4x, and 5x must update MMR1 if updating enabled
   - Modes 46 and 56 must check for stack overflow if kernel mode
*/

/* Effective address calculation for words */

int32 GeteaW (int32 spec)
{
int32 adr, reg, ds;

reg = spec & 07;                                        /* register number */
ds = (reg == 7)? isenable: dsenable;                    /* dspace if not PC */
switch (spec >> 3) {                                    /* decode spec<5:3> */

    default:                                            /* can't get here */
    case 1:                                             /* (R) */
        return (R[reg] | ds);

    case 2:                                             /* (R)+ */
        R[reg] = ((adr = R[reg]) + 2) & 0177777;
        reg_mods = calc_MMR1 (020 | reg);
        if (update_MM)
            MMR1 = reg_mods;
        return (adr | ds);

    case 3:                                             /* @(R)+ */
        R[reg] = ((adr = R[reg]) + 2) & 0177777;
        reg_mods = calc_MMR1 (020 | reg);
        if (update_MM)
            MMR1 = reg_mods;
        adr = ReadW (adr | ds);
        return (adr | dsenable);

    case 4:                                             /* -(R) */
        adr = R[reg] = (R[reg] - 2) & 0177777;
        reg_mods = calc_MMR1 (0360 | reg);
        if (update_MM)
            MMR1 = reg_mods;
        if ((reg == 6) && (cm == MD_KER) && (adr < (STKLIM + STKL_Y)))
            set_stack_trap (adr);
        return (adr | ds);

    case 5:                                             /* @-(R) */
        adr = R[reg] = (R[reg] - 2) & 0177777;
        reg_mods = calc_MMR1 (0360 | reg);
        if (update_MM)
            MMR1 = reg_mods;
        if ((reg == 6) && (cm == MD_KER) && (adr < (STKLIM + STKL_Y)))
            set_stack_trap (adr);
        adr = ReadW (adr | ds);
        return (adr | dsenable);

    case 6:                                             /* d(r) */
        adr = ReadW (PC | isenable);
        PC = (PC + 2) & 0177777;
        return (((R[reg] + adr) & 0177777) | dsenable);

    case 7:                                             /* @d(R) */
        adr = ReadW (PC | isenable);
        PC = (PC + 2) & 0177777;
        adr = ReadW (((R[reg] + adr) & 0177777) | dsenable);
        return (adr | dsenable);
        }                                               /* end switch */
}

/* Effective address calculation for bytes */

int32 GeteaB (int32 spec)
{
int32 adr, reg, ds, delta;

reg = spec & 07;                                        /* reg number */
ds = (reg == 7)? isenable: dsenable;                    /* dspace if not PC */
switch (spec >> 3) {                                    /* decode spec<5:3> */

    default:                                            /* can't get here */
    case 1:                                             /* (R) */
        return (R[reg] | ds);

    case 2:                                                     /* (R)+ */
        delta = 1 + (reg >= 6);                         /* 2 if R6, PC */
        R[reg] = ((adr = R[reg]) + delta) & 0177777;
        reg_mods = calc_MMR1 ((delta << 3) | reg);
        if (update_MM && (reg != 7))
            MMR1 = reg_mods;
        return (adr | ds);

    case 3:                                             /* @(R)+ */
        R[reg] = ((adr = R[reg]) + 2) & 0177777;
        reg_mods = calc_MMR1 (020 | reg);
        if (update_MM)
            MMR1 = reg_mods;
        adr = ReadW (adr | ds);
        return (adr | dsenable);

    case 4:                                             /* -(R) */
        delta = 1 + (reg >= 6);                         /* 2 if R6, PC */
        adr = R[reg] = (R[reg] - delta) & 0177777;
        reg_mods = calc_MMR1 ((((-delta) & 037) << 3) | reg);
        if (update_MM)
            MMR1 = reg_mods;
        if ((reg == 6) && (cm == MD_KER) && (adr < (STKLIM + STKL_Y)))
            set_stack_trap (adr);
        return (adr | ds);

    case 5:                                             /* @-(R) */
        adr = R[reg] = (R[reg] - 2) & 0177777;
        reg_mods = calc_MMR1 (0360 | reg);
        if (update_MM)
            MMR1 = reg_mods;
        if ((reg == 6) && (cm == MD_KER) && (adr < (STKLIM + STKL_Y)))
            set_stack_trap (adr);
        adr = ReadW (adr | ds);
        return (adr | dsenable);

    case 6:                                             /* d(r) */
        adr = ReadW (PC | isenable);
        PC = (PC + 2) & 0177777;
        return (((R[reg] + adr) & 0177777) | dsenable);

    case 7:                                             /* @d(R) */
        adr = ReadW (PC | isenable);
        PC = (PC + 2) & 0177777;
        adr = ReadW (((R[reg] + adr) & 0177777) | dsenable);
        return (adr | dsenable);
        }                                               /* end switch */
}

/* Read byte and word routines, read only and read-modify-write versions

   Inputs:
        va      =       virtual address, <18:16> = mode, I/D space
   Outputs:
        data    =       data read from memory or I/O space
*/

int32 ReadE (int32 va)
{
int32 pa, data;

if ((va & 1) && CPUT (HAS_ODD)) {                       /* odd address? */
    setCPUERR (CPUE_ODD);
    ABORT (TRAP_ODD);
    }
pa = relocR (va);                                       /* relocate */
if (BPT_SUMM_RD &&
    (sim_brk_test (va & 0177777, BPT_RDVIR) ||
     sim_brk_test (pa, BPT_RDPHY)))                     /* read breakpoint? */
    ABORT (ABRT_BKPT);                                  /* stop simulation */
if (ADDR_IS_MEM (pa))                                   /* memory address? */
#ifdef USE_REALCONS
		RETURN_REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(cpu_realcons, va, pa, RdMemW (pa));
#else
    return RdMemW (pa);
#endif
if ((pa < IOPAGEBASE) ||                                /* not I/O address */
    (CPUT (CPUT_J) && (pa >= IOBA_CPU))) {              /* or J11 int reg? */
        setCPUERR (CPUE_NXM);
        ABORT (TRAP_NXM);
        }
if (iopageR (&data, pa, READ) != SCPE_OK) {             /* invalid I/O addr? */
    setCPUERR (CPUE_TMO);
    ABORT (TRAP_NXM);
    }
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(cpu_realcons, va, pa, data);
#endif
return data;
}

int32 ReadW (int32 va)
{
int32 pa;

if ((va & 1) && CPUT (HAS_ODD)) {                       /* odd address? */
    setCPUERR (CPUE_ODD);
    ABORT (TRAP_ODD);
    }
pa = relocR (va);                                       /* relocate */
if (BPT_SUMM_RD &&
    (sim_brk_test (va & 0177777, BPT_RDVIR) ||
     sim_brk_test (pa, BPT_RDPHY)))                     /* read breakpoint? */
    ABORT (ABRT_BKPT);                                  /* stop simulation */
#ifdef USE_REALCONS
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(cpu_realcons, va, pa, PReadW (pa));
#else
return PReadW (pa);
#endif
}

int32 ReadB (int32 va)
{
int32 pa;

pa = relocR (va);                                       /* relocate */
if (BPT_SUMM_RD &&
    (sim_brk_test (va & 0177777, BPT_RDVIR) ||
     sim_brk_test (pa, BPT_RDPHY)))                     /* read breakpoint? */
    ABORT (ABRT_BKPT);                                  /* stop simulation */
#ifdef USE_REALCONS
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(cpu_realcons, va, pa, PReadB (pa));
#else
    return PReadB (pa);
#endif
}

/* Read word with breakpoint check: if a data breakpoint is encountered,
   set reason accordingly but don't do an ABORT.  This is used when we want
   to break after doing the operation, used for interrupt processing.  */
int32 ReadCW (int32 va)
{
int32 pa;

if ((va & 1) && CPUT (HAS_ODD)) {                       /* odd address? */
    setCPUERR (CPUE_ODD);
    ABORT (TRAP_ODD);
    }
pa = relocR (va);                                       /* relocate */
if (BPT_SUMM_RD &&
    (sim_brk_test (va & 0177777, BPT_RDVIR) ||
     sim_brk_test (pa, BPT_RDPHY)))                     /* read breakpoint? */
    reason = STOP_IBKPT;                                /* report that */
#ifdef USE_REALCONS
		RETURN_REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(cpu_realcons, va, pa,  PReadW (pa));
#else
return PReadW (pa);
#endif
}

int32 ReadMW (int32 va)
{
if ((va & 1) && CPUT (HAS_ODD)) {                       /* odd address? */
    setCPUERR (CPUE_ODD);
    ABORT (TRAP_ODD);
    }
last_pa = relocW (va);                                  /* reloc, wrt chk */
if (BPT_SUMM_RW &&
    (sim_brk_test (va & 0177777, BPT_RWVIR) ||
     sim_brk_test (last_pa, BPT_RWPHY)))                /* read or write breakpoint? */
    ABORT (ABRT_BKPT);                                  /* stop simulation */
#ifdef USE_REALCONS
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(cpu_realcons, va, last_pa, PReadW (last_pa));
#else
return PReadW (last_pa);
#endif
}

int32 ReadMB (int32 va)
{
last_pa = relocW (va);                                  /* reloc, wrt chk */
if (BPT_SUMM_RW &&
    (sim_brk_test (va & 0177777, BPT_RWVIR) ||
     sim_brk_test (last_pa, BPT_RWPHY)))                /* read or write breakpoint? */
    ABORT (ABRT_BKPT);                                  /* stop simulation */
#ifdef USE_REALCONS
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_VA_PA_READ(cpu_realcons, va, last_pa, PReadB (last_pa));
#else
return PReadB (last_pa);
#endif
}

int32 PReadW (int32 pa)
{
int32 data;

if (ADDR_IS_MEM (pa))                                   /* memory address? */
#ifdef USE_REALCONS
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_PA_READ(cpu_realcons, pa, RdMemW (pa));
#else
    return RdMemW (pa);
#endif
if (pa < IOPAGEBASE) {                                  /* not I/O address? */
    setCPUERR (CPUE_NXM);
    ABORT (TRAP_NXM);
    }
if (iopageR (&data, pa, READ) != SCPE_OK) {             /* invalid I/O addr? */
    setCPUERR (CPUE_TMO);
    ABORT (TRAP_NXM);
    }
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_PA_READ(cpu_realcons, pa, data);
#endif
return data;
}

int32 PReadB (int32 pa)
{
int32 data;

if (ADDR_IS_MEM (pa))                                   /* memory address? */
#ifdef USE_REALCONS
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_PA_READ(cpu_realcons, pa, RdMemB (pa));
#else
    return RdMemB (pa);
#endif
if (pa < IOPAGEBASE) {                                  /* not I/O address? */
    setCPUERR (CPUE_NXM);
    ABORT (TRAP_NXM);
    }
if (iopageR (&data, pa, READ) != SCPE_OK) {             /* invalid I/O addr? */
    setCPUERR (CPUE_TMO);
    ABORT (TRAP_NXM);
    }
#ifdef USE_REALCONS
	RETURN_REALCONS_CPU_PDP11_MEMACCESS_PA_READ(cpu_realcons, pa, (((pa & 1)? data >> 8: data) & 0377));
#else
return ((pa & 1)? data >> 8: data) & 0377;
#endif
}

/* Write byte and word routines

   Inputs:
        data    =       data to be written
        va      =       virtual address, <18:16> = mode, I/D space, or
        pa      =       physical address
   Outputs: none
*/

void WriteW (int32 data, int32 va)
{
int32 pa;

if ((va & 1) && CPUT (HAS_ODD)) {                       /* odd address? */
    setCPUERR (CPUE_ODD);
    ABORT (TRAP_ODD);
    }
pa = relocW (va);                                       /* relocate */
if (BPT_SUMM_WR &&
    (sim_brk_test (va & 0177777, BPT_WRVIR) ||
     sim_brk_test (pa, BPT_WRPHY)))                     /* write breakpoint? */
    ABORT (ABRT_BKPT);                                  /* stop simulation */
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_VA_PA_WRITE(cpu_realcons, va, pa, data);
#endif
PWriteW (data, pa);
}

void WriteB (int32 data, int32 va)
{
int32 pa;

pa = relocW (va);                                       /* relocate */
if (BPT_SUMM_WR &&
    (sim_brk_test (va & 0177777, BPT_WRVIR) ||
     sim_brk_test (pa, BPT_WRPHY)))                     /* write breakpoint? */
    ABORT (ABRT_BKPT);                                  /* stop simulation */
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_VA_PA_WRITE(cpu_realcons, va, pa, data);
#endif
PWriteB (data, pa);
}

/* Write word with breakpoint check: if a data breakpoint is encountered,
   set reason accordingly but don't do an ABORT.  This is used when we want
   to break after doing the operation, used for interrupt processing.  */
void WriteCW (int32 data, int32 va)
{
int32 pa;

if ((va & 1) && CPUT (HAS_ODD)) {                       /* odd address? */
    setCPUERR (CPUE_ODD);
    ABORT (TRAP_ODD);
    }
pa = relocW (va);                                       /* relocate */
if (BPT_SUMM_WR &&
    (sim_brk_test (va & 0177777, BPT_WRVIR) ||
     sim_brk_test (pa, BPT_WRPHY)))                     /* write breakpoint? */
    reason = STOP_IBKPT;                                /* report that */
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_VA_PA_WRITE(cpu_realcons, va, pa, data);
#endif
PWriteW (data, pa);
}

void PWriteW (int32 data, int32 pa)
{
if (ADDR_IS_MEM (pa)) {                                 /* memory address? */
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_PA_WRITE(cpu_realcons, pa, data);
#endif
    WrMemW (pa, data);
    return;
    }
if (pa < IOPAGEBASE) {                                  /* not I/O address? */
    setCPUERR (CPUE_NXM);
    ABORT (TRAP_NXM);
    }
if (iopageW (data, pa, WRITE) != SCPE_OK) {             /* invalid I/O addr? */
    setCPUERR (CPUE_TMO);
    ABORT (TRAP_NXM);
    }
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_PA_WRITE(cpu_realcons, pa, data);
#endif
return;
}

void PWriteB (int32 data, int32 pa)
{
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_PA_WRITE(cpu_realcons, pa, data);
#endif
if (ADDR_IS_MEM (pa)) {                                 /* memory address? */
    WrMemB (pa, data);
    return;
    }             
if (pa < IOPAGEBASE) {                                  /* not I/O address? */
    setCPUERR (CPUE_NXM);
    ABORT (TRAP_NXM);
    }
if (iopageW (data, pa, WRITEB) != SCPE_OK) {            /* invalid I/O addr? */
    setCPUERR (CPUE_TMO);
    ABORT (TRAP_NXM);
    }
#ifdef USE_REALCONS
	REALCONS_CPU_PDP11_MEMACCESS_PA_WRITE(cpu_realcons, pa, data);
#endif
return;
}

/* Relocate virtual address, read access

   Inputs:
        va      =       virtual address, <18:16> = mode, I/D space
   Outputs:
        pa      =       physical address
   On aborts, this routine aborts back to the top level simulator
   with an appropriate trap code.

   Notes:
   - The 'normal' read codes (010, 110) are done in-line; all
     others in a subroutine
   - APRFILE[UNUSED] is all zeroes, forcing non-resident abort
   - Aborts must update MMR0<15:13,6:1> if updating is enabled
*/

int32 relocR (int32 va)
{
int32 apridx, apr, pa;

if (MMR0 & MMR0_MME) {                                  /* if mmgt */
    apridx = (va >> VA_V_APF) & 077;                    /* index into APR */
    apr = APRFILE[apridx];                              /* with va<18:13> */
    if ((apr & PDR_PRD) != 2)                           /* not 2, 6? */
         relocR_test (va, apridx);                      /* long test */
    if (PLF_test (va, apr))                             /* pg lnt error? */
        reloc_abort (MMR0_PL, apridx);
    pa = ((va & VA_DF) + ((apr >> 10) & 017777700)) & PAMASK;
    if ((MMR3 & MMR3_M22E) == 0) {
        pa = pa & 0777777;
        if (pa >= 0760000)
            pa = 017000000 | pa;
        }
    }
else {
    pa = va & 0177777;                                  /* mmgt off */
    if (pa >= 0160000)
        pa = 017600000 | pa;
    }
return pa;
}

/* Read relocation, access control field != read only or read/write

   ACF value            11/45,11/70             all others

   0                    abort NR                abort NR
   1                    trap                    -
   2                    ok                      ok
   3                    abort NR                -
   4                    trap                    abort NR
   5                    ok                      -
   6                    ok                      ok
   7                    abort NR                -
*/

void relocR_test (int32 va, int32 apridx)
{
int32 apr, err;

err = 0;                                                /* init status */
apr = APRFILE[apridx];                                  /* get APR */
switch (apr & PDR_ACF) {                                /* case on ACF */

    case 1: case 4:                                     /* trap read */
        if (CPUT (HAS_MMTR)) {                          /* traps implemented? */
            int32 old_mmr0 = MMR0;
            APRFILE[apridx] |= PDR_A;                   /* set A */
            MMR0 = MMR0 | MMR0_TRAP;                    /* set trap flag */
            if ((MMR0 & MMR0_TENB) != 0) {              /* traps enabled? */
                if (update_MM)                          /* update MMR0 */
                    MMR0 = (MMR0 & ~MMR0_PAGE) | (apridx << MMR0_V_PAGE);
                if ((old_mmr0 & MMR0_TRAP) == 0)        /* first trap? */
                    setTRAP (TRAP_MME);                 /* set trap */
                }
            return;                                     /* continue */
            }                                           /* not impl, abort NR */
    case 0: case 3: case 7:                             /* non-resident */
        err = MMR0_NR;                                  /* set MMR0 */
        break;                                          /* go test PLF, abort */

    case 2: case 5: case 6:                             /* readable */
        return;                                         /* continue */
        }                                               /* end switch */

if (PLF_test (va, apr))                                 /* pg lnt error? */
    err = err | MMR0_PL;
reloc_abort (err, apridx);
return;
}

t_bool PLF_test (int32 va, int32 apr)
{
int32 dbn = va & VA_BN;                                 /* extr block num */
int32 plf = (apr & PDR_PLF) >> 2;                       /* extr page length */

return ((apr & PDR_ED)? (dbn < plf): (dbn > plf));      /* pg lnt error? */
}

void reloc_abort (int32 err, int32 apridx)
{
if (update_MM) {                                        /* MMR0 not frozen? */
    MMR0 = (MMR0 & ~MMR0_PAGE) | (apridx << MMR0_V_PAGE); /* record page */
    MMR0 = MMR0 | err;                                  /* OR in aborts */
    }
ABORT (TRAP_MME);                                       /* abort ref */
return;
}

/* Relocate virtual address, write access

   Inputs:
        va      =       virtual address, <18:16> = mode, I/D space
   Outputs:
        pa      =       physical address
   On aborts, this routine aborts back to the top level simulator
   with an appropriate trap code.

   Notes:
   - The 'normal' write code (110) is done in-line; all others
     in a subroutine
   - APRFILE[UNUSED] is all zeroes, forcing non-resident abort
   - Aborts must update MMR0<15:13,6:1> if updating is enabled
*/

int32 relocW (int32 va)
{
int32 apridx, apr, pa;

if (MMR0 & MMR0_MME) {                                  /* if mmgt */
    apridx = (va >> VA_V_APF) & 077;                    /* index into APR */
    apr = APRFILE[apridx];                              /* with va<18:13> */
    if ((apr & PDR_ACF) != 6)                           /* not writeable? */
        relocW_test (va, apridx);                       /* long test */
    if (PLF_test (va, apr))                             /* pg lnt error? */
        reloc_abort (MMR0_PL, apridx);
    APRFILE[apridx] |= PDR_W;                           /* set W */
    pa = ((va & VA_DF) + ((apr >> 10) & 017777700)) & PAMASK;
    if ((MMR3 & MMR3_M22E) == 0) {
        pa = pa & 0777777;
        if (pa >= 0760000)
            pa = 017000000 | pa;
        }
    }
else {
    pa = va & 0177777;                                  /* mmgt off */
    if (pa >= 0160000)
        pa = 017600000 | pa;
    }
return pa;
}

/* Write relocation, access control field != read/write

   ACF value            11/45,11/70             all others

   0                    abort NR                abort NR
   1                    abort RO                -
   2                    abort RO                abort RO
   3                    abort NR                -
   4                    trap                    abort NR
   5                    trap                    -
   6                    ok                      ok
   7                    abort NR                -
*/

void relocW_test (int32 va, int32 apridx)
{
int32 apr, err;

err = 0;                                                /* init status */
apr = APRFILE[apridx];                                  /* get APR */
switch (apr & PDR_ACF) {                                /* case on ACF */

    case 4: case 5:                                     /* trap write */
        if (CPUT (HAS_MMTR)) {                          /* traps implemented? */
            int32 old_mmr0 = MMR0;
            APRFILE[apridx] |= PDR_A;                   /* set PDR <A> */
            MMR0 = MMR0 | MMR0_TRAP;                    /* set trap flag */
            if ((MMR0 & MMR0_TENB) != 0) {              /* traps enabled? */
                if (update_MM)                          /* update MMR0 */
                    MMR0 = (MMR0 & ~MMR0_PAGE) | (apridx << MMR0_V_PAGE);
                if ((old_mmr0 & MMR0_TRAP) == 0)        /* first trap? */
                    setTRAP (TRAP_MME);                 /* set trap */
                }
            return;                               /* continue, set A */
            }                                           /* not impl, abort NR */
    case 0: case 3: case 7:                             /* non-resident */
        err = MMR0_NR;                                  /* MMR0 status */
        break;                                          /* go test PLF, abort */

    case 1: case 2:                                     /* read only */
        err = MMR0_RO;                                  /* MMR0 status */
        break;

    case 6:                                             /* read/write */
        return;                                         /* continue */
        }                                               /* end switch */
if (PLF_test (va, apr))                                 /* pg lnt error? */
    err = err | MMR0_PL;
reloc_abort (err, apridx);
return;
}

/* Relocate virtual address, console access

   Inputs:
        va      =       virtual address
        sw      =       switches
   Outputs:
        pa      =       physical address
   On aborts, this routine returns MAXMEMSIZE
*/

int32 relocC (int32 va, int32 sw)
{
int32 mode, dbn, plf, apridx, apr, pa;

if (MMR0 & MMR0_MME) {                                  /* if mmgt */
    if (sw & SWMASK ('K'))
        mode = MD_KER;
    else if (sw & SWMASK ('S'))
        mode = MD_SUP;
    else if (sw & SWMASK ('U'))
        mode = MD_USR;
    else if (sw & SWMASK ('P'))
        mode = (PSW >> PSW_V_PM) & 03;
    else mode = (PSW >> PSW_V_CM) & 03;
    va = va | ((sw & SWMASK ('T'))? calc_ds (mode): calc_is (mode));
    apridx = (va >> VA_V_APF) & 077;                    /* index into APR */
    apr = APRFILE[apridx];                              /* with va<18:13> */
    dbn = va & VA_BN;                                   /* extr block num */
    plf = (apr & PDR_PLF) >> 2;                         /* extr page length */
    if ((apr & PDR_PRD) == 0)                           /* not readable? */
        return MAXMEMSIZE;
    if ((apr & PDR_ED)? dbn < plf: dbn > plf)
        return MAXMEMSIZE;
    pa = ((va & VA_DF) + ((apr >> 10) & 017777700)) & PAMASK;
    if ((MMR3 & MMR3_M22E) == 0) {
        pa = pa & 0777777;
        if (pa >= 0760000)
            pa = 017000000 | pa;
        }
    }
else {
    pa = va & 0177777;                                  /* mmgt off */
    if (pa >= 0160000)
        pa = 017600000 | pa;
    }
return pa;
}

/* Memory management registers

   MMR0 17777572        read/write, certain bits unimplemented or read only
   MMR1 17777574        read only
   MMR2 17777576        read only
   MMR3 17772516        read/write, certain bits unimplemented
*/

t_stat MMR012_rd (int32 *data, int32 pa, int32 access)
{
switch ((pa >> 1) & 3) {                                /* decode pa<2:1> */

    case 0:                                             /* SR */
        return SCPE_NXM;

    case 1:                                             /* MMR0 */
        *data = MMR0 & cpu_tab[cpu_model].mm0;
        break;

    case 2:                                             /* MMR1 */
        MMR1 = clean_MMR1 (MMR1);                       /* clean up MMR1 */
        *data = MMR1;                                   /* return data */
        break;

    case 3:                                             /* MMR2 */
        *data = MMR2;
        break;
        }                                               /* end switch pa */

return SCPE_OK;
}

t_stat MMR012_wr (int32 data, int32 pa, int32 access)
{
switch ((pa >> 1) & 3) {                                /* decode pa<2:1> */

    case 0:                                             /* DR */
        return SCPE_NXM;

    case 1:                                             /* MMR0 */
        if (access == WRITEB)
            data = (pa & 1)? (MMR0 & 0377) | (data << 8): (MMR0 & ~0377) | data;
        data = data & cpu_tab[cpu_model].mm0;
        MMR0 = (MMR0 & ~MMR0_WR) | (data & MMR0_WR);
        return SCPE_OK;

    default:                                            /* MMR1, MMR2 */
        return SCPE_OK;
        }                                               /* end switch pa */
}

t_stat MMR3_rd (int32 *data, int32 pa, int32 access)    /* MMR3 */
{
*data = MMR3 & cpu_tab[cpu_model].mm3;
return SCPE_OK;
}

t_stat MMR3_wr (int32 data, int32 pa, int32 access)     /* MMR3 */
{
if (pa & 1)
    return SCPE_OK;
MMR3 = data & cpu_tab[cpu_model].mm3;
cpu_bme = (MMR3 & MMR3_BME) && (cpu_opt & OPT_UBM);
dsenable = calc_ds (cm);
return SCPE_OK;
}

/* Clean up MMR1 for presentation

   !HAS_SID         MMR1 is 0
   HAS_SID && J11   MMR1 values corresponding to # and @# are cleared
   HAS_SID && !J11  MMR1 is unchanged

   Note that # and @# always generate reg = 7 and change = 2;
   no other specifier combination can do that.
*/

int32 clean_MMR1 (int32 mmr1)
{
if (!CPUT (HAS_SID))                                    /* not full mmgt? */
    return 0;                                           /* always 0 */
if (CPUT (CPUT_J)) {                                    /* J11? */
    if ((mmr1 >> 8) == 027)                             /* high byte # or @#? */
        mmr1 = mmr1 & 0377;                             /* erase high byte */
    if ((mmr1 & 0377) == 027)                           /* low byte # or @#? */
        mmr1 = mmr1 >> 8;                               /* erase low byte */
    }
return mmr1;
}

/* PARs and PDRs.  These are grouped in I/O space as follows:

        17772200 - 17772276     supervisor block
        17772300 - 17772376     kernel block
        17777600 - 17777676     user block

   Within each block, the subblocks are I PDR's, D PDR's, I PAR's, D PAR's

   Thus, the algorithm for converting between I/O space addresses and
   APRFILE indices is as follows:

        idx<3:0> =      dspace'page     =       pa<4:1>
        par     =       PDR vs PAR      =       pa<5>
        idx<5:4> =      ker/sup/user    =       pa<8>'~pa<6>

   Note: the A,W bits are read only; they are cleared by any write to an APR
*/

t_stat APR_rd (int32 *data, int32 pa, int32 access)
{
t_stat left, idx;

idx = (pa >> 1) & 017;                                  /* dspace'page */
left = (pa >> 5) & 1;                                   /* PDR vs PAR */
if ((pa & 0100) == 0)                                   /* 1 for super, user */
    idx = idx | 020;
if (pa & 0400)                                          /* 1 for user only */
    idx = idx | 040;
if (left)
    *data = (APRFILE[idx] >> 16) & cpu_tab[cpu_model].par;
else *data = APRFILE[idx] & cpu_tab[cpu_model].pdr;
return SCPE_OK;
}

t_stat APR_wr (int32 data, int32 pa, int32 access)
{
int32 left, idx, curr;

idx = (pa >> 1) & 017;                                  /* dspace'page */
left = (pa >> 5) & 1;                                   /* PDR vs PAR */
if ((pa & 0100) == 0)                                   /* 1 for super, user */
    idx = idx | 020;
if (pa & 0400)                                          /* 1 for user only */
    idx = idx | 040;
if (left)
    curr = (APRFILE[idx] >> 16) & cpu_tab[cpu_model].par;
else curr = APRFILE[idx] & cpu_tab[cpu_model].pdr;
if (access == WRITEB)
    data = (pa & 1)? (curr & 0377) | (data << 8): (curr & ~0377) | data;
if (left)
    APRFILE[idx] = ((APRFILE[idx] & 0177777) |
        (((uint32) (data & cpu_tab[cpu_model].par)) << 16)) & ~(PDR_A|PDR_W);
else APRFILE[idx] = ((APRFILE[idx] & ~0177777) |
    (data & cpu_tab[cpu_model].pdr)) & ~(PDR_A|PDR_W);
return SCPE_OK;
}

/* Explicit PSW read */

t_stat PSW_rd (int32 *data, int32 pa, int32 access)
{
if (access == READC)
    *data = PSW;
else *data = get_PSW ();
return SCPE_OK;
}

/* Assemble PSW from pieces */

int32 get_PSW (void)
{
return (cm << PSW_V_CM) | (pm << PSW_V_PM) |
    (rs << PSW_V_RS) | (fpd << PSW_V_FPD) |
    (ipl << PSW_V_IPL) | (tbit << PSW_V_TBIT) |
    (N << PSW_V_N) | (Z << PSW_V_Z) |
    (V << PSW_V_V) | (C << PSW_V_C);
}

/* Explicit PSW write - T-bit may be protected */

t_stat PSW_wr (int32 data, int32 pa, int32 access)
{
int32 i, curr, oldrs;

if (access == WRITEC) {                                 /* console access? */
    PSW = data & cpu_tab[cpu_model].psw;
    return SCPE_OK;
    }
curr = get_PSW ();                                      /* get current */
oldrs = rs;                                             /* save reg set */
STACKFILE[cm] = SP;                                     /* save curr SP */
if (access == WRITEB) data = (pa & 1)?
    (curr & 0377) | (data << 8): (curr & ~0377) | data;
if (!CPUT (HAS_EXPT))                                   /* expl T writes? */
    data = (data & ~PSW_TBIT) | (curr & PSW_TBIT);      /* no, use old T */
put_PSW (data, 0);                                      /* call calc_is,ds */
if (rs != oldrs) {                                      /* switch reg set */
    for (i = 0; i < 6; i++) {
        REGFILE[i][oldrs] = R[i];
        R[i] = REGFILE[i][rs];
        }
    }
SP = STACKFILE[cm];                                     /* switch SP */
isenable = calc_is (cm);
dsenable = calc_ds (cm);
return SCPE_OK;
}

/* Store pieces of new PSW - implements RTI/RTT protection */

void put_PSW (int32 val, t_bool prot)
{
val = val & cpu_tab[cpu_model].psw;                     /* mask off invalid bits */
if (prot) {                                             /* protected? */
    cm = cm | ((val >> PSW_V_CM) & 03);                 /* or to cm,pm,rs */
    pm = pm | ((val >> PSW_V_PM) & 03);                 /* can't change ipl */
    rs = rs | ((val >> PSW_V_RS) & 01);
    }
else {
    cm = (val >> PSW_V_CM) & 03;                        /* write cm,pm,rs,ipl */
    pm = (val >> PSW_V_PM) & 03;
    rs = (val >> PSW_V_RS) & 01;
    ipl = (val >> PSW_V_IPL) & 07;
    }
fpd = (val >> PSW_V_FPD) & 01;                          /* always writeable */
tbit = (val >> PSW_V_TBIT) & 01;
N = (val >> PSW_V_N) & 01;
Z = (val >> PSW_V_Z) & 01;
V = (val >> PSW_V_V) & 01;
C = (val >> PSW_V_C) & 01;
return;
}

/* PIRQ write routine */

void put_PIRQ (int32 val)
{
int32 pl;

PIRQ = val & PIRQ_RW;
pl = 0;
if (PIRQ & PIRQ_PIR1) {
    SET_INT (PIR1);
    pl = 0042;
    }
else CLR_INT (PIR1);
if (PIRQ & PIRQ_PIR2) {
    SET_INT (PIR2);
    pl = 0104;
    }
else CLR_INT (PIR2);
if (PIRQ & PIRQ_PIR3) {
    SET_INT (PIR3);
    pl = 0146;
    }
else CLR_INT (PIR3);
if (PIRQ & PIRQ_PIR4) {
    SET_INT (PIR4);
    pl = 0210;
    }
else CLR_INT (PIR4);
if (PIRQ & PIRQ_PIR5) {
    SET_INT (PIR5);
    pl = 0252;
    }
else CLR_INT (PIR5);
if (PIRQ & PIRQ_PIR6) {
    SET_INT (PIR6);
    pl = 0314;
    }
else CLR_INT (PIR6);
if (PIRQ & PIRQ_PIR7) {
    SET_INT (PIR7);
    pl = 0356;
    }
else CLR_INT (PIR7);
PIRQ = PIRQ | pl;
return;
}

/* Stack trap routine */

void set_stack_trap (int32 adr)
{
if (CPUT (HAS_STKLF)) {                                 /* fixed stack? */
    setTRAP (TRAP_YEL);                                 /* always yellow trap */
    setCPUERR (CPUE_YEL);
    }
else if (CPUT (HAS_STKLR)) {                            /* register limit? */
    if (adr >= (STKLIM + STKL_R)) {                     /* yellow zone? */
        setTRAP (TRAP_YEL);                             /* still yellow trap */
        setCPUERR (CPUE_YEL);
        }
    else {                                              /* red zone abort */
        setCPUERR (CPUE_RED);
        STACKFILE[MD_KER] = 4;
        SP = 4;
        ABORT (TRAP_RED);
        }
    }
return;                                                 /* no stack limit */
}

/*
 * This sequence of instructions is a mix that mimics
 * a resonable instruction set that is a close estimate
 * to the normal calibrated result.
 */

static const char *pdp11_clock_precalibrate_commands[] = {
    "-m 100 INC 120",
    "-m 104 INC 122",
    "-m 110 BR  100",
    "PC 100",
    NULL};

/* Special boot command - linked into SCP by initial reset

   Syntax: BOOT {CPU}

*/

t_stat pdp11_boot (int32 flag, CONST char *ptr)
{
char gbuf[CBUFSIZE];

if ((ptr = get_sim_sw (ptr)) == NULL)               /* get switches */
    return SCPE_INVSW;
get_glyph (ptr, gbuf, 0);                           /* get glyph */
if (gbuf[0] && strcmp (gbuf, "CPU"))
    return run_cmd (flag, gbuf);                    /* use specified device/unit */
return run_cmd (flag, "CPU");
}


/* Special boot command, overrides regular boot */

CTAB pdp11_cmd[] = {
    { "BOOT", &pdp11_boot, RU_BOOT,
      "bo{ot} {unit}            boot simulator\n", NULL, &run_cmd_message },
    { NULL }
    };


t_stat cpu_boot (int32 unitno, DEVICE *dptr)
{
DEVICE *rom = find_dev ("ROM");

if ((rom == NULL) || 
    ((rom->flags & DEV_DIS) != 0) ||
    (SCPE_NOFNC == rom->boot (unitno, rom)))
    return SCPE_2FARG;
return SCPE_OK;
}

/* Reset routine */

t_stat cpu_reset (DEVICE *dptr)
{
PIRQ = 0;
STKLIM = 0;
if (CPUT (CPUT_T))                                      /* T11? */
    PSW = 000340;                                       /* start at IPL 7 */
else
    PSW = 0;                                            /* else at IPL 0 */
MMR0 = 0;
MMR1 = 0;
MMR2 = 0;
MMR3 = 0;
trap_req = 0;
wait_state = 0;
if (M == NULL) {                    /* First time init */
    M = (uint16 *) calloc (MEMSIZE >> 1, sizeof (uint16));
    if (M == NULL)
        return SCPE_MEM;
    sim_set_pchar (0, "01000023640"); /* ESC, CR, LF, TAB, BS, BEL, ENQ */
    sim_brk_dflt = SWMASK ('E');
    sim_brk_types = sim_brk_dflt|SWMASK ('P')|
                    SWMASK ('R')|SWMASK ('S')|
                    SWMASK ('W')|SWMASK ('X');
    sim_brk_type_desc = cpu_breakpoints;
    sim_vm_is_subroutine_call = &cpu_is_pc_a_subroutine_call;
    sim_clock_precalibrate_commands = pdp11_clock_precalibrate_commands;
    auto_config(NULL, 0);           /* do an initial auto configure */
    }
pcq_r = find_reg ("PCQ", NULL, dptr);
if (pcq_r)
    pcq_r->qptr = 0;
else
    return SCPE_IERR;
#ifdef USE_REALCONS
	// initialize realcons cpu state extension here
	realcons_bus_ID_mode = 0;
	realcons_DATAPATH_shifter = 0;
	realcons_IR = 0;
	realcons_PSW = 0;
    REALCONS_EVENT(cpu_realcons, realcons_event_cpu_reset);
#endif
set_r_display (0, MD_KER);
sim_vm_cmd = pdp11_cmd;
return build_dib_tab ();            /* build, chk dib_tab */
}

static const char *cpu_next_caveats =
"The NEXT command in the PDP11 simulator currently will enable stepping\n"
"across subroutine calls which are initiated by the JSR instruction.\n"
"This stepping works by dynamically establishing breakpoints at the\n"
"10 memory addresses immediately following the instruction which initiated\n"
"the subroutine call.  These dynamic breakpoints are automatically\n"
"removed once the simulator returns to the sim> prompt for any reason.\n"
"If the called routine returns somewhere other than one of these\n"
"locations due to a trap, stack unwind or any other reason, instruction\n"
"execution will continue until some other reason causes execution to stop.\n";

t_bool cpu_is_pc_a_subroutine_call (t_addr **ret_addrs)
{
#define MAX_SUB_RETURN_SKIP 10
static t_addr returns[MAX_SUB_RETURN_SKIP + 1] = {0};
static t_bool caveats_displayed = FALSE;
static int32 swmap[4] = {
    SWMASK ('K') | SWMASK ('V'), SWMASK ('S') | SWMASK ('V'),
    SWMASK ('U') | SWMASK ('V'), SWMASK ('U') | SWMASK ('V')
    };
int32 cm = ((PSW >> PSW_V_CM) & 03);

if (!caveats_displayed) {
    caveats_displayed = TRUE;
    sim_printf ("%s", cpu_next_caveats);
    }
if (SCPE_OK != get_aval (relocC(PC, swmap[cm]), &cpu_dev, &cpu_unit))/* get data */
    return FALSE;
if ((sim_eval[0] & 0177000) == 0004000) {               /* JSR */
    int32 dst, dstspec;
    t_addr i, max_returns = MAX_SUB_RETURN_SKIP;
    int32 save_Regs[8];

    memcpy (save_Regs, R, sizeof(R));       /* save register state */
    PC = PC + 2;                            /* account for instruction fetch */
    dstspec = sim_eval[0] & 077;
    dst = GeteaW (dstspec);
    if (CPUT (CPUT_05|CPUT_20) &&           /* 11/05, 11/20 */
        ((dstspec & 070) == 020))           /* JSR (R)+? */
        dst = R[dstspec & 07];              /* use post incr */
    memcpy (R, save_Regs, sizeof(R));       /* restore register state */
    returns[0] = PC + (1 - fprint_sym (stdnul, PC, sim_eval, &cpu_unit, SWMASK ('M')));
    if (((t_addr)dst > returns[0]) && ((dst - returns[0]) < max_returns*2))
        max_returns = (dst - returns[0])/2;
    for (i=1; i<max_returns; i++)
        returns[i] = returns[i-1] + 2;      /* Possible skip return */
    returns[i] = 0;                         /* Make sure the address list ends with a zero */
    *ret_addrs = returns;
    return TRUE;
    }
return FALSE;
}

/* Boot setup routine */

void cpu_set_boot (int32 pc)
{
saved_PC = pc;
PSW = 000340;
return;
}

/* Memory examine */

t_stat cpu_ex (t_value *vptr, t_addr addr, UNIT *uptr, int32 sw)
{
int32 iodata;
t_stat stat;

if (vptr == NULL)
    return SCPE_ARG;
if (sw & SWMASK ('V')) {                                /* -v */
    if (addr >= VASIZE)
        return SCPE_NXM;
    addr = relocC (addr, sw);                           /* relocate */
    if (addr >= MAXMEMSIZE)
        return SCPE_REL;
    }
if (ADDR_IS_MEM (addr)) {
    *vptr = RdMemW (addr) & 0177777;
    return SCPE_OK;
    }
if (addr < IOPAGEBASE)
    return SCPE_NXM;
stat = iopageR (&iodata, addr, READC);
*vptr = iodata;
return stat;
}

/* Memory deposit */

t_stat cpu_dep (t_value val, t_addr addr, UNIT *uptr, int32 sw)
{
if (sw & SWMASK ('V')) {                                /* -v */
    if (addr >= VASIZE)
        return SCPE_NXM;
    addr = relocC (addr, sw);                           /* relocate */
    if (addr >= MAXMEMSIZE)
        return SCPE_REL;
    }
if (ADDR_IS_MEM (addr)) {
    WrMemW (addr, val & 0177777);
    return SCPE_OK;
    }
if (addr < IOPAGEBASE)
    return SCPE_NXM;
return iopageW ((int32) val, addr, WRITEC);
}

/* Set R, SP register display addresses */

void set_r_display (int32 rs, int32 cm)
{
REG *rptr;
int32 i;

rptr = find_reg ("R0", NULL, &cpu_dev);
if (rptr == NULL)
    return;
for (i = 0; i < 6; i++, rptr++)
    rptr->loc = (void *) &REGFILE[i][rs];
rptr->loc = (void *) &STACKFILE[cm];
return;
}

/* Set history */

t_stat cpu_set_hist (UNIT *uptr, int32 val, CONST char *cptr, void *desc)
{
int32 i, lnt;
t_stat r;

if (cptr == NULL) {
    for (i = 0; i < hst_lnt; i++)
        hst[i].pc = 0;
    hst_p = 0;
    return SCPE_OK;
    }
lnt = (int32) get_uint (cptr, 10, HIST_MAX, &r);
if (r != SCPE_OK)
    return sim_messagef (SCPE_ARG, "Invalid Numeric Value: %s.  Maximum is %d\n", cptr, HIST_MAX);
if (lnt && (lnt < HIST_MIN))
    return sim_messagef (SCPE_ARG, "%d is less than the minumum history value of %d\n", lnt, HIST_MIN);
hst_p = 0;
if (hst_lnt) {
    free (hst);
    hst_lnt = 0;
    hst = NULL;
    }
if (lnt) {
    hst = (InstHistory *) calloc (lnt, sizeof (InstHistory));
    if (hst == NULL)
        return SCPE_MEM;
    hst_lnt = lnt;
    }
return SCPE_OK;
}

/* Show history */

t_stat cpu_show_hist (FILE *st, UNIT *uptr, int32 val, CONST void *desc)
{
int32 j, k, di, lnt, ir;
const char *cptr = (const char *) desc;
t_value sim_eval[HIST_ILNT];
t_stat r;
InstHistory *h;

if (hst_lnt == 0)                                       /* enabled? */
    return SCPE_NOFNC;
if (cptr) {
    lnt = (int32) get_uint (cptr, 10, hst_lnt, &r);
    if ((r != SCPE_OK) || (lnt == 0))
        return sim_messagef (SCPE_ARG, "Invalid count specifier: %s, max is %d\n", cptr, hst_lnt);
    }
else lnt = hst_lnt;
di = hst_p - lnt;                                       /* work forward */
if (di < 0)
    di = di + hst_lnt;
fprintf (st, "PC     SP     PSW     src    dst     IR\n\n");
for (k = 0; k < lnt; k++) {                             /* print specified */
    if (stop_cpu) {                                     /* Control-C (SIGINT) */
        stop_cpu = FALSE;
        break;                                          /* abandon remaining output */
        }
    h = &hst[(di++) % hst_lnt];                         /* entry pointer */
    if (h->pc & HIST_VLD) {                             /* instruction? */
        ir = h->inst[0];
        fprintf (st, "%06o %06o %06o|", h->pc & ~HIST_VLD, h->sp, h->psw);
        if (((ir & 0070000) != 0) ||                    /* dops, eis, fpp */
            ((ir & 0177000) == 0004000))                /* jsr */
            fprintf (st, "%06o %06o  ", h->src, h->dst);
        else if ((ir >= 0000100) &&                     /* not no opnd */
            (((ir & 0007700) <  0000300) ||             /* not branch */
             ((ir & 0007700) >= 0004000)))
            fprintf (st, "       %06o  ", h->dst);
        else fprintf (st, "               ");
        for (j = 0; j < HIST_ILNT; j++)
            sim_eval[j] = h->inst[j];
        if ((fprint_sym (st, h->pc & ~HIST_VLD, sim_eval, &cpu_unit, SWMASK ('M'))) > 0)
            fprintf (st, "(undefined) %06o", h->inst[0]);
        fputc ('\n', st);                               /* end line */
        }                                               /* end else instruction */
    }                                                   /* end for */
return SCPE_OK;
}

/* Virtual address translation */

t_stat cpu_show_virt (FILE *of, UNIT *uptr, int32 val, CONST void *desc)
{
t_stat r;
const char *cptr = (const char *) desc;
uint32 va, pa;

if (cptr) {
    va = (uint32) get_uint (cptr, 8, VAMASK, &r);
    if (r == SCPE_OK) {
        pa = relocC (va, sim_switches);                 /* relocate */
        if (pa < MAXMEMSIZE)
            fprintf (of, "Virtual %-o = physical %-o\n", va, pa);
        else fprintf (of, "Virtual %-o is not valid\n", va);
        return SCPE_OK;
        }
    }
fprintf (of, "Invalid argument\n");
return SCPE_OK;
}

const char *cpu_description (DEVICE *dptr)
{
return "PDP-11 CPU";
}

t_stat cpu_help (FILE *st, DEVICE *dptr, UNIT *uptr, int32 flag, const char *cptr)
{
int i;

fprintf (st, "The %s (%s) device help\n\n", dptr->description (dptr), dptr->name);
fprintf (st, "The CPU options include CPU type, CPU instruction set options for the\n");
fprintf (st, "specified type, and the size of main memory.\n\n");
fprint_set_help (st, dptr);
fprintf (st, "\n");
fprintf (st, "The CPU types and their capabilities are shown in the following table:\n\n");
fprintf (st, "    cpu         max           Unibus\n");
fprintf (st, "    type   bus  mem     MMU?  map?   EIS?  FIS?  FPP?  CIS?  BEVENT?\n");
fprintf (st, "    ================================================================\n");
#define _OPT(OPT) ((cpu->std & OPT) ? ((cpu->opt & OPT) ? "opt" : "std") : ((cpu->opt & OPT) ? "opt" : "no"))
for (i = 0; i < MOD_MAX; i++) {
    CPUTAB *cpu = &cpu_tab[i];

    fprintf (st, "    %-6s  %s   %-7s %-3s   %-3s    %-3s   %-3s   %-3s   %-3s   %-3s\n", 
                     cpu->name, 
                     (cpu->std & BUS_U) ? "U" : "Q",
                     (cpu->maxm == MEMSIZE64K) ? "64K" : ((cpu->maxm == MAXMEMSIZE) ? "4M" : ((cpu->maxm == UNIMEMSIZE) ? "256K" : "UNK")),
                     _OPT(OPT_MMU), _OPT(OPT_UBM), _OPT(OPT_EIS), _OPT(OPT_FIS),
                     _OPT(OPT_FPP), _OPT(OPT_CIS), _OPT(OPT_BVT));
    }
fprintf (st, "\n");
fprintf (st, "If a capability is standard, it cannot be disabled; if a capability\n");
fprintf (st, "is not included, it cannot be enabled.\n\n");
fprint_show_help (st, dptr);
fprintf (st, "\n");
fprintf (st, "If memory size is being reduced, and the memory being truncated contains\n");
fprintf (st, "non-zero data, the simulator asks for confirmation.  Data in the truncated\n");
fprintf (st, "portion of memory is lost.  Initial memory size is 256KB.  If memory size\n");
fprintf (st, "is increased to more than 256KB, or the bus structure is changed, the\n");
fprintf (st, "simulator disables peripherals that can't run in the current bus structure.\n\n");
fprintf (st, "These switches are recognized when examining or depositing in CPU memory:\n\n");
fprintf (st, "      -v           interpret address as virtual\n");
fprintf (st, "      -t           if mem mgt enabled, force data space\n");
fprintf (st, "      -k           if mem mgt enabled, force kernel mode\n");
fprintf (st, "      -s           if mem mgt enabled, force supervisor mode\n");
fprintf (st, "      -u           if mem mgt enabled, force user mode\n");
fprintf (st, "      -p           if mem mgt enabled, force previous mode\n\n");
fprintf (st, "CPU registers include the architectural state of the PDP-11 processor as\n");
fprintf (st, "well as the control registers for the interrupt system.\n\n");
fprint_reg_help (st, dptr);
fprintf (st, "\n");
fprintf (st, "The CPU attempts to detect when the simulator is idle.  When idle, the\n");
fprintf (st, "simulator does not use any resources on the host system.  Idle detection\n");
fprintf (st, "is controlled by the SET IDLE and SET NOIDLE commands:\n\n");
fprintf (st, "     SET CPU IDLE             enable idle detection\n");
fprintf (st, "     SET CPU NOIDLE           disable idle detection\n\n");
fprintf (st, "Idle detection is disabled by default.  The CPU is considered idle if a\n");
fprintf (st, "WAIT instruction is executed.  This will work for RSTS/E and RSX-11M+, but\n");
fprintf (st, "not for RT-11 or UNIX.\n\n");
fprintf (st, "The CPU can maintain a history of the most recently executed instructions.\n");
fprintf (st, "This is controlled by the SET CPU HISTORY and SHOW CPU HISTORY commands:\n\n");
fprintf (st, "     SET CPU HISTORY          clear history buffer\n");
fprintf (st, "     SET CPU HISTORY=0        disable history\n");
fprintf (st, "     SET CPU HISTORY=n        enable history, length = n\n");
fprintf (st, "     SHOW CPU HISTORY         print CPU history\n");
fprintf (st, "     SHOW CPU HISTORY=n       print first n entries of CPU history\n\n");
fprintf (st, "The maximum length for the history is 262144 entries.\n\n");

fprintf (st, "Unibus and Qbus DMA Devices\n\n");
fprintf (st, "DMA peripherals function differently, depending on whether the CPU type\n");
fprintf (st, "supports the Unibus or the Qbus, and whether the Unibus supports 22b direct\n");
fprintf (st, "memory access (11/70 with RH70 controllers):\n\n");
fprintf (st, "       peripheral   11/70      all         Qbus\n");
fprintf (st, "                    +RH70      other\n");
fprintf (st, "                               Unibus\n");
fprintf (st, "       ==========================================================\n");
fprintf (st, "       CD           18b        18b         disabled\n");
fprintf (st, "       RC           18b        18b         disabled\n");
fprintf (st, "       RF           18b        18b         disabled\n");
fprintf (st, "       RK           18b        18b         disabled if mem > 256K\n");
fprintf (st, "       HK           18b        18b         disabled if mem > 256K\n");
fprintf (st, "       RL           18b        18b         22b RLV12\n");
fprintf (st, "       RP           22b        18b         22b third party\n");
fprintf (st, "       RQ           18b        18b         22b RQDX3\n");
fprintf (st, "       RY           18b        18b         disabled if mem > 256K\n");
fprintf (st, "       TC           18b        18b         disabled\n");
fprintf (st, "       TM           18b        18b         disabled if mem > 256K\n");
fprintf (st, "       TS           18b        18b         22b TSV05\n");
fprintf (st, "       TQ           18b        18b         22b TQK50\n");
fprintf (st, "       TU           22b        18b         22b third party\n");
fprintf (st, "       VH           18b        18b         22b DHQ11\n");
fprintf (st, "       XQ           disabled   disabled    22b DELQA\n");
fprintf (st, "       XU           18b        18b         disabled\n\n");
fprintf (st, "Non-DMA peripherals work the same in all configurations.  Unibus-only\n");
fprintf (st, "peripherals are disabled in a Qbus configuration, and Qbus-only peripherals\n");
fprintf (st, "are disabled in a Unibus configuration.  In addition, Qbus DMA peripherals\n");
fprintf (st, "with only 18b addressing capability are disabled in a Qbus configuration\n");
fprintf (st, "with more than 256KB memory.\n\n");
fprintf (st, "I/O Device Addressing\n\n");
fprintf (st, "PDP-11 I/O space and vector space are not large enough to allow all\n");
fprintf (st, "theoretically possible devices to be configured simultaneously at\n");
fprintf (st, "fixed addresses.  Instead, many devices have floating addresses and\n");
fprintf (st, "vectors; that is, the assigned device address and vector depend on the\n");
fprintf (st, "presence of other devices in the configuration:\n\n");
fprintf (st, "       DZ11/DZV11     all instances have floating addresses\n");
fprintf (st, "       DHU11/DHQ11    all instances have floating addresses\n");
fprintf (st, "       RL11           first instance has fixed address, rest floating\n");
fprintf (st, "       RX11/RX211     first instance has fixed address, rest floating\n");
fprintf (st, "       DEUNA/DELUA    first instance has fixed address, rest floating\n");
fprintf (st, "       MSCP disk      first instance has fixed address, rest floating\n");
fprintf (st, "       TMSCP tape     first instance has fixed address, rest floating\n\n");
fprintf (st, "In addition, some devices with fixed I/O space addresses have floating\n");
fprintf (st, "vector addresses.  DCI/DCO and DLI/DLO have floating vector addresses.\n\n");
fprintf (st, "To maintain addressing consistency as the configuration changes, the\n");
fprintf (st, "simulator implements DEC's standard I/O address and vector autoconfiguration.\n");
fprintf (st, "This allows the user to enable or disable devices without needing to\n");
fprintf (st, "manage I/O addresses and vectors.  For example, if RY is enabled while\n");
fprintf (st, "RX is present, RY is assigned an I/O address in the floating I/O space\n");
fprintf (st, "range; but if RX is disabled and then RY is enabled, RY is assigned the\n");
fprintf (st, "fixed \"first instance\" I/O address for floppy disks.\n\n");
fprintf (st, "Autoconfiguration cannot solve address conflicts between devices with\n");
fprintf (st, "overlapping fixed addresses.  For example, with default I/O page addressing,\n");
fprintf (st, "the PDP-11 can support either a TM11 or a TS11, but not both, since they\n");
fprintf (st, "use the same I/O addresses.\n\n");
fprintf (st, "In addition to autoconfiguration, most devices support the SET <device>\n");
fprintf (st, "ADDRESS command, which allows the I/O page address of the device to be\n");
fprintf (st, "changed, and the SET <device> VECTOR command, which allows the vector of\n");
fprintf (st, "the device to be changed.  Explicitly setting the I/O address or vector of\n");
fprintf (st, "any device DISABLES autoconfiguration for the entire system.  As\n");
fprintf (st, "a consequence, when autoconfiguration is disabled, the user may have to\n");
fprintf (st, "manually configure all remaining devices in the system that are explicitly\n");
fprintf (st, "enabled after autoconfiguration has been disabled.  Autoconfiguration can\n");
fprintf (st, "be restored for the entire system with the SET CPU AUTOCONFIGURE command.\n\n");
fprintf (st, "The current I/O map can be displayed with the SHOW CPU IOSPACE command.\n");
fprintf (st, "Addresses that have set by autoconfiguration are marked with an asterisk (*).\n");
fprintf (st, "All devices support the SHOW <device> ADDRESS and SHOW <device> VECTOR\n");
fprintf (st, "commands, which display the device address and vector, respectively.\n");
fprint_brk_help (st, dptr);
return SCPE_OK;
}
