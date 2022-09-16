/**
 * nanoasm.c - The NanoVM Assembler
 *
 * Grammar:
 *
 * assemble ::= org <number> <statement>* EOF
 * statement ::= <newline> | 
 *               <comment> | 
 *               <code> [<comment>] <newline>
 *
 * comment ::= ; <string> <newline>
 * string ::= <empty> | <printable character>
 * code ::= <mnemonic> [<address_mode>] [<operand>]
 *          | mnemonic (<operand>)
 * address_mode::= #
 * operand ::= <number>
 * number ::= <decimal> | $<hex> | %<binary>
 *
 * Author: Mario Gianota July 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "opcodes.h"

FILE *fp;		// Assembly language source file
FILE *ofp;		// Assembled output file
int look;		// Lookahead character
int line_no;	// Track line numbers
int buf[6];		// Mnemonic buffer

char tokens[][6] = {
{"LDA"}, {"STA"}, {"ADD"}, {"SUB"}, {"MUL"}, 
{"DIV"}, {"JMP"}, {"JEQ"}, {"JNE"}, {"HALT"}, {"IN"}, {"OUT"},{"ORG"},
{"JSR"}, {"RTS"}, {"CMP"}, {"PUSHA"}, {"POPA"}, {"SHL"}, {"SHR"}, 
{"INC"}, {"DEC"}, {"NOP"}, {"LDX"}, {"LDY"}, {"STX"}, {"STY"},
{"CPX"}, {"CPY"}, {"TAX"}, {"TAY"}, {"TXA"}, {"TYA"},
{"INX"}, {"INY"}, {"DEX"}, {"DEY"},
{"NEG"},{"DUP"}, {"SWAP"}, {"AND"}, {"OR"},{"XOR"},{"NOT"},
{"CLC"}, {"SEC"}, {"JCS"}, {"JCC"}};

int num_tokens = 48;

int mindex;		// match token index

unsigned char instruction;		// The assembled instruction
unsigned char amode;			// Addressing mode 0 = absolute, 1 = immmediate
unsigned short operand;			// Instruction's operand


char* ASM_VERSION = "NanoASM Version: 0.5.2 July 2021";

void la() {
	look = fgetc(fp);
	if( look == '\n' )
		line_no++;
	}

void skipWS() {
	while( look == ' ' || look == '\t')
		la();
}

int is_alpha(int c) {
	return (c >='a' && c <= 'z') || (c >='A' && c <='Z');
}

int is_digit(int c) {
	return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' ||
		c == '6' || c == '7' || c == '8' || c == '9';
	
}

int striccmp(char const *a, char const *b) {
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

unsigned short bin_to_decimal(long long n) {
    unsigned short int dec = 0;
	int i = 0, rem;
    
	while (n != 0) {
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
    }
    return dec;
}

unsigned short hex_to_decimal(char *hex) {
	unsigned short decimal, place;
	int i=0, val, len;
	
	decimal = 0;
	place = 1;
	len = strlen(hex);
	len--;
	
    for(i=0; hex[i]!='\0'; i++) {
 
        /* Find the decimal representation of hex[i] */
        if(hex[i]>='0' && hex[i]<='9') {
            val = hex[i] - 48;
        } else if(hex[i]>='a' && hex[i]<='f') {
            val = hex[i] - 97 + 10;
        } else if(hex[i]>='A' && hex[i]<='F') {
            val = hex[i] - 65 + 10;
        }

        decimal += val * pow(16, len);
        len--;
    }
	return decimal;
	
}

void newline() {
	if( look == '\r' )
		la();
	if( look != '\n' ) {
		printf("Syntax error. Line: %d. Expected new line.", line_no);
		exit(1);
	}
	la();
}

void comment() {
	while( look != '\n' && look != EOF )
		la();
	la();
}

void address_mode() {
	skipWS();
	if( look == '#' ) {
		// immediate mode
		amode = 1;
		la();
	} else if( look == '(' ) {
		amode = 2;
		la();
	} else {
		// absolute mode
		amode = 0;
	} 
}

unsigned short _operand() {
	skipWS();
	char num[16];
	int i = 0;
	int found = 0;
	const int hex = 1;
	const int bin = 2;
	const int dec = 4;
	int base;
	
	if( look == '$' ) {
		base = hex;
		la();
	} else if( look == '%' ) {
		base = bin;
		la();
	} else {
		base = dec;
	}
	
	while( is_digit(look) || is_alpha(look)) {
		found = 1;
		num[i++] = (char)look;
		la();
	}
	
	if( ! found ) {
		printf("Syntax error. Line: %d. Expected a number after instruction mnemonic. Found '%c' (%d).\n", line_no, (char)look, look);
		exit(1);
	}
		
	num[i] = '\0';
		
	if( base == hex )
		operand = hex_to_decimal(num);
	else if( base == dec )
		operand = atoi(num);
	else if( base == bin )
		operand = bin_to_decimal(atoi(num));
		
	if( operand > 65535 ) {
		printf("Syntax error. Line: %d. Number too large. An operand must lie in the range 0 to 65535.");
		exit(1);
	}
	
	return operand;
}	

