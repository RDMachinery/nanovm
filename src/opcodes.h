#ifndef opcodes
#define opcodes

// Opcodes
#define LDA_IMM    	0   // Load accumulator with immediate value
#define LDA_ABS		1	// Load accumulator from address (absolute mode)
#define STA    		2   // Store operand in accumulator in memory
#define ADD_IMM    	3   // Add immediate value to accumulator
#define ADD_ABS    	4   // Add memory to accumulator
#define SUB_IMM    	5   // Subtract immediate value from accumulator
#define SUB_ABS    	6   // Subtract memory from accumulator
#define MUL_IMM    	7   // Multiply immediate value with accumulator
#define MUL_ABS    	8   // Multiply memory with accumulator
#define DIV_IMM    	9   // Divide accumulator by immediate value
#define DIV_ABS    	10  // Divide accumulator by memory
#define JMP    		11  // Jump to address
#define JEQ    		12  // Jump if Z flag is zero
#define JNE	   		13	// Jump if Z flag not equal to zero
#define HALT   		14  // Halt execution
#define IN     		15  // Read a short number from stdin into accumulator
#define OUT    		16  // Print accumulator to stdout
#define JSR			17  // Jump to subroutine
#define RTS			18 	// Return from subroutine
#define CMP_IMM		19	// Compare a value with accumulator and set Z flag appropriately
#define CMP_ABS		20  // Compare value at an address with accumulator and set Z flag appropriately
#define JMP_IND		21  // Jump indirectly to memory address
#define PUSHA		22  // Push accumulator on stack
#define POPA		23  // Pop top of stack into accumulator
#define SHL			24  // Shift accumulator left
#define SHR			25  // Shift accumulator right
#define INC			26  // Increment accumulator
#define DEC			27  // Decrement accumulator
#define NOP			28	// No operation
#define LDX_IMM		29  // Load X register with immediate value
#define LDX_ABS		30	// Load X register with value from memory
#define LDY_IMM		31	// Load Y register with immediate value
#define LDY_ABS		32	// Load Y register with value from memory
#define STX			33	// Store X register to memory
#define STY			34	// Store Y register to memory
#define CPX_IMM		35  // Compare a value with the X register and set Z flag appropriately
#define CPX_ABS		36  // Compare a value at a memory address with the X register and set Z flag appropriately
#define CPY_IMM		37  // Compare a value with the X register and set Z flag appropriately
#define CPY_ABS		38  // Compare a value at a memory address with the X register and set Z flag appropriately
#define TAX			39	// Transfer accumulator to X register
#define TAY			40  // Transfer accumulator to Y register
#define TXA			41  // Transfer X register to accumulator
#define TYA			42	// Transfer Y register to accumulator
#define INX			43	// Increment X register
#define INY			44	// Increment Y register
#define DEX			45	// Decrement X register
#define DEY			46	// Decrement Y register
#define NEG			47  // Negate the accumulator. This converts the value in the accumulator to two's complement form.
#define DUP			48  // Duplicates the top of the stack. If the stack is empty, this instruction does nothing
#define SWAP		49  // Swap the top two stack values
#define AND_IMM		50  // AND accumulator with immediate value
#define AND_ABS		51  // AND accumulator with contents of memory
#define OR_IMM		52  // OR accumulator with immediate value
#define OR_ABS		53  // OR accumulator with contents of memory
#define XOR_IMM		54  // XOR accumulator with immediate value
#define XOR_ABS		55  // XOR accumulator with contents of memory
#define NOT			56  // Invert accumulator
#define CLC			57	// Clear carry flag
#define SEC			58	// Set carry flag
#define JCS			59	// Jump if carry set
#define JCC			60	// Jump if carry clear
#endif