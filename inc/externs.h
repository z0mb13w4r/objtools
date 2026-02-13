#ifndef __EXTERNS_H_
#define __EXTERNS_H_

#include "objutils.h"

extern pick_t ecATE[];
extern pick_t ecATDEC[];
extern pick_t ecATDEC8[];
extern pick_t ecATDEC16[];
extern pick_t ecATHEX32[];
extern pick_t ecATSRCFILE[];

extern pick_t ecFORMBOOL[];
extern pick_t ecFORMADDR[];
extern pick_t ecFORMGREF[];
extern pick_t ecFORMBLOCK[];
extern pick_t ecFORMUDATA[];
extern pick_t ecFORMOFFSET[];
extern pick_t ecFORMSTRING[];

extern pick_t ecMACRODEF[];
extern pick_t ecMACRODEFSTR[];

extern pick_t ecDYNTAGNAME[];

extern convert_t ecDWAT[];
extern convert_t ecDWOP[];
extern convert_t ecDWUT[];
extern convert_t ecDWEND[];
extern convert_t ecDWLLE[];
extern convert_t ecDWLNE[];
extern convert_t ecDWLNS[];
extern convert_t ecDWRLE[];
extern convert_t ecDWTAG[];
extern convert_t ecDWGNUM[];
extern convert_t ecDWFORM[];
extern convert_t ecDWLANG[];
extern convert_t ecDWADDR[];
extern convert_t ecDWMACRO[];
extern convert_t ecDWMACINFO[];

extern convert_t ecDWCFA[];
extern convert_t ecDWCFAJOIN[];
extern convert_t ecDWCFALITE[];
extern convert_t ecDWATE[];
extern convert_t ecDWATELITE[];
extern convert_t ecDWEXPR[];
extern convert_t ecDWEXPRLITE[];
extern convert_t ecDWCHILDREN[];
extern convert_t ecDWCHILDRENLITE[];

extern convert_t oeSEGMENTFLAGS_x86_64[];

extern convert_t ecHAS_FLAGS[];

extern convert_t zBFDSYMBOL_FLAGS[];

extern convert_t zSECTIONHDR1_FLAGS[];
extern convert_t zSECTIONHDR2_FLAGS[];
extern convert_t zSECTIONHDR3_FLAGS[];
extern convert_t zSECTIONHDRLINKDUP[];
extern convert_t zSECTIONHDRELF_FLAGS[];
extern convert_t zSECTIONHDRCOFF_FLAGS[];
extern convert_t zSECTIONHDRTIC54X_FLAGS[];
extern convert_t zSECTIONHDRARCHMEP_FLAGS[];

extern convert_t ecREGISTERS_i386[];
extern convert_t ecREGISTERS_x86_64[];
extern convert_t ecREGISTERS_ARM32[];
extern convert_t ecREGISTERS_ARM64[];
extern convert_t ecREGISTERS_RISCV32[];
extern convert_t ecREGISTERS_RISCV64[];

extern args_t zSCRIPTCOMMANDS[];
extern args_t zSCRIPTCOMMANDSEX[];

extern char zHEXDUMP[];
extern char zSTRDUMP[];
extern char zCODEDUMP[];
extern char zSCRIPTING[];

extern char zDEBUGABBREV[];
extern char zDEBUGADDR[];
extern char zDEBUGCUINDEX[];
extern char zDEBUGFRAME[];
extern char zDEBUGFRAMEINTERP[];
extern char zDEBUGINFO[];
extern char zDEBUGLINKS[];
extern char zDEBUGMACRO[];
extern char zDEBUGLOC[];
extern char zDEBUGARANGES[];
extern char zDEBUGRANGES[];
extern char zDEBUGSTR[];
extern char zDEBUGGDBINDEX[];
extern char zDEBUGLINKSFOLLOW[];
extern char zDEBUGLINE[];
extern char zDEBUGLINEDECODED[];
extern char zDEBUGPUBNAMES[];
extern char zDEBUGPUBTYPES[];
extern char zDEBUGTRACEARANGES[];
extern char zDEBUGTRACEABBREV[];
extern char zDEBUGTRACEINFO[];

extern char zDISASSEMBLEARCH[];
extern char zDISASSEMBLEATTSYNTAX[];
extern char zDISASSEMBLEINTELSYNTAX[];
extern char zDISASSEMBLEISA[];
extern char zDISASSEMBLEDATA[];

extern const char zDESCRIPTION[];
extern const char zDESCRIPTIONPE[];
extern const char zDESCRIPTIONELF[];

extern const char *zALLSEE[];
extern const char *zLICENSE[];

#endif

