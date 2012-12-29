GAS LISTING /tmp/ccGFQem8.s 			page 1


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
  20              	.LFB16:
  21              		.cfi_startproc
  22 0010 4157     		pushq	%r15
  23              		.cfi_def_cfa_offset 16
  24              		.cfi_offset 15, -16
  25 0012 4531FF   		xorl	%r15d, %r15d
  26 0015 4156     		pushq	%r14
  27              		.cfi_def_cfa_offset 24
  28              		.cfi_offset 14, -24
  29 0017 4531F6   		xorl	%r14d, %r14d
  30 001a 4155     		pushq	%r13
  31              		.cfi_def_cfa_offset 32
  32              		.cfi_offset 13, -32
  33 001c 4154     		pushq	%r12
  34              		.cfi_def_cfa_offset 40
  35              		.cfi_offset 12, -40
  36 001e 55       		pushq	%rbp
  37              		.cfi_def_cfa_offset 48
  38              		.cfi_offset 6, -48
  39 001f 53       		pushq	%rbx
  40              		.cfi_def_cfa_offset 56
  41              		.cfi_offset 3, -56
  42 0020 48897424 		movq	%rsi, -16(%rsp)
  42      F0
  43 0025 48895424 		movq	%rdx, -8(%rsp)
  43      F8
  44 002a 660F1F44 		.p2align 4,,10
  44      0000
  45              		.p2align 3
  46              	.L3:
  47 0030 438D0436 		leal	(%r14,%r14), %eax
  48 0034 4C8B6C24 		movq	-16(%rsp), %r13
  48      F0
  49 0039 4C8B6424 		movq	-8(%rsp), %r12
  49      F8
  50 003e 89C2     		movl	%eax, %edx
GAS LISTING /tmp/ccGFQem8.s 			page 2


  51 0040 83C001   		addl	$1, %eax
  52 0043 4C8D1440 		leaq	(%rax,%rax,2), %r10
  53 0047 4C8D1C52 		leaq	(%rdx,%rdx,2), %r11
  54 004b 4D01FD   		addq	%r15, %r13
  55 004e 4D01FC   		addq	%r15, %r12
  56 0051 31C0     		xorl	%eax, %eax
  57 0053 49C1E304 		salq	$4, %r11
  58 0057 49C1E204 		salq	$4, %r10
  59 005b 4901FB   		addq	%rdi, %r11
  60 005e 4901FA   		addq	%rdi, %r10
  61              		.p2align 4,,10
  62 0061 0F1F8000 		.p2align 3
  62      000000
  63              	.L4:
  64 0068 8D3400   		leal	(%rax,%rax), %esi
  65 006b 89F2     		movl	%esi, %edx
  66 006d 83C601   		addl	$1, %esi
  67 0070 4C8D0476 		leaq	(%rsi,%rsi,2), %r8
  68 0074 4C8D0C52 		leaq	(%rdx,%rdx,2), %r9
  69 0078 4B8D1C03 		leaq	(%r11,%r8), %rbx
  70 007c 4B8D140B 		leaq	(%r11,%r9), %rdx
  71 0080 4D01D1   		addq	%r10, %r9
  72 0083 4D01D0   		addq	%r10, %r8
  73 0086 0FB60A   		movzbl	(%rdx), %ecx
  74 0089 0FB633   		movzbl	(%rbx), %esi
  75 008c 0FB66A01 		movzbl	1(%rdx), %ebp
  76 0090 01F1     		addl	%esi, %ecx
  77 0092 410FB631 		movzbl	(%r9), %esi
  78 0096 01F1     		addl	%esi, %ecx
  79 0098 410FB630 		movzbl	(%r8), %esi
  80 009c 01F1     		addl	%esi, %ecx
  81 009e 0FB67301 		movzbl	1(%rbx), %esi
  82 00a2 C1F902   		sarl	$2, %ecx
  83 00a5 01EE     		addl	%ebp, %esi
  84 00a7 410FB669 		movzbl	1(%r9), %ebp
  84      01
  85 00ac 450FB649 		movzbl	2(%r9), %r9d
  85      02
  86 00b1 01EE     		addl	%ebp, %esi
  87 00b3 410FB668 		movzbl	1(%r8), %ebp
  87      01
  88 00b8 450FB640 		movzbl	2(%r8), %r8d
  88      02
  89 00bd 01EE     		addl	%ebp, %esi
  90 00bf 0FB66A02 		movzbl	2(%rdx), %ebp
  91 00c3 0FB65302 		movzbl	2(%rbx), %edx
  92 00c7 C1FE02   		sarl	$2, %esi
  93 00ca 01EA     		addl	%ebp, %edx
  94 00cc 4401CA   		addl	%r9d, %edx
  95 00cf 4401C2   		addl	%r8d, %edx
  96 00d2 4469C9CE 		imull	$-11058, %ecx, %r9d
  96      D4FFFF
  97 00d9 C1FA02   		sarl	$2, %edx
  98 00dc 4469C633 		imull	$-21709, %esi, %r8d
  98      ABFFFF
  99 00e3 478D8C01 		leal	8421376(%r9,%r8), %r9d
  99      00808000 
