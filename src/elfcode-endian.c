#include "elfcode-endian.h"

static uint16_t ecconvert_u8(const pbuffer_t p, const uint8_t v) {
  return v;
}

uint16_t ecconvert_u16(const pbuffer_t p, const uint16_t v) {
  return endian_u16(isELFbe(p), v);
}

uint32_t ecconvert_u32(const pbuffer_t p, const uint32_t v) {
  return endian_u32(isELFbe(p), v);
}

uint64_t ecconvert_u64(const pbuffer_t p, const uint64_t v) {
  return endian_u64(isELFbe(p), v);
}

Elf32_Dyn* ecconvert_dyn32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Dyn* pdst = CAST(Elf32_Dyn*, dst);
    Elf32_Dyn* psrc = CAST(Elf32_Dyn*, src);

    pdst->d_tag      = ecconvert_u32(p, psrc->d_tag);          // Elf32_Sword
    pdst->d_un.d_val = ecconvert_u32(p, psrc->d_un.d_val);     // Elf32_Word

    return dst;
  }

  return NULL;
}

Elf64_Dyn* ecconvert_dyn64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Dyn* pdst = CAST(Elf64_Dyn*, dst);
    Elf64_Dyn* psrc = CAST(Elf64_Dyn*, src);

    pdst->d_tag      = ecconvert_u64(p, psrc->d_tag);          // Elf64_Sxword
    pdst->d_un.d_val = ecconvert_u64(p, psrc->d_un.d_val);     // Elf64_Xword

    return dst;
  }

  return NULL;
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

Elf32_Nhdr* ecconvert_nhdr32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Nhdr* pdst = CAST(Elf32_Nhdr*, dst);
    Elf32_Nhdr* psrc = CAST(Elf32_Nhdr*, src);

    pdst->n_namesz = ecconvert_u32(p, psrc->n_namesz);         // Elf32_Word
    pdst->n_descsz = ecconvert_u32(p, psrc->n_descsz);         // Elf32_Word
    pdst->n_type   = ecconvert_u32(p, psrc->n_type);           // Elf32_Word

    return dst;
  }

  return NULL;
}

Elf64_Nhdr* ecconvert_nhdr64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Nhdr* pdst = CAST(Elf64_Nhdr*, dst);
    Elf64_Nhdr* psrc = CAST(Elf64_Nhdr*, src);

    pdst->n_namesz = ecconvert_u32(p, psrc->n_namesz);         // Elf64_Word
    pdst->n_descsz = ecconvert_u32(p, psrc->n_descsz);         // Elf64_Word
    pdst->n_type   = ecconvert_u32(p, psrc->n_type);           // Elf64_Word

    return dst;
  }

  return NULL;
}

Elf32_Phdr* ecconvert_phdr32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Phdr* pdst = CAST(Elf32_Phdr*, dst);
    Elf32_Phdr* psrc = CAST(Elf32_Phdr*, src);

    pdst->p_type   = ecconvert_u32(p, psrc->p_type);           // Elf32_Word
    pdst->p_offset = ecconvert_u32(p, psrc->p_offset);         // Elf32_Off
    pdst->p_vaddr  = ecconvert_u32(p, psrc->p_vaddr);          // Elf32_Addr
    pdst->p_paddr  = ecconvert_u32(p, psrc->p_paddr);          // Elf32_Addr
    pdst->p_filesz = ecconvert_u32(p, psrc->p_filesz);         // Elf32_Word
    pdst->p_memsz  = ecconvert_u32(p, psrc->p_memsz);          // Elf32_Word
    pdst->p_flags  = ecconvert_u32(p, psrc->p_flags);          // Elf32_Word
    pdst->p_align  = ecconvert_u32(p, psrc->p_align);          // Elf32_Word

    return dst;
  }

  return NULL;
}

