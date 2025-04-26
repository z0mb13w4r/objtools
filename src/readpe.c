#include "dump.h"
#include "decode.h"
#include "opcode.h"
#include "pecode.h"
#include "pefunc.h"
#include "printf.h"
#include "readpe.h"
#include "memfind.h"

#include "static/dbghdr.ci"
#include "static/filehdr.ci"
#include "static/opthdr.ci"
#include "static/sechdr.ci"
#include "static/string_type.ci"
#include "static/res_types.ci"
#include "static/unw_flags.ci"
#include "static/verinfo.ci"

const int MAXSIZE = 36;

static int dump_dosheaderNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_DOS_HEADER dos = get_doshdr(p);
  if (dos) {
    size_t i = 0;

    n += printf_text("IMAGE DOS HEADER", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("e_magic", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_magic, USE_FHEX16 | USE_EOL);
    n += printf_text("e_cblp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_cblp, USE_FHEX16 | USE_EOL);
    n += printf_text("e_cp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_cp, USE_FHEX16 | USE_EOL);
    n += printf_text("e_crlc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_crlc, USE_FHEX16 | USE_EOL);
    n += printf_text("e_cparhdr", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_cparhdr, USE_FHEX16 | USE_EOL);
    n += printf_text("e_minalloc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_minalloc, USE_FHEX16 | USE_EOL);
    n += printf_text("e_maxalloc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_maxalloc, USE_FHEX16 | USE_EOL);
    n += printf_text("e_ss", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_ss, USE_FHEX16 | USE_EOL);
    n += printf_text("e_sp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_sp, USE_FHEX16 | USE_EOL);
    n += printf_text("e_csum", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_csum, USE_FHEX16 | USE_EOL);
    n += printf_text("e_ip", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_ip, USE_FHEX16 | USE_EOL);
    n += printf_text("e_cs", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_cs, USE_FHEX16 | USE_EOL);
    n += printf_text("e_lfarlc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_lfarlc, USE_FHEX16 | USE_EOL);
    n += printf_text("e_ovno", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_ovno, USE_FHEX16 | USE_EOL);
    n += printf_text("e_res", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    for (i = 0; i < NELEMENTS(dos->e_res) - 1; ++i) n += printf_nice(dos->e_res[i], USE_FHEX16);
    n += printf_nice(dos->e_res[i], USE_FHEX16 | USE_EOL);
    n += printf_text("e_oemid", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_oemid, USE_FHEX16 | USE_EOL);
    n += printf_text("e_oeminfo", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_oeminfo, USE_FHEX16 | USE_EOL);
    n += printf_text("e_res2", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    for (i = 0; i < NELEMENTS(dos->e_res2) - 1; ++i) n += printf_nice(dos->e_res2[i], USE_FHEX16);
    n += printf_nice(dos->e_res2[i], USE_FHEX16 | USE_EOL);
    n += printf_text("e_lfanew", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(dos->e_lfanew, USE_FHEX16 | USE_EOL);
    n += printf_eol();
  }

  return n;
}

static int dump_ntheader0(const pbuffer_t p, const uint32_t Signature) {
  int n = 0;
  n += printf_text("IMAGE NT HEADER", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("Signature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Signature, USE_FHEX32 | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_ntheader1(const pbuffer_t p, const uint16_t Machine, const uint16_t NumberOfSections,
                   const uint32_t TimeDateStamp, const uint32_t PointerToSymbolTable, const uint32_t NumberOfSymbols,
                   const uint16_t SizeOfOptionalHeader, const uint16_t Characteristics) {
  int n = 0;
  n += printf_text("IMAGE FILE HEADER", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("Machine", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Machine, USE_FHEX16 | USE_EOL);
  n += printf_text("NumberOfSections", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(NumberOfSections, USE_FHEX16 | USE_EOL);
  n += printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(TimeDateStamp, USE_FHEX32);
  n += printf_nice(TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
  n += printf_text("PointerToSymbolTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(PointerToSymbolTable, USE_FHEX32 | USE_EOL);
  n += printf_text("NumberOfSymbols", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(NumberOfSymbols, USE_FHEX32 | USE_EOL);
  n += printf_text("SizeOfOptionalHeader", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(SizeOfOptionalHeader, USE_FHEX16 | USE_EOL);
  n += printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Characteristics, USE_FHEX16);
  n += printf_mask(zFILEHDR, Characteristics, USE_LT | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_ntheader2(const pbuffer_t p, const uint16_t Magic, const uint8_t MajorLinkerVersion, const uint8_t MinorLinkerVersion,
                   const uint32_t SizeOfCode, const uint32_t SizeOfInitializedData, const uint32_t SizeOfUninitializedData,
                   const uint32_t AddressOfEntryPoint, const uint32_t BaseOfCode, const uint32_t BaseOfData, const uint64_t ImageBase,
                   const uint32_t SectionAlignment,const uint32_t FileAlignment,
                   const uint16_t MajorOperatingSystemVersion, const uint16_t MinorOperatingSystemVersion,
                   const uint16_t MajorImageVersion, const uint16_t MinorImageVersion, const uint16_t MajorSubsystemVersion,
                   const uint16_t MinorSubsystemVersion, const uint32_t Win32VersionValue, const uint32_t SizeOfImage,
                   const uint32_t SizeOfHeaders, const uint32_t CheckSum, const uint16_t Subsystem, const uint16_t DllCharacteristics,
                   const uint64_t SizeOfStackReserve, const uint64_t SizeOfStackCommit, const uint64_t SizeOfHeapReserve,
                   const uint64_t SizeOfHeapCommit, const uint32_t LoaderFlags, const uint32_t NumberOfRvaAndSizes) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = isPE64(p) ? USE_FHEX64 : USE_FHEX32;

    n += printf_text("IMAGE OPTIONAL HEADER", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("Magic", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Magic, USE_FHEX16);
    n += printf_pick(zOPTHDRMAGIC, Magic, USE_LT | USE_SPACE | USE_EOL);
    n += printf_text("LinkerVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(MajorLinkerVersion, USE_DEC);
    n += printf_nice(MinorLinkerVersion, USE_DEC | USE_DOT | USE_EOL);
    n += printf_text("SizeOfCode", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfCode, USE_FHEX32 | USE_EOL);
    n += printf_text("SizeOfInitializedData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfInitializedData, USE_FHEX32 | USE_EOL);
    n += printf_text("SizeOfUninitializedData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfUninitializedData, USE_FHEX32 | USE_EOL);
    n += printf_text("AddressOfEntryPoint", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(AddressOfEntryPoint, USE_FHEX32 | USE_EOL);
    n += printf_text("BaseOfCode", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(BaseOfCode, USE_FHEX32 | USE_EOL);
    if (isPE32(p)) {
      n += printf_text("BaseOfData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(BaseOfData, USE_FHEX32 | USE_EOL);
    }
    n += printf_text("ImageBase", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(ImageBase, USE_FHEXNN | USE_EOL);
    n += printf_text("SectionAlignment", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SectionAlignment, USE_FHEX32 | USE_EOL);
    n += printf_text("FileAlignment", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(FileAlignment, USE_FHEX32 | USE_EOL);
    n += printf_text("OperatingSystemVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(MajorOperatingSystemVersion, USE_DEC);
    n += printf_nice(MinorOperatingSystemVersion, USE_DEC | USE_DOT | USE_EOL);
    n += printf_text("ImageVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(MajorImageVersion, USE_DEC);
    n += printf_nice(MinorImageVersion, USE_DEC | USE_DOT | USE_EOL);
    n += printf_text("SubsystemVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(MajorSubsystemVersion, USE_DEC);
    n += printf_nice(MinorSubsystemVersion, USE_DEC | USE_DOT | USE_EOL);
    n += printf_text("Win32VersionValue", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Win32VersionValue, USE_FHEX32 | USE_EOL);
    n += printf_text("SizeOfImage", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfImage, USE_FHEX32 | USE_EOL);
    n += printf_text("SizeOfHeaders", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfHeaders, USE_FHEX32 | USE_EOL);
    n += printf_text("CheckSum", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CheckSum, USE_FHEX32 | USE_EOL);
    n += printf_text("Subsystem", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Subsystem, USE_FHEX16);
    n += printf_pick(zOPTHDRSUBSYSTEM, Subsystem, USE_LT | USE_SPACE | USE_EOL);
    n += printf_text("DllCharacteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DllCharacteristics, USE_FHEX16);
    n += printf_mask(zOPTHDRCHARACTERISTICS, DllCharacteristics, USE_LT | USE_EOL);
    n += printf_text("SizeOfStackReserve", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfStackReserve, USE_FHEXNN | USE_EOL);
    n += printf_text("SizeOfStackCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfStackCommit, USE_FHEXNN | USE_EOL);
    n += printf_text("SizeOfHeapReserve", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfHeapReserve, USE_FHEXNN | USE_EOL);
    n += printf_text("SizeOfHeapCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfHeapCommit, USE_FHEXNN | USE_EOL);
    n += printf_text("LoaderFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(LoaderFlags, USE_FHEX32 | USE_EOL);
    n += printf_text("NumberOfRvaAndSizes", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(NumberOfRvaAndSizes, USE_FHEX32 | USE_EOL);
    n += printf_eol();
  }

  return n;
}

static int dump_ntheader32(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    n += dump_ntheader0(p, nt->Signature);

    PIMAGE_FILE_HEADER fp = &nt->FileHeader;
    PIMAGE_OPTIONAL_HEADER32 op = &nt->OptionalHeader;

    n += dump_ntheader1(p, fp->Machine, fp->NumberOfSections, fp->TimeDateStamp, fp->PointerToSymbolTable,
                   fp->NumberOfSymbols, fp->SizeOfOptionalHeader, fp->Characteristics);
    n += dump_ntheader2(p, op->Magic, op->MajorLinkerVersion, op->MinorLinkerVersion, op->SizeOfCode, op->SizeOfInitializedData,
                   op->SizeOfUninitializedData, op->AddressOfEntryPoint, op->BaseOfCode, op->BaseOfData, op->ImageBase,
                   op->SectionAlignment, op->FileAlignment, op->MajorOperatingSystemVersion, op->MinorOperatingSystemVersion,
                   op->MajorImageVersion, op->MinorImageVersion, op->MajorSubsystemVersion, op->MinorSubsystemVersion,
                   op->Win32VersionValue, op->SizeOfImage, op->SizeOfHeaders, op->CheckSum, op->Subsystem, op->DllCharacteristics,
                   op->SizeOfStackReserve, op->SizeOfStackCommit, op->SizeOfHeapReserve, op->SizeOfHeapCommit,
                   op->LoaderFlags, op->NumberOfRvaAndSizes);
  }

  return n;
}

static int dump_ntheader64(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  PIMAGE_NT_HEADERS64 nt = get_nt64hdr(p);
  if (nt) {
    n += dump_ntheader0(p, nt->Signature);

    PIMAGE_FILE_HEADER fp = &nt->FileHeader;
    PIMAGE_OPTIONAL_HEADER64 op = &nt->OptionalHeader;

    n += dump_ntheader1(p, fp->Machine, fp->NumberOfSections, fp->TimeDateStamp, fp->PointerToSymbolTable,
                   fp->NumberOfSymbols, fp->SizeOfOptionalHeader, fp->Characteristics);
    n += dump_ntheader2(p, op->Magic, op->MajorLinkerVersion, op->MinorLinkerVersion, op->SizeOfCode, op->SizeOfInitializedData,
                   op->SizeOfUninitializedData, op->AddressOfEntryPoint, op->BaseOfCode, 0L, op->ImageBase,
                   op->SectionAlignment, op->FileAlignment, op->MajorOperatingSystemVersion, op->MinorOperatingSystemVersion,
                   op->MajorImageVersion, op->MinorImageVersion, op->MajorSubsystemVersion, op->MinorSubsystemVersion,
                   op->Win32VersionValue, op->SizeOfImage, op->SizeOfHeaders, op->CheckSum, op->Subsystem, op->DllCharacteristics,
                   op->SizeOfStackReserve, op->SizeOfStackCommit, op->SizeOfHeapReserve, op->SizeOfHeapCommit,
                   op->LoaderFlags, op->NumberOfRvaAndSizes);
  }

  return n;
}

static int dump_sectionheaders0(const pbuffer_t p, const uint16_t NumberOfSections) {
  int n = 0;
  for (uint16_t i = 0; i < NumberOfSections; ++i) {
    PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyindex(p, i);
    if (p0) {
      n += printf_text("IMAGE SECTION HEADER", USE_LT | USE_COLON | USE_EOL);
      n += printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_sore(p0->Name, sizeof(p0->Name), USE_STR | USE_SPACE | USE_EOL);
      n += printf_text("Misc.PhysicalAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->Misc.PhysicalAddress, USE_FHEX32 | USE_EOL);
      n += printf_text("Misc.VirtualSize", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->Misc.VirtualSize, USE_FHEX32 | USE_EOL);
      n += printf_text("VirtualAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->VirtualAddress, USE_FHEX32 | USE_EOL);
      n += printf_text("SizeOfRawData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->SizeOfRawData, USE_FHEX32 | USE_EOL);
      n += printf_text("PointerToRawData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->PointerToRawData, USE_FHEX32 | USE_EOL);
      n += printf_text("PointerToRelocations", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->PointerToRelocations, USE_FHEX32 | USE_EOL);
      n += printf_text("PointerToLinenumbers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->PointerToLinenumbers, USE_FHEX32 | USE_EOL);
      n += printf_text("NumberOfRelocations", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->NumberOfRelocations, USE_FHEX16 | USE_EOL);
      n += printf_text("NumberOfLinenumbers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->NumberOfLinenumbers, USE_FHEX16 | USE_EOL);
      n += printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p0->Characteristics, USE_FHEX32);
      n += printf_mask(zSECTIONHDR, p0->Characteristics, USE_LT | USE_EOL);
      n += printf_eol();
    }
  }

  return n;
}

static int dump_sectionheaders32(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  PIMAGE_NT_HEADERS32 p0 = get_nt32hdr(p);
  if (p0) {
    n += dump_sectionheaders0(p, p0->FileHeader.NumberOfSections);
  }

  return n;
}

static int dump_sectionheaders64(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  PIMAGE_NT_HEADERS64 p0 = get_nt64hdr(p);
  if (p0) {
    n += dump_sectionheaders0(p, p0->FileHeader.NumberOfSections);
  }

  return n;
}

static int dump_sectiongroups0(const pbuffer_t p) {
  const int MAXSIZE = strlenpick(zOPTHDRENTRY) + 2;

  int n = 0;
  n += printf_text("IMAGE DIRECTORY", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("Name", USE_LT | USE_TAB | SET_PAD(MAXSIZE));
  n += printf_text("RVA", USE_LT | USE_SPACE | SET_PAD(11));
  n += printf_text("Size", USE_LT | USE_SPACE | USE_EOL);

  return n;
}

static int dump_sectiongroups1(const pbuffer_t p, const int index, const uint32_t VirtualAddress, const uint32_t Size) {
  const int MAXSIZE = strlenpick(zOPTHDRENTRY) + 2;

  int n = 0;
  n += printf_pick(zOPTHDRENTRY, index, USE_LT | USE_TAB | SET_PAD(MAXSIZE));
  if (0 != VirtualAddress) n += printf_nice(VirtualAddress, USE_FHEX32);
  else                     n += printf_text("NONE", USE_LT | USE_SPACE | SET_PAD(11));
  if (0 != Size)           n += printf_nice(Size, USE_FHEX32 | USE_EOL);
  else                     n += printf_text("NONE", USE_LT | USE_SPACE | USE_EOL);

  return n;
}

static int dump_sectiongroups32(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    PIMAGE_OPTIONAL_HEADER32 op = &nt->OptionalHeader;
    PIMAGE_DATA_DIRECTORY dd = op->DataDirectory;

    n += dump_sectiongroups0(p);

    for (size_t i = 0; i < op->NumberOfRvaAndSizes; ++i, ++dd) {
      n += dump_sectiongroups1(p, i, dd->VirtualAddress, dd->Size);
    }

    n += printf_eol();
  }

  return n;
}

static int dump_sectiongroups64(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  PIMAGE_NT_HEADERS64 nt = get_nt64hdr(p);
  if (nt) {
    PIMAGE_OPTIONAL_HEADER64 op = &nt->OptionalHeader;
    PIMAGE_DATA_DIRECTORY dd = op->DataDirectory;

    n += dump_sectiongroups0(p);

    for (size_t i = 0; i < op->NumberOfRvaAndSizes; ++i, ++dd) {
      n += dump_sectiongroups1(p, i, dd->VirtualAddress, dd->Size);
    }

    n += printf_eol();
  }

  return n;
}

static int dump_version0(const pbuffer_t p, const uint16_t wLength, const uint16_t wValueLength, const uint16_t wType,
                         const pushort_t szKey, const size_t szKeySize, const char* name) {
  int n = 0;
  n += printf_text(name, USE_LT | USE_COLON | USE_EOL);
  n += printf_text("wLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wValueLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wValueLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wType", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wType, USE_FHEX16);
  n += printf_pick(zSTRINGTYPE, wType, USE_SPACE | USE_EOL);
  n += printf_text("szKey", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE + 1));
  n += printf_sore(szKey, szKeySize, USE_STR16 | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_version1(const pbuffer_t p, const uint16_t wLength, const uint16_t wValueLength, const uint16_t wType,
                         const pushort_t szKey, const size_t szKeySize, const pushort_t szValue, const size_t szValueSize,
                         const char* name) {
  int n = 0;
  n += printf_text(name, USE_LT | USE_COLON | USE_EOL);
  n += printf_text("wLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wValueLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wValueLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wType", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wType, USE_FHEX16);
  n += printf_pick(zSTRINGTYPE, wType, USE_SPACE | USE_EOL);
  n += printf_text("szKey", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE + 1));
  n += printf_sore(szKey, szKeySize, USE_STR16 | USE_EOL);
  n += printf_text("szValue", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE + 1));
  n += printf_sore(szValue, szValueSize, USE_STR16 | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_version2(const pbuffer_t p, const uint16_t wLength, const uint16_t wValueLength, const uint16_t wType,
                         const pushort_t szKey, const size_t szKeySize, const pushort_t Value, const size_t ValueSize) {
  int n = 0;
  n += printf_text("Var", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("wLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wValueLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wValueLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wType", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wType, USE_FHEX16);
  n += printf_pick(zSTRINGTYPE, wType, USE_SPACE | USE_EOL);
  n += printf_text("szKey", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE + 1));
  n += printf_sore(szKey, szKeySize, USE_STR16 | USE_EOL);
  n += printf_text("Value", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE + 1));
  n += printf_sore(Value, ValueSize, USE_HEX | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_version3(const pbuffer_t p, const uint32_t dwSignature, const uint32_t dwStrucVersion,
                         const uint32_t dwFileVersionMS, const uint32_t dwFileVersionLS,
                         const uint32_t dwProductVersionMS, const uint32_t dwProductVersionLS,
                         const uint32_t dwFileFlagsMask, const uint32_t dwFileFlags, const uint32_t dwFileOS,
                         const uint32_t dwFileType, const uint32_t dwFileSubtype,
                         const uint32_t dwFileDateMS, const uint32_t dwFileDateLS) {
  int n = 0;
  n += printf_text("VS FIXEDFILEINFO", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("dwSignature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwSignature, USE_FHEX32 | USE_EOL);
  n += printf_text("dwStrucVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwStrucVersion, USE_FHEX32 | USE_EOL);
  n += printf_text("dwFileVersionMS", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileVersionMS, USE_FHEX32 | USE_EOL);
  n += printf_text("dwFileVersionLS", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileVersionLS, USE_FHEX32 | USE_EOL);
  n += printf_text("dwProductVersionMS", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwProductVersionMS, USE_FHEX32 | USE_EOL);
  n += printf_text("dwProductVersionLS", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwProductVersionLS, USE_FHEX32 | USE_EOL);
  n += printf_text("dwFileFlagsMask", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileFlagsMask, USE_FHEX32 | USE_EOL);
  n += printf_text("dwFileFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileFlags, USE_FHEX32);
  n += printf_mask(zVERFILEFLAGS, dwFileFlags, USE_LT | USE_EOL);
  n += printf_text("dwFileOS", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileOS, USE_FHEX32);
  n += printf_pick(zVERFILEOS, dwFileOS, USE_SPACE | USE_EOL);
  n += printf_text("dwFileType", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileType, USE_FHEX32);
  n += printf_pick(zVERFILETYPE, dwFileType, USE_SPACE | USE_EOL);
  n += printf_text("dwFileSubtype", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (VFT_DRV == dwFileType) {
    n += printf_nice(dwFileSubtype, USE_FHEX32 | USE_EOL);
    n += printf_pick(zVERFILESUBTYPEDRV, dwFileSubtype, USE_SPACE | USE_EOL);
  } else if (VFT_FONT == dwFileType) {
    n += printf_nice(dwFileSubtype, USE_FHEX32 | USE_EOL);
    n += printf_pick(zVERFILESUBTYPEFONT, dwFileSubtype, USE_SPACE | USE_EOL);
  } else if (VFT_VXD == dwFileType) {
    n += printf_nice(dwFileSubtype, USE_UNKNOWN | USE_EOL);
  } else {
    n += printf_nice(dwFileSubtype, USE_FHEX32 | USE_EOL);
  }
  n += printf_text("dwFileDateMS", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileDateMS, USE_FHEX32 | USE_EOL);
  n += printf_text("dwFileDateLS", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(dwFileDateLS, USE_FHEX32 | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_versionV(const pbuffer_t p, handle_t f) {
  int n = 0;
  if (isvchunkkey(f, "VS_VERSION_INFO")) {
    PVERSION_CHUNK v0 = fget(f);
    n += dump_version0(p, v0->wLength, v0->wValueLength, v0->wType, v0->szKey, fget_vchunkkeysize(f), "VS VERSIONINFO");
    if (0 != v0->wValueLength) {
      f = fnext_vchunk(f);
      PVS_FIXEDFILEINFO v1 = fget(f);
      n += dump_version3(p, v1->dwSignature, v1->dwStrucVersion, v1->dwFileVersionMS, v1->dwFileVersionLS,
                   v1->dwProductVersionMS, v1->dwProductVersionLS, v1->dwFileFlagsMask, v1->dwFileFlags,
                   v1->dwFileOS, v1->dwFileType, v1->dwFileSubtype, v1->dwFileDateMS, v1->dwFileDateLS);

      f = fnext_vchunksize(f, sizeof(VS_FIXEDFILEINFO));
    }
  } else if (isvchunkkey(f, "StringFileInfo")) {
    PVERSION_CHUNK v0 = fget(f);
    n += dump_version0(p, v0->wLength, v0->wValueLength, v0->wType, v0->szKey, fget_vchunkkeysize(f), "StringFileInfo");
    f = fnext_vchunksize(f, STRING_FILE_INFO_SIZE);

    PVERSION_CHUNK v1 = fget(f);
    n += dump_version0(p, v1->wLength, v1->wValueLength, v1->wType, v1->szKey, fget_vchunkkeysize(f), "StringTable");

    WORD xx = STRING_TABLE_SIZE;
    f = fnext_vchunksize(f, STRING_TABLE_SIZE);
    while (xx < v1->wLength) {
      PVERSION_CHUNK v2 = fget(f);
      f = fnext_vchunk(f);
      PVERSION_VALUE v3 = fget(f);

      size_t ksiz = strsize16(v2->szKey, 50);
      size_t vsiz = strsize16(v3->szValue, 100);
      n += dump_version1(p, v2->wLength,  v2->wValueLength, v2->wType, v2->szKey, ksiz, v3->szValue, vsiz, "String");
      f = fnext_vchunksize(f, vsiz);
      xx += BOUND32(VERSION_CHUNK_MINSIZE + ksiz + vsiz + 2);
    }
  } else if (isvchunkkey(f, "VarFileInfo")) {
    PVERSION_CHUNK v0 = fget(f);
    n += dump_version0(p, v0->wLength, v0->wValueLength, v0->wType, v0->szKey, fget_vchunkkeysize(f), "VarFileInfo");
    f = fnext_vchunk(f);

    PVERSION_CHUNK v1 = fget(f);
    f = fnext_vchunk(f);

    PVERSION_VALUE v2 = fget(f);
    n += dump_version2(p, v1->wLength, v1->wValueLength, v1->wType, v1->szKey, strsize16(v1->szKey, 50), v2->szValue, 4);
    f = fnext_vchunksize(f, 4);
  }

  return n;
}

static int dump_versionY(const pbuffer_t p, PIMAGE_RESOURCE_DATA_ENTRY p0) {
  int n = 0;

  if (p0) {
    handle_t p1 = fget_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_UNKNOWN, p0->OffsetToData, p0->Size);
    if (p1) {
      int x = dump_versionV(p, p1);
      while (x) {
        n += x;
        x  = dump_versionV(p, p1);
      }

      ffree(p1);
    }
  }

  return n;
}

static int dump_versionZ(const pbuffer_t p, PIMAGE_RESOURCE_DIRECTORY p0) {
  int n = 0;

  if (p0) {
    PIMAGE_RESOURCE_DIRECTORY_ENTRY p1 = CAST(PIMAGE_RESOURCE_DIRECTORY_ENTRY, p0 + 1);
    for (int i = 0; i < (p0->NumberOfNamedEntries + p0->NumberOfIdEntries); ++i, ++p1) {
      if (p1->DataIsDirectory) {
        n += dump_versionZ(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE) + p1->OffsetToDirectory);
      } else {
        n += dump_versionY(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE) + p1->OffsetToDirectory);
      }
    }
  }

  return n;
}

static int dump_versionNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_RESOURCE_DIRECTORY p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE);
  if (p0) {
    PIMAGE_RESOURCE_DIRECTORY_ENTRY p1 = CAST(PIMAGE_RESOURCE_DIRECTORY_ENTRY, p0 + 1);
    for (int i = 0; i < (p0->NumberOfNamedEntries + p0->NumberOfIdEntries); ++i, ++p1) {
      if (p1->DataIsDirectory && !p1->NameIsString && RT_VERSION == p1->Name) {
        n += dump_versionZ(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE) + p1->OffsetToDirectory);
      }
    }
  }

  return n;
}

static int dump_iat0(const pbuffer_t p, PIMAGE_IMPORT_DESCRIPTOR p0) {
  int n = 0;
  if (p0) {
    n += printf_text("IMAGE IMPORT DESCRIPTOR", USE_LT | USE_COLON | USE_EOL);
  }

  return n;
}

static int dump_iat1(const pbuffer_t p, const uint32_t Characteristics, const uint32_t OriginalFirstThunk,
              const uint32_t TimeDateStamp, const uint32_t ForwarderChain,
              const uint32_t Name, const char* sname, const uint32_t FirstThunk) {
  int n = 0;
  n += printf_text("OriginalFirstThunk", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(OriginalFirstThunk, USE_FHEX32 | USE_EOL);
  n += printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Characteristics, USE_FHEX32 | USE_EOL);
  n += printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(TimeDateStamp, USE_FHEX32);
  n += printf_nice(TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
  n += printf_text("ForwarderChain", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(ForwarderChain, USE_FHEX32 | USE_EOL);
  n += printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Name, USE_FHEX32);
  n += printf_text(sname, USE_LT | USE_SPACE | USE_SB | USE_EOL);
  n += printf_text("FirstThunk", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(FirstThunk, USE_FHEX32 | USE_EOL);

  return n;
}

static int dump_iat2(const pbuffer_t p, const uint64_t AddressOfData) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = isPE64(p) ? USE_FHEX64 : USE_FHEX32;
    if (isPE32(p))      n += printf_text("IMAGE THUNK DATA32", USE_LT | USE_COLON | USE_EOL);
    else if (isPE64(p)) n += printf_text("IMAGE THUNK DATA64", USE_LT | USE_COLON | USE_EOL);

    n += printf_text("AddressOfData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(AddressOfData, USE_FHEXNN | USE_EOL);
  }

  return n;
}

static int dump_iat32(const pbuffer_t p, const poptions_t o) {
  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

  int n = 0;
  if (p0 && s0 && 0 != p0->VirtualAddress && 0 != p0->Size) {
    PIMAGE_IMPORT_DESCRIPTOR p1 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

    while (p1 && p1->FirstThunk) {
      char* Name = getp(p, peconvert2va(s0, p1->Name), 1);

      n += dump_iat0(p, p1);
      n += dump_iat1(p, p1->Characteristics, p1->OriginalFirstThunk, p1->TimeDateStamp, p1->ForwarderChain,
                p1->Name, Name, p1->FirstThunk);

      PIMAGE_THUNK_DATA32 p2 = (PIMAGE_THUNK_DATA32)
        getp(p, peconvert2va(s0, p1->OriginalFirstThunk), sizeof(IMAGE_THUNK_DATA32));

      while (p2 && p2->AddressOfData) {
        n += dump_iat2(p, p2->AddressOfData);
        uint64_t Ordinal = p2->Ordinal & ~IMAGE_THUNK_DATA_IS_IMPORT_ORDINAL;
        if (p2->AddressOfData & IMAGE_THUNK_DATA_IS_IMPORT_ORDINAL) {
          n += printf_text(get_namebyord(p, Name, Ordinal), USE_LT | USE_TAB2);
          n += printf_nice(Ordinal, USE_DEC | USE_SB);
          n += printf_text("(Imported by Ordinal)", USE_LT | USE_SPACE | USE_EOL);
        } else {
          PIMAGE_IMPORT_BY_NAME p3 = getp(p, peconvert2va(s0, p2->AddressOfData), sizeof(IMAGE_IMPORT_BY_NAME));
          if (p3) {
            n += printf_text(p3->Name, USE_LT | USE_TAB2);
            n += printf_nice(p3->Hint, USE_DEC | USE_SB | USE_EOL);
          }
        }

        ++p2;
      }

      n += printf_eol();
      ++p1;
    }
  }

  return n;
}

static int dump_iat64(const pbuffer_t p, const poptions_t o) {
  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

  int n = 0;
  if (p0 && s0 && 0 != p0->VirtualAddress && 0 != p0->Size) {
    PIMAGE_IMPORT_DESCRIPTOR p1 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

    while (p1 && p1->FirstThunk) {
      char* Name = getp(p, peconvert2va(s0, p1->Name), 1);

      n += dump_iat0(p, p1);
      n += dump_iat1(p, p1->Characteristics, p1->OriginalFirstThunk, p1->TimeDateStamp, p1->ForwarderChain,
                p1->Name, Name, p1->FirstThunk);

      PIMAGE_THUNK_DATA64 p2 = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_IMPORT, p1->OriginalFirstThunk, sizeof(PIMAGE_THUNK_DATA64));

      while (p2 && p2->AddressOfData) {
        uint64_t Ordinal = p2->Ordinal & ~IMAGE_THUNK_DATA_IS_IMPORT_ORDINAL;
        if (p2->AddressOfData & IMAGE_THUNK_DATA_IS_IMPORT_ORDINAL) {
          n += printf_text(get_namebyord(p, Name, Ordinal), USE_LT | USE_TAB2);
          n += printf_nice(Ordinal, USE_DEC | USE_SB);
          n += printf_text("(Imported by Ordinal)", USE_LT | USE_SPACE | USE_EOL);
        } else {
          PIMAGE_IMPORT_BY_NAME p3 = getp(p, peconvert2va(s0, p2->AddressOfData), sizeof(IMAGE_IMPORT_BY_NAME));
          if (p3) {
            n += printf_text(p3->Name, USE_LT | USE_TAB2);
            n += printf_nice(p3->Hint, USE_DEC | USE_SB | USE_EOL);
          }
        }

        ++p2;
      }

      n += printf_eol();
      ++p1;
    }
  }

  return n;
}

static int dump_eat0(const pbuffer_t p, const uint32_t Characteristics, const uint32_t TimeDateStamp,
                     const uint16_t MajorVersion, const uint16_t MinorVersion, const uint64_t Name, const char* sname,
                     const uint32_t Base, const uint32_t NumberOfFunctions, const uint32_t NumberOfNames,
                     const uint32_t AddressOfFunctions, const uint32_t AddressOfNames, const uint32_t AddressOfNameOrdinals) {
  int n = 0;
  n += printf_text("IMAGE EXPORT DIRECTORY", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Characteristics, USE_FHEX32 | USE_EOL);
  n += printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(TimeDateStamp, USE_FHEX32);
  n += printf_nice(TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
  n += printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(MajorVersion, USE_DEC);
  n += printf_nice(MinorVersion, USE_DEC | USE_DOT | USE_EOL);
  n += printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Name, USE_FHEX32);
  n += printf_text(sname, USE_LT | USE_SPACE | USE_SB | USE_EOL);
  n += printf_text("Base", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Base, USE_FHEX32 | USE_EOL);
  n += printf_text("NumberOfFunctions", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(NumberOfFunctions, USE_FHEX32 | USE_EOL);
  n += printf_text("NumberOfNames", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(NumberOfNames, USE_FHEX32 | USE_EOL);
  n += printf_text("AddressOfFunctions", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(AddressOfFunctions, USE_FHEX32 | USE_EOL);
  n += printf_text("AddressOfNames", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(AddressOfNames, USE_FHEX32 | USE_EOL);
  n += printf_text("AddressOfNameOrdinals", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(AddressOfNameOrdinals, USE_FHEX32 | USE_EOL);

  return n;
}

static int dump_eatNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);
  PIMAGE_EXPORT_DIRECTORY p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);
  PIMAGE_DATA_DIRECTORY p1 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);

  if (p0 && p1 && s0 && p0->Name && 0 != p1->VirtualAddress && 0 != p1->Size) {
    n += dump_eat0(p, p0->Characteristics, p0->TimeDateStamp, p0->MajorVersion, p0->MinorVersion,
              p0->Name, getp(p, peconvert2va(s0, p0->Name), 1), p0->Base, p0->NumberOfFunctions, p0->NumberOfNames,
              p0->AddressOfFunctions, p0->AddressOfNames, p0->AddressOfNameOrdinals);

    PDWORD addrOfNames = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_EXPORT, p0->AddressOfNames, sizeof(DWORD) * p0->NumberOfNames);
    PDWORD addrOfFunctions = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_EXPORT, p0->AddressOfFunctions, sizeof(DWORD) * p0->NumberOfFunctions);
    PWORD  addrOfNameOrdinals = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_EXPORT, p0->AddressOfNameOrdinals, sizeof(WORD) * p0->NumberOfFunctions);

    n += printf_text("Ord", USE_RT | SET_PAD(6));
    n += printf_text("RVA", USE_LT | USE_SPACE | SET_PAD(12));
    n += printf_text("Name", USE_LT | USE_EOL);

    for (int i = 0; i < p0->NumberOfNames; ++i) {
      n += printf_nice(addrOfNameOrdinals[i] + 1, USE_DEC5);
      n += printf_nice(addrOfFunctions[i], USE_FHEX32);
      n += printf_text(getp(p, peconvert2va(s0, addrOfNames[i]), 1), USE_LT | USE_SPACE | USE_EOL);
    }
  }

  return n;
}

static int dump_resource0(const pbuffer_t p, const uint32_t Characteristics, const uint32_t TimeDateStamp,
                          const uint16_t MajorVersion, const uint16_t MinorVersion,
                          const uint16_t NumberOfNamedEntries, const uint16_t NumberOfIdEntries) {
  int n = 0;
  n += printf_text("IMAGE RESOURCE DIRECTORY", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Characteristics, USE_FHEX32 | USE_EOL);
  n += printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(TimeDateStamp, USE_FHEX32);
  n += printf_nice(TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
  n += printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(MajorVersion, USE_DEC);
  n += printf_nice(MinorVersion, USE_DEC | USE_DOT | USE_EOL);
  n += printf_text("NumberOfNamedEntries", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(NumberOfNamedEntries, USE_DEC | USE_EOL);
  n += printf_text("NumberOfIdEntries", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(NumberOfIdEntries, USE_DEC | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_resource1(const pbuffer_t p, const uint32_t NameIsString, const uint32_t Name,
                          const uint32_t OffsetToData, const int z) {
  int n = 0;
  n += printf_text("IMAGE RESOURCE DIRECTORY ENTRY", USE_LT);
  n += printf_nice(z, USE_SB | USE_COLON | USE_EOL);
  n += printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  if (NameIsString || 1 != z) {
    n += printf_nice(Name, USE_FHEX32 | USE_EOL);
  } else {
    n += printf_nice(Name, USE_FHEX32);
    n += printf_pick(zRESOURCETYPE, Name, USE_SPACE | USE_EOL);
  }
  n += printf_text("OffsetToData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(OffsetToData, USE_FHEX32 | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_resource2(const pbuffer_t p, const uint32_t OffsetToData, const uint32_t Size,
                          const uint32_t CodePage, const uint32_t Reserved) {
  int n = 0;
  n += printf_text("IMAGE RESOURCE DATA ENTRY", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("OffsetToData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(OffsetToData, USE_FHEX32 | USE_EOL);
  n += printf_text("Size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Size, USE_FHEX32 | USE_EOL);
  n += printf_text("CodePage", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(CodePage, USE_FHEX32 | USE_EOL);
  n += printf_text("Reserved", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Reserved, USE_FHEX32 | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_resourceY(const pbuffer_t p, PIMAGE_RESOURCE_DATA_ENTRY p0, const uint32_t Name) {
  int n = 0;

  if (p0) {
    n += dump_resource2(p, p0->OffsetToData, p0->Size, p0->CodePage, p0->Reserved);
    if (RT_VERSION == Name) {
      n += dump_versionY(p, p0);
    }
  }

  return n;
}

static int dump_resourceZ(const pbuffer_t p, PIMAGE_RESOURCE_DIRECTORY p0, const uint32_t Name, const int z) {
  int n = 0;

  if (p0) {
    n += dump_resource0(p, p0->Characteristics, p0->TimeDateStamp,
                   p0->MajorVersion, p0->MinorVersion, p0->NumberOfNamedEntries, p0->NumberOfIdEntries);

    PIMAGE_RESOURCE_DIRECTORY_ENTRY p1 = CAST(PIMAGE_RESOURCE_DIRECTORY_ENTRY, p0 + 1);
    for (int i = 0; i < (p0->NumberOfNamedEntries + p0->NumberOfIdEntries); ++i, ++p1) {
      n += dump_resource1(p, p1->NameIsString, p1->Name, p1->OffsetToData, z);
      if (p1->DataIsDirectory) {
        n += dump_resourceZ(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE) + p1->OffsetToDirectory,
	                    !Name && !p1->NameIsString ? p1->Name : Name, z + 1);
      } else {
        n += dump_resourceY(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE) + p1->OffsetToDirectory, Name);
      }
    }
  }

  return n;
}

static int dump_resourceNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  n += dump_resourceZ(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE), 0, 1);

  return n;
}

static int dump_config0(const pbuffer_t p, const uint32_t Size, const uint32_t TimeDateStamp,
                        const uint16_t MajorVersion, const uint16_t MinorVersion,
                        const uint32_t GlobalFlagsClear, const uint32_t GlobalFlagsSet, const uint32_t CriticalSectionDefaultTimeout,
                        const uint64_t DeCommitFreeBlockThreshold, const uint64_t DeCommitTotalFreeThreshold,
                        const uint64_t LockPrefixTable, const uint64_t MaximumAllocationSize, const uint64_t VirtualMemoryThreshold,
                        const uint64_t ProcessAffinityMask, const uint32_t ProcessHeapFlags, const uint16_t CSDVersion,
                        const uint16_t Reserved1,const uint64_t EditList, const uint64_t SecurityCookie,
                        const uint64_t SEHandlerTable, const uint64_t SEHandlerCount) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = isPE64(p) ? USE_FHEX64 : USE_FHEX32;

    printf_text("IMAGE LOAD CONFIG DIRECTORY", USE_LT | USE_COLON | USE_EOL);

    n += printf_text("Size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Size, USE_DEC | USE_EOL);
    n += printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(TimeDateStamp, USE_FHEX32);
    n += printf_nice(TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
    n += printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(MajorVersion, USE_DEC);
    n += printf_nice(MinorVersion, USE_DEC | USE_DOT | USE_EOL);
    n += printf_text("GlobalFlagsClear", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GlobalFlagsClear, USE_FHEX32 | USE_EOL);
    n += printf_text("GlobalFlagsSet", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GlobalFlagsSet, USE_FHEX32 | USE_EOL);
    n += printf_text("CriticalSectionDefaultTimeout", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CriticalSectionDefaultTimeout, USE_FHEX32 | USE_EOL);
    n += printf_text("DeCommitFreeBlockThreshold", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DeCommitFreeBlockThreshold, USE_FHEXNN | USE_EOL);
    n += printf_text("DeCommitTotalFreeThreshold", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DeCommitTotalFreeThreshold, USE_FHEXNN | USE_EOL);
    n += printf_text("LockPrefixTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(LockPrefixTable, USE_FHEXNN | USE_EOL);
    n += printf_text("MaximumAllocationSize", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(MaximumAllocationSize, USE_FHEXNN | USE_EOL);
    n += printf_text("VirtualMemoryThreshold", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(VirtualMemoryThreshold, USE_FHEXNN | USE_EOL);
    n += printf_text("ProcessAffinityMask", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(ProcessAffinityMask, USE_FHEXNN | USE_EOL);
    n += printf_text("ProcessHeapFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(ProcessHeapFlags, USE_FHEX32 | USE_EOL);
    n += printf_text("CSDVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CSDVersion, USE_FHEX16 | USE_EOL);
    n += printf_text("Reserved1", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Reserved1, USE_FHEX16 | USE_EOL);
    n += printf_text("EditList", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(EditList, USE_FHEXNN | USE_EOL);
    n += printf_text("SecurityCookie", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SecurityCookie, USE_FHEXNN | USE_EOL);
    n += printf_text("SEHandlerTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SEHandlerTable, USE_FHEXNN | USE_EOL);
    n += printf_text("SEHandlerCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SEHandlerCount, USE_FHEXNN | USE_EOL);
    n += printf_eol();
  }

  return n;
}

static int dump_config1(const pbuffer_t p, const uint64_t GuardCFCheckFunctionPointer, const uint64_t GuardCFDispatchFunctionPointer,
                        const uint64_t GuardCFFunctionTable, const uint64_t GuardCFFunctionCount, const uint32_t GuardFlags) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = isPE64(p) ? USE_FHEX64 : USE_FHEX32;

    n += printf_text("GuardCFCheckFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFCheckFunctionPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardCFDispatchFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFDispatchFunctionPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardCFFunctionTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFFunctionTable, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardCFFunctionCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFFunctionCount, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardFlags, USE_FHEX32 | USE_EOL);
  }

  return n;
}

static int dump_config2(const pbuffer_t p, const uint16_t CodeIntegrityFlags, const uint16_t CodeIntegrityCatalog,
                        const uint32_t CodeIntegrityCatalogOffset, const uint32_t CodeIntegrityReserved,
                        const uint64_t GuardAddressTakenIatEntryTable, const uint64_t GuardAddressTakenIatEntryCount,
                        const uint64_t GuardLongJumpTargetTable, const uint64_t GuardLongJumpTargetCount,
                        const uint64_t DynamicValueRelocTable, const uint64_t CHPEMetadataPointer) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = isPE64(p) ? USE_FHEX64 : USE_FHEX32;

    n += printf_text("CodeIntegrityFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityFlags, USE_FHEX16 | USE_EOL);
    n += printf_text("CodeIntegrityCatalog", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityCatalog, USE_FHEX16 | USE_EOL);
    n += printf_text("CodeIntegrityCatalogOffset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityCatalogOffset, USE_FHEX32 | USE_EOL);
    n += printf_text("CodeIntegrityReserved", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityReserved, USE_FHEX32 | USE_EOL);
    n += printf_text("GuardAddressTakenIatEntryTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardAddressTakenIatEntryTable, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardAddressTakenIatEntryCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardAddressTakenIatEntryCount, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardLongJumpTargetTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardLongJumpTargetTable, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardLongJumpTargetCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardLongJumpTargetCount, USE_FHEXNN | USE_EOL);
    n += printf_text("DynamicValueRelocTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DynamicValueRelocTable, USE_FHEXNN | USE_EOL);
    n += printf_text("CHPEMetadataPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CHPEMetadataPointer, USE_FHEXNN | USE_EOL);
  }

  return n;
}

static int dump_config3(const pbuffer_t p, const uint64_t GuardRFFailureRoutine, const uint64_t GuardRFFailureRoutineFunctionPointer,
                        const uint32_t DynamicValueRelocTableOffset, const uint16_t DynamicValueRelocTableSection, const uint16_t Reserved2,
                        const uint64_t GuardRFVerifyStackPointerFunctionPointer, const uint32_t HotPatchTableOffset) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = isPE64(p) ? USE_FHEX64 : USE_FHEX32;

    n += printf_text("GuardRFFailureRoutine", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardRFFailureRoutine, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardRFFailureRoutineFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardRFFailureRoutineFunctionPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("DynamicValueRelocTableOffset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DynamicValueRelocTableOffset, USE_FHEX32 | USE_EOL);
    n += printf_text("DynamicValueRelocTableSection", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DynamicValueRelocTableSection, USE_FHEX16 | USE_EOL);
    n += printf_text("Reserved2", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Reserved2, USE_FHEX16 | USE_EOL);
    n += printf_text("GuardRFVerifyStackPointerFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardRFVerifyStackPointerFunctionPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("HotPatchTableOffset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(HotPatchTableOffset, USE_FHEX32 | USE_EOL);
  }

  return n;
}

static int dump_config4(const pbuffer_t p, const uint32_t Reserved3, const uint64_t EnclaveConfigurationPointer,
                        const uint64_t VolatileMetadataPointer, const uint64_t GuardEHContinuationTable,
                        const uint64_t GuardEHContinuationCount, const uint64_t GuardXFGCheckFunctionPointer,
                        const uint64_t GuardXFGDispatchFunctionPointer, const uint64_t GuardXFGTableDispatchFunctionPointer,
                        const uint64_t CastGuardOsDeterminedFailureMode, const uint64_t GuardMemcpyFunctionPointer) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = isPE64(p) ? USE_FHEX64 : USE_FHEX32;

    n += printf_text("Reserved3", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Reserved3, USE_FHEX32 | USE_EOL);
    n += printf_text("EnclaveConfigurationPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(EnclaveConfigurationPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("VolatileMetadataPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(VolatileMetadataPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardEHContinuationTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardEHContinuationTable, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardEHContinuationCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardEHContinuationCount, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardXFGCheckFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardXFGCheckFunctionPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardXFGDispatchFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardXFGDispatchFunctionPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardXFGTableDispatchFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardXFGTableDispatchFunctionPointer, USE_FHEXNN | USE_EOL);
    n += printf_text("CastGuardOsDeterminedFailureMode", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CastGuardOsDeterminedFailureMode, USE_FHEXNN | USE_EOL);
    n += printf_text("GuardMemcpyFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardMemcpyFunctionPointer, USE_FHEXNN | USE_EOL);
  }

  return n;
}

static int dump_config32(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_LOAD_CONFIG_DIRECTORY32 p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
  if (p0 && p0->Size >= 72) {
    n += dump_config0(p, p0->Size, p0->TimeDateStamp, p0->MajorVersion, p0->MinorVersion, p0->GlobalFlagsClear, p0->GlobalFlagsSet,
                 p0->CriticalSectionDefaultTimeout, p0->DeCommitFreeBlockThreshold, p0-> DeCommitTotalFreeThreshold, p0->LockPrefixTable,
                 p0->MaximumAllocationSize, p0->VirtualMemoryThreshold, p0->ProcessAffinityMask, p0->ProcessHeapFlags, p0->CSDVersion,
                 p0->Reserved1, p0->EditList, p0->SecurityCookie, p0->SEHandlerTable, p0->SEHandlerCount);
  }
  if (p0 && p0->Size >= 92) {
    n += dump_config1(p, p0->GuardCFCheckFunctionPointer, p0->GuardCFDispatchFunctionPointer,
                 p0->GuardCFFunctionTable, p0->GuardCFFunctionCount, p0->GuardFlags);
  }
  if (p0 && p0->Size >= 128) {
    n += dump_config2(p, p0->CodeIntegrity.Flags, p0->CodeIntegrity.Catalog, p0->CodeIntegrity.CatalogOffset, p0->CodeIntegrity.Reserved,
                 p0->GuardAddressTakenIatEntryTable, p0->GuardAddressTakenIatEntryCount, p0->GuardLongJumpTargetTable,
                 p0->GuardLongJumpTargetCount, p0->DynamicValueRelocTable, p0->CHPEMetadataPointer);
  }
  if (p0 && p0->Size >= 152) {
    n += dump_config3(p, p0->GuardRFFailureRoutine, p0->GuardRFFailureRoutineFunctionPointer, p0->DynamicValueRelocTableOffset,
                 p0->DynamicValueRelocTableSection, p0->Reserved2, p0->GuardRFVerifyStackPointerFunctionPointer, p0->HotPatchTableOffset);
  }
  if (p0 && p0->Size >= 192) {
    n += dump_config4(p, p0->Reserved3, p0->EnclaveConfigurationPointer, p0->VolatileMetadataPointer,
                 p0->GuardEHContinuationTable, p0->GuardEHContinuationCount, p0->GuardXFGCheckFunctionPointer,
                 p0->GuardXFGDispatchFunctionPointer, p0->GuardXFGTableDispatchFunctionPointer,
                 p0->CastGuardOsDeterminedFailureMode, p0->GuardMemcpyFunctionPointer);
  }
  if (p0) {
    n += printf_eol();
  }

  return n;
}

static int dump_config64(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_LOAD_CONFIG_DIRECTORY64 p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
  if (p0 && p0->Size >= 112) {
    n += dump_config0(p, p0->Size, p0->TimeDateStamp, p0->MajorVersion, p0->MinorVersion, p0->GlobalFlagsClear, p0->GlobalFlagsSet,
                 p0->CriticalSectionDefaultTimeout, p0->DeCommitFreeBlockThreshold, p0-> DeCommitTotalFreeThreshold, p0->LockPrefixTable,
                 p0->MaximumAllocationSize, p0->VirtualMemoryThreshold, p0->ProcessAffinityMask, p0->ProcessHeapFlags, p0->CSDVersion,
                 p0->Reserved1, p0->EditList, p0->SecurityCookie, p0->SEHandlerTable, p0->SEHandlerCount);
  }
  if (p0 && p0->Size >= 152) {
    n += dump_config1(p, p0->GuardCFCheckFunctionPointer, p0->GuardCFDispatchFunctionPointer,
                 p0->GuardCFFunctionTable, p0->GuardCFFunctionCount, p0->GuardFlags);
  }
  if (p0 && p0->Size >= 208) {
    n += dump_config2(p, p0->CodeIntegrity.Flags, p0->CodeIntegrity.Catalog, p0->CodeIntegrity.CatalogOffset, p0->CodeIntegrity.Reserved,
                 p0->GuardAddressTakenIatEntryTable, p0->GuardAddressTakenIatEntryCount, p0->GuardLongJumpTargetTable,
                 p0->GuardLongJumpTargetCount, p0->DynamicValueRelocTable, p0->CHPEMetadataPointer);
  }
  if (p0 && p0->Size >= 248) {
    n += dump_config3(p, p0->GuardRFFailureRoutine, p0->GuardRFFailureRoutineFunctionPointer, p0->DynamicValueRelocTableOffset,
                 p0->DynamicValueRelocTableSection, p0->Reserved2, p0->GuardRFVerifyStackPointerFunctionPointer, p0->HotPatchTableOffset);
  }
  if (p0 && p0->Size >= 320) {
    n += dump_config4(p, p0->Reserved3, p0->EnclaveConfigurationPointer, p0->VolatileMetadataPointer,
                 p0->GuardEHContinuationTable, p0->GuardEHContinuationCount, p0->GuardXFGCheckFunctionPointer,
                 p0->GuardXFGDispatchFunctionPointer, p0->GuardXFGTableDispatchFunctionPointer,
                 p0->CastGuardOsDeterminedFailureMode, p0->GuardMemcpyFunctionPointer);
  }

  return n;
}

static int dump_debugNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_DEBUG_DIRECTORY p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_DEBUG);
  if (p0) {
    n += printf_text("IMAGE_DEBUG_DIRECTORY", USE_LT | USE_COLON | USE_EOL);

    n += printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(p0->Characteristics, USE_FHEX32 | USE_EOL);
    n += printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(p0->TimeDateStamp, USE_FHEX32);
    n += printf_nice(p0->TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
    n += printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(p0->MajorVersion, USE_DEC);
    n += printf_nice(p0->MinorVersion, USE_DEC | USE_DOT | USE_EOL);
    n += printf_text("Type", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(p0->Type, USE_FHEX32);
    n += printf_pick(zDEBUGHDR, p0->Type, USE_SPACE | USE_EOL);
    n += printf_text("SizeOfData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(p0->SizeOfData, USE_FHEX32 | USE_EOL);
    n += printf_text("AddressOfRawData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(p0->AddressOfRawData, USE_FHEX32 | USE_EOL);
    n += printf_text("PointerToRawData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(p0->PointerToRawData, USE_FHEX32 | USE_EOL);
    n += printf_eol();
// TBD
    DWORD p1 = get_dwordbyRVA(p, IMAGE_DIRECTORY_ENTRY_DEBUG, p0->AddressOfRawData);
    if (p1 == CV_SIGNATURE_RSDS) {
      PCV_INFO_PDB70 p2 = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_DEBUG, p0->AddressOfRawData, sizeof(CV_INFO_PDB70));
      if (p2) {
        n += printf_text("CV INFO PDB70", USE_LT | USE_COLON | USE_EOL);
        n += printf_text("CvSignature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p2->CvSignature, USE_FHEX32 | USE_EOL);
        n += printf_text("Signature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_sore(&p2->Signature, sizeof(p2->Signature), USE_GUID | USE_SPACE | USE_EOL);
        n += printf_text("Age", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p2->Age, USE_FHEX32 | USE_EOL);
        n += printf_text("PdbFileName", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_text(CAST(const char*, p2->PdbFileName), USE_LT | USE_SPACE | USE_EOL);
        n += printf_eol();
      }
    } else if (p1 == CV_SIGNATURE_NB10) {
      PCV_INFO_PDB20 p2 = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_DEBUG, p0->AddressOfRawData, sizeof(CV_INFO_PDB20));
      if (p2) {
        n += printf_text("CV INFO PDB20", USE_LT | USE_COLON | USE_EOL);
        n += printf_text("CvSignature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p2->CvHeader.Signature, USE_FHEX32 | USE_EOL);
        n += printf_text("CvOffset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p2->CvHeader.Offset, USE_FHEX32 | USE_EOL);
        n += printf_text("Signature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p2->Signature, USE_FHEX32 | USE_EOL);
        n += printf_text("Age", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p2->Age, USE_FHEX32 | USE_EOL);
        n += printf_text("PdbFileName", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_text(CAST(const char*, p2->PdbFileName), USE_LT | USE_SPACE | USE_EOL);
        n += printf_eol();
      }
    }
  }

  return n;
}

static int dump_relocNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyname(p, SECTION_RELOC);
  PIMAGE_BASE_RELOCATION p1 = get_chunkbyname(p, SECTION_RELOC);
  if (p0 && p1) {
    for (DWORD x = 0; x < p0->SizeOfRawData; ) {
      if (0 != p1->SizeOfBlock) {
        n += printf_text("IMAGE BASE RELOCATION", USE_LT | USE_COLON | USE_EOL);
        n += printf_text("VirtualAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p1->VirtualAddress, USE_FHEX32 | USE_EOL);
        n += printf_text("SizeOfBlock", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p1->SizeOfBlock, USE_FHEX32 | USE_EOL);
        n += printf_eol();
      }
      x += p1->SizeOfBlock ? p1->SizeOfBlock : sizeof(IMAGE_BASE_RELOCATION);
      p1 = CAST(PIMAGE_BASE_RELOCATION, CAST(puchar_t, p1) + p1->SizeOfBlock);
    }
  }

  return n;
}

static int dump_runtimeNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_EXCEPTION);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_EXCEPTION);
  PIMAGE_RUNTIME_FUNCTION_ENTRY p1 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_EXCEPTION);

  if (p0 && p1 && s0) {
    for (DWORD x = 0; x < s0->SizeOfRawData; x += sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY), ++p1) {
      if (0 != p1->BeginAddress) {
        n += printf_text("RUNTIME FUNCTION", USE_LT | USE_COLON | USE_EOL);
        n += printf_text("BeginAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p1->BeginAddress, USE_FHEX32 | USE_EOL);
        n += printf_text("EndAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p1->EndAddress, USE_FHEX32 | USE_EOL);
        n += printf_text("UnwindData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(p1->UnwindData, USE_FHEX32 | USE_EOL);
        n += printf_eol();

        PUNWIND_INFO p2 = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_UNKNOWN, p1->UnwindData, sizeof(UNWIND_INFO));
        if (p2) {
          n += printf_text("UNWIND INFO", USE_LT | USE_COLON | USE_EOL);
          n += printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n += printf_nice(p2->Version, USE_FHEX32 | USE_EOL);
          n += printf_text("Flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n += printf_nice(p2->Flags, USE_FHEX32);
          n += printf_pick(zUNW_FLAGS, p2->Flags, USE_SPACE | USE_EOL);
          n += printf_text("SizeOfProlog", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n += printf_nice(p2->SizeOfProlog, USE_FHEX32 | USE_EOL);
          n += printf_text("CountOfCodes", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n += printf_nice(p2->CountOfCodes, USE_FHEX32 | USE_EOL);
          n += printf_text("FrameRegister", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n += printf_nice(p2->FrameRegister, USE_FHEX32 | USE_EOL);
          n += printf_text("FrameOffset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n += printf_nice(p2->FrameOffset, USE_FHEX32 | USE_EOL);
          n += printf_eol();

          PBYTE x3 = CAST(PBYTE, p2->UnwindCode);
          for (BYTE i = 0; i < p2->CountOfCodes; ++i, x3 += 2) {
            PUNWIND_CODE p3 = CAST(PUNWIND_CODE, x3);
            n += printf_text("UNWIND CODE", USE_LT | USE_COLON | USE_EOL);
            n += printf_text("OffsetProlog", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
            n += printf_nice(p3->OffsetProlog, USE_FHEX8 | USE_EOL);
            n += printf_text("UnwindCode", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
            n += printf_nice(p3->UnwindCode, USE_FHEX8);
            n += printf_pick(zUNW_CODES, p3->UnwindCode, USE_SPACE | USE_EOL);
            n += printf_text("OpInfo", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
            n += printf_nice(p3->OpInfo, USE_FHEX8);
            if (UWOP_ALLOC_SMALL == p3->UnwindCode) {
              n += printf_text(".ALLOCSTACK", USE_LT | USE_SPACE);
              n += printf_nice(p3->OpInfo * 8 + 8, USE_FHEX8 | USE_EOL);
            } else if (UWOP_ALLOC_LARGE == p3->UnwindCode) {
              n += printf_text(".ALLOCSTACK", USE_LT | USE_SPACE);
              if (0 == p3->OpInfo) {
                n += printf_nice(x3[2] * 8, USE_FHEX16 | USE_EOL);
                x3 += 2;
                ++i;
              }
            } else if (UWOP_PUSH_NONVOL == p3->UnwindCode) {
              n += printf_text(".PUSHREG", USE_LT | USE_SPACE);
              n += printf_pick(zUNW_INFO, p3->OpInfo, USE_SPACE | USE_EOL);
            } else if (UWOP_SAVE_NONVOL == p3->UnwindCode || UWOP_SAVE_NONVOL_FAR == p3->UnwindCode) {
              n += printf_nice(x3[2], USE_FHEX8);
              n += printf_nice(x3[3], USE_FHEX8);
              n += printf_text(".SAVEREG", USE_LT | USE_SPACE);
              n += printf_pick(zUNW_INFO, p3->OpInfo, USE_SPACE);
              n += printf_nice(x3[2] * 8, USE_FHEX8 | USE_COMMA | USE_EOL);
              x3 += 2;
              ++i;
            } else if (UWOP_SAVE_XMM128 == p3->UnwindCode || UWOP_SAVE_XMM128_FAR == p3->UnwindCode) {
              n += printf_nice(x3[2], USE_FHEX8);
              n += printf_nice(x3[3], USE_FHEX8);
              n += printf_text(".SAVEXMM128", USE_LT | USE_SPACE);
              n += printf_join("XMM", p3->OpInfo, USE_DEC | USE_SPACE);
              n += printf_nice(x3[2] * 16, USE_FHEX16 | USE_COMMA | USE_EOL);
              x3 += 2;
              ++i;
            } else {
              n += printf_nice(p3->OpInfo, USE_SPACE | USE_UNKNOWN | USE_EOL);
            }
            n += printf_eol();
          }
        }
      }
    }
  }

  return n;
}

static int dump_actionsNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  paction_t x = o->actions;
  while (x) {
    if (x->secname[0]) {
      PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyname(p, x->secname);
      if (p0) {
        MALLOCSWRAP(opwrap_t, s, MODE_OCSHDRPE, p0);
        n += dump_actions0(p, o, peconvert2va(p0, p0->VirtualAddress), p0->SizeOfRawData);
        n += dump_actions1(p, o, ps, x->secname, x->action, peconvert2va(p0, p0->VirtualAddress), p0->SizeOfRawData, p0->VirtualAddress);
      } else {
        printf_w("section '%s' was not dumped because it does not exist!", x->secname);
      }
    }

    x = x->actions;
  }

  return n;
}

int readpe(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {
    dump_summary(p, o);

    if (MODE_ISSET(o->action, OPTREADELF_FILEHEADER))         dump_dosheaderNN(p, o);

    if (isPE32(p)) {
      if (MODE_ISSET(o->action, OPTREADELF_FILEHEADER))       dump_ntheader32(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SECTIONHEADERS))   dump_sectionheaders32(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SECTIONGROUPS))    dump_sectiongroups32(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_VERSION))          dump_versionNN(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SYMBOLS))          dump_eatNN(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SYMBOLS))          dump_iat32(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_NOTES))            dump_resourceNN(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_NOTES))            dump_config32(p, o);
    } else if (isPE64(p)) {
      if (MODE_ISSET(o->action, OPTREADELF_FILEHEADER))       dump_ntheader64(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SECTIONHEADERS))   dump_sectionheaders64(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SECTIONGROUPS))    dump_sectiongroups64(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_VERSION))          dump_versionNN(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SYMBOLS))          dump_eatNN(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_SYMBOLS))          dump_iat64(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_NOTES))            dump_resourceNN(p, o);
      if (MODE_ISSET(o->action, OPTREADELF_NOTES))            dump_config64(p, o);
    }

    if (MODE_ISSET(o->action, OPTREADELF_NOTES))              dump_debugNN(p, o);
    if (MODE_ISSET(o->action, OPTREADELF_RELOCS))             dump_relocNN(p, o);
    if (MODE_ISSET(o->action, OPTREADELF_UNWIND))             dump_runtimeNN(p, o);

    if (isPE32(p) || isPE64(p)) {
      if (o->actions)                                         dump_actionsNN(p, o);
    }
  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