GAS LISTING /tmp/ccGFQem8.s 			page 3


 100 00eb 4189D0   		movl	%edx, %r8d
 101 00ee 41C1E00F 		sall	$15, %r8d
 102 00f2 4129D0   		subl	%edx, %r8d
 103 00f5 4501C8   		addl	%r9d, %r8d
 104 00f8 41C1F810 		sarl	$16, %r8d
 105 00fc 4183C080 		addl	$-128, %r8d
 106 0100 66458944 		movw	%r8w, 0(%r13,%rax,2)
 106      4500
 107 0106 4189C8   		movl	%ecx, %r8d
 108 0109 69F6D294 		imull	$-27438, %esi, %esi
 108      FFFF
 109 010f 41C1E00F 		sall	$15, %r8d
 110 0113 4129C8   		subl	%ecx, %r8d
 111 0116 69D22FEB 		imull	$-5329, %edx, %edx
 111      FFFF
 112 011c 418D8C30 		leal	8421376(%r8,%rsi), %ecx
 112      00808000 
 113 0124 01CA     		addl	%ecx, %edx
 114 0126 C1FA10   		sarl	$16, %edx
 115 0129 83C280   		addl	$-128, %edx
 116 012c 66418914 		movw	%dx, (%r12,%rax,2)
 116      44
 117 0131 4883C001 		addq	$1, %rax
 118 0135 4883F808 		cmpq	$8, %rax
 119 0139 0F8529FF 		jne	.L4
 119      FFFF
 120 013f 4183C601 		addl	$1, %r14d
 121 0143 4983C710 		addq	$16, %r15
 122 0147 4183FE08 		cmpl	$8, %r14d
 123 014b 0F85DFFE 		jne	.L3
 123      FFFF
 124 0151 5B       		popq	%rbx
 125              		.cfi_def_cfa_offset 48
 126 0152 5D       		popq	%rbp
 127              		.cfi_def_cfa_offset 40
 128 0153 415C     		popq	%r12
 129              		.cfi_def_cfa_offset 32
 130 0155 415D     		popq	%r13
 131              		.cfi_def_cfa_offset 24
 132 0157 415E     		popq	%r14
 133              		.cfi_def_cfa_offset 16
 134 0159 415F     		popq	%r15
 135              		.cfi_def_cfa_offset 8
 136 015b C3       		ret
 137              		.cfi_endproc
 138              	.LFE16:
 139              		.size	subsample, .-subsample
 140              		.section	.rodata.str1.8,"aMS",@progbits,1
 141              		.align 8
 142              	.LC0:
 143 0000 55736167 		.string	"Usage: %s file-in.bmp file-out.jpg\n"
 143      653A2025 
 143      73206669 
 143      6C652D69 
 143      6E2E626D 
 144              		.section	.rodata.str1.1,"aMS",@progbits,1
 145              	.LC1:
GAS LISTING /tmp/ccGFQem8.s 			page 4


 146 0000 4572726F 		.string	"Error: cannot open %s\n"
 146      723A2063 
 146      616E6E6F 
 146      74206F70 
 146      656E2025 
 147              	.LC2:
 148 0017 4572726F 		.string	"Error: cannot create %s (%i)\n"
 148      723A2063 
 148      616E6E6F 
 148      74206372 
 148      65617465 
 149              	.LC3:
 150 0035 4572726F 		.string	"Error: getBlock(%d,%d)\n"
 150      723A2067 
 150      6574426C 
 150      6F636B28 
 150      25642C25 
 151              		.section	.text.startup,"ax",@progbits
 152              		.p2align 4,,15
 153              		.globl	main
 154              		.type	main, @function
 155              	main:
 156              	.LFB17:
 157              		.cfi_startproc
 158 0000 4157     		pushq	%r15
 159              		.cfi_def_cfa_offset 16
 160              		.cfi_offset 15, -16
 161 0002 4156     		pushq	%r14
 162              		.cfi_def_cfa_offset 24
 163              		.cfi_offset 14, -24
 164 0004 4155     		pushq	%r13
 165              		.cfi_def_cfa_offset 32
 166              		.cfi_offset 13, -32
 167 0006 4154     		pushq	%r12
 168              		.cfi_def_cfa_offset 40
 169              		.cfi_offset 12, -40
 170 0008 55       		pushq	%rbp
 171              		.cfi_def_cfa_offset 48
 172              		.cfi_offset 6, -48
 173 0009 4889F5   		movq	%rsi, %rbp
 174 000c 53       		pushq	%rbx
 175              		.cfi_def_cfa_offset 56
 176              		.cfi_offset 3, -56
 177 000d 4881EC28 		subq	$1576, %rsp
 177      060000
 178              		.cfi_def_cfa_offset 1632
 179 0014 83FF02   		cmpl	$2, %edi
 180 0017 0F8ECE02 		jle	.L29
 180      0000
 181 001d 488B7E08 		movq	8(%rsi), %rdi
 182 0021 488D7424 		leaq	24(%rsp), %rsi
 182      18
 183 0026 48C74424 		movq	$0, 24(%rsp)
 183      18000000 
 183      00
 184 002f E8000000 		call	load_rgb
 184      00
GAS LISTING /tmp/ccGFQem8.s 			page 5


 185 0034 84C0     		testb	%al, %al
 186 0036 0F848302 		je	.L30
 186      0000
 187 003c 488B7D10 		movq	16(%rbp), %rdi
 188 0040 31C0     		xorl	%eax, %eax
 189 0042 BA800100 		movl	$384, %edx
 189      00
 190 0047 BE410200 		movl	$577, %esi
 190      00
 191 004c E8000000 		call	open
 191      00
 192 0051 85C0     		testl	%eax, %eax
 193 0053 89050000 		movl	%eax, file_jpg(%rip)
 193      0000
 194 0059 0F88A902 		js	.L31
 194      0000
 195 005f BE800200 		movl	$640, %esi
 195      00
 196 0064 BFE00100 		movl	$480, %edi
 196      00
 197 0069 E8000000 		call	huffman_start
 197      00
 198 006e 488D8C24 		leaq	544(%rsp), %rcx
 198      20020000 
 199 0076 48896C24 		movq	%rbp, 8(%rsp)
 199      08
 200 007b C7442404 		movl	$0, 4(%rsp)
 200      00000000 
 201 0083 4889CD   		movq	%rcx, %rbp
 202              	.L14:
 203 0086 31DB     		xorl	%ebx, %ebx
 204              	.L24:
 205 0088 4C8B4C24 		movq	24(%rsp), %r9
 205      18
 206 008d 8B742404 		movl	4(%rsp), %esi
 207 0091 4C8D8424 		leaq	800(%rsp), %r8
 207      20030000 
 208 0099 B9100000 		movl	$16, %ecx
 208      00
 209 009e BA100000 		movl	$16, %edx
 209      00
 210 00a3 89DF     		movl	%ebx, %edi
 211 00a5 E8000000 		call	rgb_get_block
 211      00
 212 00aa 84C0     		testb	%al, %al
 213 00ac 0F84E601 		je	.L15
 213      0000
 214 00b2 4C8DB424 		leaq	416(%rsp), %r14
 214      A0010000 
 215 00ba 4531ED   		xorl	%r13d, %r13d
 216              	.L16:
 217 00bd 468D24ED 		leal	0(,%r13,8), %r12d
 217      00000000 
 218 00c5 4D89F2   		movq	%r14, %r10
 219 00c8 4531DB   		xorl	%r11d, %r11d
 220              	.L23:
 221 00cb 468D04DD 		leal	0(,%r11,8), %r8d
