GAS LISTING /tmp/cc8vARfS.s 			page 1


   1              		.file	"main.c"
   2              		.text
   3              		.p2align 4,,15
   4              		.globl	write_jpeg
   5              		.type	write_jpeg, @function
   6              	write_jpeg:
   7              	.LFB12:
   8              		.cfi_startproc
   9 0000 89F2     		movl	%esi, %edx
  10 0002 4889FE   		movq	%rdi, %rsi
  11 0005 8B3D0000 		movl	file_jpg(%rip), %edi
  11      0000
  12 000b E9000000 		jmp	write
  12      00
  13              		.cfi_endproc
  14              	.LFE12:
  15              		.size	write_jpeg, .-write_jpeg
  16              		.p2align 4,,15
  17              		.globl	subsample
  18              		.type	subsample, @function
  19              	subsample:
  20              	.LFB13:
  21              		.cfi_startproc
  22 0010 53       		pushq	%rbx
  23              		.cfi_def_cfa_offset 16
  24              		.cfi_offset 3, -16
  25 0011 31DB     		xorl	%ebx, %ebx
  26              		.p2align 4,,10
  27 0013 0F1F4400 		.p2align 3
  27      00
  28              	.L3:
  29 0018 4C8D1C1E 		leaq	(%rsi,%rbx), %r11
  30 001c 4C8D141A 		leaq	(%rdx,%rbx), %r10
  31 0020 4889F9   		movq	%rdi, %rcx
  32 0023 31C0     		xorl	%eax, %eax
  33              		.p2align 4,,10
  34 0025 0F1F00   		.p2align 3
  35              	.L4:
  36 0028 440FB649 		movzbl	1(%rcx), %r9d
  36      01
  37 002d 4189C0   		movl	%eax, %r8d
  38 0030 83C002   		addl	$2, %eax
  39 0033 41D1E8   		shrl	%r8d
  40 0036 4183C180 		addl	$-128, %r9d
  41 003a 6647890C 		movw	%r9w, (%r11,%r8,2)
  41      43
  42 003f 440FB649 		movzbl	3(%rcx), %r9d
  42      03
  43 0044 4883C104 		addq	$4, %rcx
  44 0048 4183C180 		addl	$-128, %r9d
  45 004c 83F810   		cmpl	$16, %eax
  46 004f 6647890C 		movw	%r9w, (%r10,%r8,2)
  46      42
  47 0054 75D2     		jne	.L4
  48 0056 4883C310 		addq	$16, %rbx
  49 005a 4883C720 		addq	$32, %rdi
  50 005e 4881FB80 		cmpq	$128, %rbx
GAS LISTING /tmp/cc8vARfS.s 			page 2


  50      000000
  51 0065 75B1     		jne	.L3
  52 0067 5B       		popq	%rbx
  53              		.cfi_def_cfa_offset 8
  54 0068 C3       		ret
  55              		.cfi_endproc
  56              	.LFE13:
  57              		.size	subsample, .-subsample
  58              		.section	.rodata.str1.8,"aMS",@progbits,1
  59              		.align 8
  60              	.LC0:
  61 0000 55736167 		.string	"Usage: %s file-in.bmp file-out.jpg\n"
  61      653A2025 
  61      73206669 
  61      6C652D69 
  61      6E2E626D 
  62              		.section	.rodata.str1.1,"aMS",@progbits,1
  63              	.LC1:
  64 0000 4572726F 		.string	"Error: cannot open %s\n"
  64      723A2063 
  64      616E6E6F 
  64      74206F70 
  64      656E2025 
  65              	.LC2:
  66 0017 4572726F 		.string	"Error: cannot create %s (%i)\n"
  66      723A2063 
  66      616E6E6F 
  66      74206372 
  66      65617465 
  67              	.LC3:
  68 0035 4572726F 		.string	"Error: getBlock(%d,%d)\n"
  68      723A2067 
  68      6574426C 
  68      6F636B28 
  68      25642C25 
  69              		.section	.text.startup,"ax",@progbits
  70              		.p2align 4,,15
  71              		.globl	main
  72              		.type	main, @function
  73              	main:
  74              	.LFB14:
  75              		.cfi_startproc
  76 0000 4155     		pushq	%r13
  77              		.cfi_def_cfa_offset 16
  78              		.cfi_offset 13, -16
  79 0002 4989F5   		movq	%rsi, %r13
  80 0005 4154     		pushq	%r12
  81              		.cfi_def_cfa_offset 24
  82              		.cfi_offset 12, -24
  83 0007 55       		pushq	%rbp
  84              		.cfi_def_cfa_offset 32
  85              		.cfi_offset 6, -32
  86 0008 53       		pushq	%rbx
  87              		.cfi_def_cfa_offset 40
  88              		.cfi_offset 3, -40
  89 0009 4881EC18 		subq	$792, %rsp
  89      030000
