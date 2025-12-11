#ifndef __OPCODE_EXAMINE_ARM_H_
#define __OPCODE_EXAMINE_ARM_H_

#define OCINSNARM_PUSH             ((0x0b) | OCINSN_OPERAND1)
#define OCINSNARM_POP              ((0x0c) | OCINSN_OPERAND1)
#define OCINSNARM_ADD              ((0x0d) | OCINSN_OPERAND2)
#define OCINSNARM_SUB              ((0x18) | OCINSN_OPERAND2)
#define OCINSNARM_LDR              ((0x20) | OCINSN_OPERAND2)

#endif

