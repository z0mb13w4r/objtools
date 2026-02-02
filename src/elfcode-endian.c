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

    pdst->e_type      = ecconvert_u16(p, psrc->e_type);        // Elf32_Half
    pdst->e_machine   = ecconvert_u16(p, psrc->e_machine);     // Elf32_Half
    pdst->e_version   = ecconvert_u32(p, psrc->e_version);     // Elf32_Word
    pdst->e_entry     = ecconvert_u32(p, psrc->e_entry);       // Elf32_Addr
    pdst->e_phoff     = ecconvert_u32(p, psrc->e_phoff);       // Elf32_Off
    pdst->e_shoff     = ecconvert_u32(p, psrc->e_shoff);       // Elf32_Off
    pdst->e_flags     = ecconvert_u32(p, psrc->e_flags);       // Elf32_Word
    pdst->e_ehsize    = ecconvert_u16(p, psrc->e_ehsize);      // Elf32_Half
    pdst->e_phentsize = ecconvert_u16(p, psrc->e_phentsize);   // Elf32_Half
    pdst->e_phnum     = ecconvert_u16(p, psrc->e_phnum);       // Elf32_Half
    pdst->e_shentsize = ecconvert_u16(p, psrc->e_shentsize);   // Elf32_Half
    pdst->e_shnum     = ecconvert_u16(p, psrc->e_shnum);       // Elf32_Half
    pdst->e_shstrndx  = ecconvert_u16(p, psrc->e_shstrndx);    // Elf32_Half

    return dst;
  }

  return NULL;
}

Elf64_Ehdr* ecconvert_ehdr64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Ehdr* pdst = CAST(Elf64_Ehdr*, dst);
    Elf64_Ehdr* psrc = CAST(Elf64_Ehdr*, src);

    pdst->e_type      = ecconvert_u16(p, psrc->e_type);        // Elf64_Half
    pdst->e_machine   = ecconvert_u16(p, psrc->e_machine);     // Elf64_Half
    pdst->e_version   = ecconvert_u32(p, psrc->e_version);     // Elf64_Word
    pdst->e_entry     = ecconvert_u64(p, psrc->e_entry);       // Elf64_Addr
    pdst->e_phoff     = ecconvert_u64(p, psrc->e_phoff);       // Elf64_Off
    pdst->e_shoff     = ecconvert_u64(p, psrc->e_shoff);       // Elf64_Off
    pdst->e_flags     = ecconvert_u32(p, psrc->e_flags);       // Elf64_Word
    pdst->e_ehsize    = ecconvert_u16(p, psrc->e_ehsize);      // Elf64_Half
    pdst->e_phentsize = ecconvert_u16(p, psrc->e_phentsize);   // Elf64_Half
    pdst->e_phnum     = ecconvert_u16(p, psrc->e_phnum);       // Elf64_Half
    pdst->e_shentsize = ecconvert_u16(p, psrc->e_shentsize);   // Elf64_Half
    pdst->e_shnum     = ecconvert_u16(p, psrc->e_shnum);       // Elf64_Half
    pdst->e_shstrndx  = ecconvert_u16(p, psrc->e_shstrndx);    // Elf64_Half

    return dst;
  }

  return NULL;
}

Elf32_Shdr* ecconvert_shdr32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Shdr* pdst = CAST(Elf32_Shdr*, dst);
    Elf32_Shdr* psrc = CAST(Elf32_Shdr*, src);

    pdst->sh_name      = ecconvert_u32(p, psrc->sh_name);      // Elf32_Word
    pdst->sh_type      = ecconvert_u32(p, psrc->sh_type);      // Elf32_Word
    pdst->sh_flags     = ecconvert_u32(p, psrc->sh_flags);     // Elf32_Word
    pdst->sh_addr      = ecconvert_u32(p, psrc->sh_addr);      // Elf32_Addr
    pdst->sh_offset    = ecconvert_u32(p, psrc->sh_offset);    // Elf32_Off
    pdst->sh_size      = ecconvert_u32(p, psrc->sh_size);      // Elf32_Word
    pdst->sh_link      = ecconvert_u32(p, psrc->sh_link);      // Elf32_Word
    pdst->sh_info      = ecconvert_u32(p, psrc->sh_info);      // Elf32_Word
    pdst->sh_addralign = ecconvert_u32(p, psrc->sh_addralign); // Elf32_Word
    pdst->sh_entsize   = ecconvert_u32(p, psrc->sh_entsize);   // Elf32_Word

    return dst;
  }

  return NULL;
}

Elf64_Shdr* ecconvert_shdr64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Shdr* pdst = CAST(Elf64_Shdr*, dst);
    Elf64_Shdr* psrc = CAST(Elf64_Shdr*, src);

    pdst->sh_name      = ecconvert_u32(p, psrc->sh_name);      // Elf64_Word
    pdst->sh_type      = ecconvert_u32(p, psrc->sh_type);      // Elf64_Word
    pdst->sh_flags     = ecconvert_u64(p, psrc->sh_flags);     // Elf64_Xword
    pdst->sh_addr      = ecconvert_u64(p, psrc->sh_addr);      // Elf64_Addr
    pdst->sh_offset    = ecconvert_u64(p, psrc->sh_offset);    // Elf64_Off
    pdst->sh_size      = ecconvert_u64(p, psrc->sh_size);      // Elf64_Xword
    pdst->sh_link      = ecconvert_u32(p, psrc->sh_link);      // Elf64_Word
    pdst->sh_info      = ecconvert_u32(p, psrc->sh_info);      // Elf64_Word
    pdst->sh_addralign = ecconvert_u64(p, psrc->sh_addralign); // Elf64_Xword
    pdst->sh_entsize   = ecconvert_u64(p, psrc->sh_entsize);   // Elf64_Xword

    return dst;
  }

  return NULL;
}

