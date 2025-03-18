#include "readpe.h"
#include "printf.h"

#include "static/filehdr.ci"

int dump_dosheader(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = 36;

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

int dump_ntheader32(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = 36;

  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    printf_text("NT HEADER", USE_LT | USE_COLON | USE_EOL);
    printf_text("Signature", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(nt->Signature, USE_FHEX32 | USE_EOL);
    printf_eol();

    PIMAGE_FILE_HEADER fp = &nt->FileHeader;
    printf_text("FILE HEADER", USE_LT | USE_COLON | USE_EOL);
    printf_text("Machine", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(fp->Machine, USE_FHEX16 | USE_EOL);
    printf_text("NumberOfSections", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(fp->NumberOfSections, USE_FHEX16 | USE_EOL);
    printf_text("TimeDateStamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(fp->TimeDateStamp, USE_FHEX32 | USE_EOL);
    printf_text("PointerToSymbolTable", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(fp->PointerToSymbolTable, USE_FHEX32 | USE_EOL);
    printf_text("NumberOfSymbols", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(fp->NumberOfSymbols, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfOptionalHeader", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(fp->SizeOfOptionalHeader, USE_FHEX16 | USE_EOL);
    printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(fp->Characteristics, USE_FHEX16);
    printf_mask(zFILEHDR, fp->Characteristics, USE_LT | USE_EOL);
    printf_eol();

    PIMAGE_OPTIONAL_HEADER32 op = &nt->OptionalHeader;
    printf_text("OPTIONAL HEADER", USE_LT | USE_COLON | USE_EOL);
    printf_text("Magic", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->Magic, USE_FHEX16 | USE_EOL);
    printf_text("MajorLinkerVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MajorLinkerVersion, USE_FHEX8 | USE_EOL);
    printf_text("MinorLinkerVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MinorLinkerVersion, USE_FHEX8 | USE_EOL);
    printf_text("SizeOfCode", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfCode, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfInitializedData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfInitializedData, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfUninitializedData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfUninitializedData, USE_FHEX32 | USE_EOL);
    printf_text("AddressOfEntryPoint", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->AddressOfEntryPoint, USE_FHEX32 | USE_EOL);
    printf_text("BaseOfCode", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->BaseOfCode, USE_FHEX32 | USE_EOL);
    printf_text("ImageBase", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->ImageBase, USE_FHEX32 | USE_EOL);
    printf_text("SectionAlignment", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SectionAlignment, USE_FHEX32 | USE_EOL);
    printf_text("FileAlignment", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->FileAlignment, USE_FHEX32 | USE_EOL);
    printf_text("MajorOperatingSystemVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MajorOperatingSystemVersion, USE_FHEX16 | USE_EOL);
    printf_text("MinorOperatingSystemVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MinorOperatingSystemVersion, USE_FHEX16 | USE_EOL);
    printf_text("MajorImageVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MajorImageVersion, USE_FHEX16 | USE_EOL);
    printf_text("MinorImageVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MinorImageVersion, USE_FHEX16 | USE_EOL);
    printf_text("MajorSubsystemVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MajorSubsystemVersion, USE_FHEX16 | USE_EOL);
    printf_text("MinorSubsystemVersion", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->MinorSubsystemVersion, USE_FHEX16 | USE_EOL);
    printf_text("Win32VersionValue", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->Win32VersionValue, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfImage", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfImage, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfHeaders", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfHeaders, USE_FHEX32 | USE_EOL);
    printf_text("CheckSum", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->CheckSum, USE_FHEX32 | USE_EOL);
    printf_text("Subsystem", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->Subsystem, USE_FHEX16 | USE_EOL);
    printf_text("DllCharacteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->DllCharacteristics, USE_FHEX16 | USE_EOL);
    printf_text("SizeOfStackReserve", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfStackReserve, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfStackCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfStackCommit, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfHeapReserve", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfHeapReserve, USE_FHEX32 | USE_EOL);
    printf_text("SizeOfHeapCommit", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->SizeOfHeapCommit, USE_FHEX32 | USE_EOL);
    printf_text("LoaderFlags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->LoaderFlags, USE_FHEX32 | USE_EOL);
    printf_text("NumberOfRvaAndSizes", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(op->NumberOfRvaAndSizes, USE_FHEX32 | USE_EOL);
    printf_eol();
  }

  return 0;
}

int dump_sectionheaders32(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = 36;

  PIMAGE_NT_HEADERS32 nt = get_nt32hdr(p);
  if (nt) {
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
      PIMAGE_SECTION_HEADER sec = get_sectionhdrbyindex(p, i);
      if (sec) {
        printf_text("SECTION HEADER", USE_LT | USE_COLON | USE_EOL);
        printf_text("Name", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_data(sec->Name, sizeof(sec->Name), 0, USE_STR | USE_EOL);

        printf_text("Misc.PhysicalAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->Misc.PhysicalAddress, USE_FHEX32 | USE_EOL);
        printf_text("Misc.VirtualSize", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->Misc.VirtualSize, USE_FHEX32 | USE_EOL);

        printf_text("VirtualAddress", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->VirtualAddress, USE_FHEX32 | USE_EOL);
        printf_text("SizeOfRawData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->SizeOfRawData, USE_FHEX32 | USE_EOL);
        printf_text("PointerToRawData", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->PointerToRawData, USE_FHEX32 | USE_EOL);
        printf_text("PointerToRelocations", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->PointerToRelocations, USE_FHEX32 | USE_EOL);
        printf_text("PointerToLinenumbers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->PointerToLinenumbers, USE_FHEX32 | USE_EOL);
        printf_text("NumberOfRelocations", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->NumberOfRelocations, USE_FHEX16 | USE_EOL);
        printf_text("NumberOfLinenumbers", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->NumberOfLinenumbers, USE_FHEX16 | USE_EOL);
        printf_text("Characteristics", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(sec->Characteristics, USE_FHEX32 | USE_EOL);
        printf_eol();
      }
    }
  }

  return 0;
}

int readpe(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {
    if (o->action & OPTREADELF_FILEHEADER)         dump_dosheader(p, o);

    if (isPE32(p)) {
      if (o->action & OPTREADELF_FILEHEADER)       dump_ntheader32(p, o);
      if (o->action & OPTREADELF_SECTIONHEADERS)   dump_sectionheaders32(p, o);
    } else if (isPE64(p)) {
    }
  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

