/* Definitions of target machine for GNU compiler,
   for 64 bit powerpc linux.
   Copyright (C) 2000, 2001 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* Yes!  We are AIX! Err. Wait. We're Linux!. No, wait, we're a
  combo of both!*/
#undef DEFAULT_ABI
#define DEFAULT_ABI ABI_AIX

#undef TARGET_AIX
#define TARGET_AIX 1

#undef TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_POWERPC | MASK_POWERPC64 | MASK_64BIT | MASK_NEW_MNEMONICS)

/* AIX does not have any init/fini or ctor/dtor sections, so create
    static constructors and destructors as normal functions.  */
/* #define ASM_OUTPUT_CONSTRUCTOR(file, name) */
/* #define ASM_OUTPUT_DESTRUCTOR(file, name) */
#define USER_LABEL_PREFIX  ""

/* Tell the assembler to assume that all undefined names are external.

   Don't do this until the fixed IBM assembler is more generally available.
   When this becomes permanently defined, the ASM_OUTPUT_EXTERNAL,
   ASM_OUTPUT_EXTERNAL_LIBCALL, and RS6000_OUTPUT_BASENAME macros will no
   longer be needed.  Also, the extern declaration of mcount in ASM_FILE_START
   will no longer be needed.  */

/* #define ASM_SPEC "-u %(asm_cpu)" */

/* AIX word-aligns FP doubles but doubleword-aligns 64-bit ints.  */
#define ADJUST_FIELD_ALIGN(FIELD, COMPUTED) \
  (TYPE_MODE (TREE_CODE (TREE_TYPE (FIELD)) == ARRAY_TYPE \
	      ? get_inner_array_type (FIELD) \
	      : TREE_TYPE (FIELD)) == DFmode \
   ? MIN ((COMPUTED), 32) : (COMPUTED))

/* AIX increases natural record alignment to doubleword if the first
   field is an FP double while the FP fields remain word aligned.  */
#define ROUND_TYPE_ALIGN(STRUCT, COMPUTED, SPECIFIED)	\
  ((TREE_CODE (STRUCT) == RECORD_TYPE			\
    || TREE_CODE (STRUCT) == UNION_TYPE			\
    || TREE_CODE (STRUCT) == QUAL_UNION_TYPE)		\
   && TYPE_FIELDS (STRUCT) != 0				\
   && DECL_MODE (TYPE_FIELDS (STRUCT)) == DFmode	\
   ? MAX (MAX ((COMPUTED), (SPECIFIED)), BIGGEST_ALIGNMENT) \
   : MAX ((COMPUTED), (SPECIFIED)))

/* Indicate that jump tables go in the text section.  */
#undef JUMP_TABLES_IN_TEXT_SECTION
#define JUMP_TABLES_IN_TEXT_SECTION 1

/* Define cutoff for using external functions to save floating point.  */
#undef FP_SAVE_INLINE
#define FP_SAVE_INLINE(FIRST_REG) ((FIRST_REG) == 62 || (FIRST_REG) == 63)

#undef TARGET_NO_TOC
#undef TARGET_TOC

/* 64-bit PowerPC Linux always has a TOC.  */
#define TARGET_NO_TOC		0
#define	TARGET_TOC		1

/* 64-bit PowerPC Linux always has GPR13 fixed.  */
#define FIXED_R13		1

/* __throw will restore its own return address to be the same as the
   return address of the function that the throw is being made to.
   This is unfortunate, because we want to check the original
   return address to see if we need to restore the TOC.
   So we have to squirrel it away with this.  */
#define SETUP_FRAME_ADDRESSES() rs6000_aix_emit_builtin_unwind_init ()

#define PROFILE_HOOK(LABEL)   output_profile_hook (LABEL)
/* Don't assume anything about the header files. */
#define NO_IMPLICIT_EXTERN_C

#undef MD_EXEC_PREFIX
#undef MD_STARTFILE_PREFIX

#undef CPP_PREDEFINES
#define CPP_PREDEFINES \
 "-D_PPC_ -D__PPC__ -D_PPC64_ -D__PPC64__ -D__powerpc__ -D__powerpc64__ \
  -D_PIC_ -D__PIC__ -D_BIG_ENDIAN -D__BIG_ENDIAN__ -D__ELF__ \
  -D__LONG_MAX__=9223372036854775807L \
  -Acpu=powerpc64 -Amachine=powerpc64"

