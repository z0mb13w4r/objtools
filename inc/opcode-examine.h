#ifndef __OPCODE_EXAMINE_H_
#define __OPCODE_EXAMINE_H_

//#define OPCODE_EXAMINE_MNEMONIC
//#define OPCODE_EXAMINE_OPERAND

#include "opcode.h"
#include "objutils.h"

#define OCFLAG_MASK(x)                 MODE_HIDE16(x)
#define OCINSN_MASK(x)                 MODE_MASK16(x)
#define OCINSN_GET0(x)                 MODE_GET0(x)
#define OCINSN_GET1(x)                 MODE_GET1(x)

#define OECODE_THIS                    (0)
#define OECODE_PREFIX1                 (1)
#define OECODE_PREFIX2                 (2)
#define OECODE_PREFIX3                 (3)
#define OECODE_MNEMONIC                (4)
#define OECODE_OPERAND1                (5)
#define OECODE_OPERAND2                (6)
#define OECODE_OPERAND3                (7)
#define OECODE_OPERAND4                (8)

#define OCINSN_PI                      (205)
#define OCINSN_LOGE                    (206)
#define OCINSN_LOG2                    (207)
#define OCINSN_LOG10                   (208)
#define OCINSN_FXCH                    (209)
#define OCINSN_XCHG                    (210)
#define OCINSN_IN                      (211)
#define OCINSN_OUT                     (212)
#define OCINSN_CMP                     (213)
#define OCINSN_TST                     (214)
#define OCINSN_BTST                    (215)
#define OCINSN_LDA                     (216)
#define OCINSN_MOV                     (217)
#define OCINSN_STA                     (218)
#define OCINSN_CLR                     (219)
#define OCINSN_SET                     (220)
#define OCINSN_Bxx                     (221)
#define OCINSN_Jxx                     (222)
#define OCINSN_ADD                     (223)
#define OCINSN_AND                     (224)
#define OCINSN_DEC                     (225)
#define OCINSN_DIV                     (226)
#define OCINSN_INC                     (227)
#define OCINSN_MAX                     (228)
#define OCINSN_MIN                     (229)
#define OCINSN_MUL                     (230)
#define OCINSN_NEG                     (231)
#define OCINSN_NOT                     (232)
#define OCINSN_ORR                     (233)
#define OCINSN_REM                     (234)
#define OCINSN_ROx                     (235)
#define OCINSN_SCA                     (236)
#define OCINSN_SUB                     (237)
#define OCINSN_SWP                     (238)
#define OCINSN_SxA                     (239)
#define OCINSN_SxL                     (240)
#define OCINSN_XOR                     (241)
#define OCINSN_XNOR                    (242)
#define OCINSN_PSH                     (243)
#define OCINSN_POP                     (244)
#define OCINSN_HLT                     (245)
#define OCINSN_RET                     (246)
#define OCINSN_REV                     (247)
#define OCINSN_NOP                     (248)
#define OCINSN_CALL                    (249)
#define OCINSN_CPUID                   (250)
#define OCINSN_BREAK                   (251)
#define OCINSN_UNDEF                   (252)
#define OCINSN_TOINT                   (253)
#define OCINSN_TOFLOAT                 (254)
#define OCINSN_TOEXTEND                (255)

#define OCINSN_JOIN(x,y)               (MODE_PUT0(x) | MODE_PUT1(y))

#define OCINSN_ORN                     OCINSN_JOIN(OCINSN_ORR, OCINSN_NOT)
#define OCINSN_ANDN                    OCINSN_JOIN(OCINSN_AND, OCINSN_NOT)
#define OCINSN_MADD                    OCINSN_JOIN(OCINSN_MUL, OCINSN_ADD)
#define OCINSN_MSUB                    OCINSN_JOIN(OCINSN_MUL, OCINSN_SUB)
#define OCINSN_RSB                     OCINSN_JOIN(OCINSN_REV, OCINSN_SUB)
#define OCINSN_MOVyxx                  OCINSN_JOIN(OCINSN_MOV, OCINSN_TOEXTEND)

#define OCINSN_00               U64MASK(16)
#define OCINSN_01               U64MASK(17)
#define OCINSN_02               U64MASK(18)
#define OCINSN_03               U64MASK(19)
#define OCINSN_04               U64MASK(20)
#define OCINSN_05               U64MASK(21)
#define OCINSN_06               U64MASK(22)
#define OCINSN_07               U64MASK(23)
#define OCINSN_08               U64MASK(24)
#define OCINSN_09               U64MASK(25)

