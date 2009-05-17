	.file "fpclink.pas"
# Begin asmlist al_begin
# End asmlist al_begin
# Begin asmlist al_stabs
# End asmlist al_stabs
# Begin asmlist al_procedures

.section .text
	.balign 16,0x90
.globl	P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER
	.type	P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER,@function
P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER:
.Lc1:
# Temps allocated between ebp-4 and ebp-4
# [fpclink.pas]
# [4] begin
	pushl	%ebp
.Lc3:
.Lc4:
	movl	%esp,%ebp
.Lc5:
	subl	$4,%esp
# Var size located at ebp+8
# Var $result located at ebp-4
# [5] GetMem(pas_malloc,size);
	movl	8(%ebp),%edx
	leal	-4(%ebp),%eax
	call	SYSTEM_GETMEM$POINTER$LONGINT
# [6] end;
	movl	-4(%ebp),%eax
	leave
	ret	$4
.Lc2:
.Le0:
	.size	P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER, .Le0 - P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER

.section .text
	.balign 16,0x90
.globl	P$FPCLINK_PAS_FREE$POINTER
	.type	P$FPCLINK_PAS_FREE$POINTER,@function
P$FPCLINK_PAS_FREE$POINTER:
.Lc6:
# Temps allocated between ebp+0 and ebp+0
# [9] begin
	pushl	%ebp
.Lc8:
.Lc9:
	movl	%esp,%ebp
.Lc10:
# Var mem located at ebp+8
# [10] FreeMem(mem);
	movl	8(%ebp),%eax
	call	SYSTEM_FREEMEM$POINTER$$LONGINT
# [11] end;
	leave
	ret	$4
.Lc7:
.Le1:
	.size	P$FPCLINK_PAS_FREE$POINTER, .Le1 - P$FPCLINK_PAS_FREE$POINTER

.section .text
	.balign 16,0x90
.globl	P$FPCLINK_TEST_STATIC_ARRAY$$TINTARRAY
	.type	P$FPCLINK_TEST_STATIC_ARRAY$$TINTARRAY,@function
P$FPCLINK_TEST_STATIC_ARRAY$$TINTARRAY:
.Lc11:
# Temps allocated between ebp-12 and ebp-12
# [22] begin
	pushl	%ebp
.Lc13:
.Lc14:
	movl	%esp,%ebp
.Lc15:
	subl	$12,%esp
# Var $result located at ebp-4
# Var ptr located at ebp-8
# Var aptr located at ebp-12
	movl	%eax,-4(%ebp)
# [23] ptr:= pas_malloc(5*4);
	pushl	$20
	call	P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER
	movl	%eax,-8(%ebp)
# [24] aptr:= pintarray(ptr);
	movl	-8(%ebp),%eax
	movl	%eax,-12(%ebp)
# [25] test_static_array:= aptr^;
	movl	-12(%ebp),%edx
	movl	-4(%ebp),%ecx
	movl	(%edx),%eax
	movl	%eax,(%ecx)
	movl	4(%edx),%eax
	movl	%eax,4(%ecx)
	movw	8(%edx),%ax
	movw	%ax,8(%ecx)
# [27] pas_free(ptr);
	pushl	-8(%ebp)
	call	P$FPCLINK_PAS_FREE$POINTER
# [28] end;
	leave
	ret
.Lc12:
.Le2:
	.size	P$FPCLINK_TEST_STATIC_ARRAY$$TINTARRAY, .Le2 - P$FPCLINK_TEST_STATIC_ARRAY$$TINTARRAY

.section .text
	.balign 16,0x90
.globl	P$FPCLINK_TEST_DYNAMIC_ARRAY$$TDYNINTARRAY
	.type	P$FPCLINK_TEST_DYNAMIC_ARRAY$$TDYNINTARRAY,@function
P$FPCLINK_TEST_DYNAMIC_ARRAY$$TDYNINTARRAY:
.Lc16:
# Temps allocated between ebp-16 and ebp-16
# [43] begin
	pushl	%ebp