GAS LISTING /tmp/cc8vARfS.s 			page 3


  90              		.cfi_def_cfa_offset 832
  91 0010 83FF02   		cmpl	$2, %edi
  92 0013 0F8E2102 		jle	.L25
  92      0000
  93 0019 488B7E08 		movq	8(%rsi), %rdi
  94 001d 488D7424 		leaq	8(%rsp), %rsi
  94      08
  95 0022 48C74424 		movq	$0, 8(%rsp)
  95      08000000 
  95      00
  96 002b E8000000 		call	load_yuv
  96      00
  97 0030 84C0     		testb	%al, %al
  98 0032 0F84DA01 		je	.L26
  98      0000
  99 0038 498B7D10 		movq	16(%r13), %rdi
 100 003c 31C0     		xorl	%eax, %eax
 101 003e BA800100 		movl	$384, %edx
 101      00
 102 0043 BE410200 		movl	$577, %esi
 102      00
 103 0048 E8000000 		call	open
 103      00
 104 004d 85C0     		testl	%eax, %eax
 105 004f 89050000 		movl	%eax, file_jpg(%rip)
 105      0000
 106 0055 0F880802 		js	.L27
 106      0000
 107 005b 488DAC24 		leaq	400(%rsp), %rbp
 107      90010000 
 108 0063 BE400100 		movl	$320, %esi
 108      00
 109 0068 BFF00000 		movl	$240, %edi
 109      00
 110 006d 4531E4   		xorl	%r12d, %r12d
 111 0070 E8000000 		call	huffman_start
 111      00
 112              	.L14:
 113 0075 31DB     		xorl	%ebx, %ebx
 114 0077 660F1F84 		.p2align 4,,10
 114      00000000 
 114      00
 115              		.p2align 3
 116              	.L19:
 117 0080 4C8B4C24 		movq	8(%rsp), %r9
 117      08
 118 0085 4C8D8424 		leaq	528(%rsp), %r8
 118      10020000 
 119 008d B9080000 		movl	$8, %ecx
 119      00
 120 0092 BA100000 		movl	$16, %edx
 120      00
 121 0097 4489E6   		movl	%r12d, %esi
 122 009a 89DF     		movl	%ebx, %edi
 123 009c E8000000 		call	yuv_get_block
 123      00
 124 00a1 84C0     		testb	%al, %al
GAS LISTING /tmp/cc8vARfS.s 			page 4


 125 00a3 0F844701 		je	.L28
 125      0000
 126 00a9 31FF     		xorl	%edi, %edi
 127 00ab 0F1F4400 		.p2align 4,,10
 127      00
 128              		.p2align 3
 129              	.L15:
 130 00b0 89F8     		movl	%edi, %eax
 131 00b2 488D9424 		leaq	528(%rsp), %rdx
 131      10020000 
 132 00ba 4889C1   		movq	%rax, %rcx
 133 00bd 48C1E004 		salq	$4, %rax
 134 00c1 48C1E105 		salq	$5, %rcx
 135 00c5 4801D1   		addq	%rdx, %rcx
 136 00c8 488D9424 		leaq	272(%rsp), %rdx
 136      10010000 
 137 00d0 4801D0   		addq	%rdx, %rax
 138 00d3 31D2     		xorl	%edx, %edx
 139              		.p2align 4,,10
 140 00d5 0F1F00   		.p2align 3
 141              	.L17:
 142 00d8 0FB631   		movzbl	(%rcx), %esi
 143 00db 83C201   		addl	$1, %edx
 144 00de 83C680   		addl	$-128, %esi
 145 00e1 668930   		movw	%si, (%rax)
 146 00e4 0FB67110 		movzbl	16(%rcx), %esi
 147 00e8 4883C102 		addq	$2, %rcx
 148 00ec 83C680   		addl	$-128, %esi
 149 00ef 6689B080 		movw	%si, 128(%rax)
 149      000000
 150 00f6 4883C002 		addq	$2, %rax
 151 00fa 83FA08   		cmpl	$8, %edx
 152 00fd 75D9     		jne	.L17
 153 00ff 83C701   		addl	$1, %edi
 154 0102 83FF08   		cmpl	$8, %edi
 155 0105 75A9     		jne	.L15
 156 0107 488D9424 		leaq	144(%rsp), %rdx
 156      90000000 
 157 010f 488D7424 		leaq	16(%rsp), %rsi
 157      10
 158 0114 488DBC24 		leaq	528(%rsp), %rdi
 158      10020000 
 159 011c 83C310   		addl	$16, %ebx
 160 011f E8000000 		call	subsample
 160      00
 161 0124 488DB424 		leaq	272(%rsp), %rsi
 161      10010000 
 162 012c 4889F7   		movq	%rsi, %rdi
 163 012f E8000000 		call	dct
 163      00
 164 0134 488DB424 		leaq	272(%rsp), %rsi
 164      10010000 
 165 013c BF000000 		movl	$huffman_ctx, %edi
 165      00
 166 0141 E8000000 		call	huffman_encode
 166      00
 167 0146 4889EE   		movq	%rbp, %rsi
