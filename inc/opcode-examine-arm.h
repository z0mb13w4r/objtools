#ifndef __OPCODE_EXAMINE_ARM_H_
#define __OPCODE_EXAMINE_ARM_H_

#define OCINSNARM_BR               ((0x04) | OCINSN_OPERAND1)

#define OCINSNARM_PUSH             ((0x0b) | OCINSN_OPERAND1)
#define OCINSNARM_POP              ((0x0c) | OCINSN_OPERAND1)
#define OCINSNARM_ADD              ((0x0d) | OCINSN_OPERAND2)
#define OCINSNARM_SUB              ((0x18) | OCINSN_OPERAND2)
#define OCINSNARM_LDR              ((0x20) | OCINSN_OPERAND2)

#define OCINSNARM_LINK             U64MASK(16)

#define OCINSNARM_BEQ              (OCINSNARM_BR | OCINSN_E)
#define OCINSNARM_BGE              (OCINSNARM_BR | OCINSN_GE)
#define OCINSNARM_BGT              (OCINSNARM_BR | OCINSN_G)
#define OCINSNARM_BLE              (OCINSNARM_BR | OCINSN_LE)
#define OCINSNARM_BNE              (OCINSNARM_BR | OCINSN_NE)

#define OCINSNARM_BL               (OCINSNARM_BR | OCINSNARM_LINK)
#define OCINSNARM_BLEQ             (OCINSNARM_BL | OCINSN_E)
#define OCINSNARM_BLGE             (OCINSNARM_BL | OCINSN_GE)
#define OCINSNARM_BLGT             (OCINSNARM_BL | OCINSN_G)
#define OCINSNARM_BLLE             (OCINSNARM_BL | OCINSN_LE)
#define OCINSNARM_BLNE             (OCINSNARM_BL | OCINSN_NE)

#endif

