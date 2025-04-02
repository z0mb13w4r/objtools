#include "readpe.h"
#include "printf.h"

#include "static/dbghdr.ci"
#include "static/filehdr.ci"
#include "static/opthdr.ci"
#include "static/sechdr.ci"
#include "static/unw_flags.ci"

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

static int dump_ntheader0(const pbuffer_t p, const uint64_t Machine, const uint64_t NumberOfSections,
                   const uint64_t TimeDateStamp, const uint64_t PointerToSymbolTable, const uint64_t NumberOfSymbols,
                   const uint64_t SizeOfOptionalHeader, const uint64_t Characteristics) {
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

static int dump_ntheader1(const pbuffer_t p, const uint64_t Magic, const uint64_t MajorLinkerVersion, const uint64_t MinorLinkerVersion,
                   const uint64_t SizeOfCode, const uint64_t SizeOfInitializedData, const uint64_t SizeOfUninitializedData,
                   const uint64_t AddressOfEntryPoint, const uint64_t BaseOfCode, const uint64_t ImageBase, const uint64_t SectionAlignment,
                   const uint64_t FileAlignment, const uint64_t MajorOperatingSystemVersion, const uint64_t MinorOperatingSystemVersion,
                   const uint64_t MajorImageVersion, const uint64_t MinorImageVersion, const uint64_t MajorSubsystemVersion, const uint64_t MinorSubsystemVersion,
                   const uint64_t Win32VersionValue, const uint64_t SizeOfImage, const uint64_t SizeOfHeaders, const uint64_t CheckSum,
                   const uint64_t Subsystem, const uint64_t DllCharacteristics, const uint64_t SizeOfStackReserve, const uint64_t SizeOfStackCommit,
                   const uint64_t SizeOfHeapReserve, const uint64_t SizeOfHeapCommit, const uint64_t LoaderFlags, const uint64_t NumberOfRvaAndSizes) {
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

    dump_ntheader0(p, fp->Machine, fp->NumberOfSections, fp->TimeDateStamp, fp->PointerToSymbolTable, fp->NumberOfSymbols, fp->SizeOfOptionalHeader, fp->Characteristics);
    dump_ntheader1(p, op->Magic, op->MajorLinkerVersion, op->MinorLinkerVersion, op->SizeOfCode, op->SizeOfInitializedData, op->SizeOfUninitializedData,
                   op->AddressOfEntryPoint, op->BaseOfCode, op->ImageBase, op->SectionAlignment, op->FileAlignment, op->MajorOperatingSystemVersion,
                   op->MinorOperatingSystemVersion, op->MajorImageVersion, op->MinorImageVersion, op->MajorSubsystemVersion, op->MinorSubsystemVersion,
                   op->Win32VersionValue, op->SizeOfImage, op->SizeOfHeaders, op->CheckSum, op->Subsystem, op->DllCharacteristics, op->SizeOfStackReserve,
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

    dump_ntheader0(p, fp->Machine, fp->NumberOfSections, fp->TimeDateStamp, fp->PointerToSymbolTable, fp->NumberOfSymbols, fp->SizeOfOptionalHeader, fp->Characteristics);
    dump_ntheader1(p, op->Magic, op->MajorLinkerVersion, op->MinorLinkerVersion, op->SizeOfCode, op->SizeOfInitializedData, op->SizeOfUninitializedData,
                   op->AddressOfEntryPoint, op->BaseOfCode, op->ImageBase, op->SectionAlignment, op->FileAlignment, op->MajorOperatingSystemVersion,
                   op->MinorOperatingSystemVersion, op->MajorImageVersion, op->MinorImageVersion, op->MajorSubsystemVersion, op->MinorSubsystemVersion,
                   op->Win32VersionValue, op->SizeOfImage, op->SizeOfHeaders, op->CheckSum, op->Subsystem, op->DllCharacteristics, op->SizeOfStackReserve,
                   op->SizeOfStackCommit, op->SizeOfHeapReserve, op->SizeOfHeapCommit, op->LoaderFlags, op->NumberOfRvaAndSizes);
  }

  return 0;
}

static int dump_sectionheaders0(const pbuffer_t p, const uint64_t NumberOfSections) {
  int n = 0;
  for (int i = 0; i < NumberOfSections; ++i) {
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

static int dump_sectiongroups32(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = strlenpick(zOPTHDRENTRY) + 2;

  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    PIMAGE_OPTIONAL_HEADER32 op = &nt->OptionalHeader;
    PIMAGE_DATA_DIRECTORY dd = op->DataDirectory;

    dump_sectiongroups0(p);

    for (size_t i = 0; i < op->NumberOfRvaAndSizes; ++i, ++dd) {
      printf_pick(zOPTHDRENTRY, i, USE_LT | USE_TAB | SET_PAD(MAXSIZE));
      if (0 != dd->VirtualAddress) printf_nice(dd->VirtualAddress, USE_FHEX32);
      else                         printf_text("NONE", USE_LT | USE_SPACE | SET_PAD(11));
      if (0 != dd->Size)           printf_nice(dd->Size, USE_FHEX32 | USE_EOL);
      else                         printf_text("NONE", USE_LT | USE_SPACE | USE_EOL);
    }

    printf_eol();
  }

  return 0;
}

static int dump_sectiongroups64(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = strlenpick(zOPTHDRENTRY) + 2;

  PIMAGE_NT_HEADERS64 nt = get_nt64hdr(p);
  if (nt) {
    PIMAGE_OPTIONAL_HEADER64 op = &nt->OptionalHeader;
    PIMAGE_DATA_DIRECTORY dd = op->DataDirectory;

    dump_sectiongroups0(p);

    for (size_t i = 0; i < op->NumberOfRvaAndSizes; ++i, ++dd) {
      printf_pick(zOPTHDRENTRY, i, USE_LT | USE_TAB | SET_PAD(MAXSIZE));
      if (0 != dd->VirtualAddress) printf_nice(dd->VirtualAddress, USE_FHEX32);
      else                         printf_text("NONE", USE_LT | USE_SPACE | SET_PAD(11));
      if (0 != dd->Size)           printf_nice(dd->Size, USE_FHEX32 | USE_EOL);
      else                         printf_text("NONE", USE_LT | USE_SPACE | USE_EOL);
    }

    printf_eol();
  }

  return 0;
}

int dump_iat0(const pbuffer_t p, const uint64_t Characteristics, const uint64_t OriginalFirstThunk,
              const uint64_t TimeDateStamp, const uint64_t ForwarderChain, const uint64_t Name, const char* sname, const uint64_t FirstThunk) {
  int n = 0;
  n += printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Characteristics, USE_FHEX32 | USE_EOL);
  n += printf_text("OriginalFirstThunk", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(OriginalFirstThunk, USE_FHEX32 | USE_EOL);
  n += printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(TimeDateStamp, USE_FHEX32);
  n += printf_nice(TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
  n += printf_text("ForwarderChain", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(ForwarderChain, USE_FHEX32 | USE_EOL);
  n += printf_text("FirstThunk", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(FirstThunk, USE_FHEX32 | USE_EOL);
  n += printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(Name, USE_FHEX32);
  n += printf_text(sname, USE_LT | USE_SPACE | USE_SB | USE_EOL);

  return n;
}

int dump_iat32(const pbuffer_t p, const poptions_t o) {
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

int dump_iat64(const pbuffer_t p, const poptions_t o) {
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

    for (int i = 0; i < p1->NumberOfNames; ++i) {
      n += printf_nice(addrOfNameOrdinals[i] + 1, USE_DEC5);
      n += printf_nice(addrOfFunctions[i], USE_FHEX32);
      n += printf_text(getp(p, peconvert2va(s0, addrOfNames[i]), 1), USE_LT | USE_SPACE | USE_EOL);
    }
  }

  return n;
}

static int dump_resource0(const pbuffer_t p, const uint64_t Characteristics, const uint64_t TimeDateStamp,
                          const uint64_t MajorVersion, const uint64_t MinorVersion, const uint64_t NumberOfNamedEntries, const uint64_t NumberOfIdEntries) {
  int n = 0;
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

  return 0;
}

static int dump_resourceNN(const pbuffer_t p, const poptions_t o) {
  PIMAGE_RESOURCE_DIRECTORY p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_RESOURCE);
  if (p0) {
      printf_text("IMAGE RESOURCE DIRECTORY", USE_LT | USE_COLON | USE_EOL);

      dump_resource0(p, p0->Characteristics, p0->TimeDateStamp,
                     p0->MajorVersion, p0->MinorVersion, p0->NumberOfNamedEntries, p0->NumberOfIdEntries);
  }
  // TBD
  return 0;
}

static int dump_config0(const pbuffer_t p, const uint64_t Size, const uint64_t TimeDateStamp,
                        const uint64_t MajorVersion, const uint64_t MinorVersion, const uint64_t GlobalFlagsClear, const uint64_t GlobalFlagsSet,
                        const uint64_t CriticalSectionDefaultTimeout, const uint64_t DeCommitFreeBlockThreshold, const uint64_t DeCommitTotalFreeThreshold,
                        const uint64_t LockPrefixTable, const uint64_t MaximumAllocationSize, const uint64_t VirtualMemoryThreshold,
                        const uint64_t ProcessAffinityMask, const uint64_t ProcessHeapFlags, const uint64_t CSDVersion, const uint64_t Reserved1,
                        const uint64_t EditList, const uint64_t SecurityCookie, const uint64_t SEHandlerTable, const uint64_t SEHandlerCount) {
  int n = 0;
  if (issafe(p)) {
    const imode_t USE_FHEXNN = (isPE64(p) ? USE_FHEX64 : USE_FHEX32) | USE_EOL;

    printf_text("IMAGE LOAD CONFIG DIRECTORY", USE_LT | USE_COLON | USE_EOL);

    n += printf_text("Size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(Size, USE_FHEX32 | USE_EOL);
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
    n += printf_eol();
  }

  return n;
}

static int dump_config32(const pbuffer_t p, const poptions_t o) {
  PIMAGE_LOAD_CONFIG_DIRECTORY32 p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
  if (p0) {
    dump_config0(p, p0->Size, p0->TimeDateStamp, p0->MajorVersion, p0->MinorVersion, p0->GlobalFlagsClear, p0->GlobalFlagsSet,
                 p0->CriticalSectionDefaultTimeout, p0->DeCommitFreeBlockThreshold, p0-> DeCommitTotalFreeThreshold, p0->LockPrefixTable,
                 p0->MaximumAllocationSize, p0->VirtualMemoryThreshold, p0->ProcessAffinityMask, p0->ProcessHeapFlags, p0->CSDVersion,
                 p0->Reserved1, p0->EditList, p0->SecurityCookie, p0->SEHandlerTable, p0->SEHandlerCount);
  }

  return 0;
}

static int dump_config64(const pbuffer_t p, const poptions_t o) {
  PIMAGE_LOAD_CONFIG_DIRECTORY64 p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
  if (p0) {
    dump_config0(p, p0->Size, p0->TimeDateStamp, p0->MajorVersion, p0->MinorVersion, p0->GlobalFlagsClear, p0->GlobalFlagsSet,
                 p0->CriticalSectionDefaultTimeout, p0->DeCommitFreeBlockThreshold, p0-> DeCommitTotalFreeThreshold, p0->LockPrefixTable,
                 p0->MaximumAllocationSize, p0->VirtualMemoryThreshold, p0->ProcessAffinityMask, p0->ProcessHeapFlags, p0->CSDVersion,
                 p0->Reserved1, p0->EditList, p0->SecurityCookie, p0->SEHandlerTable, p0->SEHandlerCount);
  }

  return 0;
}

static int dump_debugNN(const pbuffer_t p, const poptions_t o) {
  int n = 0;

  PIMAGE_DEBUG_DIRECTORY p0 = get_chunkbyentry(p, IMAGE_DIRECTORY_ENTRY_DEBUG);
  if (p0) {
    n += printf_text("IMAGE LOAD CONFIG DIRECTORY", USE_LT | USE_COLON | USE_EOL);

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
      n += printf_text("IMAGE BASE RELOCATION", USE_LT | USE_COLON | USE_EOL);
      n += printf_text("VirtualAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p1->VirtualAddress, USE_FHEX32 | USE_EOL);
      n += printf_text("SizeOfBlock", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(p1->SizeOfBlock, USE_FHEX32 | USE_EOL);
      n += printf_eol();

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
//break;  // JUST FOR DEVELOPMENT
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

      if (o->action & OPTREADELF_SYMBOLS)          dump_eatNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_iat32(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_resourceNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_config32(p, o);
    } else if (isPE64(p)) {
      if (o->action & OPTREADELF_FILEHEADER)       dump_ntheader64(p, o);
      if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders64(p, o);
      if (o->action & OPTREADELF_SECTIONGROUPS)    dump_sectiongroups64(p, o);

      if (o->action & OPTREADELF_SYMBOLS)          dump_eatNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_iat64(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_resourceNN(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_config64(p, o);
    }

    if (o->action & OPTREADELF_SYMBOLS)            dump_debugNN(p, o);
    if (o->action & OPTREADELF_SYMBOLS)            dump_relocNN(p, o);
    if (o->action & OPTREADELF_SYMBOLS)            dump_runtimeNN(p, o);
  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