GAS LISTING /tmp/ccGFQem8.s 			page 6


 221      00000000 
 222 00d3 498D7280 		leaq	-128(%r10), %rsi
 223 00d7 4589E1   		movl	%r12d, %r9d
 224 00da 660F1F44 		.p2align 4,,10
 224      0000
 225              		.p2align 3
 226              	.L21:
 227 00e0 4489CA   		movl	%r9d, %edx
 228 00e3 31C0     		xorl	%eax, %eax
 229 00e5 488D3C52 		leaq	(%rdx,%rdx,2), %rdi
 230 00e9 48C1E704 		salq	$4, %rdi
 231 00ed 0F1F00   		.p2align 4,,10
 232              		.p2align 3
 233              	.L18:
 234 00f0 418D1400 		leal	(%r8,%rax), %edx
 235 00f4 488D0C52 		leaq	(%rdx,%rdx,2), %rcx
 236 00f8 4801F9   		addq	%rdi, %rcx
 237 00fb 440FB6BC 		movzbl	800(%rsp,%rcx), %r15d
 237      0C200300 
 237      00
 238 0104 0FB6940C 		movzbl	801(%rsp,%rcx), %edx
 238      21030000 
 239 010c 4569FF8B 		imull	$19595, %r15d, %r15d
 239      4C0000
 240 0113 69D24696 		imull	$38470, %edx, %edx
 240      0000
 241 0119 458DBC17 		leal	32768(%r15,%rdx), %r15d
 241      00800000 
 242 0121 0FB6940C 		movzbl	802(%rsp,%rcx), %edx
 242      22030000 
 243 0129 69D22F1D 		imull	$7471, %edx, %edx
 243      0000
 244 012f 4401FA   		addl	%r15d, %edx
 245 0132 C1FA10   		sarl	$16, %edx
 246 0135 83C280   		addl	$-128, %edx
 247 0138 66891446 		movw	%dx, (%rsi,%rax,2)
 248 013c 4883C001 		addq	$1, %rax
 249 0140 4883F808 		cmpq	$8, %rax
 250 0144 75AA     		jne	.L18
 251 0146 4883C610 		addq	$16, %rsi
 252 014a 4183C101 		addl	$1, %r9d
 253 014e 4939F2   		cmpq	%rsi, %r10
 254 0151 758D     		jne	.L21
 255 0153 4183C301 		addl	$1, %r11d
 256 0157 4983EA80 		subq	$-128, %r10
 257 015b 4183FB02 		cmpl	$2, %r11d
 258 015f 0F8566FF 		jne	.L23
 258      FFFF
 259 0165 4183C501 		addl	$1, %r13d
 260 0169 4981C600 		addq	$256, %r14
 260      010000
 261 0170 4183FD02 		cmpl	$2, %r13d
 262 0174 0F8543FF 		jne	.L16
 262      FFFF
 263 017a 488D9424 		leaq	160(%rsp), %rdx
 263      A0000000 
 264 0182 488D7424 		leaq	32(%rsp), %rsi
