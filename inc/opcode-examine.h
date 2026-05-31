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

#define OCREG_HIDE(x)                  MODE_HIDE8(x)
#define OCREG_MASK(x)                  MODE_MASK8(x)

#define OECODE_THIS                    (0)
#define OECODE_PREFIX1                 (1)
#define OECODE_PREFIX2                 (2)
#define OECODE_PREFIX3                 (3)
#define OECODE_MNEMONIC                (4)
#define OECODE_OPERAND1                (5)
#define OECODE_OPERAND2                (6)
#define OECODE_OPERAND3                (7)
#define OECODE_OPERAND4                (8)

#define OCINSN_POSMIN                  (OCINSN_ABS)
#define OCINSN_POSMAX                  (OCINSN_TOEXTEND)

#define OCINSN_ABS                     (194)
#define OCINSN_ATAN                    (195)
#define OCINSN_COS                     (196)
#define OCINSN_SHA1                    (197)
#define OCINSN_SIN                     (198)
#define OCINSN_SQRT                    (199)
#define OCINSN_TAN                     (200)
#define OCINSN_PI                      (201)
#define OCINSN_LOGE                    (202)
#define OCINSN_LOG2                    (203)
#define OCINSN_LOG10                   (204)
#define OCINSN_FXCH                    (205)
#define OCINSN_XCHG                    (206)
#define OCINSN_IN                      (207)
#define OCINSN_OUT                     (208)
#define OCINSN_CMP                     (209)
#define OCINSN_TST                     (210)
#define OCINSN_BTST                    (211)
#define OCINSN_LDA                     (212)
#define OCINSN_MOV                     (213)
#define OCINSN_STA                     (214)
#define OCINSN_CLR                     (215) /* CLEAR Instruction */
#define OCINSN_SET                     (216) /* SET Instruction */
#define OCINSN_EXT                     (217) /* EXTRACT Instruction */
#define OCINSN_Bxx                     (218)
#define OCINSN_Jxx                     (219)
#define OCINSN_ADD                     (220)
#define OCINSN_AND                     (221)
#define OCINSN_DEC                     (222)
#define OCINSN_DIV                     (223)
#define OCINSN_INC                     (224)
#define OCINSN_MAX                     (225)
#define OCINSN_MIN                     (226)
#define OCINSN_MSK                     (227)
#define OCINSN_MUL                     (228)
#define OCINSN_NEG                     (229)
#define OCINSN_NOT                     (230)
#define OCINSN_ORR                     (231)
#define OCINSN_REM                     (232)
#define OCINSN_ROx                     (233)
#define OCINSN_SCA                     (234)
#define OCINSN_SEL                     (235)
#define OCINSN_SUB                     (236)
#define OCINSN_SWP                     (237)
#define OCINSN_SxA                     (238)
#define OCINSN_SxL                     (239)
#define OCINSN_XOR                     (240)
#define OCINSN_XNOR                    (241)
#define OCINSN_PSH                     (242)
#define OCINSN_POP                     (243)
#define OCINSN_HLT                     (244)
#define OCINSN_RET                     (245)
#define OCINSN_REV                     (246)
#define OCINSN_NOP                     (247)
#define OCINSN_CALL                    (248)
#define OCINSN_SYSCALL                 (249)
#define OCINSN_CPUID                   (250)
#define OCINSN_BREAK                   (251)
#define OCINSN_UNDEF                   (252)
#define OCINSN_TOINT                   (253)
#define OCINSN_TOFLOAT                 (254)
#define OCINSN_TOEXTEND                (255)

#define OCINSN_JOIN(x,y)               (MODE_HIDE16(x) | MODE_PUT0(x) | MODE_PUT1(y))

#define OCINSN_ORN                     OCINSN_JOIN(OCINSN_ORR, OCINSN_NOT)
#define OCINSN_ANDN                    OCINSN_JOIN(OCINSN_AND, OCINSN_NOT)
#define OCINSN_MADD                    OCINSN_JOIN(OCINSN_MUL, OCINSN_ADD)
#define OCINSN_MNEG                    OCINSN_JOIN(OCINSN_MUL, OCINSN_NEG)
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
#define OCINSN_LINK             U64MASK(28)
#define OCINSN_STACK            U64MASK(29)
#define OCINSN_ADDRESS          U64MASK(30)
#define OCINSN_IMMEDIATE        U64MASK(31)
#define OCINSN_REGISTER         U64MASK(32)
#define OCINSN_LTSHIFT          U64MASK(33)
#define OCINSN_RTSHIFT          U64MASK(34)
#define OCINSN_A                U64MASK(35) // CONDITIONAL ABOVE
#define OCINSN_B                U64MASK(36) // CONDITIONAL BELOW | BORROW
#define OCINSN_C                U64MASK(37) // CONDITIONAL CARRY
#define OCINSN_E                U64MASK(38) // CONDITIONAL EQUAL
#define OCINSN_G                U64MASK(39) // CONDITIONAL GREATER | UP
#define OCINSN_L                U64MASK(40) // CONDITIONAL LESS | DN
#define OCINSN_N                U64MASK(41) // CONDITIONAL NOT | NEGIVATE | UNSIGNED
#define OCINSN_O                U64MASK(42) // CONDITIONAL OVERFLOW
#define OCINSN_P                U64MASK(43) // CONDITIONAL PARITY
#define OCINSN_S                U64MASK(44) // CONDITIONAL SIGN
#define OCINSN_Z                U64MASK(45) // CONDITIONAL ZERO
#define OCINSN_1BIT             U64MASK(46)
#define OCINSN_8BIT             U64MASK(47)
#define OCINSN_16BIT            U64MASK(48)
#define OCINSN_32BIT            U64MASK(49)
#define OCINSN_64BIT            U64MASK(50)
#define OCINSN_80BIT            U64MASK(51)
#define OCINSN_128BIT           U64MASK(52)
#define OCINSN_256BIT           U64MASK(53)
#define OCINSN_512BIT           U64MASK(54)
#define OCINSN_FLOAT            U64MASK(55)
#define OCINSN_STRING           U64MASK(56)
#define OCINSN_TRYINSN          U64MASK(57)
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

