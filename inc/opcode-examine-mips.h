#ifndef __OPCODE_EXAMINE_MIPS_H_
#define __OPCODE_EXAMINE_MIPS_H_

#define OCINSNMIPS_Bxx             (OCINSN_Bxx | OCINSN_OPERAND1 | OCINSN_OPERAND2 | OCINSN_OPERAND3)
#define OCINSNMIPS_Jxx             (OCINSN_Jxx | OCINSN_OPERAND1 | OCINSN_OPERAND2 | OCINSN_OPERAND3)
#define OCINSNMIPS_NOP             (OCINSN_NOP | OCINSN_OPERAND0)

#define OCINSNMIPS_LINK            (OCINSN_00)
#define OCINSNMIPS_REGISTER        (OCINSN_01)
#define OCINSNMIPS_IMMEDIATE       (OCINSN_02)

#define OCINSNMIPS_ZERO            (OCINSN_Z)

#define OCINSNMIPS_EQ              (OCINSN_E)
#define OCINSNMIPS_EQZ             (OCINSN_E | OCINSN_Z)
#define OCINSNMIPS_GE              (OCINSN_GE)
#define OCINSNMIPS_GEZ             (OCINSN_GE | OCINSN_Z)

#define OCINSNMIPS_BEQ             (OCINSNMIPS_Bxx | OCINSNMIPS_EQ)
#define OCINSNMIPS_BEQZ            (OCINSNMIPS_Bxx | OCINSNMIPS_EQZ)
#define OCINSNMIPS_BGEZ            (OCINSNMIPS_Bxx | OCINSNMIPS_GEZ)

#define OCINSNMIPS_JAL             (OCINSNMIPS_Jxx | OCINSNMIPS_LINK)
#define OCINSNMIPS_JALR            (OCINSNMIPS_JAL | OCINSNMIPS_REGISTER)
#define OCINSNMIPS_JR              (OCINSNMIPS_Jxx | OCINSNMIPS_REGISTER)

#define OCREGMIPS_R0               (0x00)
#define OCREGMIPS_R1               (0x01)
#define OCREGMIPS_R2               (0x02)
#define OCREGMIPS_R3               (0x03)
#define OCREGMIPS_R4               (0x04)
#define OCREGMIPS_R5               (0x05)
#define OCREGMIPS_R6               (0x06)
#define OCREGMIPS_R7               (0x07)
#define OCREGMIPS_R8               (0x08)
#define OCREGMIPS_R9               (0x09)
#define OCREGMIPS_R10              (0x0a)
#define OCREGMIPS_R11              (0x0b)
#define OCREGMIPS_R12              (0x0c)
#define OCREGMIPS_R13              (0x0d)
#define OCREGMIPS_R14              (0x0e)
#define OCREGMIPS_R15              (0x0f)
#define OCREGMIPS_R16              (0x10)
#define OCREGMIPS_R17              (0x11)
#define OCREGMIPS_R18              (0x12)
#define OCREGMIPS_R19              (0x13)
#define OCREGMIPS_R20              (0x14)
#define OCREGMIPS_R21              (0x15)
#define OCREGMIPS_R22              (0x16)
#define OCREGMIPS_R23              (0x17)
#define OCREGMIPS_R24              (0x18)
#define OCREGMIPS_R25              (0x19)
#define OCREGMIPS_R26              (0x1a)
#define OCREGMIPS_R27              (0x1b)
#define OCREGMIPS_R28              (0x1c)
#define OCREGMIPS_R29              (0x1d)
#define OCREGMIPS_R30              (0x1e)
#define OCREGMIPS_R31              (0x1f)

#endif

