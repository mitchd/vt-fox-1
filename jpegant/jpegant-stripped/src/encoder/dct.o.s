GAS LISTING /tmp/ccsrCKie.s 			page 1


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
  25 0012 4156     		pushq	%r14
  26              		.cfi_def_cfa_offset 24
  27              		.cfi_offset 14, -24
  28 0014 4155     		pushq	%r13
  29              		.cfi_def_cfa_offset 32
  30              		.cfi_offset 13, -32
  31 0016 4154     		pushq	%r12
  32              		.cfi_def_cfa_offset 40
  33              		.cfi_offset 12, -40
  34 0018 55       		pushq	%rbp
  35              		.cfi_def_cfa_offset 48
  36              		.cfi_offset 6, -48
  37 0019 53       		pushq	%rbx
  38              		.cfi_def_cfa_offset 56
  39              		.cfi_offset 3, -56
  40 001a 4889FB   		movq	%rdi, %rbx
  41 001d 4883EC38 		subq	$56, %rsp
  42              		.cfi_def_cfa_offset 112
  43 0021 48897424 		movq	%rsi, 32(%rsp)
  43      20
  44 0026 48895424 		movq	%rdx, 40(%rsp)
  44      28
  45 002b 48C74424 		movq	$0, 24(%rsp)
  45      18000000 
  45      00
  46              		.p2align 4,,10
  47 0034 0F1F4000 		.p2align 3
  48              	.L3:
  49 0038 488B4C24 		movq	32(%rsp), %rcx
  49      20
  50 003d 488B5424 		movq	40(%rsp), %rdx
GAS LISTING /tmp/ccsrCKie.s 			page 2


  50      28
  51 0042 4531FF   		xorl	%r15d, %r15d
  52 0045 48034C24 		addq	24(%rsp), %rcx
  52      18
  53 004a 48035424 		addq	24(%rsp), %rdx
  53      18
  54 004f 48894C24 		movq	%rcx, 8(%rsp)
  54      08
  55 0054 48895424 		movq	%rdx, 16(%rsp)
  55      10
  56 0059 0F1F8000 		.p2align 4,,10
  56      000000
  57              		.p2align 3
  58              	.L4:
  59 0060 4589FE   		movl	%r15d, %r14d
  60 0063 458D6F01 		leal	1(%r15), %r13d
  61 0067 420FB73C 		movzwl	(%rbx,%r14,2), %edi
  61      73
  62 006c E8000000 		call	getRed
  62      00
  63 0071 420FB73C 		movzwl	(%rbx,%r13,2), %edi
  63      6B
  64 0076 0FB6E8   		movzbl	%al, %ebp
  65 0079 E8000000 		call	getRed
  65      00
  66 007e 420FB73C 		movzwl	(%rbx,%r14,2), %edi
  66      73
  67 0083 0FB6C0   		movzbl	%al, %eax
  68 0086 448D6405 		leal	0(%rbp,%rax), %r12d
  68      00
  69 008b 41D1FC   		sarl	%r12d
  70 008e E8000000 		call	getGreen
  70      00
  71 0093 420FB73C 		movzwl	(%rbx,%r13,2), %edi
  71      6B
  72 0098 0FB6E8   		movzbl	%al, %ebp
  73 009b E8000000 		call	getGreen
  73      00
  74 00a0 420FB73C 		movzwl	(%rbx,%r14,2), %edi
  74      73
  75 00a5 0FB6C0   		movzbl	%al, %eax
  76 00a8 8D540500 		leal	0(%rbp,%rax), %edx
  77 00ac D1FA     		sarl	%edx
  78 00ae 891424   		movl	%edx, (%rsp)
  79 00b1 E8000000 		call	getBlue
  79      00
  80 00b6 420FB73C 		movzwl	(%rbx,%r13,2), %edi
  80      6B
  81 00bb 0FB6E8   		movzbl	%al, %ebp
  82 00be E8000000 		call	getBlue
  82      00
  83 00c3 8B1424   		movl	(%rsp), %edx
  84 00c6 0FB6C0   		movzbl	%al, %eax
  85 00c9 488B4C24 		movq	8(%rsp), %rcx
  85      08
  86 00ce 4569C4CE 		imull	$-11058, %r12d, %r8d
  86      D4FFFF
