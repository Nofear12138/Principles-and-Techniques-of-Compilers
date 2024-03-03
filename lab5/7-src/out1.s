.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

main:
  addi $sp, $sp, -44
  sw $ra, 40($sp)
  sw $fp, 36($sp)
  addi $fp, $sp, 44
  lw $t0, -12($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -12($fp)
  lw $t0, -16($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -16($fp)
  lw $t0, -20($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -20($fp)
  jal read
  move $t0, $v0
  sw $t0, -24($fp)
  lw $t0, -28($fp)
  lw $t1, -24($fp)
  move $t0, $t1
  sw $t0, -28($fp)
label1:
  lw $t0, -20($fp)
  lw $t1, -28($fp)
  bge $t0, $t1, label2
  lw $t1, -12($fp)
  lw $t2, -16($fp)
  add $t0, $t1, $t2
  sw $t0, -32($fp)
  lw $t0, -36($fp)
  lw $t1, -32($fp)
  move $t0, $t1
  sw $t0, -36($fp)
  lw $t0, -16($fp)
  move $a0, $t0
  jal write
  lw $t0, -40($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -40($fp)
  lw $t0, -12($fp)
  lw $t1, -16($fp)
  move $t0, $t1
  sw $t0, -12($fp)
  lw $t0, -16($fp)
  lw $t1, -36($fp)
  move $t0, $t1
  sw $t0, -16($fp)
  lw $t1, -20($fp)
  addi $t0, $t1, 1
  sw $t0, -44($fp)
  lw $t0, -20($fp)
  lw $t1, -44($fp)
  move $t0, $t1
  sw $t0, -20($fp)
  j label1
label2:
  move $v0, $zero
  move $sp, $fp
  lw $ra, -4($fp)
  lw $fp, -8($fp)
  jr $ra
