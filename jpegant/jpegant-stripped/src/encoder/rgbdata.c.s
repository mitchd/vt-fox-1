GAS LISTING /tmp/ccdZ6DII.s 			page 1


   1              		.file	"rgbdata.c"
   2              		.text
   3              		.p2align 4,,15
   4              		.globl	rgb_get_block
   5              		.type	rgb_get_block, @function
   6              	rgb_get_block:
   7              	.LFB2:
   8              		.cfi_startproc
   9 0000 448D1C31 		leal	(%rcx,%rsi), %r11d
  10 0004 31C0     		xorl	%eax, %eax
  11 0006 4181FBE0 		cmpl	$480, %r11d
  11      010000
  12 000d 776D     		ja	.L17
  13 000f 448D143A 		leal	(%rdx,%rdi), %r10d
  14 0013 4181FA80 		cmpl	$640, %r10d
  14      020000
  15 001a 7760     		ja	.L17
  16 001c 85C9     		testl	%ecx, %ecx
  17 001e 7470     		je	.L19
  18 0020 448D14B6 		leal	(%rsi,%rsi,4), %r10d
  19 0024 55       		pushq	%rbp
  20              		.cfi_def_cfa_offset 16
  21              		.cfi_offset 6, -16
  22 0025 BD800200 		movl	$640, %ebp
  22      00
  23 002a 29D5     		subl	%edx, %ebp
  24 002c 31F6     		xorl	%esi, %esi
  25 002e 41C1E207 		sall	$7, %r10d
  26 0032 53       		pushq	%rbx
  27              		.cfi_def_cfa_offset 24
  28              		.cfi_offset 3, -24
  29 0033 31DB     		xorl	%ebx, %ebx
  30 0035 4101FA   		addl	%edi, %r10d
  31 0038 0F1F8400 		.p2align 4,,10
  31      00000000 
  32              		.p2align 3
  33              	.L5:
  34 0040 85D2     		testl	%edx, %edx
  35 0042 448D1C16 		leal	(%rsi,%rdx), %r11d
  36 0046 89F0     		movl	%esi, %eax
  37 0048 7434     		je	.L20
  38 004a 660F1F44 		.p2align 4,,10
  38      0000
  39              		.p2align 3
  40              	.L11:
  41 0050 428D3C10 		leal	(%rax,%r10), %edi
  42 0054 89C6     		movl	%eax, %esi
  43 0056 83C001   		addl	$1, %eax
  44 0059 4439D8   		cmpl	%r11d, %eax
  45 005c 410FB73C 		movzwl	(%r9,%rdi,2), %edi
  45      79
  46 0061 6641893C 		movw	%di, (%r8,%rsi,2)
  46      70
  47 0066 75E8     		jne	.L11
  48 0068 83C301   		addl	$1, %ebx
  49 006b 4101EA   		addl	%ebp, %r10d
  50 006e 4489DE   		movl	%r11d, %esi
