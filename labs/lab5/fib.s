# MIPS assembler to compute Fibonacci numbers

   .data
msg1:
   .asciiz "n = "
msg2:
   .asciiz "fib(n) = "
msg3:
   .asciiz "n must be > 0"
   
   .text

# int main(void)
# {
#    int n;
#    printf("n = ");
#    scanf("%d", &n);
#    if (n >= 1)
#       printf("fib(n) = %d\n", fib(n));
#    else {
#       printf("n must be > 0\n");
#       exit(1);
#    }
#    return 0;
# }

   .globl main
main:
   # prologue
   addi $sp, $sp, -4
   sw   $fp, ($sp)
   move $fp, $sp
   addi $sp, $sp, -4
   sw   $ra, ($sp)

   # function body
   la   $a0, msg1       # printf("n = ");
   li   $v0, 4
   syscall

   li   $v0, 5          # scanf("%d", &n);
   syscall
   move $a0, $v0

   # ... add code to check (n >= 1)
   li $t0, 1
   bge $a0, $t0, right
   
   # ... print an error message, if needed
   la   $a0, msg3
   li   $v0, 4
   syscall
   li   $a0, '\n'
   li   $v0, 11
   syscall
   
   # ... and return a suitable value from main()
   move $v0, $0
   j epilogue

right:
   jal  fib             # $s0 = fib(n);
   nop
   move $s0, $v0

   la   $a0, msg2       # printf((fib(n) = ");
   li   $v0, 4
   syscall

   move $a0, $s0        # printf("%d", $s0);
   li   $v0, 1
   syscall

   li   $a0, '\n'       # printf("\n");
   li   $v0, 11
   syscall

epilogue:
   # epilogue
   lw   $ra, ($sp)
   addi $sp, $sp, 4
   lw   $fp, ($sp)
   addi $sp, $sp, 4
   jr   $ra


# int fib(int n)
# {
#    if (n < 1)
#       return 0;
#    else if (n == 1)
#       return 1;
#    else
#       return fib(n-1) + fib(n-2);
# }

li $v0, 1
syscall



fib:
   # prologue
   # ... add a suitable prologue
   addi $sp, $sp, -4
   sw   $fp, ($sp)
   move $fp, $sp
   addi $sp, $sp, -4
   sw   $ra, ($sp)
   move $s0, $a0
   addi $sp, $sp, -4
   sw   $s0, ($sp)
   addi $sp, $sp, -4
   sw   $0,  ($sp)
   addi $sp, $sp, -4
   sw   $0,  ($sp)

   #li $v0, 1
   #syscall

   # function body
   li   $t0, 1
   blt  $s0, $t0, zero_case
   beq  $s0, $t0, one_case
   lw   $s0, 8($sp)
   addi $a0, $s0, -1
   jal  fib             
   nop
   sw   $v0, 4($sp)
   lw   $s0, 8($sp)
   addi $a0, $s0, -2
   jal  fib
   nop
   lw   $t2, 4($sp)
   add  $v0, $v0, $t2
   j end
   
zero_case:
   li   $v0, 0
   j end
one_case:
   li   $v0, 1
   j end
   # epilogue
   # ... add a suitable epilogue
end:
   #move $t0, $v0
   #move $a0, $v0
   #li  $v0, 1
   #syscall
   #li   $a0, ' '       # printf(" ");
   #li   $v0, 11
   #syscall
   #move $v0, $t0
   
   add  $sp, $sp, 12
   lw   $ra, ($sp)
   add  $sp, $sp, 4
   lw   $fp, ($sp)
   add  $sp, $sp, 4
   jr   $ra