void mnemonic() {
	int j = 0;
	char cbuf[80];			// Temporary buffer
	mindex = -1;
	
	// Copy read buffer to temporary string buffer
	while( is_alpha(look) ) {
		cbuf[j++] = (char)look;
		la();
		if( j >= 80 ) {
			printf("Syntax error. Line: %d. Menmomic too long.\n", line_no);
			exit(1);
		}
	}
	cbuf[j] = '\0';
		
	// Compare temporary buffer to token list
	for(int i=0; i<num_tokens; i++) {
		if( striccmp(cbuf, tokens[i]) == 0) {
			mindex = i;
			break;
		}
	}
	
	// If no match generate an error
	if( mindex == -1 ) {
		printf("Syntax error. Line: %d. Unknown assembler mnemonic '%s'. This error also occurs if you neglect to include an ORG directive in your source code. \n", line_no - 1, cbuf);
		exit(1);
	}
}


void code() {
	char buf[2];
	
	mnemonic();
	address_mode(); // Get the address mode

	// Decode instruction index and write instruction + operand
	switch(mindex) {
		case 0: 
			if( amode == 0 ) {
				instruction = LDA_ABS;
				_operand(); 
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = LDA_IMM;
				_operand();
				if( operand > 255 ) {
					printf("Syntax error. Line: %d. Operand too large: $%x (%d).\n", line_no, operand, operand);
					exit(1);
				}
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
				
			break; // LDA
		case 1: 
			instruction = STA;
			_operand(); //printf("Operand: %d\n", operand);
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			buf[0] = (unsigned char) (operand >> 8); 
			buf[1] = (unsigned char) (operand & 0xff);
			//printf("Hi: %d, Lo: %d\n", buf[0], buf[1]);
			fwrite(&buf, sizeof(unsigned char), 2, ofp);
			break; // STA
		case 2: 
			if( amode == 0 ) {
				instruction = ADD_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = ADD_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // ADD
		case 3: 
			if( amode == 0 ) {
				instruction = SUB_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = SUB_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // SUB
		case 4: 
			if( amode == 0 ) {
				instruction = MUL_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = MUL_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // MUL
		case 5: 
			if( amode == 0 ) {
				instruction = DIV_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = DIV_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // DIV
		case 6: 
			if( amode == 0 ) {
				instruction = JMP; 
				fwrite(&instruction, sizeof(instruction), 1, ofp); 
				_operand();
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else if( amode == 2 ) {
				instruction = JMP_IND; 
				fwrite(&instruction, sizeof(instruction), 1, ofp); 
				_operand();
				//printf("look='%c' operand=%x\n", look, operand);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
				skipWS();
				if( look != ')' ) {
					printf("Syntax error. Line: %d. Expected closing parenthesis ')' Found '%c'.\n", line_no, look);
					exit(1);
				}
				la();
			}
			break; // JMP
		case 7: 
			instruction = JEQ; 
			fwrite(&instruction, sizeof(instruction), 1, ofp); 
			_operand();
			buf[0] = (unsigned char) (operand >> 8);
			buf[1] = (unsigned char) (operand & 0xff);
			fwrite(&buf, sizeof(unsigned char), 2, ofp);
			break; // JEQ
		case 8: 
			instruction = JNE; 
			fwrite(&instruction, sizeof(instruction), 1, ofp); 
			_operand();
			buf[0] = (unsigned char) (operand >> 8);
			buf[1] = (unsigned char) (operand & 0xff);
			fwrite(&buf, sizeof(unsigned char), 2, ofp);
			break; // JNE
		case 9: instruction = HALT; fwrite(&instruction, sizeof(instruction), 1, ofp); HALT; break; // HALT
		case 10: instruction = IN; fwrite(&instruction, sizeof(instruction), 1, ofp); break; // IN
		case 11: instruction = OUT; fwrite(&instruction, sizeof(instruction), 1, ofp); break; // OUT
		case 13:
			instruction = JSR;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			_operand();
			buf[0] = (unsigned char) (operand >> 8);
			buf[1] = (unsigned char) (operand & 0xff);
			fwrite(&buf, sizeof(unsigned char), 2, ofp);
			break; // JSR
		case 14:
			instruction = RTS;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // RTS
		case 15:
			if( amode == 0 ) {
				instruction = CMP_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = CMP_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // CMP
		case 16:
			instruction = PUSHA;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break;
		case 17:
			instruction = POPA;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break;
		case 18:
			instruction = SHL;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break;
		case 19:
			instruction = SHR;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break;
		case 20:
			instruction = INC;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break;
		case 21:
			instruction = DEC;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break;
		case 22:
			instruction = NOP;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break;
		case 23:
			if( amode == 0 ) {
				instruction = LDX_ABS;
				_operand(); 
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = LDX_IMM;
				_operand();
				if( operand > 255 ) {
					printf("Syntax error. Line: %d. Operand too large: $%x (%d).\n", line_no, operand, operand);
					exit(1);
				}
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // LDX
		case 24:
			if( amode == 0 ) {
				instruction = LDY_ABS;
				_operand(); 
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = LDY_IMM;
				_operand();
				if( operand > 255 ) {
					printf("Syntax error. Line: %d. Operand too large: $%x (%d).\n", line_no, operand, operand);
					exit(1);
				}
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // LDY
		case 25: 
			instruction = STX;
			_operand(); //printf("Operand: %d\n", operand);
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			buf[0] = (unsigned char) (operand >> 8); 
			buf[1] = (unsigned char) (operand & 0xff);
			//printf("Hi: %d, Lo: %d\n", buf[0], buf[1]);
			fwrite(&buf, sizeof(unsigned char), 2, ofp);
			break; // STX
		case 26: 
			instruction = STY;
			_operand(); //printf("Operand: %d\n", operand);
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			buf[0] = (unsigned char) (operand >> 8); 
			buf[1] = (unsigned char) (operand & 0xff);
			//printf("Hi: %d, Lo: %d\n", buf[0], buf[1]);
			fwrite(&buf, sizeof(unsigned char), 2, ofp);
			break; // STY
		case 27:
			if( amode == 0 ) {
				instruction = CPX_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = CPX_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // CPX
		case 28:
			if( amode == 0 ) {
				instruction = CPY_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = CPY_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // CPY
			// {"TAX"}, {"TAY"}, {"TXA"}, {"TYA"}
		case 29:
			instruction = TAX;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // TAX
		case 30:
			instruction = TAY;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // TAY
		case 31:
			instruction = TXA;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // TXA
		case 32:
			instruction = TYA;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // TYA	
		case 33:
			instruction = INX;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // INX	
		case 34:
			instruction = INY;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // INY
		case 35:
			instruction = DEX;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // DEX
		case 36:
			instruction = DEY;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // DEY
		case 37:
			instruction = NEG;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // NEG
		case 38:
			instruction = DUP;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // DUP
		case 39:
			instruction = SWAP;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // SWAP
		case 40: 
			if( amode == 0 ) {
				instruction = AND_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = AND_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // AND
		case 41: 
			if( amode == 0 ) {
				instruction = OR_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = OR_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // OR
		case 42: 
			if( amode == 0 ) {
				instruction = XOR_ABS;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				buf[0] = (unsigned char) (operand >> 8);
				buf[1] = (unsigned char) (operand & 0xff);
				fwrite(&buf, sizeof(unsigned char), 2, ofp);
			} else {
				instruction = XOR_IMM;
				_operand();
				fwrite(&instruction, sizeof(instruction), 1, ofp);
				fwrite(&operand, sizeof(unsigned char), 1, ofp);
			}
			break; // XOR
		case 43:
			instruction = NOT;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // NOT
		case 44:
			instruction = CLC;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // CLC
		case 45:
			instruction = SEC;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // SEC
		case 46:
			instruction = JCS;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // JCS
		case 47:
			instruction = JCC;
			fwrite(&instruction, sizeof(instruction), 1, ofp);
			break; // JCC
		default:
			printf("Internal error. Unhandled instruction index: %d.", mindex);
			exit(1);
			break;
	}
	skipWS();
	comment();	
}	


void line() {
	skipWS();
	if( look == ';' ) {
		comment();
	} else if(is_alpha(look) ) {
		code();
	}
}

void org() {
	char buf[2];
	
	mnemonic();
	if( mindex != 12 ) {
		printf("Syntax error. Missing ORG directive at start of code. The ORG directive must appear as the first line in your assembly soure code file.\n");
		exit(1);
	}
	_operand();
	if( operand <= 0xff ) {
		printf("Warning: Program originates in an area of memory used by the system. Addresses $0x00 to $0xFF are reserved for system use.\n");
	}
	fwrite(&operand, sizeof(unsigned short), 1, ofp);
}

void assemble() {
	skipWS();
	org();
	skipWS();
	comment();
	skipWS();
	
	while( look != EOF ) {
		if( look == '\n' || look == '\r') {
			newline();
			continue;
		}
		line();		
	}	
}	

int main(int argc, char* argv[]) {
	if( argc != 3 ) {
		printf("%s\n", ASM_VERSION);
		printf("\n\tusage: nanoasm <source file> <out file>  e.g., nanoasm hello.asm hello.bin");
		exit(1);
	}
		
	fp = fopen(argv[1], "r");
	if( ! fp) {
		printf("Error: Can't open file %s for reading.\n", argv[1]);
		exit(1);
	}
	
	ofp = fopen(argv[2], "wb");
	if( ! ofp) {
		printf("Error: Can't open file %s for writing.\n", argv[2]);
		exit(1);
	}
	
	line_no = 1;
	la();
	
	// Write magic number
	unsigned short magic = 0xd00d;
	fwrite(&magic,sizeof(magic),1,ofp); 

	assemble();
	fclose(fp);
	fclose(ofp);
	
	return 0;
}		