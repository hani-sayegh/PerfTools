global mov32x8

section .text

mov32x8:
align 64
mov r9, rdx
mov r10, rcx
.repeat:
mov rdx, r9
mov rcx, r10
.done:
vmovdqu ymm0, [rdx] 
vmovdqu ymm0, [rdx + 32] 
vmovdqu ymm0, [rdx + 64] 
vmovdqu ymm0, [rdx + 96] 
vmovdqu ymm0, [rdx + 128] 
vmovdqu ymm0, [rdx + 160] 
vmovdqu ymm0, [rdx + 192] 
vmovdqu ymm0, [rdx + 224] 
add rdx, 256
sub rcx, 256
jg .done
dec r8
jg .repeat
ret