#define OCINSN_PACKED           U64MASK(26)
#define OCINSN_ALIGNED          U64MASK(27)
#define OCINSN_UNALIGNED        U64MASK(28)
#define OCINSN_LINK             U64MASK(29)
#define OCINSN_ADDRESS          U64MASK(30)
#define OCINSN_IMMEDIATE        U64MASK(31)
#define OCINSN_REGISTER         U64MASK(32)
#define OCINSN_LT               U64MASK(33)
#define OCINSN_RT               U64MASK(34)
#define OCINSN_A                U64MASK(35)
#define OCINSN_B                U64MASK(36)
#define OCINSN_C                U64MASK(37)
#define OCINSN_E                U64MASK(39)
#define OCINSN_G                U64MASK(40)
#define OCINSN_L                U64MASK(41)
#define OCINSN_N                U64MASK(42)
#define OCINSN_O                U64MASK(43)
#define OCINSN_P                U64MASK(44)
#define OCINSN_S                U64MASK(45)
#define OCINSN_Z                U64MASK(46)
#define OCINSN_1BIT             U64MASK(47)
#define OCINSN_8BIT             U64MASK(48)
#define OCINSN_16BIT            U64MASK(49)
#define OCINSN_32BIT            U64MASK(50)
#define OCINSN_64BIT            U64MASK(51)
#define OCINSN_80BIT            U64MASK(52)
#define OCINSN_128BIT           U64MASK(53)
#define OCINSN_256BIT           U64MASK(54)
#define OCINSN_512BIT           U64MASK(55)
#define OCINSN_FLOAT            U64MASK(56)
#define OCINSN_STRING           U64MASK(57)
#define OCINSN_OPERAND0         U64MASK(58)
#define OCINSN_OPERAND1         U64MASK(59)
#define OCINSN_OPERAND2         U64MASK(60)
#define OCINSN_OPERAND3         U64MASK(61)
#define OCINSN_OPERAND4         U64MASK(62)
#define OCINSN_OPERAND5         U64MASK(63)

#define OCINSN_AE               (OCINSN_A | OCINSN_E)
#define OCINSN_BE               (OCINSN_B | OCINSN_E)
#define OCINSN_GE               (OCINSN_G | OCINSN_E)
#define OCINSN_LE               (OCINSN_L | OCINSN_E)
#define OCINSN_NE               (OCINSN_N | OCINSN_E)
#define OCINSN_NO               (OCINSN_N | OCINSN_O)
#define OCINSN_NP               (OCINSN_N | OCINSN_P)
#define OCINSN_NS               (OCINSN_N | OCINSN_S)

#define OCOPERAND_IVALUE0              (0x0001)
#define OCOPERAND_UVALUE0              (0x0002)
#define OPOPERAND_REGISTER0            (0x0003)
#define OCOPERAND_IVALUE1              (0x0004)
#define OCOPERAND_UVALUE1              (0x0008)
#define OPOPERAND_REGISTER1            (0x000c)
#define OCOPERAND_IVALUE2              (0x0010)
#define OCOPERAND_UVALUE2              (0x0020)
#define OPOPERAND_REGISTER2            (0x0030)
#define OCOPERAND_IVALUE3              (0x0040)
#define OCOPERAND_UVALUE3              (0x0080)
#define OPOPERAND_REGISTER3            (0x00c0)
#define OCOPERAND_IVALUE4              (0x0100)
#define OCOPERAND_UVALUE4              (0x0200)
#define OPOPERAND_REGISTER4            (0x0300)
#define OCOPERAND_IVALUE5              (0x0400)
#define OCOPERAND_UVALUE5              (0x0800)
#define OPOPERAND_REGISTER5            (0x0c00)
#define OCOPERAND_IVALUE6              (0x1000)
#define OCOPERAND_UVALUE6              (0x2000)
#define OPOPERAND_REGISTER6            (0x3000)
#define OCOPERAND_IVALUE7              (0x4000)
#define OCOPERAND_UVALUE7              (0x8000)
#define OPOPERAND_REGISTER7            (0xc000)
#define OPOPERAND_00                   U64MASK(50)
#define OPOPERAND_01                   U64MASK(51)
#define OPOPERAND_02                   U64MASK(52)
#define OPOPERAND_03                   U64MASK(53)
#define OPOPERAND_04                   U64MASK(54)
#define OPOPERAND_05                   U64MASK(55)
#define OPOPERAND_06                   U64MASK(56)
#define OPOPERAND_07                   U64MASK(57)
#define OPOPERAND_08                   U64MASK(58)
#define OPOPERAND_09                   U64MASK(59)
#define OPOPERAND_10                   U64MASK(60)
#define OPOPERAND_11                   U64MASK(61)
#define OPOPERAND_12                   U64MASK(62)
#define OPOPERAND_13                   U64MASK(63)