.Lc18:
.Lc19:
	movl	%esp,%ebp
.Lc20:
	subl	$16,%esp
# Var $result located at ebp-4
# Var ptr located at ebp-8
# Var aptr located at ebp-12
# Var temp located at ebp-16
	movl	$0,-4(%ebp)
# [44] ptr:= pas_malloc(4*5+8); //elemsz * numelems + header
	pushl	$28
	call	P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER
	movl	%eax,-8(%ebp)
# [45] temp:= pbytearray(ptr);
	movl	-8(%ebp),%eax
	movl	%eax,-16(%ebp)
# [46] temp^[0]:= 1; //32 bit refcount
	movl	-16(%ebp),%eax
	movb	$1,1(%eax)
# [47] temp^[1]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,2(%eax)
# [48] temp^[2]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,3(%eax)
# [49] temp^[3]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,4(%eax)
# [50] temp^[4]:= 4; //32 bit size (n+1) so this is 5
	movl	-16(%ebp),%eax
	movb	$4,5(%eax)
# [51] temp^[5]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,6(%eax)
# [52] temp^[6]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,7(%eax)
# [53] temp^[7]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,8(%eax)
# [54] temp:= temp + 4; //why this increases by 8 bytes, i'll never know... but it does
	movl	-16(%ebp),%eax
	addl	$8,%eax
	movl	%eax,-16(%ebp)
# [55] aptr:= pdynintarray(@temp);
	leal	-16(%ebp),%eax
	movl	%eax,-12(%ebp)
# [56] aptr^[0]:= 7;
	movl	-12(%ebp),%eax
	movl	(%eax),%eax
	movl	$7,(%eax)
# [57] aptr^[1]:= 7;
	movl	-12(%ebp),%eax
	movl	(%eax),%eax
	movl	$7,4(%eax)
# [58] aptr^[2]:= 7;
	movl	-12(%ebp),%eax
	movl	(%eax),%eax
	movl	$7,8(%eax)
# [59] aptr^[3]:= 7;
	movl	-12(%ebp),%eax
	movl	(%eax),%eax
	movl	$7,12(%eax)
# [60] aptr^[4]:= 7;
	movl	-12(%ebp),%eax
	movl	(%eax),%eax
	movl	$7,16(%eax)
# [61] test_dynamic_array:= aptr^;
	movl	$INIT_P$FPCLINK_TDYNINTARRAY,%ecx
	leal	-4(%ebp),%edx
	movl	-12(%ebp),%eax
	call	fpc_copy
# [63] pas_free(ptr);
	pushl	-8(%ebp)
	call	P$FPCLINK_PAS_FREE$POINTER
# [64] end;
	movl	-4(%ebp),%eax
	leave
	ret
.Lc17:
.Le3:
	.size	P$FPCLINK_TEST_DYNAMIC_ARRAY$$TDYNINTARRAY, .Le3 - P$FPCLINK_TEST_DYNAMIC_ARRAY$$TDYNINTARRAY

.section .text
	.balign 16,0x90
.globl	P$FPCLINK_TEST_ANSI_STRING$$ANSISTRING
	.type	P$FPCLINK_TEST_ANSI_STRING$$ANSISTRING,@function
P$FPCLINK_TEST_ANSI_STRING$$ANSISTRING:
.Lc21:
# Temps allocated between ebp-20 and ebp-16
# [79] begin
	pushl	%ebp
.Lc23:
.Lc24:
	movl	%esp,%ebp
.Lc25:
	subl	$20,%esp
	movl	%ebx,-20(%ebp)
# Var $result located at ebp-4
# Var ptr located at ebp-8
# Var aptr located at ebp-12
# Var temp located at ebp-16
	movl	$0,-4(%ebp)
# [80] ptr:= pas_malloc(5+1+8+5); //chars + null + header
	pushl	$19
	call	P$FPCLINK_PAS_MALLOC$LONGINT$$POINTER
	movl	%eax,-8(%ebp)
