################################################################################
# Homework 3: Matrix-matrix multiplication
# CSCI-2500 Fall 2021
################################################################################
## Data segment
################################################################################
.data
  newline:  .asciiz "\n"
  tab:      .asciiz "\t"

################################################################################
## Text segment
################################################################################
.text
  .align 2          # instructions must be on word boundaries
  .globl main       # main is a global label
  .globl mm_read    # read in NxM matrix
  .globl mm_alloc   # allocate space for NxM matrix
  .globl mm_print   # print out formatted NxM matrix
  .globl mm_mult    # multiple two non-square matrices

################################################################################
# Main segment
################################################################################
main:
  # save return address on the stack
  sub   $sp, $sp, 4   
  sw    $ra, 0($sp)

  # Read in a matrix 1 of some NxM size
  # Return in $v0 - N
  # Return in $v1 - M
  # Return in 0($sp) - base address of matrix
  sub   $sp, $sp, 4             # make space on stack for return value
  jal   mm_read

  # save matrix 1 values
  move  $s0, $v0
  move  $s1, $v1
  lw    $s2, 0($sp)
  add   $sp, $sp, 4             # restore stack

  # Setup arguments and print out matrix 1
  move  $a0, $s0
  move  $a1, $s1
  move  $a2, $s2
  jal   mm_print

  # Read in matrix 2 of some NxM size
  sub   $sp, $sp, 4             # make space on stack for return value
  jal   mm_read

  # save matrix 2 values
  move  $s3, $v0
  move  $s4, $v1
  lw    $s5, 0($sp)
  add   $sp, $sp, 4             # restore stack

  # Setup arguments and print out matrix 2
  move  $a0, $s3
  move  $a1, $s4
  move  $a2, $s5
  jal   mm_print

  # Setup arguments and call mm_mult
  # Note: 5 arguments, so we need to use stack
  # Note: num cols matrix 1 == num rows matrix 2
  move  $a0, $s0                # num rows matrix1
  move  $a1, $s1                # num cols matrix1
  move  $a2, $s2                # address of matrix 1
  move  $a3, $s4                # num cols matrix2
  sub   $sp, $sp, 4   
  sw    $s5, 0($sp)             # address of matrix 2
  jal   mm_mult
  add   $sp, $sp, 4

  # print the result
  move  $a0, $s0 
  move  $a1, $s4
  move  $a2, $v0
  jal   mm_print

  # restore $ra, free stack space, and return
  lw    $ra, 0($sp)
  add   $sp, $sp, 4
  jr    $ra
  li    $v0, 10
  syscall

##############################################################################
# # mm_read: Read in a NxM matrix from standard input
# # Note: You will be calling mm_alloc from this function, so you'll need to save 
# #       $ra on the stack. Also, you'll be returning 3 values back to main, one
# #       value (the matrix base address), will need to be saved on the stack.
#       Matrix format is:
#         First line:  N = num rows
#         Second line: M = num cols
#         Subsequent lines contain one value each, filling each row in turn
#         (same format as hw01, except N,M on separate lines)  
# Input:
#   None
# Return:
#   v0 = N
#   v1 = M
#   0($sp) = base address of matrix
################################################################################
mm_read:
  # save return address and any saved registers on the stack, if necessary
  sub   $sp, $sp, 4
  sw    $ra, 0($sp)             # save return address to stack
  sub   $sp, $sp, 16
  sw    $s0, 0($sp)
  sw    $s1, 4($sp)
  sw    $s2, 8($sp)
  sw    $s3, 12($sp)

  # get N
  li    $v0, 5
  syscall
  move  $s0, $v0                # $s0 = N

  # get M
  li    $v0, 5
  syscall
  move  $s1, $v0                # $s1 = M

  # Setup up arguments and call mm_alloc - v0 is returned as base address
  move  $a0, $s0
  move  $a1, $s1
  jal   mm_alloc
  move  $s2, $v0                # s2 = address of current element
  move  $s3, $v0                # preserve $v0 across syscalls
  move  $s4, $v0

  # do nested loops to read in values
  li    $t0, 0                  # $t0 = 0
  mul   $t1, $s0, $s1           # $t1 = M*N
  read_values:
  beq   $t0, $t1, loop_exit1    # if ($t0 == $t1) break
  li    $v0, 5
  syscall
  move  $t2, $v0
  sw    $t2, 0($s2)             # store at address of current element
  addi  $s2, $s2, 4             # move to next element's address
  addi  $t0, $t0, 1             # increment loop counter
  j     read_values

  loop_exit1:
  # setup up return values
  move  $v0, $s0                # restore $v0 with N
  move  $v1, $s1                # restore $v1 with M
  # Note: third return value goes on the stack *after* restoration below

  # restore stack, ra, and any saved registers, if necessary
  lw    $s3, 12($sp) 
  lw    $s2, 8($sp)
  lw    $s1, 4($sp)
  lw    $s0, 0($sp)
  addi  $sp, $sp, 16            # saved registers are restored
  
  lw    $ra, 0($sp)
  addi  $sp, $sp, 4             # stack and return address are restored
  sw    $s4, 0($sp)

  # return to main
  jr    $ra

