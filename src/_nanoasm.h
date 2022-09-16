#ifndef nanoasm
#define nanoasm

FILE *fp;		// Assembly language source file
FILE *ofp;		// Assembled output file
int look;		// Lookahead character
int line_no;	// Track line numbers
int buf[4];		// Mnemonic buffer

char tokens[][5] = {
{"LDA"}, {"STA"}, {"ADD"}, {"SUB"}, {"MUL"}, 
{"DIV"}, {"BRA"}, {"BEQ"}, {"HALT"}, {"IN"}, {"OUT"}
};


unsigned char instruction;		// The assembled instruction
unsigned char amode;			// Addressing mode 0 = absolute, 1 = immmediate
unsigned short operand;			// Instruction's operand


#endif