GAS LISTING /tmp/ccsrCKie.s 			page 3


  87 00d5 01E8     		addl	%ebp, %eax
  88 00d7 D1F8     		sarl	%eax
  89 00d9 69FA33AB 		imull	$-21709, %edx, %edi
  89      FFFF
  90 00df 69F2D294 		imull	$-27438, %edx, %esi
  90      FFFF
  91 00e5 488B5424 		movq	16(%rsp), %rdx
  91      10
  92 00ea 458D8438 		leal	8421376(%r8,%rdi), %r8d
  92      00808000 
  93 00f2 89C7     		movl	%eax, %edi
  94 00f4 C1E70F   		sall	$15, %edi
  95 00f7 29C7     		subl	%eax, %edi
  96 00f9 4401C7   		addl	%r8d, %edi
  97 00fc C1FF10   		sarl	$16, %edi
  98 00ff 83C780   		addl	$-128, %edi
  99 0102 6642893C 		movw	%di, (%rcx,%r15)
  99      39
 100 0107 4489E7   		movl	%r12d, %edi
 101 010a C1E70F   		sall	$15, %edi
 102 010d 4429E7   		subl	%r12d, %edi
 103 0110 69C02FEB 		imull	$-5329, %eax, %eax
 103      FFFF
 104 0116 8D8C3700 		leal	8421376(%rdi,%rsi), %ecx
 104      808000
 105 011d 01C8     		addl	%ecx, %eax
 106 011f C1F810   		sarl	$16, %eax
 107 0122 83C080   		addl	$-128, %eax
 108 0125 66428904 		movw	%ax, (%rdx,%r15)
 108      3A
 109 012a 4983C702 		addq	$2, %r15
 110 012e 4983FF10 		cmpq	$16, %r15
 111 0132 0F8528FF 		jne	.L4
 111      FFFF
 112 0138 48834424 		addq	$16, 24(%rsp)
 112      1810
 113 013e 4883C320 		addq	$32, %rbx
 114 0142 48817C24 		cmpq	$128, 24(%rsp)
 114      18800000 
 114      00
 115 014b 0F85E7FE 		jne	.L3
 115      FFFF
 116 0151 4883C438 		addq	$56, %rsp
 117              		.cfi_def_cfa_offset 56
 118 0155 5B       		popq	%rbx
 119              		.cfi_def_cfa_offset 48
 120 0156 5D       		popq	%rbp
 121              		.cfi_def_cfa_offset 40
 122 0157 415C     		popq	%r12
 123              		.cfi_def_cfa_offset 32
 124 0159 415D     		popq	%r13
 125              		.cfi_def_cfa_offset 24
 126 015b 415E     		popq	%r14
 127              		.cfi_def_cfa_offset 16
 128 015d 415F     		popq	%r15
 129              		.cfi_def_cfa_offset 8
 130 015f C3       		ret