# [81] temp:= pbytearray(ptr);
	movl	-8(%ebp),%eax
	movl	%eax,-16(%ebp)
# [82] temp^[0]:= 1; //32 bit refcount
	movl	-16(%ebp),%eax
	movb	$1,1(%eax)
# [83] temp^[1]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,2(%eax)
# [84] temp^[2]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,3(%eax)
# [85] temp^[3]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,4(%eax)
# [86] temp^[4]:= 4; //32 bit size (n+1) so this is 5
	movl	-16(%ebp),%eax
	movb	$4,5(%eax)
# [87] temp^[5]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,6(%eax)
# [88] temp^[6]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,7(%eax)
# [89] temp^[7]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,8(%eax)
# [90] temp^[8]:=  Ord('h');
	movl	-16(%ebp),%eax
	movb	$104,9(%eax)
# [91] temp^[9]:=  Ord('e');
	movl	-16(%ebp),%eax
	movb	$101,10(%eax)
# [92] temp^[10]:= Ord('l');
	movl	-16(%ebp),%eax
	movb	$108,11(%eax)
# [93] temp^[11]:= Ord('l');
	movl	-16(%ebp),%eax
	movb	$108,12(%eax)
# [94] temp^[12]:= Ord('o');
	movl	-16(%ebp),%eax
	movb	$111,13(%eax)
# [95] temp^[13]:= 0;
	movl	-16(%ebp),%eax
	movb	$0,14(%eax)
# [96] temp:= temp + 4; //why this increases by 8 bytes, i'll never know... but it does
	movl	-16(%ebp),%eax
	addl	$8,%eax
	movl	%eax,-16(%ebp)
# [97] aptr:= pansistring(@temp);
	leal	-16(%ebp),%eax
	movl	%eax,-12(%ebp)
# [98] writeln(length(aptr^));
	call	fpc_get_output
	movl	%eax,%ebx
	movl	-12(%ebp),%eax
	movl	(%eax),%ecx
	testl	%ecx,%ecx
	je	.Lj117
	movl	-4(%ecx),%ecx
.Lj117:
	movl	%ebx,%edx
	movl	$0,%eax
	call	fpc_write_text_sint
	call	FPC_IOCHECK
	movl	%ebx,%eax
	call	fpc_writeln_end
	call	FPC_IOCHECK
# [99] writeln(aptr^);
	call	fpc_get_output
	movl	%eax,%ebx
	movl	-12(%ebp),%eax
	movl	(%eax),%ecx
	movl	%ebx,%edx
	movl	$0,%eax
	call	fpc_write_text_ansistr
	call	FPC_IOCHECK
	movl	%ebx,%eax
	call	fpc_writeln_end
	call	FPC_IOCHECK
# [100] test_ansi_string:= aptr^;
	movl	-12(%ebp),%ebx
	movl	(%ebx),%eax
	call	FPC_ANSISTR_INCR_REF
	leal	-4(%ebp),%eax
	call	FPC_ANSISTR_DECR_REF
	movl	(%ebx),%eax
	movl	%eax,-4(%ebp)
# [101] end;
	movl	-4(%ebp),%eax
	movl	-20(%ebp),%ebx
	leave
	ret
.Lc22:
.Le4:
	.size	P$FPCLINK_TEST_ANSI_STRING$$ANSISTRING, .Le4 - P$FPCLINK_TEST_ANSI_STRING$$ANSISTRING

.section .text
	.balign 16,0x90
.globl	PASCALMAIN
	.type	PASCALMAIN,@function
PASCALMAIN:
.globl	main
	.type	main,@function
main:
.Lc26:
# Temps allocated between ebp-80 and ebp+0
# [107] begin
	pushl	%ebp
.Lc28:
.Lc29:
	movl	%esp,%ebp
.Lc30:
	subl	$80,%esp
	movl	%ebx,-80(%ebp)
	movl	%esi,-76(%ebp)
	call	FPC_INITIALIZEUNITS
	movl	$0,-72(%ebp)
	movl	$0,-68(%ebp)
	leal	-16(%ebp),%ecx
	leal	-40(%ebp),%edx
	movl	$1,%eax
	call	FPC_PUSHEXCEPTADDR
	call	FPC_SETJMP
	pushl	%eax
	testl	%eax,%eax
	jne	.Lj142