################################################################################
# mm_alloc: Allocate an NxM matrix
# Note: You can just allocate a single array of length N*M.
# Input:
#   a0 = N
#   a1 = M
# Return:
#   v0 = base address of allocated space
################################################################################
mm_alloc:
  # save return address and any saved registers on the stack, if necessary

  # Allocate sufficient space
  mul   $a0, $a0, $a1
  mul   $a0, $a0, 4
  li    $v0, 9
  syscall

  # set return value
  # done automatically by syscall

  # restore stack, ra, and any saved registers, if necessary

  # return to main
  jr  $ra

################################################################################
# mm_print: Output the formatted contents of NxM matrix
# Note: Within a row, separate each value by tab
#       End each row with a newline
# Input:
#   a0 = N
#   a1 = M
#   a2 = base pointer
# Return:
#   None
################################################################################
mm_print:
  # save return address and any saved registers on the stack, if necessary
  # do nested loops to print out values
  move  $t5, $a0                # preserve row number across syscalls
  li    $t0, 0                  # row counter

print_rows:
  beq   $t0, $t5, rows_exit     # stop after printing rows 
  sll   $t1, $t0, 2
  mul   $t1, $t1, $a1
  add   $t1, $t1, $a2           # base of row = 4*row_counter*M + base
  li    $t2, 0                  # column counter

print_cols:
  beq   $t2, $a1, cols_exit     # stop after printing t2 entries in row
  
  sll   $t3, $t2, 2
  add   $t3, $t3, $t1           # entry = base of row + 4*column_counter
  li    $v0, 1
  lw    $a0, 0($t3)
  syscall                       # print integer at t3
  li    $v0, 4
  la    $a0, tab
  syscall

  addi  $t2, $t2, 1
  j     print_cols
  
cols_exit:
  li    $v0, 4
  la    $a0, newline
  syscall
  addi  $t0, $t0, 1             # increment row counter
  j     print_rows

rows_exit:

  # restore stack, ra, and any saved registers, if necessary

  # return to main
  jr    $ra

################################################################################
# mm_mult: Perform the actual NxM matrix multiplication
# Input:
#   a0 = N for matrix 1
#   a1 = M for matrix 1 <-> N for matrix 2
#   a2 = base address for matrix 1
#   a3 = M for matrix 2
#   0($sp) = base address for matrix 2
# Return:
#   v0 = base address for result matrix
################################################################################
mm_mult:
  # save return address and any saved registers on the stack, if necessary
  lw    $s5, 0($sp)
  sub   $sp, $sp, 8
  sw    $ra, 4($sp)
  sw    $a1, 0($sp)

  # set up and call mm_alloc for result matrix
  move  $a1, $a3
  jal   mm_alloc
  lw    $a1, 0($sp)
  addi  $sp, $sp, 4

  # three nested loops for matrix-matrix multiplication
#  li    $t0, 0                  # row1 == row3 counter
#for_loop_1:
#  beq   $t0, $a1, exit_1        # break after finishing rows1
#  li    $t1, 0                  # col2 == col3 counter
#for_loop_2:
#  beq   $t1, $a3, exit_2        # break after finishing cols2
#  li    $t3, 0                  # dot product
#  li    $t2, 0                  # inner counter (rows2 == cols1)
#for_loop_3:
#  beq   $t0, $a0, exit_3        # break after cols1
#  
#  mul   $t4, $a1, $t0           # t4 *= current row * N
#  add   $t4, $t4, $t2           # t4 += column offset into row
#  sll   $t4, $t4, 2             # t4 *= 4
#  add   $t4, $t4, $a2            # t4 += base address
#
#  mul   $t5, $a0, $t2           # likewise for t5, with different indices
#  add   $t5, $t5, $t1
#  sll   $t5, $t5, 2
#  add   $t5, $t5, $s5
#
#  lw    $t4, 0($t4)
#  lw    $t5, 0($t5)
#
#  mul   $t4, $t4, $t5
#  add   $t3, $t3, $t4           # add product of elements to dot product t3
#
#  li    $v0, 1
#  move  $a0, $t4
#  syscall
#  li    $v0, 4
#  la    $a0, newline
#  syscall
#
#  addi  $t2, $t2, 1
#  j     for_loop_3
#
#exit_3:
#  # store dot product
#  mul   $t4, $a1, $t0           # t4 = row in matrix 3 * N
#  add   $t4, $t4, $t1           # t4 += column offset into row
#  sll   $t4, $t4, 2             # t4 *= 2
#  add   $t4, $t4, $v0           # t4 += base address of matrix3
#  sw    $t3, 0($t4)             # store dot product at address in matrix3
#
#  # increment counter
#  addi  $t1, $t1, 1
#  j     for_loop_2
#
#exit_2:
#  # increment counter
#  addi  $t0, $t0, 1
#  j     for_loop_1
#
#exit_1:
  # set return value — already set by mm_alloc
  # restore stack, ra, and any saved registers, if necessary
  lw    $ra, 0($sp)
  addi  $sp, $sp, 4
  
  # return to main
  jr    $ra