#ifndef __OPCODE_BFD_H_
#define __OPCODE_BFD_H_

#include "opcode.h"

#define NUM_KNOWN_OBJ_ATTRIBUTES   (77)

#define bfdget_tdata(x)            ((x)->tdata.elf_obj_data)
#define bfdget_ehdr(x)             (bfdget_tdata(x)->ehdr)
#define bfdget_phdr(x)             (bfdget_tdata(x)->phdr)

typedef struct bfd_ehdr_s {
  uchar_t        e_ident[16];
  bfd_vma        e_entry;
  bfd_size_type  e_phoff;
  bfd_size_type  e_shoff;
  ulong_t        e_version;
  ulong_t        e_flags;
  ushort_t       e_type;
  ushort_t       e_machine;
  uint_t         e_ehsize;
  uint_t         e_phentsize;
  uint_t         e_phnum;
  uint_t         e_shentsize;
  uint_t         e_shnum;
  uint_t         e_shstrndx;
} bfd_ehdr_t, *pbfd_ehdr_t;

typedef struct bfd_phdr_s {
  ulong_t        p_type;
  ulong_t        p_flags;
  bfd_vma        p_offset;
  bfd_vma        p_vaddr;
  bfd_vma        p_paddr;
  bfd_vma        p_filesz;
  bfd_vma        p_memsz;
  bfd_vma        p_align;
} bfd_phdr_t, *pbfd_phdr_t;

typedef struct bfd_shdr_s {
  uint_t         sh_name;
  uint_t         sh_type;
  bfd_vma        sh_flags;
  bfd_vma        sh_addr;
  file_ptr       sh_offset;
  bfd_size_type  sh_size;
  uint_t         sh_link;
  uint_t         sh_info;
  bfd_vma        sh_addralign;
  bfd_size_type  sh_entsize;

  asection      *sections;
  uchar_t       *contents;
} bfd_shdr_t, *pbfd_shdr_t;

typedef struct bfd_sym_s {
  bfd_vma        st_value;
  bfd_vma        st_size;
  ulong_t        st_name;
  uchar_t        st_info;
  uchar_t        st_other;
  uchar_t        st_target;
  uint_t         st_shndx;
} bfd_sym_t, *pbfd_sym_t;

typedef struct bfd_verdaux_s {
  ulong_t        vda_name;
  ulong_t        vda_next;

  const char           *vda_cname;
  struct bfd_verdaux_s *vda_pnext;
} bfd_verdaux_t, *pbfd_verdaux_t;

typedef struct bfd_verdef_s {
  ushort_t       vd_version;
  ushort_t       vd_flags;
  ushort_t       vd_ndx;
  ushort_t       vd_cnt;
  ulong_t        vd_hash;
  ulong_t        vd_aux;
  ulong_t        vd_next;

  bfd                  *vd_bfd;
  const char           *vd_cname;
  struct bfd_verdef_s  *vd_pnext;
  struct bfd_verdaux_s *vd_paux;
  uint_t                vd_expref;
} bfd_verdef_t, *pbfd_verdef_t;

typedef struct bfd_vernaux_s {
  ulong_t        vna_hash;
  ushort_t       vna_flags;
  ushort_t       vna_other;
  ulong_t        vna_name;
  ulong_t        vna_next;

  const char           *vna_cname;
  struct bfd_vernaux_s *vna_pnext;
} bfd_vernaux_t, *pbfd_vernaux_t;

typedef struct bfd_verneed_s {
  ushort_t       vn_version;
  ushort_t       vn_cnt;
  ulong_t        vn_file;
  ulong_t        vn_aux;
  ulong_t        vn_next;

  bfd                  *vn_bfd;
  const char           *vn_cname;
  struct bfd_vernaux_s *vn_paux;
  struct bfd_verneed_s *vn_pnext;
} bfd_verneed_t, *pbfd_verneed_t;

typedef struct bfd_lshdr_s {
  bfd_shdr_t    shdr;
  uint_t        ndx;
  struct bfd_lshdr_s *next;
} bfd_lshdr_t, *pbfd_lshdr_t;