#define OCINSN_EQ               (OCINSN_E)
#define OCINSN_EQZ              (OCINSN_EQ | OCINSN_Z)
#define OCINSN_GEZ              (OCINSN_GE | OCINSN_Z)
#define OCINSN_GT               (OCINSN_G)
#define OCINSN_GTZ              (OCINSN_GT | OCINSN_Z)
#define OCINSN_LEZ              (OCINSN_LE | OCINSN_Z)
#define OCINSN_LT               (OCINSN_L)
#define OCINSN_LTZ              (OCINSN_LT | OCINSN_Z)
#define OCINSN_NEZ              (OCINSN_NE | OCINSN_Z)

#define OCFLAG_CARRY            (OCINSN_C)
#define OCFLAG_OVERFLOW         (OCINSN_O)
#define OCFLAG_PARITY           (OCINSN_P)
#define OCFLAG_SIGN             (OCINSN_S)
#define OCFLAG_ZERO             (OCINSN_Z)

#define OCOPERAND_IVALUE0              (0x00000001)
#define OCOPERAND_UVALUE0              (0x00000002)
#define OCOPERAND_MVALUE0              (0x00000003)
#define OCOPERAND_REGISTER0            (0x0000000f)
#define OCOPERAND_IVALUE1              (0x00000010)
#define OCOPERAND_UVALUE1              (0x00000020)
#define OCOPERAND_MVALUE1              (0x00000030)
#define OCOPERAND_REGISTER1            (0x000000f0)
#define OCOPERAND_IVALUE2              (0x00000100)
#define OCOPERAND_UVALUE2              (0x00000200)
#define OCOPERAND_MVALUE2              (0x00000300)
#define OCOPERAND_REGISTER2            (0x00000f00)
#define OCOPERAND_IVALUE3              (0x00001000)
#define OCOPERAND_UVALUE3              (0x00002000)
#define OCOPERAND_MVALUE3              (0x00003000)
#define OCOPERAND_REGISTER3            (0x0000f000)
#define OCOPERAND_IVALUE4              (0x00010000)
#define OCOPERAND_UVALUE4              (0x00020000)
#define OCOPERAND_MVALUE4              (0x00030000)
#define OCOPERAND_REGISTER4            (0x000f0000)
#define OCOPERAND_IVALUE5              (0x00100000)
#define OCOPERAND_UVALUE5              (0x00200000)
#define OCOPERAND_MVALUE5              (0x00300000)
#define OCOPERAND_REGISTER5            (0x00f00000)
#define OCOPERAND_IVALUE6              (0x01000000)
#define OCOPERAND_UVALUE6              (0x02000000)
#define OCOPERAND_MVALUE6              (0x03000000)
#define OCOPERAND_REGISTER6            (0x0f000000)
#define OCOPERAND_IVALUE7              (0x10000000)
#define OCOPERAND_UVALUE7              (0x20000000)
#define OCOPERAND_MVALUE7              (0x30000000)
#define OCOPERAND_REGISTER7            (0xf0000000)

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

#define OCOPERAND_MVALUEMASK           (OCOPERAND_MVALUE0 | OCOPERAND_MVALUE1 | OCOPERAND_MVALUE2 \
                                         | OCOPERAND_MVALUE3 | OCOPERAND_MVALUE4 | OCOPERAND_MVALUE5 \
                                         | OCOPERAND_MVALUE6 | OCOPERAND_MVALUE7)

#define OCOPERAND_UVALUEMASK           (OCOPERAND_UVALUE0 | OCOPERAND_UVALUE1 | OCOPERAND_UVALUE2 \
                                         | OCOPERAND_UVALUE3 | OCOPERAND_UVALUE4 | OCOPERAND_UVALUE5 \
                                         | OCOPERAND_UVALUE6 | OCOPERAND_UVALUE7)

#define OCOPERAND_REGISTERMASK         (OCOPERAND_REGISTER0 | OCOPERAND_REGISTER1 | OCOPERAND_REGISTER2 \
                                         | OCOPERAND_REGISTER3 | OCOPERAND_REGISTER4 | OCOPERAND_REGISTER5 \
                                         | OCOPERAND_REGISTER6 | OCOPERAND_REGISTER7)

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
#define OCREGISTER_8BIT                U64MASK(55)
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
pconvert_t oegetOPERANDFLAGS(handle_t p);
pconvert_t oegetINSTRUCTIONFLAGS(handle_t p);
pconvert_t oegetINSTRUCTIONNAMES(handle_t p);
pconvert_t oegetREGISTERFLAGS(handle_t p);
pconvert_t oegetREGISTERNAMES(handle_t p);

#endif

