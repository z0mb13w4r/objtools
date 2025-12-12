#ifndef __OPCODE_EXAMINE_H_
#define __OPCODE_EXAMINE_H_

//#define OPCODE_EXAMINE_DEBUGX

#include "opcode.h"
#include "objutils.h"

#define OCINSN_MASK(x)                 MODE_MASK16(x)
#define OCFLAG_MASK(x)                 MODE_HIDE16(x)

#define OECODE_THIS                    (0)
#define OECODE_MNEMONIC                (1)
#define OECODE_OPERAND1                (2)
#define OECODE_OPERAND2                (3)
#define OECODE_OPERAND3                (4)
#define OECODE_OPERAND4                (5)

#define OCINSTRUCTION_ALIGNED          U64MASK(16)
#define OCINSTRUCTION_UNALIGNED        U64MASK(17)
#define OCINSTRUCTION_NOCHECK          U64MASK(18)
#define OCINSTRUCTION_NOTRACK          U64MASK(19)
#define OCINSTRUCTION_PI               U64MASK(20)
#define OCINSTRUCTION_ONE              U64MASK(21)
#define OCINSTRUCTION_TEN              U64MASK(22)
#define OCINSTRUCTION_TWO              U64MASK(23)
#define OCINSTRUCTION_FOUR             U64MASK(24)
#define OCINSTRUCTION_ZERO             U64MASK(25)
#define OCINSTRUCTION_LOG2             U64MASK(26)
#define OCINSTRUCTION_LOGE             U64MASK(27)
#define OCINSTRUCTION_LOG10            U64MASK(28)
#define OCINSTRUCTION_EULER            U64MASK(29)
#define OCINSTRUCTION_LT               U64MASK(30)
#define OCINSTRUCTION_RT               U64MASK(31)
#define OCINSTRUCTION_BND              U64MASK(32)
#define OCINSTRUCTION_REP              U64MASK(33)
#define OCINSTRUCTION_LOCK             U64MASK(34)
#define OCINSTRUCTION_DATA             U64MASK(35)

#define OCINSN_A                U64MASK(41)
#define OCINSN_B                U64MASK(42)
#define OCINSN_C                U64MASK(43)
#define OCINSN_E                U64MASK(44)
#define OCINSN_G                U64MASK(45)
#define OCINSN_L                U64MASK(46)
#define OCINSN_N                U64MASK(47)
#define OCINSN_O                U64MASK(48)
#define OCINSN_S                U64MASK(49)
#define OCINSN_1BIT             U64MASK(50)
#define OCINSN_8BIT             U64MASK(51)
#define OCINSN_16BIT            U64MASK(52)
#define OCINSN_32BIT            U64MASK(53)
#define OCINSN_64BIT            U64MASK(54)
#define OCINSN_80BIT            U64MASK(55)
#define OCINSN_128BIT           U64MASK(56)
#define OCINSN_256BIT           U64MASK(57)
#define OCINSN_512BIT           U64MASK(58)
#define OCINSN_OPERAND0         U64MASK(59)
#define OCINSN_OPERAND1         U64MASK(60)
#define OCINSN_OPERAND2         U64MASK(61)
#define OCINSN_OPERAND3         U64MASK(62)
#define OCINSN_OPERAND4         U64MASK(63)

#define OCINSN_AE               (OCINSN_A | OCINSN_E)
#define OCINSN_BE               (OCINSN_B | OCINSN_E)
#define OCINSN_GE               (OCINSN_G | OCINSN_E)
#define OCINSN_LE               (OCINSN_L | OCINSN_E)
#define OCINSN_NE               (OCINSN_N | OCINSN_E)
#define OCINSN_NO               (OCINSN_N | OCINSN_O)
#define OCINSN_NS               (OCINSN_N | OCINSN_S)

#define OCINSTRUCTION_REPE             (OCINSTRUCTION_REP | OCINSN_E)
#define OCINSTRUCTION_REPNE            (OCINSTRUCTION_REP | OCINSN_NE)
#define OCINSTRUCTION_DATA16           (OCINSTRUCTION_DATA | OCINSN_16BIT)
#define OCINSTRUCTION_DATA32           (OCINSTRUCTION_DATA | OCINSN_32BIT)

#define OCOPERAND_IVALUE0              (0x01)
#define OCOPERAND_UVALUE0              (0x02)
#define OPOPERAND_REGISTER0            (0x03)
#define OCOPERAND_IVALUE1              (0x04)
#define OCOPERAND_UVALUE1              (0x08)
#define OPOPERAND_REGISTER1            (0x0c)
#define OCOPERAND_IVALUE2              (0x10)
#define OCOPERAND_UVALUE2              (0x20)
#define OPOPERAND_REGISTER2            (0x30)
#define OCOPERAND_IVALUE3              (0x40)
#define OCOPERAND_UVALUE3              (0x80)
#define OPOPERAND_REGISTER3            (0xc0)