enum bdf_property_kind_t {
  eBFD_PROPERTY_UNKNOWN = 0,
  eBFD_PROPERTY_IGNORED,
  eBFD_PROPERTY_CORRUPT,
  eBFD_PROPERTY_REMOVE,
  eBFD_PROPERTY_NUMBER
 };

typedef struct gnu_property_s {
  uint_t         pr_type;
  uint_t         pr_datasz;
  union {
      bfd_vma number;
  } u;
  enum bdf_property_kind_t pr_kind;
} gnu_property_t;

typedef struct gnu_lproperty_s {
  struct gnu_lproperty_s *next;
  struct gnu_property_s   property;
} gnu_lproperty_t;

typedef struct objattr_s {
  int            type;
  uint_t         idx;
  char          *s;
} objattr_t, *pobjattr_t;

typedef struct lobjattr_s {
  struct lobjattr_s *next;
  uint_t         tag;
  objattr_t      attr;
} lobjattr_t, *plobjattr_t;

struct elf_obj_tdata {
  bfd_ehdr_t     ehdr[1];
  pbfd_shdr_t   *shdr;
  pbfd_phdr_t    phdr;
  bfd_shdr_t     symtab_shdr;
  bfd_shdr_t     shstrtab_shdr;
  bfd_shdr_t     strtab_shdr;
  bfd_shdr_t     dynsymtab_shdr;
  bfd_shdr_t     dynstrtab_shdr;
  bfd_shdr_t     dynversym_shdr;
  bfd_shdr_t     dynverref_shdr;
  bfd_shdr_t     dynverdef_shdr;
  pbfd_sym_t     dt_symtab;

  bfd_byte      *dt_versym;
  bfd_byte      *dt_verdef;
  bfd_byte      *dt_verneed;

  size_t         dt_symtabsize;
  size_t         dt_verdefsize;
  size_t         dt_verneedsize;

  char          *dt_strtab;

  bfd_lshdr_t   *symtab_shndx;

  bfd_vma        gp;
  uint_t         gpsize;
  uint_t         shdrsize;
  uchar_t       *being_created;

  struct elf_link_hash_entry **sym_hashes;

  union {
    bfd_signed_vma    *refcounts;
    bfd_vma           *offsets;
    struct got_entry **ents;
  } got_locals;

  const char    *dt_name;
  const char    *dt_audit;

  unknown_t      line_info;
  unknown_t      dwarf1_find_line_info;
  unknown_t      dwarf2_find_line_info;
  unknown_t      bfd_find_function_cache;

  uint_t         verdefsize;
  uint_t         verneedsize;

  pbfd_verdef_t  verdef;
  pbfd_verneed_t verneed;

  asection      *eh_frame_section;

  unknown_t      symbuf;

  gnu_lproperty_t    *gnu_properties;

  objattr_t      known_obj_attributes[2][NUM_KNOWN_OBJ_ATTRIBUTES];
  plobjattr_t    other_obj_attributes[2];

  struct sdt_note *sdt_note_head;

  pbfd_shdr_t   *groups;
  uint_t         group_num;
  uint_t         group_offset;

  uint_t         symtab_section;
  uint_t         dynsymtab_section;
  uint_t         dynversym_section;
  uint_t         dynverdef_section;
  uint_t         dynverref_section;

  uint_t         object_id : 6;
  uint_t         dyn_lib_class : 4;
  uint_t         has_gnu_osabi : 4;

  uint_t         has_gnu_no_copy_on_protected : 1;
  uint_t         has_gnu_indirect_extern_access : 1;

  uint_t         isbad_symtab : 1;
  uint_t         is_dfpie : 1;

  struct core_elf_obj_tdata   *core;
  struct output_elf_obj_tdata *output;
};

int opcodebfd_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param);
int opcodebfd_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param);

char* opcodebfd_getsymbol(handle_t p, const uint64_t vaddr, uint64_t *offset);

#endif

