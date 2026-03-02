    .equ MAX, 128
    .equ INT_MAX, 2147483647
    .equ INT_MIN, -2147483648

    .section .rodata
fmt_bar:
    .string "|%s|\n"
fmt_sum:
    .string "%d\n"

msg_empty:
    .string "error: input is empty\n"
msg_spaces_gt1:
    .string "error: the format is incorrect ( > 1 spaces). There must be one space.\n"
msg_spaces_eq0:
    .string "error: the format is incorrect ( = 0 spaces). There must be one space.\n"
msg_no_first:
    .string "error: no first number\n"
msg_no_second:
    .string "error: no second number\n"
msg_first_not_num:
    .string "error: first is not a non-negative number\n"
msg_second_not_num:
    .string "error: second is not a non-negative number\n"
msg_overflow:
    .string "error: overflow\n"

    .section .bss
    .align 3
buf:
    .space MAX

    .section .text

    .extern fprintf

    .globl main
main:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)
    sd   s1, 8(sp)
    sd   s2, 0(sp)

    la   a0, buf
    li   a1, MAX
    call gets

    la   s0, buf
    lbu  t0, 0(s0)
    beqz t0, input_empty

    la   s0, buf
trim_loop:
    lbu  t0, 0(s0)
    beqz t0, trim_done         

    li   t1, 10                 
    beq  t0, t1, trim_here

    li   t1, 13                 
    beq  t0, t1, trim_here

    addi s0, s0, 1
    j    trim_loop

trim_here:
    sb   zero, 0(s0)            

trim_done:
    la   a0, fmt_bar
    la   a1, buf
    call printf

    li   s1, 0         
    li   s2, 0          
    la   s0, buf

find_space_loop:
    lbu  t0, 0(s0)
    beqz t0, end_scan

    li   t1, ' '
    bne  t0, t1, next_char

    mv   s1, s0
    addi s2, s2, 1
    li   t2, 1
    bgt  s2, t2, spaces_gt1

next_char:
    addi s0, s0, 1
    j    find_space_loop

end_scan:
    beqz s2, spaces_eq0

    la   t0, buf
    beq  s1, t0, no_first

    addi t1, s1, 1
    lbu  t2, 0(t1)
    beqz t2, no_second

    sb   zero, 0(s1)

    la   a0, buf
    call check_is_number_asm
    beqz a0, first_not_num

    addi a0, s1, 1
    call check_is_number_asm
    beqz a0, second_not_num

    la   a0, buf
    call atoi
    mv   t3, a0

    addi a0, s1, 1
    call atoi
    mv   t4, a0

    li   t0, 0
    ble  t4, t0, check_b_neg

    li   t5, INT_MAX
    sub  t5, t5, t4
    bgt  t3, t5, overflow

check_b_neg:
    li   t0, 0
    bge  t4, t0, ok_add

    li   t5, INT_MIN
    sub  t5, t5, t4
    blt  t3, t5, overflow

ok_add:
    add  t6, t3, t4

    la   a0, fmt_sum
    mv   a1, t6
    call printf

    li   a0, 0
    call exit

check_is_number_asm:
    beqz a0, cis_fail
    lbu  t0, 0(a0)
    beqz t0, cis_fail

cis_loop:
    lbu  t0, 0(a0)
    beqz t0, cis_ok

    li   t1, '0'
    blt  t0, t1, cis_fail
    li   t2, '9'
    bgt  t0, t2, cis_fail

    addi a0, a0, 1
    j    cis_loop

cis_ok:
    li   a0, 1
    ret

cis_fail:
    li   a0, 0
    ret

print_err:
    addi sp, sp, -16
    sd   ra, 8(sp)
    sd   a0, 0(sp)

    li   a0, 2          
    ld   a1, 0(sp)    
    call fprintf       

    ld   ra, 8(sp)
    addi sp, sp, 16
    ret

input_empty:
    la   a0, msg_empty
    call print_err
    li   a0, 1
    call exit

spaces_gt1:
    la   a0, msg_spaces_gt1
    call print_err
    li   a0, 1
    call exit

spaces_eq0:
    la   a0, msg_spaces_eq0
    call print_err
    li   a0, 1
    call exit

no_first:
    la   a0, msg_no_first
    call print_err
    li   a0, 1
    call exit

no_second:
    la   a0, msg_no_second
    call print_err
    li   a0, 1
    call exit

first_not_num:
    la   a0, msg_first_not_num
    call print_err
    li   a0, 1
    call exit

second_not_num:
    la   a0, msg_second_not_num
    call print_err
    li   a0, 1
    call exit

overflow:
    la   a0, msg_overflow
    call print_err
    li   a0, 1
    call exit