GAS LISTING /tmp/ccsrCKie.s 			page 4


 131              		.cfi_endproc
 132              	.LFE16:
 133              		.size	subsample, .-subsample
 134              		.section	.rodata.str1.8,"aMS",@progbits,1
 135              		.align 8
 136              	.LC0:
 137 0000 55736167 		.string	"Usage: %s file-in.bmp file-out.jpg\n"
 137      653A2025 
 137      73206669 
 137      6C652D69 
 137      6E2E626D 
 138              		.section	.rodata.str1.1,"aMS",@progbits,1
 139              	.LC1:
 140 0000 4572726F 		.string	"Error: cannot open %s\n"
 140      723A2063 
 140      616E6E6F 
 140      74206F70 
 140      656E2025 
 141              	.LC2:
 142 0017 4572726F 		.string	"Error: cannot create %s (%i)\n"
 142      723A2063 
 142      616E6E6F 
 142      74206372 
 142      65617465 
 143              	.LC3:
 144 0035 4572726F 		.string	"Error: getBlock(%d,%d)\n"
 144      723A2067 
 144      6574426C 
 144      6F636B28 
 144      25642C25 
 145              		.section	.text.startup,"ax",@progbits
 146              		.p2align 4,,15
 147              		.globl	main
 148              		.type	main, @function
 149              	main:
 150              	.LFB17:
 151              		.cfi_startproc
 152 0000 4157     		pushq	%r15
 153              		.cfi_def_cfa_offset 16
 154              		.cfi_offset 15, -16
 155 0002 4156     		pushq	%r14
 156              		.cfi_def_cfa_offset 24
 157              		.cfi_offset 14, -24
 158 0004 4155     		pushq	%r13
 159              		.cfi_def_cfa_offset 32
 160              		.cfi_offset 13, -32
 161 0006 4154     		pushq	%r12
 162              		.cfi_def_cfa_offset 40
 163              		.cfi_offset 12, -40
 164 0008 55       		pushq	%rbp
 165              		.cfi_def_cfa_offset 48
 166              		.cfi_offset 6, -48
 167 0009 53       		pushq	%rbx
 168              		.cfi_def_cfa_offset 56
 169              		.cfi_offset 3, -56
 170 000a 4881EC38 		subq	$824, %rsp
 170      030000
GAS LISTING /tmp/ccsrCKie.s 			page 5


 171              		.cfi_def_cfa_offset 880
 172 0011 83FF02   		cmpl	$2, %edi
 173 0014 48897424 		movq	%rsi, 24(%rsp)
 173      18
 174 0019 0F8E0103 		jle	.L25
 174      0000
 175 001f 488B4424 		movq	24(%rsp), %rax
 175      18
 176 0024 488D7424 		leaq	40(%rsp), %rsi
 176      28
 177 0029 48C74424 		movq	$0, 40(%rsp)
 177      28000000 
 177      00
 178 0032 488B7808 		movq	8(%rax), %rdi
 179 0036 E8000000 		call	load_rgb
 179      00
 180 003b 84C0     		testb	%al, %al
 181 003d 0F84AC02 		je	.L26
 181      0000
 182 0043 488B4424 		movq	24(%rsp), %rax
 182      18
 183 0048 BA800100 		movl	$384, %edx
 183      00
 184 004d BE410200 		movl	$577, %esi
 184      00
 185 0052 488B7810 		movq	16(%rax), %rdi
 186 0056 31C0     		xorl	%eax, %eax
 187 0058 E8000000 		call	open
 187      00
 188 005d 85C0     		testl	%eax, %eax
 189 005f 89050000 		movl	%eax, file_jpg(%rip)
 189      0000
 190 0065 0F88D202 		js	.L27
 190      0000
 191 006b BE800200 		movl	$640, %esi
 191      00
 192 0070 BFE00100 		movl	$480, %edi
 192      00
 193 0075 E8000000 		call	huffman_start
 193      00
 194 007a 488D8424 		leaq	432(%rsp), %rax
 194      B0010000 
 195 0082 C7442414 		movl	$0, 20(%rsp)
 195      00000000 
 196 008a 48894424 		movq	%rax, 8(%rsp)
 196      08
 197              	.L14:
 198 008f C7442410 		movl	$0, 16(%rsp)
 198      00000000 
 199 0097 660F1F84 		.p2align 4,,10
 199      00000000 
 199      00
 200              		.p2align 3
 201              	.L19:
 202 00a0 4C8B4C24 		movq	40(%rsp), %r9
 202      28
 203 00a5 8B742414 		movl	20(%rsp), %esi