GAS LISTING /tmp/ccGFQem8.s 			page 7


 264      20
 265 0187 488DBC24 		leaq	800(%rsp), %rdi
 265      20030000 
 266 018f 4C8DA424 		leaq	416(%rsp), %r12
 266      A0010000 
 267 0197 83C310   		addl	$16, %ebx
 268 019a E8000000 		call	subsample
 268      00
 269 019f 488DB424 		leaq	288(%rsp), %rsi
 269      20010000 
 270 01a7 4889F7   		movq	%rsi, %rdi
 271 01aa E8000000 		call	dct
 271      00
 272 01af 488DB424 		leaq	288(%rsp), %rsi
 272      20010000 
 273 01b7 BF000000 		movl	$huffman_ctx, %edi
 273      00
 274 01bc E8000000 		call	huffman_encode
 274      00
 275 01c1 4C89E6   		movq	%r12, %rsi
 276 01c4 4C89E7   		movq	%r12, %rdi
 277 01c7 E8000000 		call	dct
 277      00
 278 01cc 4C89E6   		movq	%r12, %rsi
 279 01cf BF000000 		movl	$huffman_ctx, %edi
 279      00
 280 01d4 4C8DA424 		leaq	672(%rsp), %r12
 280      A0020000 
 281 01dc E8000000 		call	huffman_encode
 281      00
 282 01e1 4889EE   		movq	%rbp, %rsi
 283 01e4 4889EF   		movq	%rbp, %rdi
 284 01e7 E8000000 		call	dct
 284      00
 285 01ec 4889EE   		movq	%rbp, %rsi
 286 01ef BF000000 		movl	$huffman_ctx, %edi
 286      00
 287 01f4 E8000000 		call	huffman_encode
 287      00
 288 01f9 4C89E6   		movq	%r12, %rsi
 289 01fc 4C89E7   		movq	%r12, %rdi
 290 01ff E8000000 		call	dct
 290      00
 291 0204 4C89E6   		movq	%r12, %rsi
 292 0207 BF000000 		movl	$huffman_ctx, %edi
 292      00
 293 020c E8000000 		call	huffman_encode
 293      00
 294 0211 488D7424 		leaq	32(%rsp), %rsi
 294      20
 295 0216 4889F7   		movq	%rsi, %rdi
 296 0219 E8000000 		call	dct
 296      00
 297 021e 488D7424 		leaq	32(%rsp), %rsi
 297      20
 298 0223 BF000000 		movl	$huffman_ctx+48, %edi
 298      00
GAS LISTING /tmp/ccGFQem8.s 			page 8


 299 0228 E8000000 		call	huffman_encode
 299      00
 300 022d 488DB424 		leaq	160(%rsp), %rsi
 300      A0000000 
 301 0235 4889F7   		movq	%rsi, %rdi
 302 0238 E8000000 		call	dct
 302      00
 303 023d 488DB424 		leaq	160(%rsp), %rsi
 303      A0000000 
 304 0245 BF000000 		movl	$huffman_ctx+96, %edi
 304      00
 305 024a E8000000 		call	huffman_encode
 305      00
 306 024f 81FB8002 		cmpl	$640, %ebx
 306      0000
 307 0255 0F852DFE 		jne	.L24
 307      FFFF
 308 025b 83442404 		addl	$16, 4(%rsp)
 308      10
 309 0260 817C2404 		cmpl	$480, 4(%rsp)
 309      E0010000 
 310 0268 0F8518FE 		jne	.L14
 310      FFFF
 311              	.L25:
 312 026e 488B6C24 		movq	8(%rsp), %rbp
 312      08
 313 0273 E8000000 		call	huffman_stop
 313      00
 314 0278 8B3D0000 		movl	file_jpg(%rip), %edi
 314      0000
 315 027e E8000000 		call	close
 315      00
 316 0283 488B7D08 		movq	8(%rbp), %rdi
 317 0287 488B7424 		movq	24(%rsp), %rsi
 317      18
 318 028c E8000000 		call	close_rgb
 318      00
 319 0291 31C0     		xorl	%eax, %eax
 320 0293 EB44     		jmp	.L11
 321              		.p2align 4,,10
 322 0295 0F1F00   		.p2align 3
 323              	.L15:
 324 0298 8B542404 		movl	4(%rsp), %edx
 325 029c 31C0     		xorl	%eax, %eax
 326 029e 89DE     		movl	%ebx, %esi
 327 02a0 BF000000 		movl	$.LC3, %edi
 327      00
 328 02a5 E8000000 		call	printf
 328      00
 329 02aa 83442404 		addl	$16, 4(%rsp)
 329      10
 330 02af 817C2404 		cmpl	$480, 4(%rsp)
 330      E0010000 
 331 02b7 0F85C9FD 		jne	.L14
 331      FFFF
 332 02bd EBAF     		jmp	.L25
 333              	.L30:
