#include "readpe.h"
#include "printf.h"

#include "static/filehdr.ci"
#include "static/opthdr.ci"
#include "static/sechdr.ci"

const int MAXSIZE = 36;

int dump_peheader(const pbuffer_t p, const poptions_t o) {
  if (issafe(p)) {
    if (o->action & OPTPROGRAM_HASH)   printf_sore(p->data, p->size, USE_HASHALL | USE_EOL);
  }

  return 0;
}

int dump_dosheader(const pbuffer_t p, const poptions_t o) {
  PIMAGE_DOS_HEADER dos = get_doshdr(p);
  if (dos) {
    size_t i = 0;

    printf_text("DOS HEADER", USE_LT | USE_COLON | USE_EOL);
    printf_text("e_magic", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_magic, USE_FHEX16 | USE_EOL);
    printf_text("e_cblp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_cblp, USE_FHEX16 | USE_EOL);
    printf_text("e_cp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_cp, USE_FHEX16 | USE_EOL);
    printf_text("e_crlc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_crlc, USE_FHEX16 | USE_EOL);
    printf_text("e_cparhdr", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_cparhdr, USE_FHEX16 | USE_EOL);
    printf_text("e_minalloc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_minalloc, USE_FHEX16 | USE_EOL);
    printf_text("e_maxalloc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_maxalloc, USE_FHEX16 | USE_EOL);
    printf_text("e_ss", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_ss, USE_FHEX16 | USE_EOL);
    printf_text("e_sp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_sp, USE_FHEX16 | USE_EOL);
    printf_text("e_csum", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_csum, USE_FHEX16 | USE_EOL);
    printf_text("e_ip", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_ip, USE_FHEX16 | USE_EOL);
    printf_text("e_cs", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_cs, USE_FHEX16 | USE_EOL);
    printf_text("e_lfarlc", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_lfarlc, USE_FHEX16 | USE_EOL);
    printf_text("e_ovno", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_ovno, USE_FHEX16 | USE_EOL);
    printf_text("e_res", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    for (i = 0; i < NELEMENTS(dos->e_res) - 1; ++i) printf_nice(dos->e_res[i], USE_FHEX16);
    printf_nice(dos->e_res[i], USE_FHEX16 | USE_EOL);
    printf_text("e_oemid", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_oemid, USE_FHEX16 | USE_EOL);
    printf_text("e_oeminfo", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_oeminfo, USE_FHEX16 | USE_EOL);
    printf_text("e_res2", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    for (i = 0; i < NELEMENTS(dos->e_res2) - 1; ++i) printf_nice(dos->e_res2[i], USE_FHEX16);
    printf_nice(dos->e_res2[i], USE_FHEX16 | USE_EOL);
    printf_text("e_lfanew", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(dos->e_lfanew, USE_FHEX16 | USE_EOL);
    printf_eol();
  }

  return 0;
}

int dump_ntheader0(const pbuffer_t p, const uint64_t Machine, const uint64_t NumberOfSections,
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

int dump_ntheader1(const pbuffer_t p, const uint64_t Magic, const uint64_t MajorLinkerVersion, const uint64_t MinorLinkerVersion,
                   const uint64_t SizeOfCode, const uint64_t SizeOfInitializedData, const uint64_t SizeOfUninitializedData,
                   const uint64_t AddressOfEntryPoint, const uint64_t BaseOfCode, const uint64_t ImageBase, const uint64_t SectionAlignment,
                   const uint64_t FileAlignment, const uint64_t MajorOperatingSystemVersion, const uint64_t MinorOperatingSystemVersion,
                   const uint64_t MajorImageVersion, const uint64_t MinorImageVersion, const uint64_t MajorSubsystemVersion, const uint64_t MinorSubsystemVersion,
                   const uint64_t Win32VersionValue, const uint64_t SizeOfImage, const uint64_t SizeOfHeaders, const uint64_t CheckSum,
                   const uint64_t Subsystem, const uint64_t DllCharacteristics, const uint64_t SizeOfStackReserve, const uint64_t SizeOfStackCommit,
                   const uint64_t SizeOfHeapReserve, const uint64_t SizeOfHeapCommit, const uint64_t LoaderFlags, const uint64_t NumberOfRvaAndSizes) {
  int n = 0;
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
  n += printf_nice(ImageBase, USE_FHEX32 | USE_EOL);
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
  n += printf_nice(SizeOfStackReserve, USE_FHEX32 | USE_EOL);
  n += printf_text("SizeOfStackCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(SizeOfStackCommit, USE_FHEX32 | USE_EOL);
  n += printf_text("SizeOfHeapReserve", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(SizeOfHeapReserve, USE_FHEX32 | USE_EOL);
  n += printf_text("SizeOfHeapCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(SizeOfHeapCommit, USE_FHEX32 | USE_EOL);
  n += printf_text("LoaderFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(LoaderFlags, USE_FHEX32 | USE_EOL);
  n += printf_text("NumberOfRvaAndSizes", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(NumberOfRvaAndSizes, USE_FHEX32 | USE_EOL);
  n += printf_eol();

  return n;
}

int dump_ntheader32(const pbuffer_t p, const poptions_t o) {
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

int dump_ntheader64(const pbuffer_t p, const poptions_t o) {
  PIMAGE_NT_HEADERS32 nt = get_nt64hdr(p);
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

int dump_sectionheaders0(const pbuffer_t p, const uint64_t NumberOfSections) {
  int n = 0;
  for (int i = 0; i < NumberOfSections; ++i) {
    PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyindex(p, i);
    if (p0) {
      n += printf_text("SECTION HEADER", USE_LT | USE_COLON | USE_EOL);
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

int dump_sectionheaders32(const pbuffer_t p, const poptions_t o) {
  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    dump_sectionheaders0(p, nt->FileHeader.NumberOfSections);
  }

  return 0;
}

int dump_sectionheaders64(const pbuffer_t p, const poptions_t o) {
  PIMAGE_NT_HEADERS64 nt = get_nt64hdr(p);
  if (nt) {
    dump_sectionheaders0(p, nt->FileHeader.NumberOfSections);
  }

  return 0;
}

int dump_sectiongroups32(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = strlenpick(zOPTHDRENTRY) + 2;

  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    PIMAGE_OPTIONAL_HEADER32 op = &nt->OptionalHeader;
    PIMAGE_DATA_DIRECTORY dd = op->DataDirectory;

    printf_text("DATA DIRECTORY", USE_LT | USE_COLON | USE_EOL);
    printf_text("Name", USE_LT | USE_TAB | SET_PAD(MAXSIZE));
    printf_text("RVA", USE_LT | USE_SPACE | SET_PAD(11));
    printf_text("Size", USE_LT | USE_SPACE | USE_EOL);

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

int dump_iat32(const pbuffer_t p, const poptions_t o) {
  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_IMPORT);

  if (p0 && s0) {

    PIMAGE_IMPORT_DESCRIPTOR p1 = (PIMAGE_IMPORT_DESCRIPTOR)
      getp(p, peconvert2va(s0, p0->VirtualAddress), sizeof(IMAGE_IMPORT_DESCRIPTOR));

    if (p1) {
      printf_text("IMAGE IMPORT DESCRIPTOR", USE_LT | USE_COLON | USE_EOL);
    }

    while (p1 && p1->FirstThunk) {
      printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      printf_nice(p1->Characteristics, USE_FHEX32 | USE_EOL);
      printf_text("OriginalFirstThunk", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      printf_nice(p1->OriginalFirstThunk, USE_FHEX32 | USE_EOL);
      printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      printf_nice(p1->TimeDateStamp, USE_FHEX32);
      printf_nice(p1->TimeDateStamp, USE_TIMEDATE | USE_SB | USE_EOL);
      printf_text("ForwarderChain", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      printf_nice(p1->ForwarderChain, USE_FHEX32 | USE_EOL);
      printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      printf_nice(p1->Name, USE_FHEX32 | USE_EOL);
      printf_text("FirstThunk", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      printf_nice(p1->FirstThunk, USE_FHEX32 | USE_EOL);

      printf_text(getp(p, peconvert2va(s0, p1->Name), 1), USE_LT | USE_TAB | USE_EOL);
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

int dump_eat32(const pbuffer_t p, const poptions_t o) {
  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);
  PIMAGE_SECTION_HEADER s0 = get_sectionhdrbyentry(p, IMAGE_DIRECTORY_ENTRY_EXPORT);

  if (p0 && s0) {
    PIMAGE_EXPORT_DIRECTORY p1 = (PIMAGE_EXPORT_DIRECTORY)
      getp(p, peconvert2va(s0, p0->VirtualAddress), sizeof(IMAGE_EXPORT_DIRECTORY));

   if (p1) {
      printf_text("IMAGE EXPORT DIRECTORY", USE_LT | USE_COLON | USE_EOL);

   }
  }

  return 0;
}

int readpe(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {
    if (o->action & OPTREADELF_FILEHEADER)         dump_peheader(p, o);
    if (o->action & OPTREADELF_FILEHEADER)         dump_dosheader(p, o);

    if (isPE32(p)) {
      if (o->action & OPTREADELF_FILEHEADER)       dump_ntheader32(p, o);
      if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders32(p, o);
      if (o->action & OPTREADELF_SECTIONGROUPS)    dump_sectiongroups32(p, o);

      if (o->action & OPTREADELF_SYMBOLS)          dump_iat32(p, o);
      if (o->action & OPTREADELF_SYMBOLS)          dump_eat32(p, o);
    } else if (isPE64(p)) {
      if (o->action & OPTREADELF_FILEHEADER)       dump_ntheader64(p, o);
      if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders64(p, o);

    }
  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