GAS LISTING /tmp/ccsrCKie.s 			page 6


 204 00a9 4C8D8424 		leaq	560(%rsp), %r8
 204      30020000 
 205 00b1 8B7C2410 		movl	16(%rsp), %edi
 206 00b5 B9080000 		movl	$8, %ecx
 206      00
 207 00ba BA100000 		movl	$16, %edx
 207      00
 208 00bf E8000000 		call	rgb_get_block
 208      00
 209 00c4 84C0     		testb	%al, %al
 210 00c6 0F84FC01 		je	.L28
 210      0000
 211 00cc C7442404 		movl	$0, 4(%rsp)
 211      00000000 
 212              		.p2align 4,,10
 213 00d4 0F1F4000 		.p2align 3
 214              	.L15:
 215 00d8 8B6C2404 		movl	4(%rsp), %ebp
 216 00dc 488D9C24 		leaq	304(%rsp), %rbx
 216      30010000 
 217 00e4 4531FF   		xorl	%r15d, %r15d
 218 00e7 48C1E504 		salq	$4, %rbp
 219 00eb 4801EB   		addq	%rbp, %rbx
 220 00ee 6690     		.p2align 4,,10
 221              		.p2align 3
 222              	.L17:
 223 00f0 4489F9   		movl	%r15d, %ecx
 224 00f3 4C8D740D 		leaq	0(%rbp,%rcx), %r14
 224      00
 225 00f8 420FB7BC 		movzwl	560(%rsp,%r14,2), %edi
 225      74300200 
 225      00
 226 0101 E8000000 		call	getRed
 226      00
 227 0106 420FB7BC 		movzwl	560(%rsp,%r14,2), %edi
 227      74300200 
 227      00
 228 010f 440FB6E8 		movzbl	%al, %r13d
 229 0113 4569ED8B 		imull	$19595, %r13d, %r13d
 229      4C0000
 230 011a E8000000 		call	getGreen
 230      00
 231 011f 420FB7BC 		movzwl	560(%rsp,%r14,2), %edi
 231      74300200 
 231      00
 232 0128 440FB6E0 		movzbl	%al, %r12d
 233 012c 4569E446 		imull	$38470, %r12d, %r12d
 233      960000
 234 0133 E8000000 		call	getBlue
 234      00
 235 0138 0FB6C0   		movzbl	%al, %eax
 236 013b 438D8C25 		leal	32768(%r13,%r12), %ecx
 236      00800000 
 237 0143 69C02F1D 		imull	$7471, %eax, %eax
 237      0000
 238 0149 01C8     		addl	%ecx, %eax
 239 014b 418D4F08 		leal	8(%r15), %ecx
GAS LISTING /tmp/ccsrCKie.s 			page 7


 240 014f 4183C701 		addl	$1, %r15d
 241 0153 C1F810   		sarl	$16, %eax
 242 0156 4C8D740D 		leaq	0(%rbp,%rcx), %r14
 242      00
 243 015b 83C080   		addl	$-128, %eax
 244 015e 668903   		movw	%ax, (%rbx)
 245 0161 420FB7BC 		movzwl	560(%rsp,%r14,2), %edi
 245      74300200 
 245      00
 246 016a E8000000 		call	getRed
 246      00
 247 016f 420FB7BC 		movzwl	560(%rsp,%r14,2), %edi
 247      74300200 
 247      00
 248 0178 440FB6E8 		movzbl	%al, %r13d
 249 017c 4569ED8B 		imull	$19595, %r13d, %r13d
 249      4C0000
 250 0183 E8000000 		call	getGreen
 250      00
 251 0188 420FB7BC 		movzwl	560(%rsp,%r14,2), %edi
 251      74300200 
 251      00
 252 0191 440FB6E0 		movzbl	%al, %r12d
 253 0195 4569E446 		imull	$38470, %r12d, %r12d
 253      960000
 254 019c E8000000 		call	getBlue
 254      00
 255 01a1 0FB6C0   		movzbl	%al, %eax
 256 01a4 438D8C25 		leal	32768(%r13,%r12), %ecx
 256      00800000 
 257 01ac 69C02F1D 		imull	$7471, %eax, %eax
 257      0000
 258 01b2 01C8     		addl	%ecx, %eax
 259 01b4 C1F810   		sarl	$16, %eax
 260 01b7 83C080   		addl	$-128, %eax
 261 01ba 66898380 		movw	%ax, 128(%rbx)
 261      000000
 262 01c1 4883C302 		addq	$2, %rbx
 263 01c5 4183FF08 		cmpl	$8, %r15d
 264 01c9 0F8521FF 		jne	.L17
 264      FFFF
 265 01cf 83442404 		addl	$1, 4(%rsp)
 265      01
 266 01d4 837C2404 		cmpl	$8, 4(%rsp)
 266      08
 267 01d9 0F85F9FE 		jne	.L15
 267      FFFF
 268 01df 488D9424 		leaq	176(%rsp), %rdx
 268      B0000000 
 269 01e7 488D7424 		leaq	48(%rsp), %rsi
 269      30
 270 01ec 488DBC24 		leaq	560(%rsp), %rdi
 270      30020000 
 271 01f4 E8000000 		call	subsample
 271      00
 272 01f9 488DB424 		leaq	304(%rsp), %rsi
 272      30010000 
