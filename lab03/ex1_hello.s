.text

addi a0 x0 1
li a1 2026

# This prints out the integer stored in a1
ecall 

# This exits the program
addi a0 x0 17
addi a1 x0 0
ecall
