#include "readpe.h"
#include "printf.h"

#include "static/dbghdr.ci"
#include "static/filehdr.ci"
#include "static/opthdr.ci"
#include "static/sechdr.ci"
#include "static/res_types.ci"
#include "static/unw_flags.ci"
#include "static/verinfo.ci"

const int MAXSIZE = 36;

static int dump_peheader(const pbuffer_t p, const poptions_t o) {
  if (issafe(p)) {
    if (o->action & OPTPROGRAM_HASH)   printf_sore(p->data, p->size, USE_HASHALL | USE_EOL);
  }

  return 0;
}

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

static int dump_ntheader0(const pbuffer_t p, const uint16_t Machine, const uint16_t NumberOfSections,
                   const uint32_t TimeDateStamp, const uint32_t PointerToSymbolTable, const uint32_t NumberOfSymbols,
                   const uint16_t SizeOfOptionalHeader, const uint16_t Characteristics) {
  int n = 0;
  n += printf_text("FILE HEADER", USE_LT | USE_COLON | USE_EOL);
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

static int dump_ntheader1(const pbuffer_t p, const uint16_t Magic, const uint8_t MajorLinkerVersion, const uint8_t MinorLinkerVersion,
                   const uint32_t SizeOfCode, const uint32_t SizeOfInitializedData, const uint32_t SizeOfUninitializedData,
                   const uint32_t AddressOfEntryPoint, const uint32_t BaseOfCode, const uint64_t ImageBase, const uint32_t SectionAlignment,
                   const uint32_t FileAlignment, const uint16_t MajorOperatingSystemVersion, const uint16_t MinorOperatingSystemVersion,
                   const uint16_t MajorImageVersion, const uint16_t MinorImageVersion, const uint16_t MajorSubsystemVersion,
                   const uint16_t MinorSubsystemVersion, const uint32_t Win32VersionValue, const uint32_t SizeOfImage,
                   const uint32_t SizeOfHeaders, const uint32_t CheckSum, const uint16_t Subsystem, const uint16_t DllCharacteristics,
                   const uint64_t SizeOfStackReserve, const uint64_t SizeOfStackCommit, const uint64_t SizeOfHeapReserve,
                   const uint64_t SizeOfHeapCommit, const uint32_t LoaderFlags, const uint32_t NumberOfRvaAndSizes) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = (isPE64(p) ? USE_FHEX64 : USE_FHEX32) | USE_EOL;

    n += printf_text("OPTIONAL HEADER", USE_LT | USE_COLON | USE_EOL);
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
    n += printf_text("ImageBase", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(ImageBase, USE_FHEXNN);
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
    n += printf_nice(SizeOfStackReserve, USE_FHEXNN);
    n += printf_text("SizeOfStackCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfStackCommit, USE_FHEXNN);
    n += printf_text("SizeOfHeapReserve", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfHeapReserve, USE_FHEXNN);
    n += printf_text("SizeOfHeapCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SizeOfHeapCommit, USE_FHEXNN);
    n += printf_text("LoaderFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(LoaderFlags, USE_FHEX32 | USE_EOL);
    n += printf_text("NumberOfRvaAndSizes", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(NumberOfRvaAndSizes, USE_FHEX32 | USE_EOL);
    n += printf_eol();
  }

  return n;
}

static int dump_ntheader32(const pbuffer_t p, const poptions_t o) {
  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    printf_text("NT HEADER", USE_LT | USE_COLON | USE_EOL);
    printf_text("Signature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(nt->Signature, USE_FHEX32 | USE_EOL);
    printf_eol();

    PIMAGE_FILE_HEADER fp = &nt->FileHeader;
    PIMAGE_OPTIONAL_HEADER32 op = &nt->OptionalHeader;

    dump_ntheader0(p, fp->Machine, fp->NumberOfSections, fp->TimeDateStamp, fp->PointerToSymbolTable,
                   fp->NumberOfSymbols, fp->SizeOfOptionalHeader, fp->Characteristics);
    dump_ntheader1(p, op->Magic, op->MajorLinkerVersion, op->MinorLinkerVersion, op->SizeOfCode, op->SizeOfInitializedData,
                   op->SizeOfUninitializedData, op->AddressOfEntryPoint, op->BaseOfCode, op->ImageBase, op->SectionAlignment,
                   op->FileAlignment, op->MajorOperatingSystemVersion, op->MinorOperatingSystemVersion, op->MajorImageVersion,
                   op->MinorImageVersion, op->MajorSubsystemVersion, op->MinorSubsystemVersion, op->Win32VersionValue,
                   op->SizeOfImage, op->SizeOfHeaders, op->CheckSum, op->Subsystem, op->DllCharacteristics, op->SizeOfStackReserve,
                   op->SizeOfStackCommit, op->SizeOfHeapReserve, op->SizeOfHeapCommit, op->LoaderFlags, op->NumberOfRvaAndSizes);
  }

  return 0;
}

static int dump_ntheader64(const pbuffer_t p, const poptions_t o) {
  PIMAGE_NT_HEADERS64 nt = get_nt64hdr(p);
  if (nt) {
    printf_text("NT HEADER", USE_LT | USE_COLON | USE_EOL);
    printf_text("Signature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(nt->Signature, USE_FHEX32 | USE_EOL);
    printf_eol();

    PIMAGE_FILE_HEADER fp = &nt->FileHeader;
    PIMAGE_OPTIONAL_HEADER64 op = &nt->OptionalHeader;

    dump_ntheader0(p, fp->Machine, fp->NumberOfSections, fp->TimeDateStamp, fp->PointerToSymbolTable,
                  fp->NumberOfSymbols, fp->SizeOfOptionalHeader, fp->Characteristics);
    dump_ntheader1(p, op->Magic, op->MajorLinkerVersion, op->MinorLinkerVersion, op->SizeOfCode, op->SizeOfInitializedData,
                   op->SizeOfUninitializedData, op->AddressOfEntryPoint, op->BaseOfCode, op->ImageBase, op->SectionAlignment,
                   op->FileAlignment, op->MajorOperatingSystemVersion, op->MinorOperatingSystemVersion, op->MajorImageVersion,
                   op->MinorImageVersion, op->MajorSubsystemVersion, op->MinorSubsystemVersion, op->Win32VersionValue,
                   op->SizeOfImage, op->SizeOfHeaders, op->CheckSum, op->Subsystem, op->DllCharacteristics, op->SizeOfStackReserve,
                   op->SizeOfStackCommit, op->SizeOfHeapReserve, op->SizeOfHeapCommit, op->LoaderFlags, op->NumberOfRvaAndSizes);
  }

  return 0;
}

static int dump_sectionheaders0(const pbuffer_t p, const uint16_t NumberOfSections) {
  int n = 0;
  for (uint16_t i = 0; i < NumberOfSections; ++i) {
    PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyindex(p, i);
    if (p0) {
      n += printf_text("IMAGE SECTION HEADER", USE_LT | USE_COLON | USE_EOL);
      n += printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_data(p0->Name, sizeof(p0->Name), 0, USE_STR | USE_EOL);

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
  n += printf_text("DATA DIRECTORY", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("Name", USE_LT | USE_TAB | SET_PAD(MAXSIZE));
  n += printf_text("RVA", USE_LT | USE_SPACE | SET_PAD(11));
  n += printf_text("Size", USE_LT | USE_SPACE | USE_EOL);

  return n;
}

static int dump_sectiongroups1(const pbuffer_t p, const int index, const uint64_t VirtualAddress, const uint64_t Size) {
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

    dump_sectiongroups0(p);

    for (size_t i = 0; i < op->NumberOfRvaAndSizes; ++i, ++dd) {
      n += dump_sectiongroups1(p, i, dd->VirtualAddress, dd->Size);
    }

    n += printf_eol();
  }

  return n;
}

static int dump_version0(const pbuffer_t p, const uint16_t wLength, const uint16_t wValueLength, const uint16_t wType,
                         const uint16_t Padding1, const uint16_t Padding2, const uint16_t Children) {
  int n = 0;

  n += printf_text("VS VERSIONINFO", USE_LT | USE_COLON | USE_EOL);
  n += printf_text("wLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wValueLength", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wValueLength, USE_FHEX16 | USE_EOL);
  n += printf_text("wType", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(wType, USE_FHEX16 | USE_EOL);
//  WCHAR            szKey[16];
  n += printf_text("Padding1", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Padding1, USE_FHEX16 | USE_EOL);
  n += printf_text("Padding2", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Padding2, USE_FHEX16 | USE_EOL);
  n += printf_text("Children", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Children, USE_FHEX16 | USE_EOL);
  n += printf_eol();

  return n;
}

static int dump_version1(const pbuffer_t p, const uint32_t dwSignature, const uint32_t dwStrucVersion,
                         const uint32_t dwFileVersionMS, const uint32_t dwFileVersionLS, const uint32_t dwProductVersionMS, const uint32_t dwProductVersionLS,
                         const uint32_t dwFileFlagsMask, const uint32_t dwFileFlags, const uint32_t dwFileOS,
                         const uint32_t dwFileType, const uint32_t dwFileSubtype, const uint32_t dwFileDateMS, const uint32_t dwFileDateLS) {
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

static int dump_versionNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  VS_VERSIONINFO x0;
  PVS_VERSIONINFO p1 = &x0;
  PVS_FIXEDFILEINFO p2 = &x0.Value;
  if (p1) {
    n += dump_version0(p, p1->wLength, p1->wValueLength, p1->wType, p1->Padding1, p1->Padding2, p1->Children);
    n += dump_version1(p, p2->dwSignature, p2->dwStrucVersion, p2->dwFileVersionMS, p2->dwFileVersionLS, p2->dwProductVersionMS, p2->dwProductVersionLS,
                       p2->dwFileFlagsMask, p2->dwFileFlags, p2->dwFileOS, p2->dwFileType, p2->dwFileSubtype, p2->dwFileDateMS, p2->dwFileDateLS);
  }

  return n;
}

static int dump_iat0(const pbuffer_t p, const uint64_t Characteristics, const uint64_t OriginalFirstThunk,
              const uint64_t TimeDateStamp, const uint64_t ForwarderChain, const uint64_t Name, const char* sname, const uint64_t FirstThunk) {
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

static int dump_iat32(const pbuffer_t p, const poptions_t o) {
  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

  if (p0 && s0) {

    PIMAGE_IMPORT_DESCRIPTOR p1 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

    if (p1) {
      printf_text("IMAGE IMPORT DESCRIPTOR", USE_LT | USE_COLON | USE_EOL);
    }

    while (p1 && p1->FirstThunk) {
      dump_iat0(p, p1->Characteristics, p1->OriginalFirstThunk, p1->TimeDateStamp, p1->ForwarderChain,
                p1->Name, getp(p, peconvert2va(s0, p1->Name), 1), p1->FirstThunk);

      PIMAGE_THUNK_DATA32 p2 = (PIMAGE_THUNK_DATA32)
        getp(p, peconvert2va(s0, p1->OriginalFirstThunk), sizeof(IMAGE_THUNK_DATA32));

      while (p2 && p2->AddressOfData) {
        if (p2->AddressOfData < 0x80000000) {
          PIMAGE_IMPORT_BY_NAME p3 = getp(p, peconvert2va(s0, p2->AddressOfData), sizeof(IMAGE_IMPORT_BY_NAME));
          printf_text(p3->Name, USE_LT | USE_TAB2);
          printf_nice(p3->Hint, USE_DEC | USE_SB | USE_EOL);
        } else {
          printf_text("Ordinal", USE_LT | USE_TAB2 | USE_COLON);
          printf_nice(p2->Ordinal, USE_DEC | USE_SB | USE_EOL);
        }
        ++p2;
      }

      ++p1;
    }

    printf_eol();
  }

  return 0;
}

static int dump_iat64(const pbuffer_t p, const poptions_t o) {
  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

  if (p0 && s0) {

    PIMAGE_IMPORT_DESCRIPTOR p1 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

    if (p1) {
      printf_text("IMAGE IMPORT DESCRIPTOR", USE_LT | USE_COLON | USE_EOL);
    }

    while (p1 && p1->FirstThunk) {
      dump_iat0(p, p1->Characteristics, p1->OriginalFirstThunk, p1->TimeDateStamp, p1->ForwarderChain,
                p1->Name, getp(p, peconvert2va(s0, p1->Name), 1), p1->FirstThunk);

      PIMAGE_THUNK_DATA64 p2 = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_IMPORT, p1->OriginalFirstThunk, sizeof(PIMAGE_THUNK_DATA64));

      while (p2 && p2->AddressOfData) {
        if (p2->AddressOfData < 0x80000000) {
          PIMAGE_IMPORT_BY_NAME p3 = getp(p, peconvert2va(s0, p2->AddressOfData), sizeof(IMAGE_IMPORT_BY_NAME));
          printf_text(p3->Name, USE_LT | USE_TAB2);
          printf_nice(p3->Hint, USE_DEC | USE_SB | USE_EOL);
        } else {
          printf_text("Ordinal", USE_LT | USE_TAB2 | USE_COLON);
          printf_nice(p2->Ordinal, USE_DEC | USE_SB | USE_EOL);
        }
        ++p2;
      }

      ++p1;
    }

    printf_eol();
  }

  return 0;
}

static int dump_eat0(const pbuffer_t p, const uint64_t Characteristics, const uint64_t TimeDateStamp, const uint64_t MajorVersion, const uint64_t MinorVersion,
                     const uint64_t Name, const char* sname, const uint64_t Base, const uint64_t NumberOfFunctions, const uint64_t NumberOfNames,
                     const uint64_t AddressOfFunctions, const uint64_t AddressOfNames, const uint64_t AddressOfNameOrdinals) {
  int n = 0;
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

  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);
  PIMAGE_EXPORT_DIRECTORY p1 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);

  if (p0 && p1 && s0) {
    n += printf_text("IMAGE EXPORT DIRECTORY", USE_LT | USE_COLON | USE_EOL);
    n += dump_eat0(p, p1->Characteristics, p1->TimeDateStamp, p1->MajorVersion, p1->MinorVersion,
              p1->Name, getp(p, peconvert2va(s0, p1->Name), 1), p1->Base, p1->NumberOfFunctions, p1->NumberOfNames,
              p1->AddressOfFunctions, p1->AddressOfNames, p1->AddressOfNameOrdinals);

    PDWORD addrOfNames = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_EXPORT, p1->AddressOfNames, sizeof(DWORD) * p1->NumberOfNames);
    PDWORD addrOfFunctions = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_EXPORT, p1->AddressOfFunctions, sizeof(DWORD) * p1->NumberOfFunctions);
    PWORD  addrOfNameOrdinals = get_chunkbyRVA(p, IMAGE_DIRECTORY_ENTRY_EXPORT, p1->AddressOfNameOrdinals, sizeof(WORD) * p1->NumberOfFunctions);

    n += printf_text("Ord", USE_RT | SET_PAD(6));
    n += printf_text("RVA", USE_LT | USE_SPACE | SET_PAD(12));
    n += printf_text("Name", USE_LT | USE_EOL);

    for (int i = 0; i < p1->NumberOfNames; ++i) {
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

static int dump_resourceY(const pbuffer_t p, PIMAGE_RESOURCE_DATA_ENTRY p0) {
  int n = 0;

  if (p0) {
    n += dump_resource2(p, p0->OffsetToData, p0->Size, p0->CodePage, p0->Reserved);
  }

  return n;
}

static int dump_resourceZ(const pbuffer_t p, PIMAGE_RESOURCE_DIRECTORY p0, const int z) {
  int n = 0;

  if (p0) {
    n += dump_resource0(p, p0->Characteristics, p0->TimeDateStamp,
                   p0->MajorVersion, p0->MinorVersion, p0->NumberOfNamedEntries, p0->NumberOfIdEntries);

    PIMAGE_RESOURCE_DIRECTORY_ENTRY p1 = CAST(PIMAGE_RESOURCE_DIRECTORY_ENTRY, p0 + 1);
    for (int i = 0; i < (p0->NumberOfNamedEntries + p0->NumberOfIdEntries); ++i, ++p1) {
      n += dump_resource1(p, p1->NameIsString, p1->Name, p1->OffsetToData, z);
      if (p1->DataIsDirectory) {
        n += dump_resourceZ(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE) + p1->OffsetToDirectory, z + 1);
      } else {
        n += dump_resourceY(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE) + p1->OffsetToDirectory);
      }
    }
  }

  return n;
}

static int dump_resourceNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  n += dump_resourceZ(p, get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE), 1);

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
    const imode_t USE_FHEXNN = (isPE64(p) ? USE_FHEX64 : USE_FHEX32) | USE_EOL;

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
    n += printf_nice(DeCommitFreeBlockThreshold, USE_FHEXNN);
    n += printf_text("DeCommitTotalFreeThreshold", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DeCommitTotalFreeThreshold, USE_FHEXNN);
    n += printf_text("LockPrefixTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(LockPrefixTable, USE_FHEXNN);
    n += printf_text("MaximumAllocationSize", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(MaximumAllocationSize, USE_FHEXNN);
    n += printf_text("VirtualMemoryThreshold", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(VirtualMemoryThreshold, USE_FHEXNN);
    n += printf_text("ProcessAffinityMask", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(ProcessAffinityMask, USE_FHEXNN);
    n += printf_text("ProcessHeapFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(ProcessHeapFlags, USE_FHEX32 | USE_EOL);
    n += printf_text("CSDVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CSDVersion, USE_FHEX16 | USE_EOL);
    n += printf_text("Reserved1", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Reserved1, USE_FHEX16 | USE_EOL);
    n += printf_text("EditList", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(EditList, USE_FHEXNN);
    n += printf_text("SecurityCookie", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SecurityCookie, USE_FHEXNN);
    n += printf_text("SEHandlerTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SEHandlerTable, USE_FHEXNN);
    n += printf_text("SEHandlerCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(SEHandlerCount, USE_FHEXNN);
  }

  return n;
}

static int dump_config1(const pbuffer_t p, const uint64_t GuardCFCheckFunctionPointer, const uint64_t GuardCFDispatchFunctionPointer,
                        const uint64_t GuardCFFunctionTable, const uint64_t GuardCFFunctionCount, const uint32_t GuardFlags) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = (isPE64(p) ? USE_FHEX64 : USE_FHEX32) | USE_EOL;

    n += printf_text("GuardCFCheckFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFCheckFunctionPointer, USE_FHEXNN);
    n += printf_text("GuardCFDispatchFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFDispatchFunctionPointer, USE_FHEXNN);
    n += printf_text("GuardCFFunctionTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFFunctionTable, USE_FHEXNN);
    n += printf_text("GuardCFFunctionCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardCFFunctionCount, USE_FHEXNN);
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
    const imode_t USE_FHEXNN = (isPE64(p) ? USE_FHEX64 : USE_FHEX32) | USE_EOL;

    n += printf_text("CodeIntegrityFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityFlags, USE_FHEX16 | USE_EOL);
    n += printf_text("CodeIntegrityCatalog", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityCatalog, USE_FHEX16 | USE_EOL);
    n += printf_text("CodeIntegrityCatalogOffset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityCatalogOffset, USE_FHEX32 | USE_EOL);
    n += printf_text("CodeIntegrityReserved", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CodeIntegrityReserved, USE_FHEX32 | USE_EOL);
    n += printf_text("GuardAddressTakenIatEntryTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardAddressTakenIatEntryTable, USE_FHEXNN);
    n += printf_text("GuardAddressTakenIatEntryCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardAddressTakenIatEntryCount, USE_FHEXNN);
    n += printf_text("GuardLongJumpTargetTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardLongJumpTargetTable, USE_FHEXNN);
    n += printf_text("GuardLongJumpTargetCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardLongJumpTargetCount, USE_FHEXNN);
    n += printf_text("DynamicValueRelocTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DynamicValueRelocTable, USE_FHEXNN);
    n += printf_text("CHPEMetadataPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CHPEMetadataPointer, USE_FHEXNN);
  }

  return n;
}

static int dump_config3(const pbuffer_t p, const uint64_t GuardRFFailureRoutine, const uint64_t GuardRFFailureRoutineFunctionPointer,
                        const uint32_t DynamicValueRelocTableOffset, const uint16_t DynamicValueRelocTableSection, const uint16_t Reserved2,
                        const uint64_t GuardRFVerifyStackPointerFunctionPointer, const uint32_t HotPatchTableOffset) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = (isPE64(p) ? USE_FHEX64 : USE_FHEX32) | USE_EOL;

    n += printf_text("GuardRFFailureRoutine", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardRFFailureRoutine, USE_FHEXNN);
    n += printf_text("GuardRFFailureRoutineFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardRFFailureRoutineFunctionPointer, USE_FHEXNN);
    n += printf_text("DynamicValueRelocTableOffset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DynamicValueRelocTableOffset, USE_FHEX32 | USE_EOL);
    n += printf_text("DynamicValueRelocTableSection", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(DynamicValueRelocTableSection, USE_FHEX16 | USE_EOL);
    n += printf_text("Reserved2", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Reserved2, USE_FHEX16 | USE_EOL);
    n += printf_text("GuardRFVerifyStackPointerFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardRFVerifyStackPointerFunctionPointer, USE_FHEXNN);
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
    const imode_t USE_FHEXNN = (isPE64(p) ? USE_FHEX64 : USE_FHEX32) | USE_EOL;

    n += printf_text("Reserved3", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Reserved3, USE_FHEX32 | USE_EOL);
    n += printf_text("EnclaveConfigurationPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(EnclaveConfigurationPointer, USE_FHEXNN);
    n += printf_text("VolatileMetadataPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(VolatileMetadataPointer, USE_FHEXNN);
    n += printf_text("GuardEHContinuationTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardEHContinuationTable, USE_FHEXNN);
    n += printf_text("GuardEHContinuationCount", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardEHContinuationCount, USE_FHEXNN);
    n += printf_text("GuardXFGCheckFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardXFGCheckFunctionPointer, USE_FHEXNN);
    n += printf_text("GuardXFGDispatchFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardXFGDispatchFunctionPointer, USE_FHEXNN);
    n += printf_text("GuardXFGTableDispatchFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardXFGTableDispatchFunctionPointer, USE_FHEXNN);
    n += printf_text("CastGuardOsDeterminedFailureMode", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(CastGuardOsDeterminedFailureMode, USE_FHEXNN);
    n += printf_text("GuardMemcpyFunctionPointer", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(GuardMemcpyFunctionPointer, USE_FHEXNN);
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

int readpe(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {
    if (o->action & OPTREADELF_FILEHEADER)         dump_peheader(p, o);
    if (o->action & OPTREADELF_FILEHEADER)         dump_dosheaderNN(p, o);

    if (isPE32(p)) {
      if (o->action & OPTREADELF_FILEHEADER)       dump_ntheader32(p, o);
      if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders32(p, o);
      if (o->action & OPTREADELF_SECTIONGROUPS)    dump_sectiongroups32(p, o);
      if (o->action & OPTREADELF_VERSION)          dump_versionNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_eatNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_iat32(p, o);
      if (o->action & OPTREADELF_NOTES)            dump_resourceNN(p, o);
      if (o->action & OPTREADELF_NOTES)            dump_config32(p, o);
    } else if (isPE64(p)) {
      if (o->action & OPTREADELF_FILEHEADER)       dump_ntheader64(p, o);
      if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders64(p, o);
      if (o->action & OPTREADELF_SECTIONGROUPS)    dump_sectiongroups64(p, o);
      if (o->action & OPTREADELF_VERSION)          dump_versionNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_eatNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_iat64(p, o);
      if (o->action & OPTREADELF_NOTES)            dump_resourceNN(p, o);
      if (o->action & OPTREADELF_NOTES)            dump_config64(p, o);
    }

    if (o->action & OPTREADELF_NOTES)              dump_debugNN(p, o);
    if (o->action & OPTREADELF_RELOCS)             dump_relocNN(p, o);
    if (o->action & OPTREADELF_UNWIND)             dump_runtimeNN(p, o);
  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