GAS LISTING /tmp/ccdZ6DII.s 			page 2


  51 0071 39CB     		cmpl	%ecx, %ebx
  52 0073 75CB     		jne	.L5
  53              	.L21:
  54 0075 5B       		popq	%rbx
  55              		.cfi_restore 3
  56              		.cfi_def_cfa_offset 16
  57 0076 5D       		popq	%rbp
  58              		.cfi_restore 6
  59              		.cfi_def_cfa_offset 8
  60 0077 B8010000 		movl	$1, %eax
  60      00
  61              	.L17:
  62 007c F3       		rep
  63 007d C3       		ret
  64              	.L20:
  65              		.cfi_def_cfa_offset 24
  66              		.cfi_offset 3, -24
  67              		.cfi_offset 6, -16
  68 007e 83C301   		addl	$1, %ebx
  69 0081 4189F3   		movl	%esi, %r11d
  70 0084 4101EA   		addl	%ebp, %r10d
  71 0087 39CB     		cmpl	%ecx, %ebx
  72 0089 4489DE   		movl	%r11d, %esi
  73 008c 75B2     		jne	.L5
  74 008e EBE5     		jmp	.L21
  75              	.L19:
  76              		.cfi_def_cfa_offset 8
  77              		.cfi_restore 3
  78              		.cfi_restore 6
  79 0090 B8010000 		movl	$1, %eax
  79      00
  80 0095 C3       		ret
  81              		.cfi_endproc
  82              	.LFE2:
  83              		.size	rgb_get_block, .-rgb_get_block
  84 0096 662E0F1F 		.p2align 4,,15
  84      84000000 
  84      0000
  85              		.globl	close_rgb
  86              		.type	close_rgb, @function
  87              	close_rgb:
  88              	.LFB3:
  89              		.cfi_startproc
  90 00a0 53       		pushq	%rbx
  91              		.cfi_def_cfa_offset 16
  92              		.cfi_offset 3, -16
  93 00a1 4889F3   		movq	%rsi, %rbx
  94 00a4 4889FE   		movq	%rdi, %rsi
  95 00a7 BF010000 		movl	$1, %edi
  95      00
  96 00ac 4881EC90 		subq	$144, %rsp
  96      000000
  97              		.cfi_def_cfa_offset 160
  98 00b3 4889E2   		movq	%rsp, %rdx
  99 00b6 E8000000 		call	__xstat
  99      00
 100 00bb 83F8FF   		cmpl	$-1, %eax
GAS LISTING /tmp/ccdZ6DII.s 			page 3


 101 00be 740D     		je	.L22
 102 00c0 488B7424 		movq	48(%rsp), %rsi
 102      30
 103 00c5 4889DF   		movq	%rbx, %rdi
 104 00c8 E8000000 		call	munmap
 104      00
 105              	.L22:
 106 00cd 4881C490 		addq	$144, %rsp
 106      000000
 107              		.cfi_def_cfa_offset 16
 108 00d4 5B       		popq	%rbx
 109              		.cfi_def_cfa_offset 8
 110 00d5 C3       		ret
 111              		.cfi_endproc
 112              	.LFE3:
 113              		.size	close_rgb, .-close_rgb
 114 00d6 662E0F1F 		.p2align 4,,15
 114      84000000 
 114      0000
 115              		.globl	load_rgb
 116              		.type	load_rgb, @function
 117              	load_rgb:
 118              	.LFB4:
 119              		.cfi_startproc
 120 00e0 48895C24 		movq	%rbx, -24(%rsp)
 120      E8
 121 00e5 48896C24 		movq	%rbp, -16(%rsp)
 121      F0
 122 00ea 31C0     		xorl	%eax, %eax
 123 00ec 4C896424 		movq	%r12, -8(%rsp)
 123      F8
 124 00f1 4881ECA8 		subq	$168, %rsp
 124      000000
 125              		.cfi_def_cfa_offset 176
 126              		.cfi_offset 3, -32
 127              		.cfi_offset 6, -24
 128              		.cfi_offset 12, -16
 129 00f8 4989F4   		movq	%rsi, %r12
 130 00fb 31F6     		xorl	%esi, %esi
 131 00fd 4889FB   		movq	%rdi, %rbx
 132 0100 E8000000 		call	open
 132      00
 133 0105 83F8FF   		cmpl	$-1, %eax
 134 0108 89C5     		movl	%eax, %ebp
 135 010a 7444     		je	.L31
 136 010c 4889E2   		movq	%rsp, %rdx
 137 010f 4889DE   		movq	%rbx, %rsi
 138 0112 BF010000 		movl	$1, %edi
 138      00
 139 0117 E8000000 		call	__xstat
 139      00
 140 011c 83F8FF   		cmpl	$-1, %eax
 141 011f 742F     		je	.L31
 142 0121 488B7424 		movq	48(%rsp), %rsi
 142      30
 143 0126 4531C9   		xorl	%r9d, %r9d
 144 0129 31FF     		xorl	%edi, %edi