GAS LISTING /tmp/ccsrCKie.s 			page 8


 273 0201 4889F7   		movq	%rsi, %rdi
 274 0204 E8000000 		call	dct
 274      00
 275 0209 488DB424 		leaq	304(%rsp), %rsi
 275      30010000 
 276 0211 BF000000 		movl	$huffman_ctx, %edi
 276      00
 277 0216 E8000000 		call	huffman_encode
 277      00
 278 021b 488B7424 		movq	8(%rsp), %rsi
 278      08
 279 0220 4889F7   		movq	%rsi, %rdi
 280 0223 E8000000 		call	dct
 280      00
 281 0228 488B7424 		movq	8(%rsp), %rsi
 281      08
 282 022d BF000000 		movl	$huffman_ctx, %edi
 282      00
 283 0232 E8000000 		call	huffman_encode
 283      00
 284 0237 488D7424 		leaq	48(%rsp), %rsi
 284      30
 285 023c 4889F7   		movq	%rsi, %rdi
 286 023f E8000000 		call	dct
 286      00
 287 0244 488D7424 		leaq	48(%rsp), %rsi
 287      30
 288 0249 BF000000 		movl	$huffman_ctx+48, %edi
 288      00
 289 024e E8000000 		call	huffman_encode
 289      00
 290 0253 488DB424 		leaq	176(%rsp), %rsi
 290      B0000000 
 291 025b 4889F7   		movq	%rsi, %rdi
 292 025e E8000000 		call	dct
 292      00
 293 0263 488DB424 		leaq	176(%rsp), %rsi
 293      B0000000 
 294 026b BF000000 		movl	$huffman_ctx+96, %edi
 294      00
 295 0270 E8000000 		call	huffman_encode
 295      00
 296 0275 83442410 		addl	$16, 16(%rsp)
 296      10
 297 027a 817C2410 		cmpl	$640, 16(%rsp)
 297      80020000 
 298 0282 0F8518FE 		jne	.L19
 298      FFFF
 299 0288 83442414 		addl	$8, 20(%rsp)
 299      08
 300 028d 817C2414 		cmpl	$480, 20(%rsp)
 300      E0010000 
 301 0295 0F85F4FD 		jne	.L14
 301      FFFF
 302              	.L20:
 303 029b E8000000 		call	huffman_stop
 303      00