# [108] global_static_arr:= test_static_array;
	leal	-64(%ebp),%eax
	call	P$FPCLINK_TEST_STATIC_ARRAY$$TINTARRAY
	movl	-64(%ebp),%eax
	movl	%eax,U_P$FPCLINK_GLOBAL_STATIC_ARR
	movl	-60(%ebp),%eax
	movl	%eax,U_P$FPCLINK_GLOBAL_STATIC_ARR+4
	movw	-56(%ebp),%ax
	movw	%ax,U_P$FPCLINK_GLOBAL_STATIC_ARR+8
# [109] global_dyn_array:= test_dynamic_array;
	movl	$INIT_P$FPCLINK_TDYNINTARRAY,%ebx
	movl	$U_P$FPCLINK_GLOBAL_DYN_ARRAY,%esi
	movl	$INIT_P$FPCLINK_TDYNINTARRAY,%edx
	leal	-68(%ebp),%eax
	call	FPC_FINALIZE
	call	P$FPCLINK_TEST_DYNAMIC_ARRAY$$TDYNINTARRAY
	movl	%eax,-68(%ebp)
	leal	-68(%ebp),%eax
	movl	%ebx,%ecx
	movl	%esi,%edx
	call	fpc_copy
# [110] global_ansi_string:= test_ansi_string;
	leal	-72(%ebp),%eax
	call	FPC_ANSISTR_DECR_REF
	movl	$0,-72(%ebp)
	call	P$FPCLINK_TEST_ANSI_STRING$$ANSISTRING
	movl	%eax,-72(%ebp)
	movl	-72(%ebp),%eax
	call	FPC_ANSISTR_INCR_REF
	movl	$U_P$FPCLINK_GLOBAL_ANSI_STRING,%eax
	call	FPC_ANSISTR_DECR_REF
	movl	-72(%ebp),%eax
	movl	%eax,U_P$FPCLINK_GLOBAL_ANSI_STRING
.Lj142:
	call	FPC_POPADDRSTACK
# [111] end.
	leal	-72(%ebp),%eax
	call	FPC_ANSISTR_DECR_REF
	movl	$0,-72(%ebp)
	movl	$INIT_P$FPCLINK_TDYNINTARRAY,%edx
	leal	-68(%ebp),%eax
	call	FPC_FINALIZE
	popl	%eax
	testl	%eax,%eax
	je	.Lj143
	call	FPC_RERAISE
.Lj143:
	call	FPC_DO_EXIT
	movl	-80(%ebp),%ebx
	movl	-76(%ebp),%esi
	leave
	ret
.Lc27:
.Le5:
	.size	main, .Le5 - main

.section .text
	.balign 16,0x90
.globl	INIT$_P$FPCLINK
	.type	INIT$_P$FPCLINK,@function
INIT$_P$FPCLINK:
.globl	P$FPCLINK_init_implicit
	.type	P$FPCLINK_init_implicit,@function
P$FPCLINK_init_implicit:
.Lc31:
# Temps allocated between ebp+0 and ebp+0
	pushl	%ebp
.Lc33:
.Lc34:
	movl	%esp,%ebp
.Lc35:
	movl	$0,U_P$FPCLINK_GLOBAL_DYN_ARRAY
	movl	$0,U_P$FPCLINK_GLOBAL_ANSI_STRING
	leave
	ret
.Lc32:
.Le6:
	.size	P$FPCLINK_init_implicit, .Le6 - P$FPCLINK_init_implicit

.section .text
	.balign 16,0x90
.globl	FINALIZE$_P$FPCLINK
	.type	FINALIZE$_P$FPCLINK,@function
FINALIZE$_P$FPCLINK:
.globl	P$FPCLINK_finalize_implicit
	.type	P$FPCLINK_finalize_implicit,@function
