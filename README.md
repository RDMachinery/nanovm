# nanovm version 0.5.1
 
A tiny virtual machine with assembler written in C for educational purposes.

Way back when I first encountered the Java Virtual Machine I wondered how it was written
so that I could create my own VM. It was some years later when I learned a little digital
electronics and CPU architecture that I found out how it was done.

**NanoVM** is a tiny virtual CPU with a handful of instructions and a bundled assembler. 
Don't expect to find anything that can compete with the Java Virtual Machine as nanovm was
written to teach myself aspects of CPU architecture not to compete with other virtual machines.


## The Instructions

There aren't many instructions implemented because I want to keep things simple and focus
on architecture, but there are just enough to implement primitive programs and test the CPU.

- **LDA** Load accumulator
- **STA** Store value in accumulator to memory
- **LDX** Load X register
- **STX** Store value in X register to memory
- **LDY** Load Y register
- **STY** Store value in Y register to memory
- **ADD** Add memory, or immediate value to accumulator
- **SUB** Subtract memory, or immediate value from accumulator
- **MUL** Multiply memory, or immediate value with accumulator
- **DIV** Divide accumulator by memory, or immediate value
- **CMP** Compare value against accumulator
- **JMP** Jump to address
- **JEQ** Jump if  accumulator, or Z flag is zero
- **JNE** Jump if accumulator, or Z flag is not equal to zero
- **JCS** Jump if carry set
- **JCC** Jump if carry clear
- **JSR** Jump to subroutine
- **RTS** Return from subroutine
- **PUSHA** Push accumulator onto stack
- **POPA** Pop top of stack into accumulator 
- **SHL** Shift accumulator left
- **SHR** Shift accumulator right
- **INC** Increment accumulator
- **DEC** Decrement accumulator
- **NOP** No operation
- **CPX** Compare X register
- **CPY** Compare Y register
- **LDX** Load X register
- **LDY** Load Y register
- **STX** Store X register
- **STY** Store Y register
- **TAX** Transfer accumulator to X register
- **TAY** Transfer accumulator to Y register
- **TXA** Transfer X register to accumulator
- **TYA** Transfer Y register to accumulator
- **INX** Increment X register
- **INY** Increment Y register
- **DEX** Decrement X register
- **DEY** Decrement Y register
- **DUP** Duplicate top of stack
- **NEG** Negate accumulator.
- **SWAP** Swap items on top of stack
- **AND** AND accumulator with immediate value, or value from memory.
- **OR**  OR accumulator with immediate value, or value from memory.
- **XOR** XOR accumulator with immediate value, or value from memory.
- **NOT** Invert accumulator 
- **CLC** Clear carry flag
- **SEC** Set carry flag
- **HALT** Halt execution
- **IN** Read a number from stdin into accumulator
- **OUT** Print value of accumulator to stdout

## Basic Usage

The VM executes object files containing assembled instructions. A number of examples are
provided. To run the VM do:

```
$ nanovm <object file>
```

 ## The Assembler

Like the VM, the assembler is also very simple. It is a tiny one pass assembler with no
fancy gimmicks (yet) like labels, address computation and the plethora
of other features that assembly programmers seem to think they need.

A short example program should give you an idea of the assembler syntax:

```
ORG $100  ; An ORG directive must appear as the first line in assembly source files
; Prints the number decimal 10 to standard out.
LDA #10   ; Load 10 immediately into the accumulator
OUT       ; Print the contents of the accumulator
HALT      ; Stop the VM
```

You can use hexadecimal and binary numbers as operands by prefixing them with special characters. For
hexadecimal, prefix the number with a $ and for binary, prefix the number with a % character.

There are example programs in the ***examples*** directory which you can read to find
out more about how the assembler works.

To run the assembler on a source code file do:
```
$ nanoasm <source file> <object filename>
E.g,
$ nanoasm helloworld.s helloworld.bin
```
Then, run the assembled object file in the VM with:
```
$ nanovm helloworld.bin
```
