#ifndef __OPCODE_EXAMINE_H_
#define __OPCODE_EXAMINE_H_

#include "opcode.h"

#define OECODE_THIS                    (0)
#define OECODE_MNEMONIC                (1)
#define OECODE_OPERAND1                (2)
#define OECODE_OPERAND2                (3)

#define OCINSTRUCTION_8BIT             U64MASK(56)
#define OCINSTRUCTION_16BIT            U64MASK(57)
#define OCINSTRUCTION_32BIT            U64MASK(58)
#define OCINSTRUCTION_64BIT            U64MASK(59)
#define OCINSTRUCTION_OPERAND0         U64MASK(60)
#define OCINSTRUCTION_OPERAND1         U64MASK(61)
#define OCINSTRUCTION_OPERAND2         U64MASK(62)

#define OCINSTRUCTION_CALL             ((0x01) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_CALLB            (OCINSTRUCTION_CALL | OCINSTRUCTION_8BIT)
#define OCINSTRUCTION_CALLL            (OCINSTRUCTION_CALL | OCINSTRUCTION_32BIT)
#define OCINSTRUCTION_CALLQ            (OCINSTRUCTION_CALL | OCINSTRUCTION_64BIT)
#define OCINSTRUCTION_JMP              ((0x2) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_JMPQ             (OCINSTRUCTION_JMP | OCINSTRUCTION_64BIT)
#define OCINSTRUCTION_NOP0             ((0x03) | OCINSTRUCTION_OPERAND0)
#define OCINSTRUCTION_NOP1             ((0x04) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_ENDBR64          ((0x05) | OCINSTRUCTION_OPERAND0 | OCINSTRUCTION_64BIT)
#define OCINSTRUCTION_RET              ((0x06) | OCINSTRUCTION_OPERAND0)
#define OCINSTRUCTION_RETB             (OCINSTRUCTION_RET | OCINSTRUCTION_8BIT)
#define OCINSTRUCTION_RETL             (OCINSTRUCTION_RET | OCINSTRUCTION_32BIT)
#define OCINSTRUCTION_RETQ             (OCINSTRUCTION_RET | OCINSTRUCTION_64BIT)
#define OCINSTRUCTION_PUSH             ((0x07) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_PUSHB            (OCINSTRUCTION_PUSH | OCINSTRUCTION_8BIT)
#define OCINSTRUCTION_PUSHL            (OCINSTRUCTION_PUSH | OCINSTRUCTION_32BIT)
#define OCINSTRUCTION_PUSHQ            (OCINSTRUCTION_PUSH | OCINSTRUCTION_64BIT)
#define OCINSTRUCTION_POP              ((0x08) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_ADD              ((0x09) | OCINSTRUCTION_OPERAND2)
#define OCINSTRUCTION_SUB              ((0x0a) | OCINSTRUCTION_OPERAND2)
#define OCINSTRUCTION_AND              ((0x0b) | OCINSTRUCTION_OPERAND2)
#define OCINSTRUCTION_MOV              ((0x0c) | OCINSTRUCTION_OPERAND2)
#define OCINSTRUCTION_MOVB             (OCINSTRUCTION_MOV | OCINSTRUCTION_8BIT)
#define OCINSTRUCTION_MOVL             (OCINSTRUCTION_MOV | OCINSTRUCTION_32BIT)
#define OCINSTRUCTION_MOVQ             (OCINSTRUCTION_MOV | OCINSTRUCTION_64BIT)

#define OCOPERAND_IVALUE               (0x01)
#define OCOPERAND_UVALUE               (0x02)
#define OPOPERAND_REGISTER             (0x03)
#define OPOPERAND_SEGMENT              U64MASK(55)
#define OCOPERAND_ABSOLUTE             U64MASK(62)

#define OPSEGMENT_CS                   (U64MASK(56) | OPOPERAND_SEGMENT)
#define OPSEGMENT_DS                   (U64MASK(57) | OPOPERAND_SEGMENT)
#define OPSEGMENT_SS                   (U64MASK(58) | OPOPERAND_SEGMENT)
#define OPSEGMENT_ES                   (U64MASK(59) | OPOPERAND_SEGMENT)
#define OPSEGMENT_GS                   (U64MASK(60) | OPOPERAND_SEGMENT)
#define OPSEGMENT_FS                   (U64MASK(61) | OPOPERAND_SEGMENT)

