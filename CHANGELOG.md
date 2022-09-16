# CHANGE LOG

**Version 0.1:** 
- Initial release.

**Version: 0.2**
- Changed internal representation to bytecode, rather than 16 bit unsigned shorts.
- Added new instruction BNE.
- Division now traps division by zero error.
- Increased memory to 256 bytes
- Increased address space to 16 bits, giving 65536 memory locations. Re-write addresses in your assembly to 16 bit!

**Version 0.3**
- Added ORG, JSR and RTS instructions. ORG is mandatory in assembly language source files now.
- Program image files now contain a load address at the start of the file which controls where the program gets loaded into memory.
- Increased memory to 512 bytes
- Added stack. Stack is 128 bytes in size and grows downwards in memory.

**Version 0.4**
- Added INC, DEC, CMP, SHL, SHR, PUSHA and POPA instructions and Z flag
- Renamed the Bxx branch instructions to Jxx for "jump"

**Version 0.5**
- Added NOP, CPX, CPY, LDX, LDY, STX, STY, TAX, TAY, TXA, TYA, INX, INY, DEX and DEY instructions

**Version 0.5.1**
- Added NEG, SWAP, DUP, AND, OR, XOR and NOT instructions.

**Version 0.5.2**
- Added JCS, JCC, CLC and SEC instructions. Both add and subtract are done with the carry flag now.
