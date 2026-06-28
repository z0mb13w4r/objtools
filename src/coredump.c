#include "dump.h"
#include "printf.h"
#include "dumpelf.h"
#include "elfcode.h"
#include "coredump.h"

int coredump(const pbuffer_t p, const poptions_t o) {
  if (isELF(p)) {
    if (MODE_ISNOT(o->action, OPTREADELF_FILEHEADER))           dump_summary(p, o);
    if (MODE_ISANY(o->action, OPTREADELF_FILEHEADER))           dumpelf_elfheader(p, o);

    if (isELF32(p)) {
      Elf32_Ehdr *ehdr = ecget_ehdr32(p, xmalloc(sizeof(Elf32_Ehdr), MODE_HEAP));
      if (ehdr) {
        if (MODE_ISANY(o->action, OPTREADELF_FILEHEADER))       dumpelf_fileheader32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONMASK))      dumpelf_sectionheaders32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONGROUPS))    dumpelf_sectiongroups32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_PROGRAMHEADERS))   dumpelf_programheaders32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_DYNAMIC))          dumpelf_dynamic32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_RELOCS))           dumpelf_relocs32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_UNWIND))           dumpelf_unwind32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SYMBOLS))          dumpelf_symbols32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_HISTOGRAM))        dumpelf_histogram32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_VERSION))          dumpelf_version32(p, o, ehdr);
        if (o->actions)                                         dumpelf_actions32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_NOTES))            dumpelf_notes32(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_ARCHSPECIFIC))     dumpelf_archspecific32(p, o, ehdr);
        if (MODE_ISANY(o->ocdump, OPTDWARF_DEBUGGING))          dumpelf_dwarf32(p, o, ehdr);
      }

      xfree(ehdr);
    } else if (isELF64(p)) {
      Elf64_Ehdr *ehdr = ecget_ehdr64(p, xmalloc(sizeof(Elf64_Ehdr), MODE_HEAP));
      if (ehdr) {
        if (MODE_ISANY(o->action, OPTREADELF_FILEHEADER))       dumpelf_fileheader64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONMASK))      dumpelf_sectionheaders64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SECTIONGROUPS))    dumpelf_sectiongroups64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_PROGRAMHEADERS))   dumpelf_programheaders64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_DYNAMIC))          dumpelf_dynamic64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_RELOCS))           dumpelf_relocs64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_UNWIND))           dumpelf_unwind64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_SYMBOLS))          dumpelf_symbols64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_HISTOGRAM))        dumpelf_histogram64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_VERSION))          dumpelf_version64(p, o, ehdr);
        if (o->actions)                                         dumpelf_actions64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_NOTES))            dumpelf_notes64(p, o, ehdr);
        if (MODE_ISANY(o->action, OPTREADELF_ARCHSPECIFIC))     dumpelf_archspecific64(p, o, ehdr);
        if (MODE_ISANY(o->ocdump, OPTDWARF_DEBUGGING))          dumpelf_dwarf64(p, o, ehdr);
      }

      xfree(ehdr);
    }
  } else {
    printf_e("not an ELF file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