GAS LISTING /tmp/ccGFQem8.s 			page 9


 334 02bf 488B5508 		movq	8(%rbp), %rdx
 335 02c3 488B3D00 		movq	stderr(%rip), %rdi
 335      000000
 336 02ca BE000000 		movl	$.LC1, %esi
 336      00
 337 02cf E8000000 		call	fprintf
 337      00
 338 02d4 B8FFFFFF 		movl	$-1, %eax
 338      FF
 339              	.L11:
 340 02d9 4881C428 		addq	$1576, %rsp
 340      060000
 341              		.cfi_remember_state
 342              		.cfi_def_cfa_offset 56
 343 02e0 5B       		popq	%rbx
 344              		.cfi_def_cfa_offset 48
 345 02e1 5D       		popq	%rbp
 346              		.cfi_def_cfa_offset 40
 347 02e2 415C     		popq	%r12
 348              		.cfi_def_cfa_offset 32
 349 02e4 415D     		popq	%r13
 350              		.cfi_def_cfa_offset 24
 351 02e6 415E     		popq	%r14
 352              		.cfi_def_cfa_offset 16
 353 02e8 415F     		popq	%r15
 354              		.cfi_def_cfa_offset 8
 355 02ea C3       		ret
 356              	.L29:
 357              		.cfi_restore_state
 358 02eb 488B16   		movq	(%rsi), %rdx
 359 02ee 488B3D00 		movq	stderr(%rip), %rdi
 359      000000
 360 02f5 BE000000 		movl	$.LC0, %esi
 360      00
 361 02fa 31C0     		xorl	%eax, %eax
 362 02fc E8000000 		call	fprintf
 362      00
 363 0301 B8FFFFFF 		movl	$-1, %eax
 363      FF
 364 0306 EBD1     		jmp	.L11
 365              	.L31:
 366 0308 E8000000 		call	__errno_location
 366      00
 367 030d 488B5510 		movq	16(%rbp), %rdx
 368 0311 8B08     		movl	(%rax), %ecx
 369 0313 BE000000 		movl	$.LC2, %esi
 369      00
 370 0318 488B3D00 		movq	stderr(%rip), %rdi
 370      000000
 371 031f 31C0     		xorl	%eax, %eax
 372 0321 E8000000 		call	fprintf
 372      00
 373 0326 B8FFFFFF 		movl	$-1, %eax
 373      FF
 374 032b EBAC     		jmp	.L11
 375              		.cfi_endproc
 376              	.LFE17:
GAS LISTING /tmp/ccGFQem8.s 			page 10


 377              		.size	main, .-main
 378              		.comm	file_jpg,4,4
 379              		.ident	"GCC: (GNU) 4.7.2 20120921 (Red Hat 4.7.2-2)"
 380              		.section	.note.GNU-stack,"",@progbits
GAS LISTING /tmp/ccGFQem8.s 			page 11


DEFINED SYMBOLS
                            *ABS*:0000000000000000 main.c
     /tmp/ccGFQem8.s:6      .text:0000000000000000 write_jpeg
                            *COM*:0000000000000004 file_jpg
     /tmp/ccGFQem8.s:19     .text:0000000000000010 subsample
     /tmp/ccGFQem8.s:155    .text.startup:0000000000000000 main

UNDEFINED SYMBOLS
write
load_rgb
open
huffman_start
rgb_get_block
dct
huffman_ctx
huffman_encode
huffman_stop
close
close_rgb
printf
stderr
fprintf
__errno_location
