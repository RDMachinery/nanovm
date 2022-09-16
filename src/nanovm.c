/* nanovm.c - A tiny virtual machine.
 *
 * 
 * Author: Mario Gianota July 2021
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "nanovm.h"
#include "opcodes.h"

char* VM_VERSION = "NanoVM Version: 0.5.2 July 2021";

char get_printable_char(char c) {
	if( c < 33 || c > 126 )
			return '.';
	return c;
}

void ask_dump_mem() {
	unsigned short width = 0;
	unsigned char c;
	unsigned short address = 0;
	unsigned char buf[16];
	unsigned int char_index = 0;
	
	printf("\nDump memory? (y or n) ");
	c = getchar();
	if( c == 'n' )
		return;
	
	printf("$0000 to $%04x:\n", MAX_MEM - 1);
	printf("%04x:   ", address);
	for(int i=0; i<MAX_MEM; i++) {
		c = memory[i];
		buf[char_index++] = c;
		printf("%02x ", c);
		width++;
		if( width > 15 ) {
			address += 16;
			printf("  ");
			for(int j=0; j<16; j++) {
				printf("%c", get_printable_char(buf[j]));
			}
			char_index = 0;
			if( i < MAX_MEM - 1 ) {
				printf("\n");
				printf("%04x:   ", address);
			}
			width = 0;
		}
	}
}


void load(char *fname) {
	// Read in program
	FILE *fp;
	size_t size;
	unsigned char program[MAX_MEM];
	
	fp = fopen(fname, "rb");
	if (fp == NULL){ /*ERROR detection if file == empty*/
        printf("Error: There was an error reading the program image %s. File not found. \n", fname);           
        exit(1);
    }
	
	// Read magic number & load program into memory
	unsigned short magic;
	fread(&magic, sizeof(unsigned short), 1, fp);
	if( magic != 0xd00d ) {
		printf("Not a nanovm program image file. Bad magic number.\n");
		exit(1);
	}	
		
	// Load program image starting memory address
	unsigned short org_address;
	fread(&org_address, sizeof(unsigned short), 1, fp);
	pc = org_address;
	
	fseek(fp,4, SEEK_END);
	size = ftell(fp);
	size -= 4;
	fseek(fp, 4, SEEK_SET);
	
	fread(&program, sizeof(unsigned char), size, fp);
	fclose(fp);
	
	if( org_address + size - 4 > MAX_MEM ) {
		printf("Error. Program too large. Memory is %d bytes in size.\n", MAX_MEM);
		exit(1);
	}
	
	// Init memory
	memory = (char *) malloc(MAX_MEM);
	
	// Zero memory
	for(int i=0; i<MAX_MEM; i++) {
		memory[i] = 0;
	}
	
	// Init stack. Bottom of stack is positioned by default at address $007f (decimal: 127)
	stack_pointer = STACK_BOTTOM_ADDRESS;//0x7f;
		
	// Copy program image into memory
	for(int i=0; i<size - 4; i++) {
		memory[org_address + i] = program[i];
	}	
	printf("Loaded %d bytes.\n", size - 4);
}

void push(unsigned char c) {
	if( stack_pointer == 0 ) {
		printf("Error. Out of stack space. Stack size is: %d bytes.\n", MAX_STACK);
		exit(1);
	}
	memory[--stack_pointer] = c;
}

unsigned char pop() {
	if( stack_pointer >= MAX_STACK - 1 ) {
		printf("Error. Stack underflow. \n");
		exit(1);
	}
	return memory[stack_pointer++];
}

unsigned char peek() {
	return memory[stack_pointer];
}

char stack_is_empty() {
	return stack_pointer == STACK_BOTTOM_ADDRESS;
}

unsigned char fetchUInt8(unsigned short address) {
	return memory[address];
}