#define OCOPERAND_IVALUEMASK           (OCOPERAND_IVALUE0 | OCOPERAND_IVALUE1 | OCOPERAND_IVALUE2 \
                                         | OCOPERAND_IVALUE3 | OCOPERAND_IVALUE4 | OCOPERAND_IVALUE5 \
                                         | OCOPERAND_IVALUE6 | OCOPERAND_IVALUE7)
#define OCOPERAND_UVALUEMASK           (OCOPERAND_UVALUE0 | OCOPERAND_UVALUE1 | OCOPERAND_UVALUE2 \
                                         | OCOPERAND_UVALUE3 | OCOPERAND_UVALUE4 | OCOPERAND_UVALUE5 \
                                         | OCOPERAND_UVALUE6 | OCOPERAND_UVALUE7)

#define OPOPERAND_REGISTERMASK         (OPOPERAND_REGISTER0 | OPOPERAND_REGISTER1 | OPOPERAND_REGISTER2 \
                                         | OPOPERAND_REGISTER3 | OPOPERAND_REGISTER4 | OPOPERAND_REGISTER5 \
                                         | OPOPERAND_REGISTER6 | OPOPERAND_REGISTER7)

#define OCREGISTER_00                  U64MASK(40)
#define OCREGISTER_01                  U64MASK(41)
#define OCREGISTER_02                  U64MASK(42)
#define OCREGISTER_03                  U64MASK(43)
#define OCREGISTER_04                  U64MASK(44)
#define OCREGISTER_05                  U64MASK(45)
#define OCREGISTER_06                  U64MASK(46)
#define OCREGISTER_07                  U64MASK(47)
#define OCREGISTER_08                  U64MASK(48)
#define OCREGISTER_09                  U64MASK(49)
#define OCREGISTER_10                  U64MASK(50)
#define OCREGISTER_11                  U64MASK(51)
#define OCREGISTER_12                  U64MASK(52)
#define OCREGISTER_13                  U64MASK(53)
#define OCREGISTER_14                  U64MASK(54)
#define OCREGISTER_15                  U64MASK(55)
#define OCREGISTER_8BITLO              U64MASK(56)
#define OCREGISTER_8BITHI              U64MASK(57)
#define OCREGISTER_16BIT               U64MASK(58)
#define OCREGISTER_32BIT               U64MASK(59)
#define OCREGISTER_64BIT               U64MASK(60)
#define OCREGISTER_128BIT              U64MASK(61)
#define OCREGISTER_256BIT              U64MASK(62)
#define OCREGISTER_512BIT              U64MASK(63)

#define MODE_OCEXAMINE                 (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('E'))

#define OCUNION_MAXSIZE                (32 + 1)

#include "opcode-examine-arm.h"
#include "opcode-examine-x86.h"
#include "opcode-examine-mips.h"
#include "opcode-examine-riscv.h"

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
    char     svalue0[OCUNION_MAXSIZE];
  };
  union {
    int64_t  ivalue1;
    uint64_t uvalue1;
    char     svalue1[OCUNION_MAXSIZE];
  };
  union {
    int64_t  ivalue2;
    uint64_t uvalue2;
    char     svalue2[OCUNION_MAXSIZE];
  };
  union {
    int64_t  ivalue3;
    uint64_t uvalue3;
    char     svalue3[OCUNION_MAXSIZE];
  };
  union {
    int64_t  ivalue4;
    uint64_t uvalue4;
    char     svalue4[OCUNION_MAXSIZE];
  };
  union {
    int64_t  ivalue5;
    uint64_t uvalue5;
    char     svalue5[OCUNION_MAXSIZE];
  };
  union {
    int64_t  ivalue6;
    uint64_t uvalue6;
    char     svalue6[OCUNION_MAXSIZE];
  };
  union {
    int64_t  ivalue7;
    uint64_t uvalue7;
    char     svalue7[OCUNION_MAXSIZE];
  };
} ocoperand_t, *pocoperand_t;

typedef struct ocexamine_s {
  smode_t    mode;
  uint64_t   vaddr;
  char       comment[160];

  pocmnemonic_t mc;
  pocmnemonic_t pc1;
  pocmnemonic_t pc2;
  pocmnemonic_t pc3;
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

uint64_t  oedecb(unknown_t p, const size_t size);
uint64_t  oehexb(unknown_t p, const size_t size);
bool_t    oeisdecb(unknown_t p, const size_t size);
bool_t    oeishexb(unknown_t p, const size_t size);

ppick_t oegetADDRLOOKUP(handle_t p);

pconvert_t oegetPREFIXNAMES(handle_t p);
pconvert_t oegetINSTRUCTIONFLAGS(handle_t p);
pconvert_t oegetINSTRUCTIONNAMES(handle_t p);
pconvert_t oegetREGISTERFLAGS(handle_t p);
pconvert_t oegetREGISTERNAMES(handle_t p);

#endif