#undef	CPP_OS_DEFAULT_SPEC
#define CPP_OS_DEFAULT_SPEC "%(cpp_os_linux)"

/* The GNU C++ standard library currently requires _GNU_SOURCE being
   defined on glibc-based systems. This temporary hack accomplishes this,
   it should go away as soon as libstdc++-v3 has a real fix.  */
#undef CPLUSPLUS_CPP_SPEC
#define CPLUSPLUS_CPP_SPEC "-D_GNU_SOURCE %(cpp)"

#undef LINK_SHLIB_SPEC
#define LINK_SHLIB_SPEC "%{shared:-shared} %{!shared: %{static:-static}}"

#undef	LIB_DEFAULT_SPEC
#define LIB_DEFAULT_SPEC "%(lib_linux)"

#undef	STARTFILE_DEFAULT_SPEC
#define STARTFILE_DEFAULT_SPEC "%(startfile_linux)"

#undef	ENDFILE_DEFAULT_SPEC
#define ENDFILE_DEFAULT_SPEC "%(endfile_linux)"

#undef	LINK_START_DEFAULT_SPEC
#define LINK_START_DEFAULT_SPEC "%(link_start_linux)"

#undef	LINK_OS_DEFAULT_SPEC
#define LINK_OS_DEFAULT_SPEC "%(link_os_linux)"

#undef  LINK_OS_LINUX_SPEC
#define LINK_OS_LINUX_SPEC "-m elf64ppc %{!shared: %{!static: \
  %{rdynamic:-export-dynamic} \
  %{!dynamic-linker:-dynamic-linker /lib/ld.so.1}}}"

#undef TOC_SECTION_ASM_OP
#define TOC_SECTION_ASM_OP "\t.section\t\".toc\",\"aw\""

#undef MINIMAL_TOC_SECTION_ASM_OP
#define	MINIMAL_TOC_SECTION_ASM_OP "\t.section\t\".toc1\",\"aw\"\n\t.align 3"

#undef TARGET_VERSION
#define TARGET_VERSION fprintf (stderr, " (PowerPC64 GNU/Linux)");

/* Must be at least as big as our pointer type.  */
#undef  SIZE_TYPE
#define SIZE_TYPE "long unsigned int"

#undef  PTRDIFF_TYPE
#define PTRDIFF_TYPE "long int"

#undef  WCHAR_TYPE
#define WCHAR_TYPE "int"
#undef  WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE 32

/* Override rs6000.h definition.  */
#undef ASM_APP_ON
#define ASM_APP_ON "#APP\n"

/* Override rs6000.h definition.  */
#undef ASM_APP_OFF
#define ASM_APP_OFF "#NO_APP\n"

#undef DEFAULT_VTABLE_THUNKS
#ifndef USE_GNULIBC_1
#define DEFAULT_VTABLE_THUNKS 1
#endif

/* PowerPC no-op instruction.  */
#undef RS6000_CALL_GLUE
#define RS6000_CALL_GLUE "nop"

/* Dwarf2 debugging.  */
#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE  DWARF2_DEBUG

/* This macro gets just the user-specified name
   out of the string in a SYMBOL_REF.  Discard
   a leading * or @.  */
#define	STRIP_NAME_ENCODING(VAR,SYMBOL_NAME)				\
do {									\
  const char *_name = SYMBOL_NAME;					\
  while (*_name == '*' || *_name == '@')				\
    _name++;								\
  (VAR) = _name;							\
} while (0)

/* This is how to output a reference to a user-level label named NAME.
   `assemble_name' uses this.  */

/* Override elfos.h definition.  */
#undef	ASM_OUTPUT_LABELREF
#define	ASM_OUTPUT_LABELREF(FILE,NAME)		\
do {						\
  const char *_name = NAME;			\
  if (*_name == '@')				\
    _name++;					\
 						\
  if (*_name == '*')				\
    fprintf (FILE, "%s", _name + 1);		\
  else						\
    asm_fprintf (FILE, "%U%s", _name);		\
} while (0)