GAS LISTING /tmp/ccsrCKie.s 			page 9


 304 02a0 8B3D0000 		movl	file_jpg(%rip), %edi
 304      0000
 305 02a6 E8000000 		call	close
 305      00
 306 02ab 488B4C24 		movq	24(%rsp), %rcx
 306      18
 307 02b0 488B7424 		movq	40(%rsp), %rsi
 307      28
 308 02b5 488B7908 		movq	8(%rcx), %rdi
 309 02b9 E8000000 		call	close_rgb
 309      00
 310 02be 31C0     		xorl	%eax, %eax
 311 02c0 EB4C     		jmp	.L11
 312              		.p2align 4,,10
 313 02c2 660F1F44 		.p2align 3
 313      0000
 314              	.L28:
 315 02c8 8B542414 		movl	20(%rsp), %edx
 316 02cc 8B742410 		movl	16(%rsp), %esi
 317 02d0 BF000000 		movl	$.LC3, %edi
 317      00
 318 02d5 E8000000 		call	printf
 318      00
 319 02da 83442414 		addl	$8, 20(%rsp)
 319      08
 320 02df 817C2414 		cmpl	$480, 20(%rsp)
 320      E0010000 
 321 02e7 0F85A2FD 		jne	.L14
 321      FFFF
 322 02ed EBAC     		jmp	.L20
 323              	.L26:
 324 02ef 488B4C24 		movq	24(%rsp), %rcx
 324      18
 325 02f4 488B3D00 		movq	stderr(%rip), %rdi
 325      000000
 326 02fb BE000000 		movl	$.LC1, %esi
 326      00
 327 0300 488B5108 		movq	8(%rcx), %rdx
 328 0304 E8000000 		call	fprintf
 328      00
 329 0309 B8FFFFFF 		movl	$-1, %eax
 329      FF
 330              	.L11:
 331 030e 4881C438 		addq	$824, %rsp
 331      030000
 332              		.cfi_remember_state
 333              		.cfi_def_cfa_offset 56
 334 0315 5B       		popq	%rbx
 335              		.cfi_def_cfa_offset 48
 336 0316 5D       		popq	%rbp
 337              		.cfi_def_cfa_offset 40
 338 0317 415C     		popq	%r12
 339              		.cfi_def_cfa_offset 32
 340 0319 415D     		popq	%r13
 341              		.cfi_def_cfa_offset 24
 342 031b 415E     		popq	%r14
 343              		.cfi_def_cfa_offset 16
GAS LISTING /tmp/ccsrCKie.s 			page 10


 344 031d 415F     		popq	%r15
 345              		.cfi_def_cfa_offset 8
 346 031f C3       		ret
 347              	.L25:
 348              		.cfi_restore_state
 349 0320 488B16   		movq	(%rsi), %rdx
 350 0323 488B3D00 		movq	stderr(%rip), %rdi
 350      000000
 351 032a BE000000 		movl	$.LC0, %esi
 351      00
 352 032f 31C0     		xorl	%eax, %eax
 353 0331 E8000000 		call	fprintf
 353      00
 354 0336 B8FFFFFF 		movl	$-1, %eax
 354      FF
 355 033b EBD1     		jmp	.L11
 356              	.L27:
 357 033d E8000000 		call	__errno_location
 357      00
 358 0342 488B4C24 		movq	24(%rsp), %rcx
 358      18
 359 0347 488B3D00 		movq	stderr(%rip), %rdi
 359      000000
 360 034e BE000000 		movl	$.LC2, %esi
 360      00
 361 0353 488B5110 		movq	16(%rcx), %rdx
 362 0357 8B08     		movl	(%rax), %ecx
 363 0359 31C0     		xorl	%eax, %eax
 364 035b E8000000 		call	fprintf
 364      00
 365 0360 B8FFFFFF 		movl	$-1, %eax
 365      FF
 366 0365 EBA7     		jmp	.L11
 367              		.cfi_endproc
 368              	.LFE17:
 369              		.size	main, .-main
 370              		.comm	file_jpg,4,4
 371              		.ident	"GCC: (GNU) 4.7.2 20120921 (Red Hat 4.7.2-2)"
 372              		.section	.note.GNU-stack,"",@progbits
GAS LISTING /tmp/ccsrCKie.s 			page 11


DEFINED SYMBOLS
                            *ABS*:0000000000000000 main.c
     /tmp/ccsrCKie.s:6      .text:0000000000000000 write_jpeg
                            *COM*:0000000000000004 file_jpg
     /tmp/ccsrCKie.s:19     .text:0000000000000010 subsample
     /tmp/ccsrCKie.s:149    .text.startup:0000000000000000 main

UNDEFINED SYMBOLS
write
getRed
getGreen
getBlue
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
