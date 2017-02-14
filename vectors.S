.macro movi reg, val
	lui \reg, %hi(\val)
	ori \reg, \reg, %lo(\val)
.endm

.macro STUB num
	.align 4
	.section .vector_new_\num,"ax",@progbits
	.global __vector_\num
	__vector_\num:
		movi $k0, _isr_primary_install
		lw $k0, \num * 4($k0)
		jr $k0
.endm

.align 4
.global __use_isr_install
__use_isr_install:
STUB 0


.section .text
.align 4
.global _isr_primary_install
_isr_primary_install:
.word _isr_handler



# Interrupts are handled here
.align 4
.set noreorder
.global _isr_handler
_isr_handler:
	# this is an interrupt service routine

	# tell the assembler not to use $1 right now
	.set noat

	# save all caller-save registers, and also ra
	addi $sp,$sp,-64
	sw $ra, 0($sp)
	sw  $1, 4($sp)
	sw  $2, 8($sp)
	sw  $3,12($sp)
	sw  $4,16($sp)
	sw  $5,20($sp)
	sw  $6,24($sp)
	sw  $7,28($sp)
	sw  $8,32($sp)
	sw  $9,36($sp)
	sw $10,40($sp)
	sw $11,44($sp)
	sw $12,48($sp)
	sw $13,52($sp)
	sw $14,56($sp)
	sw $15,60($sp)

	# call user's handler
	jal interrupt_handler
	nop

	# restore saved registers
	lw $15,60($sp)
	lw $14,56($sp)
	lw $13,52($sp)
	lw $12,48($sp)
	lw $11,44($sp)
	lw $10,40($sp)
	lw  $9,36($sp)
	lw  $8,32($sp)
	lw  $7,28($sp)
	lw  $6,24($sp)
	lw  $5,20($sp)
	lw  $4,16($sp)
	lw  $3,12($sp)
	lw  $2, 8($sp)
	lw  $1, 4($sp)
	lw $ra, 0($sp)
	addi $sp,$sp,64

	.set at
	# now the assembler is allowed to use $1 again

	# standard epilogue follows
	eret
	nop
	nop

	
.global enable_interrupts
enable_interrupts:
	ei
	jr $ra
	nop

# Exceptions are handled here (trap, syscall, etc)
.section .gen_handler,"ax",@progbits
.set noreorder
.ent _gen_exception
_gen_exception:
	mfc0 $k0, $14, 0
	addi $k0, $k0, 4
	mtc0 $k0, $14, 0
	eret
	nop

.end _gen_exception
