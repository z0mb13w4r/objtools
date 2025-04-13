#ifndef __PECODE_H_
#define __PECODE_H_

#include "buffer.h"

#define SECTION_DATA                             ".data"
#define SECTION_PDATA                            ".pdata"
#define SECTION_RDATA                            ".rdata"
#define SECTION_RELOC                            ".reloc"
#define SECTION_RSRC                             ".rsrc"
#define SECTION_TEXT                             ".text"

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES         (16)
#define IMAGE_SIZEOF_SHORT_NAME                  (8)

#define IMAGE_DIRECTORY_ENTRY_EXPORT             (0)
#define IMAGE_DIRECTORY_ENTRY_IMPORT             (1)
#define IMAGE_DIRECTORY_ENTRY_RESOURCE           (2)
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION          (3)
#define IMAGE_DIRECTORY_ENTRY_SECURITY           (4)
#define IMAGE_DIRECTORY_ENTRY_BASERELOC          (5)
#define IMAGE_DIRECTORY_ENTRY_DEBUG              (6)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE       (7)
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR          (8)
#define IMAGE_DIRECTORY_ENTRY_TLS                (9)
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG        (10)
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT       (11)
#define IMAGE_DIRECTORY_ENTRY_IAT                (12)
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT       (13)
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR     (14)
#define IMAGE_DIRECTORY_ENTRY_RESERVED           (15)
#define IMAGE_DIRECTORY_ENTRY_UNKNOWN            (16)

#define IMAGE_RESOURCE_NAME_IS_STRING            (0x80000000)
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY         (0x80000000)

#define CV_SIGNATURE_NB10                        (0x3031424e)
#define CV_SIGNATURE_RSDS                        (0x53445352)

#define RT_CURSOR                                (1)
#define RT_BITMAP                                (2)
#define RT_ICON                                  (3)
#define RT_MENU                                  (4)
#define RT_DIALOG                                (5)
#define RT_STRING                                (6)
#define RT_FONTDIR                               (7)
#define RT_FONT                                  (8)
#define RT_ACCELERATOR                           (9)
#define RT_RCDATA                                (10)
#define RT_MESSAGETABLE                          (11)
#define RT_GROUP_CURSOR                          (RT_CURSOR + 11)
#define RT_GROUP_ICON                            (RT_ICON + 11)
#define RT_VERSION                               (16)
#define RT_DLGINCLUDE                            (17)
#define RT_PLUGPLAY                              (19)
#define RT_VXD                                   (20)
#define RT_ANICURSOR                             (21)
#define RT_ANIICON                               (22)
#define RT_HTML                                  (23)
#define RT_MANIFEST                              (24)

#define peconvert2va(x,y)                        ((y) - (x)->VirtualAddress + (x)->PointerToRawData)

typedef BYTE GUID[16];