P$FPCLINK_finalize_implicit:
.Lc36:
# Temps allocated between ebp+0 and ebp+0
	pushl	%ebp
.Lc38:
.Lc39:
	movl	%esp,%ebp
.Lc40:
	movl	$INIT_P$FPCLINK_TDYNINTARRAY,%edx
	movl	$U_P$FPCLINK_GLOBAL_DYN_ARRAY,%eax
	call	fpc_finalize
	movl	$U_P$FPCLINK_GLOBAL_ANSI_STRING,%eax
	call	fpc_ansistr_decr_ref
	movl	$0,U_P$FPCLINK_GLOBAL_ANSI_STRING
	leave
	ret
.Lc37:
.Le7:
	.size	P$FPCLINK_finalize_implicit, .Le7 - P$FPCLINK_finalize_implicit

.section .text
# End asmlist al_procedures
# Begin asmlist al_globals

.section .bss
	.balign 16
# [fpclink.pas]
# [104] global_static_arr: tintarray;
	.lcomm	U_P$FPCLINK_GLOBAL_STATIC_ARR,10

.section .bss
	.balign 4
# [105] global_dyn_array: tdynintarray;
	.lcomm	U_P$FPCLINK_GLOBAL_DYN_ARRAY,4

.section .bss
	.balign 4
# [106] global_ansi_string: tansistring;
	.lcomm	U_P$FPCLINK_GLOBAL_ANSI_STRING,4

.section .data
	.balign 4
.globl	THREADVARLIST_P$FPCLINK
	.type	THREADVARLIST_P$FPCLINK,@object
THREADVARLIST_P$FPCLINK:
	.long	0
.Le8:
	.size	THREADVARLIST_P$FPCLINK, .Le8 - THREADVARLIST_P$FPCLINK

.section .data
	.balign 4
.globl	INITFINAL
	.type	INITFINAL,@object
INITFINAL:
	.long	2,0
	.long	INIT$_SYSTEM
	.long	0
	.long	INIT$_P$FPCLINK
	.long	FINALIZE$_P$FPCLINK
.Le9:
	.size	INITFINAL, .Le9 - INITFINAL

.section .data
	.balign 4
.globl	FPC_THREADVARTABLES
	.type	FPC_THREADVARTABLES,@object
FPC_THREADVARTABLES:
	.long	3
	.long	THREADVARLIST_SYSTEM
	.long	THREADVARLIST_SI_PRC
	.long	THREADVARLIST_P$FPCLINK
.Le10:
	.size	FPC_THREADVARTABLES, .Le10 - FPC_THREADVARTABLES

.section .data
	.balign 4
.globl	FPC_RESOURCESTRINGTABLES
	.type	FPC_RESOURCESTRINGTABLES,@object
FPC_RESOURCESTRINGTABLES:
	.long	0
.Le11:
	.size	FPC_RESOURCESTRINGTABLES, .Le11 - FPC_RESOURCESTRINGTABLES

.section .fpc
	.balign 8
	.ascii	"FPC 2.2.0 [2008/04/01] for i386 - Linux"

.section .data
	.balign 4
.globl	__stklen
	.type	__stklen,@object
__stklen:
	.long	262144

.section .data
	.balign 4
.globl	__heapsize
	.type	__heapsize,@object
__heapsize:
	.long	0

.section .data
.globl	__fpc_valgrind
	.type	__fpc_valgrind,@object
__fpc_valgrind:
	.byte	0
.globl	FPC_RESLOCATION
	.type	FPC_RESLOCATION,@object
FPC_RESLOCATION:
	.long	0
# End asmlist al_globals
# Begin asmlist al_const
# End asmlist al_const
# Begin asmlist al_typedconsts
# End asmlist al_typedconsts
# Begin asmlist al_rotypedconsts
# End asmlist al_rotypedconsts
# Begin asmlist al_threadvars
# End asmlist al_threadvars
# Begin asmlist al_imports
# End asmlist al_imports
# Begin asmlist al_exports
# End asmlist al_exports
# Begin asmlist al_resources
# End asmlist al_resources
# Begin asmlist al_rtti