GAS LISTING /tmp/cc8vARfS.s 			page 5


 168 0149 4889EF   		movq	%rbp, %rdi
 169 014c E8000000 		call	dct
 169      00
 170 0151 4889EE   		movq	%rbp, %rsi
 171 0154 BF000000 		movl	$huffman_ctx, %edi
 171      00
 172 0159 E8000000 		call	huffman_encode
 172      00
 173 015e 488D7424 		leaq	16(%rsp), %rsi
 173      10
 174 0163 4889F7   		movq	%rsi, %rdi
 175 0166 E8000000 		call	dct
 175      00
 176 016b 488D7424 		leaq	16(%rsp), %rsi
 176      10
 177 0170 BF000000 		movl	$huffman_ctx+48, %edi
 177      00
 178 0175 E8000000 		call	huffman_encode
 178      00
 179 017a 488DB424 		leaq	144(%rsp), %rsi
 179      90000000 
 180 0182 4889F7   		movq	%rsi, %rdi
 181 0185 E8000000 		call	dct
 181      00
 182 018a 488DB424 		leaq	144(%rsp), %rsi
 182      90000000 
 183 0192 BF000000 		movl	$huffman_ctx+96, %edi
 183      00
 184 0197 E8000000 		call	huffman_encode
 184      00
 185 019c 81FB4001 		cmpl	$320, %ebx
 185      0000
 186 01a2 0F85D8FE 		jne	.L19
 186      FFFF
 187 01a8 4183C408 		addl	$8, %r12d
 188 01ac 4181FCF0 		cmpl	$240, %r12d
 188      000000
 189 01b3 0F85BCFE 		jne	.L14
 189      FFFF
 190              	.L20:
 191 01b9 E8000000 		call	huffman_stop
 191      00
 192 01be 8B3D0000 		movl	file_jpg(%rip), %edi
 192      0000
 193 01c4 E8000000 		call	close
 193      00
 194 01c9 498B7D08 		movq	8(%r13), %rdi
 195 01cd 488B7424 		movq	8(%rsp), %rsi
 195      08
 196 01d2 E8000000 		call	close_yuv
 196      00
 197 01d7 4881C418 		addq	$792, %rsp
 197      030000
 198              		.cfi_remember_state
 199              		.cfi_def_cfa_offset 40
 200 01de 31C0     		xorl	%eax, %eax
 201 01e0 5B       		popq	%rbx
