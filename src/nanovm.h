#ifndef nanovm
#define nanovm


#define MAX_MEM 512 							// Maximum size of memory
#define MAX_STACK 128							// Max size of stack in bytes
#define STACK_BOTTOM_ADDRESS 0x7f				// Address of the bottom of the stack

#define zeroflag(n) { if((n) & 0x00ff) z_flag = 0; else z_flag = 1; }					// Z flag set and clear
#define carryflag(n) { if ((n) & 0x0100) carry_flag = 1; else carry_flag = 0; }			// Carry flag set and clear
unsigned short pc = 0;							// Program counter
unsigned short acc = 0;	                        // Accumulator
unsigned short x = 0;							// X register
unsigned short y = 0;							// Y register
unsigned short mar;                             // Memory address register
unsigned char ir;	                            // Instruction register
unsigned short operand;                         // The 8 bit operand from the instruction register
unsigned char* memory;							// The memory
unsigned short run;                             // Execute while run is true
unsigned char z_flag;							// Zero flag
unsigned char carry_flag;						// Carry flag
signed short stack_pointer;						// Address stack pointer is pointing to
struct timeval stop, start;
	
#endif