.section .data
	.balign 4
.globl	INIT_P$FPCLINK_TDYNINTARRAY
	.type	INIT_P$FPCLINK_TDYNINTARRAY,@object
INIT_P$FPCLINK_TDYNINTARRAY:
	.byte	21
	.ascii	"\014tdynintarray"
	.long	4
	.long	INIT_SYSTEM_LONGINT
	.long	3
.Le12:
	.size	INIT_P$FPCLINK_TDYNINTARRAY, .Le12 - INIT_P$FPCLINK_TDYNINTARRAY
# End asmlist al_rtti
# Begin asmlist al_dwarf

.section .debug_frame
.Lc41:
	.long	.Lc43-.Lc42
.Lc42:
	.long	-1
	.byte	1
	.byte	0
	.uleb128	1
	.sleb128	-4
	.byte	8
	.byte	12
	.uleb128	4
	.uleb128	4
	.byte	5
	.uleb128	8
	.uleb128	1
	.balign 4,0
.Lc43:
	.long	.Lc45-.Lc44
.Lc44:
	.long	.Lc41
	.long	.Lc1
	.long	.Lc2-.Lc1
	.byte	4
	.long	.Lc3-.Lc1
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc4-.Lc3
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc5-.Lc4
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc45:
	.long	.Lc47-.Lc46
.Lc46:
	.long	.Lc41
	.long	.Lc6
	.long	.Lc7-.Lc6
	.byte	4
	.long	.Lc8-.Lc6
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc9-.Lc8
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc10-.Lc9
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc47:
	.long	.Lc49-.Lc48
.Lc48:
	.long	.Lc41
	.long	.Lc11
	.long	.Lc12-.Lc11
	.byte	4
	.long	.Lc13-.Lc11
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc14-.Lc13
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc15-.Lc14
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc49:
	.long	.Lc51-.Lc50
.Lc50:
	.long	.Lc41
	.long	.Lc16
	.long	.Lc17-.Lc16
	.byte	4
	.long	.Lc18-.Lc16
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc19-.Lc18
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc20-.Lc19
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc51:
	.long	.Lc53-.Lc52
.Lc52:
	.long	.Lc41
	.long	.Lc21
	.long	.Lc22-.Lc21
	.byte	4
	.long	.Lc23-.Lc21
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc24-.Lc23
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc25-.Lc24
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc53:
	.long	.Lc55-.Lc54
.Lc54:
	.long	.Lc41
	.long	.Lc26
	.long	.Lc27-.Lc26
	.byte	4
	.long	.Lc28-.Lc26
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc29-.Lc28
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc30-.Lc29
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc55:
	.long	.Lc57-.Lc56
.Lc56:
	.long	.Lc41
	.long	.Lc31
	.long	.Lc32-.Lc31
	.byte	4
	.long	.Lc33-.Lc31
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc34-.Lc33
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc35-.Lc34
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc57:
	.long	.Lc59-.Lc58
.Lc58:
	.long	.Lc41
	.long	.Lc36
	.long	.Lc37-.Lc36
	.byte	4
	.long	.Lc38-.Lc36
	.byte	14
	.uleb128	8
	.byte	4
	.long	.Lc39-.Lc38
	.byte	5
	.uleb128	5
	.uleb128	2
	.byte	4
	.long	.Lc40-.Lc39
	.byte	13
	.uleb128	5
	.balign 4,0
.Lc59:
# End asmlist al_dwarf
# Begin asmlist al_dwarf_info
# End asmlist al_dwarf_info
# Begin asmlist al_dwarf_abbrev
# End asmlist al_dwarf_abbrev
# Begin asmlist al_dwarf_line
# End asmlist al_dwarf_line
# Begin asmlist al_picdata
# End asmlist al_picdata
# Begin asmlist al_resourcestrings
# End asmlist al_resourcestrings
# Begin asmlist al_end
# End asmlist al_end