GAS LISTING /tmp/cc8vARfS.s 			page 6


 202              		.cfi_def_cfa_offset 32
 203 01e1 5D       		popq	%rbp
 204              		.cfi_def_cfa_offset 24
 205 01e2 415C     		popq	%r12
 206              		.cfi_def_cfa_offset 16
 207 01e4 415D     		popq	%r13
 208              		.cfi_def_cfa_offset 8
 209 01e6 C3       		ret
 210 01e7 660F1F84 		.p2align 4,,10
 210      00000000 
 210      00
 211              		.p2align 3
 212              	.L28:
 213              		.cfi_restore_state
 214 01f0 4489E2   		movl	%r12d, %edx
 215 01f3 89DE     		movl	%ebx, %esi
 216 01f5 BF000000 		movl	$.LC3, %edi
 216      00
 217 01fa 4183C408 		addl	$8, %r12d
 218 01fe E8000000 		call	printf
 218      00
 219 0203 4181FCF0 		cmpl	$240, %r12d
 219      000000
 220 020a 0F8565FE 		jne	.L14
 220      FFFF
 221 0210 EBA7     		jmp	.L20
 222              	.L26:
 223 0212 498B5508 		movq	8(%r13), %rdx
 224 0216 488B3D00 		movq	stderr(%rip), %rdi
 224      000000
 225 021d BE000000 		movl	$.LC1, %esi
 225      00
 226 0222 E8000000 		call	fprintf
 226      00
 227 0227 4881C418 		addq	$792, %rsp
 227      030000
 228              		.cfi_remember_state
 229              		.cfi_def_cfa_offset 40
 230 022e B8FFFFFF 		movl	$-1, %eax
 230      FF
 231 0233 5B       		popq	%rbx
 232              		.cfi_def_cfa_offset 32
 233 0234 5D       		popq	%rbp
 234              		.cfi_def_cfa_offset 24
 235 0235 415C     		popq	%r12
 236              		.cfi_def_cfa_offset 16
 237 0237 415D     		popq	%r13
 238              		.cfi_def_cfa_offset 8
 239 0239 C3       		ret
 240              	.L25:
 241              		.cfi_restore_state
 242 023a 488B16   		movq	(%rsi), %rdx
 243 023d 488B3D00 		movq	stderr(%rip), %rdi
 243      000000
 244 0244 BE000000 		movl	$.LC0, %esi
 244      00
 245 0249 31C0     		xorl	%eax, %eax
GAS LISTING /tmp/cc8vARfS.s 			page 7


 246 024b E8000000 		call	fprintf
 246      00
 247 0250 4881C418 		addq	$792, %rsp
 247      030000
 248              		.cfi_remember_state
 249              		.cfi_def_cfa_offset 40
 250 0257 B8FFFFFF 		movl	$-1, %eax
 250      FF
 251 025c 5B       		popq	%rbx
 252              		.cfi_def_cfa_offset 32
 253 025d 5D       		popq	%rbp
 254              		.cfi_def_cfa_offset 24
 255 025e 415C     		popq	%r12
 256              		.cfi_def_cfa_offset 16
 257 0260 415D     		popq	%r13
 258              		.cfi_def_cfa_offset 8
 259 0262 C3       		ret
 260              	.L27:
 261              		.cfi_restore_state
 262 0263 E8000000 		call	__errno_location
 262      00
 263 0268 498B5510 		movq	16(%r13), %rdx
 264 026c 8B08     		movl	(%rax), %ecx
 265 026e BE000000 		movl	$.LC2, %esi
 265      00
 266 0273 488B3D00 		movq	stderr(%rip), %rdi
 266      000000
 267 027a 31C0     		xorl	%eax, %eax
 268 027c E8000000 		call	fprintf
 268      00
 269 0281 4881C418 		addq	$792, %rsp
 269      030000
 270              		.cfi_def_cfa_offset 40
 271 0288 B8FFFFFF 		movl	$-1, %eax
 271      FF
 272 028d 5B       		popq	%rbx
 273              		.cfi_def_cfa_offset 32
 274 028e 5D       		popq	%rbp
 275              		.cfi_def_cfa_offset 24
 276 028f 415C     		popq	%r12
 277              		.cfi_def_cfa_offset 16
 278 0291 415D     		popq	%r13
 279              		.cfi_def_cfa_offset 8
 280 0293 C3       		ret
 281              		.cfi_endproc
 282              	.LFE14:
 283              		.size	main, .-main
 284              		.comm	file_jpg,4,4
 285              		.ident	"GCC: (GNU) 4.7.2 20120921 (Red Hat 4.7.2-2)"
 286              		.section	.note.GNU-stack,"",@progbits
GAS LISTING /tmp/cc8vARfS.s 			page 8


DEFINED SYMBOLS
                            *ABS*:0000000000000000 main.c
     /tmp/cc8vARfS.s:6      .text:0000000000000000 write_jpeg
                            *COM*:0000000000000004 file_jpg
     /tmp/cc8vARfS.s:19     .text:0000000000000010 subsample
     /tmp/cc8vARfS.s:73     .text.startup:0000000000000000 main

UNDEFINED SYMBOLS
write
load_yuv
open
huffman_start
yuv_get_block
dct
huffman_ctx
huffman_encode
huffman_stop
close
close_yuv
printf
stderr
fprintf
__errno_location