#undef  ASM_DECLARE_FUNCTION_NAME
#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)			\
  do {									\
    if (TARGET_RELOCATABLE && (get_pool_size () != 0 || profile_flag)	\
	&& uses_TOC())							\
      {									\
	char buf[256];							\
									\
	ASM_OUTPUT_INTERNAL_LABEL (FILE, "LCL", rs6000_pic_labelno);	\
									\
	ASM_GENERATE_INTERNAL_LABEL (buf, "LCTOC", 1);			\
	fputs ("\t.quad ", FILE);					\
	assemble_name (FILE, buf);					\
	putc ('-', FILE);						\
	ASM_GENERATE_INTERNAL_LABEL (buf, "LCF", rs6000_pic_labelno);	\
	assemble_name (FILE, buf);					\
	putc ('\n', FILE);						\
      }									\
									\
    fprintf (FILE, "%s", TYPE_ASM_OP);					\
    assemble_name (FILE, NAME);						\
    putc (',', FILE);							\
    fprintf (FILE, TYPE_OPERAND_FMT, "function");			\
    putc ('\n', FILE);							\
    ASM_DECLARE_RESULT (FILE, DECL_RESULT (DECL));			\
									\
    if (DEFAULT_ABI == ABI_AIX)						\
      {									\
	const char *desc_name, *orig_name;				\
									\
        STRIP_NAME_ENCODING (orig_name, NAME);				\
        desc_name = orig_name;						\
	while (*desc_name == '.')					\
	  desc_name++;							\
									\
	if (TREE_PUBLIC (DECL))						\
	  fprintf (FILE, "\t.globl %s\n", desc_name);			\
									\
	fputs ("\t.section\t\".opd\",\"aw\"\n", FILE);			\
	fprintf (FILE, "%s:\n", desc_name);				\
	fprintf (FILE, "\t.quad %s\n", orig_name);			\
	fputs ("\t.quad .TOC.@tocbase\n", FILE);			\
	if (DEFAULT_ABI == ABI_AIX)					\
	  fputs ("\t.quad 0\n", FILE);					\
	fprintf (FILE, "\t.previous\n");				\
      }									\
    ASM_OUTPUT_LABEL (FILE, NAME);					\
  } while (0)

/* Return non-zero if this entry is to be written into the constant
   pool in a special way.  We do so if this is a SYMBOL_REF, LABEL_REF
   or a CONST containing one of them.  If -mfp-in-toc (the default),
   we also do this for floating-point constants.  We actually can only
   do this if the FP formats of the target and host machines are the
   same, but we can't check that since not every file that uses
   GO_IF_LEGITIMATE_ADDRESS_P includes real.h.  We also do this when
   we can write the entry into the TOC and the entry is not larger
   than a TOC entry.  */

#undef  ASM_OUTPUT_SPECIAL_POOL_ENTRY_P
#define ASM_OUTPUT_SPECIAL_POOL_ENTRY_P(X, MODE)			\
  (TARGET_TOC								\
   && (GET_CODE (X) == SYMBOL_REF					\
       || (GET_CODE (X) == CONST && GET_CODE (XEXP (X, 0)) == PLUS	\
	   && GET_CODE (XEXP (XEXP (X, 0), 0)) == SYMBOL_REF)		\
       || GET_CODE (X) == LABEL_REF					\
       || (GET_CODE (X) == CONST_INT 					\
	   && GET_MODE_BITSIZE (MODE) <= GET_MODE_BITSIZE (Pmode))	\
       || (GET_CODE (X) == CONST_DOUBLE					\
	   && (TARGET_POWERPC64						\
	       || TARGET_MINIMAL_TOC					\
	       || (GET_MODE_CLASS (GET_MODE (X)) == MODE_FLOAT		\
		   && ! TARGET_NO_FP_IN_TOC)))))

/* This is how to output an assembler line defining an `int'
   constant.  */
#undef  ASM_OUTPUT_INT
#define ASM_OUTPUT_INT(FILE, VALUE)            \
  do                                           \
    {                                          \
      fputs ("\t.long ", (FILE));              \
      output_addr_const ((FILE), (VALUE));     \
      putc ('\n', (FILE));                     \
    }                                          \
  while (0)

/* This is how to output an assembler line defining a `double int'
   constant.  */
#undef  ASM_OUTPUT_DOUBLE_INT
#define ASM_OUTPUT_DOUBLE_INT(FILE, VALUE)     \
  do                                           \
    {                                          \
      fputs (DOUBLE_INT_ASM_OP, (FILE));       \
      output_addr_const ((FILE), (VALUE));     \
      putc ('\n', (FILE));                     \
    }                                          \
  while (0)

#undef  ASM_DEFAULT_SPEC
#define	ASM_DEFAULT_SPEC "-mppc64"
