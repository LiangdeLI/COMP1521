# COMP1521 Lab 04 ... Simple MIPS assembler


### Global data

   .data
msg1:
   .asciiz "n: "
msg2:
   .asciiz "n! = "
eol:
   .asciiz "\n"

### main() function

   .data
   .align 2
main_ret_save:
   .word 4

   .text
   .globl main

main:
   sw   $ra, main_ret_save

#  ... your code for main() goes here
   la $a0, msg1
   li $v0, 4
   syscall
   
   li $v0, 5
   syscall
   
   move $s0, $v0
   
   la $a0, msg2
   li $v0, 4
   syscall
   
   jal fac
   
   move $a0, $v0
   li $v0, 1
   syscall
   
   la $a0, eol
   li $v0, 4
   syscall

   lw   $ra, main_ret_save
   jr   $ra           # return



### fac() function

   .data
   .align 2
fac_ret_save:
   .space 4

   .text

fac:
   sw   $ra, fac_ret_save
   
#  ... your code for fac() goes here
   li $t0, 1
   li $t1, 1

for: 
   bgtu $t0, $s0, end_for
   mul $t1, $t1, $t0
   addi $t0, 1
   j for
   
end_for:
   move $v0, $t1

   lw   $ra, fac_ret_save
   jr   $ra            # return ($v0)