GAS LISTING /tmp/ccdZ6DII.s 			page 4


 145 012b 4189E8   		movl	%ebp, %r8d
 146 012e B9020000 		movl	$2, %ecx
 146      00
 147 0133 BA010000 		movl	$1, %edx
 147      00
 148 0138 E8000000 		call	mmap
 148      00
 149 013d 4883F8FF 		cmpq	$-1, %rax
 150 0141 49890424 		movq	%rax, (%r12)
 151 0145 0F95C0   		setne	%al
 152 0148 EB08     		jmp	.L30
 153 014a 660F1F44 		.p2align 4,,10
 153      0000
 154              		.p2align 3
 155              	.L31:
 156 0150 31C0     		xorl	%eax, %eax
 157              	.L30:
 158 0152 488B9C24 		movq	144(%rsp), %rbx
 158      90000000 
 159 015a 488BAC24 		movq	152(%rsp), %rbp
 159      98000000 
 160 0162 4C8BA424 		movq	160(%rsp), %r12
 160      A0000000 
 161 016a 4881C4A8 		addq	$168, %rsp
 161      000000
 162              		.cfi_def_cfa_offset 8
 163 0171 C3       		ret
 164              		.cfi_endproc
 165              	.LFE4:
 166              		.size	load_rgb, .-load_rgb
 167 0172 66666666 		.p2align 4,,15
 167      662E0F1F 
 167      84000000 
 167      0000
 168              		.globl	getRed
 169              		.type	getRed, @function
 170              	getRed:
 171              	.LFB5:
 172              		.cfi_startproc
 173 0180 89F8     		movl	%edi, %eax
 174 0182 83E0F8   		andl	$-8, %eax
 175 0185 C3       		ret
 176              		.cfi_endproc
 177              	.LFE5:
 178              		.size	getRed, .-getRed
 179 0186 662E0F1F 		.p2align 4,,15
 179      84000000 
 179      0000
 180              		.globl	getBlue
 181              		.type	getBlue, @function
 182              	getBlue:
 183              	.LFB6:
 184              		.cfi_startproc
 185 0190 4889FA   		movq	%rdi, %rdx
 186 0193 0FB6C6   		movzbl	%dh, %eax
 187 0196 C1E003   		sall	$3, %eax
 188 0199 C3       		ret
GAS LISTING /tmp/ccdZ6DII.s 			page 5


 189              		.cfi_endproc
 190              	.LFE6:
 191              		.size	getBlue, .-getBlue
 192 019a 660F1F44 		.p2align 4,,15
 192      0000
 193              		.globl	getGreen
 194              		.type	getGreen, @function
 195              	getGreen:
 196              	.LFB7:
 197              		.cfi_startproc
 198 01a0 89FA     		movl	%edi, %edx
 199 01a2 89F8     		movl	%edi, %eax
 200 01a4 0FB6FE   		movzbl	%dh, %edi
 201 01a7 C1E005   		sall	$5, %eax
 202 01aa 40C0EF05 		shrb	$5, %dil
 203 01ae 09F8     		orl	%edi, %eax
 204 01b0 C3       		ret
 205              		.cfi_endproc
 206              	.LFE7:
 207              		.size	getGreen, .-getGreen
 208              		.ident	"GCC: (GNU) 4.7.2 20120921 (Red Hat 4.7.2-2)"
 209              		.section	.note.GNU-stack,"",@progbits
GAS LISTING /tmp/ccdZ6DII.s 			page 6


DEFINED SYMBOLS
                            *ABS*:0000000000000000 rgbdata.c
     /tmp/ccdZ6DII.s:6      .text:0000000000000000 rgb_get_block
     /tmp/ccdZ6DII.s:87     .text:00000000000000a0 close_rgb
     /tmp/ccdZ6DII.s:117    .text:00000000000000e0 load_rgb
     /tmp/ccdZ6DII.s:170    .text:0000000000000180 getRed
     /tmp/ccdZ6DII.s:182    .text:0000000000000190 getBlue
     /tmp/ccdZ6DII.s:195    .text:00000000000001a0 getGreen

UNDEFINED SYMBOLS
__xstat
munmap
open
mmap