Elf64_Phdr* ecconvert_phdr64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Phdr* pdst = CAST(Elf64_Phdr*, dst);
    Elf64_Phdr* psrc = CAST(Elf64_Phdr*, src);

    pdst->p_type   = ecconvert_u32(p, psrc->p_type);           // Elf64_Word
    pdst->p_offset = ecconvert_u64(p, psrc->p_offset);         // Elf64_Off
    pdst->p_vaddr  = ecconvert_u64(p, psrc->p_vaddr);          // Elf64_Addr
    pdst->p_paddr  = ecconvert_u64(p, psrc->p_paddr);          // Elf64_Addr
    pdst->p_filesz = ecconvert_u64(p, psrc->p_filesz);         // Elf64_Xword
    pdst->p_memsz  = ecconvert_u64(p, psrc->p_memsz);          // Elf64_Xword
    pdst->p_flags  = ecconvert_u32(p, psrc->p_flags);          // Elf64_Word
    pdst->p_align  = ecconvert_u64(p, psrc->p_align);          // Elf64_Xword

    return dst;
  }

  return NULL;
}

Elf32_Rel* ecconvert_rel32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Rel* pdst = CAST(Elf32_Rel*, dst);
    Elf32_Rel* psrc = CAST(Elf32_Rel*, src);

    pdst->r_offset = ecconvert_u32(p, psrc->r_offset);         // Elf32_Addr
    pdst->r_info   = ecconvert_u32(p, psrc->r_info);           // Elf32_Word

    return dst;
  }

  return NULL;
}

Elf64_Rel* ecconvert_rel64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Rel* pdst = CAST(Elf64_Rel*, dst);
    Elf64_Rel* psrc = CAST(Elf64_Rel*, src);

    pdst->r_offset = ecconvert_u64(p, psrc->r_offset);         // Elf64_Addr
    pdst->r_info   = ecconvert_u64(p, psrc->r_info);           // Elf64_Xword

    return dst;
  }

  return NULL;
}

Elf32_Rela* ecconvert_rela32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Rela* pdst = CAST(Elf32_Rela*, dst);
    Elf32_Rela* psrc = CAST(Elf32_Rela*, src);

    pdst->r_offset = ecconvert_u32(p, psrc->r_offset);         // Elf32_Addr
    pdst->r_info   = ecconvert_u32(p, psrc->r_info);           // Elf32_Word
    pdst->r_addend = ecconvert_u32(p, psrc->r_addend);         // Elf32_Sword

    return dst;
  }

  return NULL;
}

Elf64_Rela* ecconvert_rela64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Rela* pdst = CAST(Elf64_Rela*, dst);
    Elf64_Rela* psrc = CAST(Elf64_Rela*, src);

    pdst->r_offset = ecconvert_u64(p, psrc->r_offset);         // Elf64_Addr
    pdst->r_info   = ecconvert_u64(p, psrc->r_info);           // Elf64_Xword
    pdst->r_addend = ecconvert_u64(p, psrc->r_addend);         // Elf64_Sxword

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

Elf32_Sym* ecconvert_sym32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Sym* pdst = CAST(Elf32_Sym*, dst);
    Elf32_Sym* psrc = CAST(Elf32_Sym*, src);

    pdst->st_name  = ecconvert_u32(p, psrc->st_name);          // Elf32_Word
    pdst->st_value = ecconvert_u32(p, psrc->st_value);         // Elf32_Addr
    pdst->st_size  = ecconvert_u32(p, psrc->st_size);          // Elf32_Word
    pdst->st_info  = ecconvert_u8(p, psrc->st_info);           // unsigned char
    pdst->st_other = ecconvert_u8(p, psrc->st_other);          // unsigned char
    pdst->st_shndx = ecconvert_u16(p, psrc->st_shndx);         // Elf32_Section

    return dst;
  }

  return NULL;
}

Elf64_Sym* ecconvert_sym64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Sym* pdst = CAST(Elf64_Sym*, dst);
    Elf64_Sym* psrc = CAST(Elf64_Sym*, src);

    pdst->st_name  = ecconvert_u32(p, psrc->st_name);          // Elf64_Word
    pdst->st_info  = ecconvert_u8(p, psrc->st_info);           // unsigned char
    pdst->st_other = ecconvert_u8(p, psrc->st_other);          // unsigned char
    pdst->st_shndx = ecconvert_u16(p, psrc->st_shndx);         // Elf64_Section
    pdst->st_value = ecconvert_u64(p, psrc->st_value);         // Elf64_Addr
    pdst->st_size  = ecconvert_u64(p, psrc->st_size);          // Elf64_Xword

    return dst;
  }

  return NULL;
}