int main(int argc, char *argv[]) {
	if( argc != 2 ) {
		printf("%s\n", VM_VERSION);
		printf("\n\tusage: nanovm <object file> e.g., nanovm hello.bin\n");
		exit(1);
	}
	
	unsigned char input;
	unsigned short address;
	unsigned char source;
	unsigned char opcode;
	unsigned char n;
	unsigned char buf[2]; 
	unsigned char cmp_value;
	unsigned char cmp_x_value;
	unsigned char cmp_y_value;
	unsigned char a,b;
	unsigned long total_cycles = 0;
	
	acc = 0;
	x = 0;
	y = 0;
	run = 1;
	z_flag = 0;
	
	load(argv[1]);
	
	gettimeofday(&start, NULL);
	
	// Execute loaded program
	while( run ) {
		mar = pc;    					// Program counter to memory address register
		ir = memory[mar];				// Fetch instruction
		opcode = ir;					// Get the opcode
			
		//printf("pc: %x (%d) opcode: %d acc: %d memory[pc]: %d\n", pc, pc, opcode, acc, memory[pc]);
		pc++; 							// Increment pc
		
		
		// Decode & execute the instruction		
		switch(opcode) {
		case LDA_IMM: 
			acc = fetchUInt8(pc++); 
			zeroflag(acc);
			break;
		case LDA_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			acc = memory[address];
			zeroflag(acc);
			break;
		case STA: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			memory[address] = acc; 
			break;
		case LDX_IMM: 
			x = fetchUInt8(pc++); 
			zeroflag(x);
			break;
		case LDX_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			x = memory[address];
			zeroflag(x);
			break;
		case STX: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			memory[address] = x; 
			break;
		case LDY_IMM: 
			y = fetchUInt8(pc++); 
			zeroflag(y);
			break;
		case LDY_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			y = memory[address];
			zeroflag(y);
			break;
		case STY: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			memory[address] = y; 
			break;
		case ADD_IMM: 
			acc += fetchUInt8(pc++) + carry_flag; 
			zeroflag(acc);
			carryflag(acc);
			break;
		case ADD_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			acc += memory[address] + carry_flag; 
			zeroflag(acc);
			carryflag(acc);
			break; 
		case SUB_IMM: 
			source = (~fetchUInt8(pc++)) + carry_flag; 	// Two's complement subtraction with carry see: https://en.wikipedia.org/wiki/Carry_flag
			acc += source; 
			zeroflag(acc);
			carryflag(acc);
			break;
		case SUB_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			acc += (~memory[address]) + carry_flag; 		// Two's complement subtraction with carry see: https://en.wikipedia.org/wiki/Carry_flag
			zeroflag(acc);
			carryflag(acc);
			break;
		case MUL_IMM: 
			acc *= fetchUInt8(pc++); 
			zeroflag(acc);
			carryflag(acc);
			break;
		case MUL_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			acc *= memory[address]; 
			zeroflag(acc);
			carryflag(acc);
			break;
		case DIV_IMM: 
			n = fetchUInt8(pc++);
			if( n == 0 ) {
				printf("Error. Division by zero. PC: $%x\n", pc-1);
				exit(1);
			}
			acc /= n; 
			zeroflag(acc);
			//carryflag(acc);
			break;
		case DIV_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			n = memory[address];
			if( n == 0 ) {
				printf("Error. Division by zero. PC: $%x\n", pc-2);
				exit(1);
			}
			acc /= n; 
			zeroflag(acc);
			//carryflag(acc);
			break;
		case JMP: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			pc = address; 
			break;
		case JMP_IND: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			address = fetchUInt8(address) << 8 | fetchUInt8(address+1);
			pc = address; 
			break;
		case JEQ: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			if( z_flag == 1 ) 
				pc = address; 
			break;
		case JNE: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			if( z_flag == 0 ) { 
				pc = address;
			}
			break;
		case JCS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			if( carry_flag == 1 ) 
				pc = address; 
			break;
		case JCC: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			if( carry_flag == 0 ) { 
				pc = address;
			}
			break;	
		case HALT: run = 0; break;
		case IN: scanf("%d", &input); acc = input; break;
		case OUT: printf("%d\n", (unsigned char) acc); break;
		case JSR:	
			buf[0] = fetchUInt8(pc++);	// Fetch subroutine address
			buf[1] = fetchUInt8(pc++);
			push( pc >> 8);				// Push return address on stack
			push( pc & 0x0F);
			pc = buf[0] << 8 | buf[1]; // set pc to subroutine address
			break;
		case RTS:
			buf[1] = pop();				// pop return address from stack
			buf[0] = pop();
			pc = buf[0] << 8 | buf[1];	// set pc to return address
			break;
		case CMP_IMM:
			cmp_value = fetchUInt8(pc++);
			if( acc - cmp_value == 0 ) z_flag = 1;
			else z_flag = 0;
			break;
		case CMP_ABS:
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			cmp_value = memory[address]; 
			if( acc - cmp_value == 0 ) z_flag = 1;
			else z_flag = 0;
			break;
		case PUSHA:
			push(acc);
			break;
		case POPA:
			acc = pop();
			zeroflag(acc);
			break;
		case SHL:
			acc = (acc << 1);					// 6502 & ARM CPU do this. Left-most bit ends up in carry flag.
			carryflag(acc);
			zeroflag(acc);
		case SHR:
			if( acc & 1 == 1 ) carry_flag = 1;	// 6502 & ARM CPU do this. Right-most bit ends up in carry flag
			acc = (acc >> 1);
			zeroflag(acc);
			break;
		case INC:
			acc++;
			carryflag(acc);
			zeroflag(acc);
			break;
		case DEC:
			acc--;
			carryflag(acc);
			zeroflag(acc);
			break;
		case NOP:
			;
			break;
		case CPX_IMM:
			cmp_x_value = fetchUInt8(pc++);
			if( x - cmp_x_value == 0 ) z_flag = 1;
			else z_flag = 0;
			break;
		case CPX_ABS:
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			cmp_x_value = memory[address]; 
			if( x - cmp_x_value == 0 ) z_flag = 1;
			else z_flag = 0;
			break;
		case CPY_IMM:
			cmp_y_value = fetchUInt8(pc++);
			if( y - cmp_y_value == 0 ) z_flag = 1;
			else z_flag = 0;
			break;
		case CPY_ABS:
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			cmp_y_value = memory[address]; 
			if( y - cmp_y_value == 0 ) z_flag = 1;
			else z_flag = 0;
			break;
		case TAX:
			x = acc;
			break;
		case TAY:
			y = acc;
			break;
		case TXA:
			acc = x;
			zeroflag(acc);
			break;
		case TYA:
			acc = y;
			zeroflag(acc);
			break;
		case INX:
			x++;
			zeroflag(x);
			break;
		case INY:
			y++;
			zeroflag(y);
			break;
		case DEX:
			x--;
			zeroflag(x);
			break;
		case DEY:
			y--;
			zeroflag(y);
			break;
		case NEG:
			acc = (~acc) + 1;
			break;
		case DUP:
			if( ! stack_is_empty() ) {
				char c = peek();
				push(c);
			}
			break;
		case SWAP:
			a = pop();
			b = pop();
			push(a);
			push(b);
			break;
		case AND_IMM: 
			acc = acc & fetchUInt8(pc++); 
			zeroflag(acc);
			break;
		case AND_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			acc = acc & memory[address]; 
			zeroflag(acc);
			break; 	
		case OR_IMM: 
			acc = acc | fetchUInt8(pc++); 
			zeroflag(acc);
			break;
		case OR_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			acc = acc | memory[address]; 
			zeroflag(acc);
			break; 	
		case XOR_IMM: 
			acc = acc ^ fetchUInt8(pc++); 
			zeroflag(acc);
			break;
		case XOR_ABS: 
			address = fetchUInt8(pc++) << 8 | fetchUInt8(pc++); 
			acc = acc ^ memory[address]; 
			zeroflag(acc);
			break; 	
		case NOT: 
			acc = ~acc;  
			zeroflag(acc);
			break;
		case CLC: 
			carry_flag = 0;
			break;
		case SEC: 
			carry_flag = 1;
			break;
		default:
			printf("Error. Unhandled instruction code: %d Program Counter Address: $%x\n", opcode, pc);
			exit(1);
        }	
		total_cycles++;
	}
	
	gettimeofday(&stop, NULL);
	unsigned long period = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
	
	printf("Number of cycles: %lu. Execution time %lu microseconds.\n", total_cycles, period);
	ask_dump_mem();
	return 0;
}//:-)