#define OCREGISTER_GENERAL             U64MASK(48)
#define OCREGISTER_INSTRUCTIONPTR      U64MASK(49)
#define OCREGISTER_BASEPTR             U64MASK(50)
#define OCREGISTER_STACKPTR            U64MASK(51)
#define OCREGISTER_DSTINDEX            U64MASK(52)
#define OCREGISTER_SRCINDEX            U64MASK(53)
#define OCREGISTER_DATA                U64MASK(54)
#define OCREGISTER_COUNTER             U64MASK(55)
#define OCREGISTER_BASE                U64MASK(56)
#define OCREGISTER_ACCUMULATOR         U64MASK(57)
#define OCREGISTER_8BITLO              U64MASK(58)
#define OCREGISTER_8BITHI              U64MASK(59)
#define OCREGISTER_16BIT               U64MASK(60)
#define OCREGISTER_32BIT               U64MASK(61)
#define OCREGISTER_64BIT               U64MASK(62)

#define OCREGISTER_RAX                 ((0x01) | OCREGISTER_64BIT  | OCREGISTER_ACCUMULATOR)
#define OCREGISTER_EAX                 ((0x02) | OCREGISTER_32BIT  | OCREGISTER_ACCUMULATOR)
#define OCREGISTER_AX                  ((0x03) | OCREGISTER_16BIT  | OCREGISTER_ACCUMULATOR)
#define OCREGISTER_AH                  ((0x04) | OCREGISTER_8BITHI | OCREGISTER_ACCUMULATOR)
#define OCREGISTER_AL                  ((0x05) | OCREGISTER_8BITLO | OCREGISTER_ACCUMULATOR)

#define OCREGISTER_RBX                 ((0x06) | OCREGISTER_64BIT  | OCREGISTER_BASE)
#define OCREGISTER_EBX                 ((0x07) | OCREGISTER_32BIT  | OCREGISTER_BASE)
#define OCREGISTER_BX                  ((0x08) | OCREGISTER_16BIT  | OCREGISTER_BASE)
#define OCREGISTER_BH                  ((0x09) | OCREGISTER_8BITHI | OCREGISTER_BASE)
#define OCREGISTER_BL                  ((0x0a) | OCREGISTER_8BITLO | OCREGISTER_BASE)

#define OCREGISTER_RCX                 ((0x0b) | OCREGISTER_64BIT  | OCREGISTER_COUNTER)
#define OCREGISTER_ECX                 ((0x0c) | OCREGISTER_32BIT  | OCREGISTER_COUNTER)
#define OCREGISTER_CX                  ((0x0d) | OCREGISTER_16BIT  | OCREGISTER_COUNTER)
#define OCREGISTER_CH                  ((0x0e) | OCREGISTER_8BITHI | OCREGISTER_COUNTER)
#define OCREGISTER_CL                  ((0x0f) | OCREGISTER_8BITLO | OCREGISTER_COUNTER)

#define OCREGISTER_RDX                 ((0x10) | OCREGISTER_64BIT  | OCREGISTER_DATA)
#define OCREGISTER_EDX                 ((0x11) | OCREGISTER_32BIT  | OCREGISTER_DATA)
#define OCREGISTER_DX                  ((0x12) | OCREGISTER_16BIT  | OCREGISTER_DATA)
#define OCREGISTER_DH                  ((0x13) | OCREGISTER_8BITHI | OCREGISTER_DATA)
#define OCREGISTER_DL                  ((0x14) | OCREGISTER_8BITLO | OCREGISTER_DATA)

#define OCREGISTER_R8B                 ((0x15) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R8D                 ((0x16) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R8W                 ((0x17) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R8                  ((0x18) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_R9B                 ((0x19) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R9D                 ((0x1a) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R9W                 ((0x1b) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R9                  ((0x1c) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_R10B                ((0x1d) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R10D                ((0x1e) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R10W                ((0x1f) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R10                 ((0x20) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_R11B                ((0x21) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R11D                ((0x22) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R11W                ((0x23) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R11                 ((0x24) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_R12B                ((0x25) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R12D                ((0x26) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R12W                ((0x27) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R12                 ((0x28) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_R13B                ((0x29) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R13D                ((0x2a) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R13W                ((0x2b) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R13                 ((0x2c) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_R14B                ((0x2d) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R14D                ((0x2e) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R14W                ((0x2f) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R14                 ((0x30) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_R15B                ((0x31) | OCREGISTER_8BITLO | OCREGISTER_GENERAL)
#define OCREGISTER_R15D                ((0x32) | OCREGISTER_32BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R15W                ((0x33) | OCREGISTER_16BIT  | OCREGISTER_GENERAL)
#define OCREGISTER_R15                 ((0x34) | OCREGISTER_64BIT  | OCREGISTER_GENERAL)