Elf32_Vernaux* ecconvert_vernaux32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Vernaux* pdst = CAST(Elf32_Vernaux*, dst);
    Elf32_Vernaux* psrc = CAST(Elf32_Vernaux*, src);

    pdst->vna_flags = ecconvert_u32(p, psrc->vna_flags);       // Elf32_Half
    pdst->vna_hash  = ecconvert_u16(p, psrc->vna_hash);        // Elf32_Word
    pdst->vna_name  = ecconvert_u32(p, psrc->vna_name);        // Elf32_Word
    pdst->vna_next  = ecconvert_u32(p, psrc->vna_next);        // Elf32_Word
    pdst->vna_other = ecconvert_u16(p, psrc->vna_other);       // Elf32_Half

    return dst;
  }

  return NULL;
}

Elf64_Vernaux* ecconvert_vernaux64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Vernaux* pdst = CAST(Elf64_Vernaux*, dst);
    Elf64_Vernaux* psrc = CAST(Elf64_Vernaux*, src);

    pdst->vna_flags = ecconvert_u16(p, psrc->vna_flags);       // Elf64_Half
    pdst->vna_hash  = ecconvert_u32(p, psrc->vna_hash);        // Elf64_Word
    pdst->vna_name  = ecconvert_u32(p, psrc->vna_name);        // Elf64_Word
    pdst->vna_next  = ecconvert_u32(p, psrc->vna_next);        // Elf64_Word
    pdst->vna_other = ecconvert_u16(p, psrc->vna_other);       // Elf64_Half

    return dst;
  }

  return NULL;
}

Elf32_Verneed* ecconvert_verneed32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Verneed* pdst = CAST(Elf32_Verneed*, dst);
    Elf32_Verneed* psrc = CAST(Elf32_Verneed*, src);

    pdst->vn_aux     = ecconvert_u32(p, psrc->vn_aux);         // Elf32_Word
    pdst->vn_cnt     = ecconvert_u16(p, psrc->vn_cnt);         // Elf32_Half
    pdst->vn_file    = ecconvert_u32(p, psrc->vn_file);        // Elf32_Word
    pdst->vn_next    = ecconvert_u32(p, psrc->vn_next);        // Elf32_Word
    pdst->vn_version = ecconvert_u16(p, psrc->vn_version);     // Elf32_Half

    return dst;
  }

  return NULL;
}

Elf64_Verneed* ecconvert_verneed64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Verneed* pdst = CAST(Elf64_Verneed*, dst);
    Elf64_Verneed* psrc = CAST(Elf64_Verneed*, src);

    pdst->vn_aux     = ecconvert_u32(p, psrc->vn_aux);         // Elf64_Word
    pdst->vn_cnt     = ecconvert_u16(p, psrc->vn_cnt);         // Elf64_Half
    pdst->vn_file    = ecconvert_u32(p, psrc->vn_file);        // Elf64_Word
    pdst->vn_next    = ecconvert_u32(p, psrc->vn_next);        // Elf64_Word
    pdst->vn_version = ecconvert_u16(p, psrc->vn_version);     // Elf64_Half

    return dst;
  }

  return NULL;
}

Elf32_Versym* ecconvert_versym32(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF32(p) && dst && src) {
    Elf32_Versym* pdst = CAST(Elf32_Versym*, dst);
    Elf32_Versym* psrc = CAST(Elf32_Versym*, src);

    *pdst = ecconvert_u16(p, *psrc);                           // Elf32_Half

    return dst;
  }

  return NULL;
}

Elf64_Versym* ecconvert_versym64(const pbuffer_t p, unknown_t dst, unknown_t src) {
  if (isELF64(p) && dst && src) {
    Elf64_Versym* pdst = CAST(Elf64_Versym*, dst);
    Elf64_Versym* psrc = CAST(Elf64_Versym*, src);

    *pdst = ecconvert_u16(p, *psrc);                           // Elf64_Half

    return dst;
  }

  return NULL;
}