#define OPOPERAND_SIZE                 U64MASK(51)
// ----- GAP OPSIZE -----
#define OPOPERAND_SEGMENT              U64MASK(55)
// ---- GAP OPSEGMENT ----
#define OCOPERAND_ABSOLUTE             U64MASK(62)

#define OPSIZE_BYTEPTR                 (U64MASK(52) | OPOPERAND_SIZE)
#define OPSIZE_WORDPTR                 (U64MASK(53) | OPOPERAND_SIZE)
#define OPSIZE_DWORDPTR                (U64MASK(54) | OPOPERAND_SIZE)

#define OPSEGMENT_CS                   (U64MASK(56) | OPOPERAND_SEGMENT)
#define OPSEGMENT_DS                   (U64MASK(57) | OPOPERAND_SEGMENT)
#define OPSEGMENT_SS                   (U64MASK(58) | OPOPERAND_SEGMENT)
#define OPSEGMENT_ES                   (U64MASK(59) | OPOPERAND_SEGMENT)
#define OPSEGMENT_GS                   (U64MASK(60) | OPOPERAND_SEGMENT)
#define OPSEGMENT_FS                   (U64MASK(61) | OPOPERAND_SEGMENT)

#define OCREGISTER_XMM                 U64MASK(42)
#define OCREGISTER_YMM                 U64MASK(43)
#define OCREGISTER_ZMM                 U64MASK(44)
#define OCREGISTER_GENERAL             U64MASK(45)
#define OCREGISTER_INSTRUCTIONPTR      U64MASK(46)
#define OCREGISTER_BASEPTR             U64MASK(47)
#define OCREGISTER_STACKIDX            U64MASK(48)
#define OCREGISTER_STACKPTR            U64MASK(49)
#define OCREGISTER_DSTINDEX            U64MASK(50)
#define OCREGISTER_SRCINDEX            U64MASK(51)
#define OCREGISTER_DATA                U64MASK(52)
#define OCREGISTER_COUNTER             U64MASK(53)
#define OCREGISTER_BASE                U64MASK(54)
#define OCREGISTER_ACCUMULATOR         U64MASK(55)
#define OCREGISTER_8BITLO              U64MASK(56)
#define OCREGISTER_8BITHI              U64MASK(57)
#define OCREGISTER_16BIT               U64MASK(58)
#define OCREGISTER_32BIT               U64MASK(59)
#define OCREGISTER_64BIT               U64MASK(60)
#define OCREGISTER_128BIT              U64MASK(61)
#define OCREGISTER_256BIT              U64MASK(62)
#define OCREGISTER_512BIT              U64MASK(63)

#define MODE_OCEXAMINE                 (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('E'))

#include "opcode-examine-arm.h"
#include "opcode-examine-x86.h"

typedef struct ocmnemonic_s {
  char       data[160];
  uint64_t   cvalue;
  union {
    int64_t  ivalue;
    uint64_t uvalue;
  };
} ocmnemonic_t, *pocmnemonic_t;

typedef struct ocoperand_s {
  char       data[160];
  uint64_t   cvalue;
  union {
    int64_t  ivalue0;
    uint64_t uvalue0;
  };
  union {
    int64_t  ivalue1;
    uint64_t uvalue1;
  };
  union {
    int64_t  ivalue2;
    uint64_t uvalue2;
  };
  union {
    int64_t  ivalue3;
    uint64_t uvalue3;
  };
} ocoperand_t, *pocoperand_t;

typedef struct ocexamine_s {
  smode_t    mode;
  uint64_t   vaddr;
  char       comment[160];

  pocmnemonic_t mc;
  pocoperand_t  op1;
  pocoperand_t  op2;
  pocoperand_t  op3;
  pocoperand_t  op4;
} ocexamine_t, *pocexamine_t;

bool_t isocexamine(handle_t p);

handle_t oemalloc();
handle_t oefree(handle_t p);
handle_t oecreate(handle_t p, const uint64_t vaddr, unknown_t mnemonic, unknown_t operands);

unknown_t oeget(handle_t p, const imode_t mode);
unknown_t oejump(unknown_t p, const size_t size);
unknown_t oeskip(unknown_t p, const size_t size);
size_t    oeskipdec(unknown_t p, const size_t size);
size_t    oeskiphex(unknown_t p, const size_t size);

uint64_t  oedecb(unknown_t p, const size_t size);
uint64_t  oehexb(unknown_t p, const size_t size);
bool_t    oeisdecb(unknown_t p, const size_t size);
bool_t    oeishexb(unknown_t p, const size_t size);

pconvert_t oegetINSTRUCTIONFLAGS(handle_t p);
pconvert_t oegetINSTRUCTIONNAMES(handle_t p);

#endif

