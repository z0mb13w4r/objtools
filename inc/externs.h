#ifndef __EXTERNS_H_
#define __EXTERNS_H_

#include "objutils.h"

extern pick_t zATE[];
extern pick_t zATDEC[];
extern pick_t zATDEC8[];
extern pick_t zATDEC16[];
extern pick_t zATHEX32[];
extern pick_t zATSRCFILE[];

extern pick_t zFORMBOOL[];
extern pick_t zFORMADDR[];
extern pick_t zFORMGREF[];
extern pick_t zFORMBLOCK[];
extern pick_t zFORMUDATA[];
extern pick_t zFORMOFFSET[];
extern pick_t zFORMSTRING[];

extern pick_t zMACRODEF[];
extern pick_t zMACRODEFSTR[];

extern pick_t ecDYNTAGNAME[];

extern pick_t oeADDRLOOKUP[];

extern convert_t zDWAT[];
extern convert_t zDWOP[];
extern convert_t zDWATE[];
extern convert_t zDWCFA[];
extern convert_t zDWTAG[];
extern convert_t zDWGNUM[];
extern convert_t zDWFORM[];
extern convert_t zDWLANG[];
extern convert_t zDWMACRO[];
extern convert_t zDWCHILDREN[];

extern convert_t zDWEXPR[];
extern convert_t zDWEXPRLITE[];

extern convert_t ecPHDRTYPE[];
extern convert_t zSHDRTYPE[];

extern convert_t oeSEGMENTFLAGS[];
extern convert_t oeREGISTERFLAGS[];
extern convert_t oeREGISTERNAMES[];
extern convert_t oeINSTRUCTIONFLAGS[];
extern convert_t oeINSTRUCTIONNAMES[];

extern convert_t ecDYNTAG[];

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

extern char zHEXDUMP[];
extern char zSTRDUMP[];
extern char zCODEDUMP[];

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

extern const char *zALLSEE[];
extern const char *zLICENSE[];

#endif