#define OCREGISTER_RSI                 ((0x35) | OCREGISTER_64BIT  | OCREGISTER_SRCINDEX)
#define OCREGISTER_ESI                 ((0x36) | OCREGISTER_32BIT  | OCREGISTER_SRCINDEX)
#define OCREGISTER_SIL                 ((0x37) | OCREGISTER_8BITLO | OCREGISTER_SRCINDEX)
#define OCREGISTER_SI                  ((0x38) | OCREGISTER_16BIT  | OCREGISTER_SRCINDEX)

#define OCREGISTER_RDI                 ((0x39) | OCREGISTER_64BIT  | OCREGISTER_DSTINDEX)
#define OCREGISTER_EDI                 ((0x3a) | OCREGISTER_32BIT  | OCREGISTER_DSTINDEX)
#define OCREGISTER_DIL                 ((0x3b) | OCREGISTER_8BITLO | OCREGISTER_DSTINDEX)
#define OCREGISTER_DI                  ((0x3c) | OCREGISTER_16BIT  | OCREGISTER_DSTINDEX)

#define OCREGISTER_RSP                 ((0x3d) | OCREGISTER_64BIT  | OCREGISTER_STACKPTR)
#define OCREGISTER_ESP                 ((0x3e) | OCREGISTER_32BIT  | OCREGISTER_STACKPTR)
#define OCREGISTER_SPL                 ((0x3f) | OCREGISTER_8BITLO | OCREGISTER_STACKPTR)
#define OCREGISTER_SP                  ((0x40) | OCREGISTER_16BIT  | OCREGISTER_STACKPTR)

#define OCREGISTER_RBP                 ((0x41) | OCREGISTER_64BIT  | OCREGISTER_BASEPTR)
#define OCREGISTER_EBP                 ((0x42) | OCREGISTER_32BIT  | OCREGISTER_BASEPTR)
#define OCREGISTER_BPL                 ((0x43) | OCREGISTER_8BITLO | OCREGISTER_BASEPTR)
#define OCREGISTER_BP                  ((0x44) | OCREGISTER_16BIT  | OCREGISTER_BASEPTR)

#define OCREGISTER_RIP                 ((0x45) | OCREGISTER_64BIT  | OCREGISTER_INSTRUCTIONPTR)
#define OCREGISTER_EIP                 ((0x46) | OCREGISTER_32BIT  | OCREGISTER_INSTRUCTIONPTR)
#define OCREGISTER_IP                  ((0x47) | OCREGISTER_16BIT  | OCREGISTER_INSTRUCTIONPTR)

#define MODE_OCEXAMINE                 (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('E'))

typedef struct ocmnemonic_s {
  char       data[160];
} ocmnemonic_t, *pocmnemonic_t;

typedef struct ocoperand_s {
  char       data[160];
  uint64_t   cvalue;
  union {
    int64_t  ivalue;
    uint64_t uvalue;
  };
} ocoperand_t, *pocoperand_t;

typedef struct ocexamine_s {
  smode_t    mode;
  uint64_t   vaddr;
  char       comment[160];

  pocmnemonic_t mc;
  pocoperand_t  op1;
  pocoperand_t  op2;
} ocexamine_t, *pocexamine_t;

bool_t isocexamine(handle_t p);

handle_t oemalloc();
handle_t oefree(handle_t p);
handle_t oecreate(handle_t p, const uint64_t vaddr, unknown_t mnemonic, unknown_t operands);

unknown_t oeget(handle_t p, const imode_t mode);

unknown_t oeskip(unknown_t p, const size_t size);
unknown_t oeinsert_comment(handle_t p, unknown_t m);
unknown_t oeinsert_operand(handle_t p, unknown_t q, unknown_t m);
unknown_t oeinsert_operands(handle_t p, unknown_t q, unknown_t m);
unknown_t oeinsert_mnemonic(handle_t p, unknown_t q, unknown_t m);

#endif

