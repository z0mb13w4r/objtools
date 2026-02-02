#include "elfcode-endian.h"

static uint8_t ecconvert_u8(const pbuffer_t p, const uint8_t v) {
  return v;
}

static uint16_t ecconvert_u16(const pbuffer_t p, const uint16_t v) {
  if (isELFbe(p)) {
    return MODE_PUT1(MODE_GET0(v)) | MODE_PUT0(MODE_GET1(v));
  }

  return v;
}

static uint32_t ecconvert_u32(const pbuffer_t p, const uint32_t v) {
  if (isELFbe(p)) {
    return MODE_PUT3(MODE_GET0(v)) | MODE_PUT2(MODE_GET1(v))
      | MODE_PUT1(MODE_GET2(v)) | MODE_PUT0(MODE_GET3(v));
  }

  return v;
}

static uint64_t ecconvert_u64(const pbuffer_t p, const uint64_t v) {
  if (isELFbe(p)) {
    return MODE_PUT7(MODE_GET0(v)) | MODE_PUT6(MODE_GET1(v))
      | MODE_PUT5(MODE_GET2(v)) | MODE_PUT4(MODE_GET3(v))
      | MODE_PUT3(MODE_GET4(v)) | MODE_PUT2(MODE_GET5(v))
      | MODE_PUT1(MODE_GET6(v)) | MODE_PUT0(MODE_GET7(v));
  }

  return v;
}

Elf32_Ehdr* ecconvert_ehdr32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Ehdr* pdst = CAST(Elf32_Ehdr*, dst);
    Elf32_Ehdr* psrc = CAST(Elf32_Ehdr*, src);

    pdst->e_type      = ecconvert_u16(p, psrc->e_type);
    pdst->e_machine   = ecconvert_u16(p, psrc->e_machine);
    pdst->e_version   = ecconvert_u32(p, psrc->e_version);
    pdst->e_entry     = ecconvert_u32(p, psrc->e_entry);
    pdst->e_phoff     = ecconvert_u32(p, psrc->e_phoff);
    pdst->e_shoff     = ecconvert_u32(p, psrc->e_shoff);
    pdst->e_flags     = ecconvert_u32(p, psrc->e_flags);
    pdst->e_ehsize    = ecconvert_u16(p, psrc->e_ehsize);
    pdst->e_phentsize = ecconvert_u16(p, psrc->e_phentsize);
    pdst->e_phnum     = ecconvert_u16(p, psrc->e_phnum);
    pdst->e_shentsize = ecconvert_u16(p, psrc->e_shentsize);
    pdst->e_shnum     = ecconvert_u16(p, psrc->e_shnum);
    pdst->e_shstrndx  = ecconvert_u16(p, psrc->e_shstrndx);

    return dst;
  }

  return NULL;
}

Elf64_Ehdr* ecconvert_ehdr64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Ehdr* pdst = CAST(Elf64_Ehdr*, dst);
    Elf64_Ehdr* psrc = CAST(Elf64_Ehdr*, src);

    pdst->e_type      = ecconvert_u16(p, psrc->e_type);
    pdst->e_machine   = ecconvert_u16(p, psrc->e_machine);
    pdst->e_version   = ecconvert_u32(p, psrc->e_version);
    pdst->e_entry     = ecconvert_u64(p, psrc->e_entry);
    pdst->e_phoff     = ecconvert_u64(p, psrc->e_phoff);
    pdst->e_shoff     = ecconvert_u64(p, psrc->e_shoff);
    pdst->e_flags     = ecconvert_u32(p, psrc->e_flags);
    pdst->e_ehsize    = ecconvert_u16(p, psrc->e_ehsize);
    pdst->e_phentsize = ecconvert_u16(p, psrc->e_phentsize);
    pdst->e_phnum     = ecconvert_u16(p, psrc->e_phnum);
    pdst->e_shentsize = ecconvert_u16(p, psrc->e_shentsize);
    pdst->e_shnum     = ecconvert_u16(p, psrc->e_shnum);
    pdst->e_shstrndx  = ecconvert_u16(p, psrc->e_shstrndx);

    return dst;
  }

  return NULL;
}