typedef struct _IMAGE_DOS_HEADER {
  WORD e_magic;
  WORD e_cblp;
  WORD e_cp;
  WORD e_crlc;
  WORD e_cparhdr;
  WORD e_minalloc;
  WORD e_maxalloc;
  WORD e_ss;
  WORD e_sp;
  WORD e_csum;
  WORD e_ip;
  WORD e_cs;
  WORD e_lfarlc;
  WORD e_ovno;
  WORD e_res[4];
  WORD e_oemid;
  WORD e_oeminfo;
  WORD e_res2[10];
  LONG e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
  WORD  Machine;
  WORD  NumberOfSections;
  DWORD TimeDateStamp;
  DWORD PointerToSymbolTable;
  DWORD NumberOfSymbols;
  WORD  SizeOfOptionalHeader;
  WORD  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
  DWORD VirtualAddress;
  DWORD Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER32 {
  WORD                 Magic;
  BYTE                 MajorLinkerVersion;
  BYTE                 MinorLinkerVersion;
  DWORD                SizeOfCode;
  DWORD                SizeOfInitializedData;
  DWORD                SizeOfUninitializedData;
  DWORD                AddressOfEntryPoint;
  DWORD                BaseOfCode;
  DWORD                BaseOfData;
  DWORD                ImageBase;
  DWORD                SectionAlignment;
  DWORD                FileAlignment;
  WORD                 MajorOperatingSystemVersion;
  WORD                 MinorOperatingSystemVersion;
  WORD                 MajorImageVersion;
  WORD                 MinorImageVersion;
  WORD                 MajorSubsystemVersion;
  WORD                 MinorSubsystemVersion;
  DWORD                Win32VersionValue;
  DWORD                SizeOfImage;
  DWORD                SizeOfHeaders;
  DWORD                CheckSum;
  WORD                 Subsystem;
  WORD                 DllCharacteristics;
  DWORD                SizeOfStackReserve;
  DWORD                SizeOfStackCommit;
  DWORD                SizeOfHeapReserve;
  DWORD                SizeOfHeapCommit;
  DWORD                LoaderFlags;
  DWORD                NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
  WORD                 Magic;
  BYTE                 MajorLinkerVersion;
  BYTE                 MinorLinkerVersion;
  DWORD                SizeOfCode;
  DWORD                SizeOfInitializedData;
  DWORD                SizeOfUninitializedData;
  DWORD                AddressOfEntryPoint;
  DWORD                BaseOfCode;
  ULONGLONG            ImageBase;
  DWORD                SectionAlignment;
  DWORD                FileAlignment;
  WORD                 MajorOperatingSystemVersion;
  WORD                 MinorOperatingSystemVersion;
  WORD                 MajorImageVersion;
  WORD                 MinorImageVersion;
  WORD                 MajorSubsystemVersion;
  WORD                 MinorSubsystemVersion;
  DWORD                Win32VersionValue;
  DWORD                SizeOfImage;
  DWORD                SizeOfHeaders;
  DWORD                CheckSum;
  WORD                 Subsystem;
  WORD                 DllCharacteristics;
  ULONGLONG            SizeOfStackReserve;
  ULONGLONG            SizeOfStackCommit;
  ULONGLONG            SizeOfHeapReserve;
  ULONGLONG            SizeOfHeapCommit;
  DWORD                LoaderFlags;
  DWORD                NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS {
  DWORD                   Signature;
  IMAGE_FILE_HEADER       FileHeader;
  IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef struct _IMAGE_NT_HEADERS64 {
  DWORD                   Signature;
  IMAGE_FILE_HEADER       FileHeader;
  IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_SECTION_HEADER {
  BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    DWORD PhysicalAddress;
    DWORD VirtualSize;
  } Misc;
  DWORD   VirtualAddress;
  DWORD   SizeOfRawData;
  DWORD   PointerToRawData;
  DWORD   PointerToRelocations;
  DWORD   PointerToLinenumbers;
  WORD    NumberOfRelocations;
  WORD    NumberOfLinenumbers;
  DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_EXPORT_DIRECTORY {
  DWORD Characteristics;
  DWORD TimeDateStamp;
  WORD  MajorVersion;
  WORD  MinorVersion;
  DWORD Name;
  DWORD Base;
  DWORD NumberOfFunctions;
  DWORD NumberOfNames;
  DWORD AddressOfFunctions;
  DWORD AddressOfNames;
  DWORD AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
  union {
    DWORD Characteristics;
    DWORD OriginalFirstThunk;
  };
  DWORD   TimeDateStamp;
  DWORD   ForwarderChain;
  DWORD   Name;
  DWORD   FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR, *PIMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_IMPORT_BY_NAME {
  WORD Hint;
  CHAR Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA32 {
  union {
    DWORD ForwarderString;
    DWORD Function;
    DWORD Ordinal;
    DWORD AddressOfData;
  };
} IMAGE_THUNK_DATA32, *PIMAGE_THUNK_DATA32;

typedef struct _IMAGE_THUNK_DATA64 {
  union {
    ULONGLONG ForwarderString;
    ULONGLONG Function;
    ULONGLONG Ordinal;
    ULONGLONG AddressOfData;
  };
} IMAGE_THUNK_DATA64, *PIMAGE_THUNK_DATA64;

typedef struct _IMAGE_RESOURCE_DIRECTORY {
  DWORD Characteristics;
  DWORD TimeDateStamp;
  WORD  MajorVersion;
  WORD  MinorVersion;
  WORD  NumberOfNamedEntries;
  WORD  NumberOfIdEntries;
} IMAGE_RESOURCE_DIRECTORY, *PIMAGE_RESOURCE_DIRECTORY;

typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY {
  union {
    struct {
      DWORD NameOffset:31;
      DWORD NameIsString:1;
    };
    DWORD Name;
    WORD  Id;
  };
  union {
    DWORD OffsetToData;
    struct {
      DWORD OffsetToDirectory:31;
      DWORD DataIsDirectory:1;
    };
  };
} IMAGE_RESOURCE_DIRECTORY_ENTRY, *PIMAGE_RESOURCE_DIRECTORY_ENTRY;

typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING {
  WORD Length;
  CHAR NameString[1];
} IMAGE_RESOURCE_DIRECTORY_STRING, *PIMAGE_RESOURCE_DIRECTORY_STRING;

typedef struct _IMAGE_RESOURCE_DATA_ENTRY {
  DWORD OffsetToData;
  DWORD Size;
  DWORD CodePage;
  DWORD Reserved;
} IMAGE_RESOURCE_DATA_ENTRY, *PIMAGE_RESOURCE_DATA_ENTRY;

typedef struct _IMAGE_LOAD_CONFIG_CODE_INTEGRITY {
  WORD  Flags;
  WORD  Catalog;
  DWORD CatalogOffset;
  DWORD Reserved;
} IMAGE_LOAD_CONFIG_CODE_INTEGRITY;

typedef struct _IMAGE_LOAD_CONFIG_DIRECTORY32 {
  DWORD                            Size;
  DWORD                            TimeDateStamp;
  WORD                             MajorVersion;
  WORD                             MinorVersion;
  DWORD                            GlobalFlagsClear;
  DWORD                            GlobalFlagsSet;
  DWORD                            CriticalSectionDefaultTimeout;
  DWORD                            DeCommitFreeBlockThreshold;
  DWORD                            DeCommitTotalFreeThreshold;
  DWORD                            LockPrefixTable;
  DWORD                            MaximumAllocationSize;
  DWORD                            VirtualMemoryThreshold;
  DWORD                            ProcessHeapFlags;
  DWORD                            ProcessAffinityMask;
  WORD                             CSDVersion;
  WORD                             Reserved1;
  DWORD                            EditList;
  DWORD                            SecurityCookie;
  DWORD                            SEHandlerTable;
  DWORD                            SEHandlerCount;
  // Fields available in v8.1+ of the Windows SDK.
  DWORD                            GuardCFCheckFunctionPointer;
  DWORD                            GuardCFDispatchFunctionPointer;
  DWORD                            GuardCFFunctionTable;
  DWORD                            GuardCFFunctionCount;
  DWORD                            GuardFlags;
  // Fields available in v10.0.10586.0+ of the Windows SDK.
  IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
  DWORD                            GuardAddressTakenIatEntryTable;
  DWORD                            GuardAddressTakenIatEntryCount;
  DWORD                            GuardLongJumpTargetTable;
  DWORD                            GuardLongJumpTargetCount;
  DWORD                            DynamicValueRelocTable;
  DWORD                            CHPEMetadataPointer;
  // Fields available in the v10.0.15063.468+ of the SDK.
  DWORD                            GuardRFFailureRoutine;
  DWORD                            GuardRFFailureRoutineFunctionPointer;
  DWORD                            DynamicValueRelocTableOffset;
  WORD                             DynamicValueRelocTableSection;
  WORD                             Reserved2;
  DWORD                            GuardRFVerifyStackPointerFunctionPointer;
  DWORD                            HotPatchTableOffset;

  DWORD                            Reserved3;
  DWORD                            EnclaveConfigurationPointer;
  DWORD                            VolatileMetadataPointer;
  DWORD                            GuardEHContinuationTable;
  DWORD                            GuardEHContinuationCount;
  DWORD                            GuardXFGCheckFunctionPointer;
  DWORD                            GuardXFGDispatchFunctionPointer;
  DWORD                            GuardXFGTableDispatchFunctionPointer;
  DWORD                            CastGuardOsDeterminedFailureMode;
  DWORD                            GuardMemcpyFunctionPointer;
} IMAGE_LOAD_CONFIG_DIRECTORY32, *PIMAGE_LOAD_CONFIG_DIRECTORY32;

typedef struct _IMAGE_LOAD_CONFIG_DIRECTORY64 {
  DWORD                            Size;
  DWORD                            TimeDateStamp;
  WORD                             MajorVersion;
  WORD                             MinorVersion;
  DWORD                            GlobalFlagsClear;
  DWORD                            GlobalFlagsSet;
  DWORD                            CriticalSectionDefaultTimeout;
  ULONGLONG                        DeCommitFreeBlockThreshold;
  ULONGLONG                        DeCommitTotalFreeThreshold;
  ULONGLONG                        LockPrefixTable;
  ULONGLONG                        MaximumAllocationSize;
  ULONGLONG                        VirtualMemoryThreshold;
  ULONGLONG                        ProcessAffinityMask;
  DWORD                            ProcessHeapFlags;
  WORD                             CSDVersion;
  WORD                             Reserved1;
  ULONGLONG                        EditList;
  ULONGLONG                        SecurityCookie;
  ULONGLONG                        SEHandlerTable;
  ULONGLONG                        SEHandlerCount;
  // Fields available in v8.1+ of the Windows SDK.
  ULONGLONG                        GuardCFCheckFunctionPointer;
  ULONGLONG                        GuardCFDispatchFunctionPointer;
  ULONGLONG                        GuardCFFunctionTable;
  ULONGLONG                        GuardCFFunctionCount;
  DWORD                            GuardFlags;
  // Fields available in v10.0.10586.0+ of the Windows SDK.
  IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
  ULONGLONG                        GuardAddressTakenIatEntryTable;
  ULONGLONG                        GuardAddressTakenIatEntryCount;
  ULONGLONG                        GuardLongJumpTargetTable;
  ULONGLONG                        GuardLongJumpTargetCount;
  ULONGLONG                        DynamicValueRelocTable;
  ULONGLONG                        CHPEMetadataPointer;
  // Fields available in the v10.0.15063.468+ of the SDK.
  ULONGLONG                        GuardRFFailureRoutine;
  ULONGLONG                        GuardRFFailureRoutineFunctionPointer;
  DWORD                            DynamicValueRelocTableOffset;
  WORD                             DynamicValueRelocTableSection;
  WORD                             Reserved2;
  ULONGLONG                        GuardRFVerifyStackPointerFunctionPointer;
  DWORD                            HotPatchTableOffset;

  DWORD                            Reserved3;
  ULONGLONG                        EnclaveConfigurationPointer;
  ULONGLONG                        VolatileMetadataPointer;
  ULONGLONG                        GuardEHContinuationTable;
  ULONGLONG                        GuardEHContinuationCount;
  ULONGLONG                        GuardXFGCheckFunctionPointer;
  ULONGLONG                        GuardXFGDispatchFunctionPointer;
  ULONGLONG                        GuardXFGTableDispatchFunctionPointer;
  ULONGLONG                        CastGuardOsDeterminedFailureMode;
  ULONGLONG                        GuardMemcpyFunctionPointer;
} IMAGE_LOAD_CONFIG_DIRECTORY64, *PIMAGE_LOAD_CONFIG_DIRECTORY64;

typedef struct _IMAGE_DEBUG_DIRECTORY {
  DWORD Characteristics;
  DWORD TimeDateStamp;
  WORD  MajorVersion;
  WORD  MinorVersion;
  DWORD Type;
  DWORD SizeOfData;
  DWORD AddressOfRawData;
  DWORD PointerToRawData;
} IMAGE_DEBUG_DIRECTORY, *PIMAGE_DEBUG_DIRECTORY;

typedef struct _CV_HEADER {
  DWORD Signature;
  DWORD Offset;
} CV_HEADER, *PCV_HEADER;

typedef struct _CV_INFO_PDB20 {
  CV_HEADER CvHeader;
  DWORD     Signature;
  DWORD     Age;
  BYTE      PdbFileName[];
} CV_INFO_PDB20, *PCV_INFO_PDB20;

typedef struct _CV_INFO_PDB70 {
  DWORD CvSignature;
  GUID  Signature;
  DWORD Age;
  BYTE  PdbFileName[];
} CV_INFO_PDB70, *PCV_INFO_PDB70;

typedef struct _IMAGE_BASE_RELOCATION {
  DWORD VirtualAddress;
  DWORD SizeOfBlock;
} IMAGE_BASE_RELOCATION, *PIMAGE_BASE_RELOCATION;

typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY {
  DWORD BeginAddress;
  DWORD EndAddress;
  union {
    DWORD UnwindInfoAddress;
    DWORD UnwindData;
  };
} IMAGE_RUNTIME_FUNCTION_ENTRY, *PIMAGE_RUNTIME_FUNCTION_ENTRY;

typedef struct _UNWIND_CODE {
  BYTE OffsetProlog;
  BYTE UnwindCode:4;
  BYTE OpInfo:4;
} UNWIND_CODE, *PUNWIND_CODE;

typedef struct _UNWIND_INFO {
  BYTE Version:3;
  BYTE Flags:5;
  BYTE SizeOfProlog;
  BYTE CountOfCodes;
  BYTE FrameRegister:4;
  BYTE FrameOffset:4;
  UNWIND_CODE UnwindCode[1];
/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes + 1) & ~1) - 1];
 *  union {
 *    If (Flags & UNW_FLAG_EHANDLER)
 *    OPTIONAL ULONG ExceptionHandler;
 *    Else if (Flags & UNW_FLAG_CHAININFO)
 *    OPTIONAL ULONG FunctionEntry;
 *  };
 *  If (Flags & UNW_FLAG_EHANDLER)
 *  OPTIONAL ULONG ExceptionData[]; */
} UNWIND_INFO, *PUNWIND_INFO;

typedef struct _VS_FIXEDFILEINFO {
  DWORD dwSignature;
  DWORD dwStrucVersion;
  DWORD dwFileVersionMS;
  DWORD dwFileVersionLS;
  DWORD dwProductVersionMS;
  DWORD dwProductVersionLS;
  DWORD dwFileFlagsMask;
  DWORD dwFileFlags;
  DWORD dwFileOS;
  DWORD dwFileType;
  DWORD dwFileSubtype;
  DWORD dwFileDateMS;
  DWORD dwFileDateLS;
} VS_FIXEDFILEINFO, *PVS_FIXEDFILEINFO;

typedef struct _VS_VERSIONINFO {
  WORD             wLength;
  WORD             wValueLength;
  WORD             wType;
  WCHAR            szKey[16];
  WORD             Padding1;
  VS_FIXEDFILEINFO Value;
  WORD             Padding2;
  WORD             Children;
} VS_VERSIONINFO, *PVS_VERSIONINFO;

bool_t isPE(const pbuffer_t p);
bool_t isPE32(const pbuffer_t p);
bool_t isPE64(const pbuffer_t p);

PIMAGE_DOS_HEADER get_doshdr(const pbuffer_t p);
PIMAGE_NT_HEADERS32 get_nt32hdr(const pbuffer_t p);
PIMAGE_NT_HEADERS64 get_nt64hdr(const pbuffer_t p);

PIMAGE_DATA_DIRECTORY get_datadirbyentry(const pbuffer_t p, const int index);
PIMAGE_SECTION_HEADER get_sectionhdrbyname(const pbuffer_t p, const char* name);
PIMAGE_SECTION_HEADER get_sectionhdrbyindex(const pbuffer_t p, const int index);
PIMAGE_SECTION_HEADER get_sectionhdrbyentry(const pbuffer_t p, const int index);
PIMAGE_SECTION_HEADER get_sectionhdrbyRVA(const pbuffer_t p, const uint64_t vaddr);

unknown_t get_chunkbyRVA(const pbuffer_t p, const int index, const uint64_t vaddr, const size_t size);
unknown_t get_chunkbyentry(const pbuffer_t p, const int index);
unknown_t get_chunkbyname(const pbuffer_t p, const char* name);

DWORD get_dwordbyRVA(const pbuffer_t p, const int index, const uint64_t vaddr);

#endif

