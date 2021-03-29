# CPU JSON SPEC

## Microcode Execution

Microcode execution is completed in order from top to bottom of given instructions.

There are two source fields that can be filled in execution of microcode. These are filled (typically) by SRC_... instructions in order A then B.

There is one destination field typically filled by a FN_... operation, which can be directed to an output with DST_... ops. This value is trashed if not used.

## Example Snippet

This is an example snippet from [8086.json](bin/8086.json) of a single instruction in microcode

The name of the instruction set is defined at the top, with the mandatory field of `icarus` present and set to true as confirmation that this file is for Icarus86. The version is the the version of the instruction set. All instructions are placed in the `icode` array. An explanation of the fields of an icode element is given below.

```
{
	"name": "InstructionSet_8086",
	"icarus": true,
	"version": {
		"major": 0,
		"minor": 1
	},
	"icode": [
		{
			"mnemonic": "ADD r/m16, r/16",
			"hexcode": "01",
			"isPrefix": false,
			"hasModRM": true,
			"immediateBytes": 0,
			"displacementBytes": 0,
			"clockCost": 1,
			"childCodes": [],
			"microcode": [
				"RMB_16",
				"SRC_MODRM",
				"SRC_REGOP",
				"FN_ADD",
				"DST_MODRM"
			]
		},
        ...
    ]
}
```

## icode Elements

The `icode` array in the json is where all instructions are encoded.

JSON Element | Type | Purpose | Example Use
 ----------- | ---- | ------- | ---------- 
mnemonic | string | Human-readable mnemonic, typically similar to Assembly to assist understanding | `"mnemonic": "JC"`
hexcode | string | The hexadecimal value of the instruction encoded as a string | `"hexcode": "FE"`
isPrefix | boolean | If true, specifies a prefix instruction. This enables use of the `childCodes` field | `"isPrefix": false`
hasModRM | boolean | If true denotes an instruction that has an appended ModRM Byte | `"hasModRM": true`
immediateBytes | unsigned int | The number of immediate bytes that come after the instruction and any ModRM byte | `"immediateBytes": 2`
displacementBytes | unsigned int | The number of displacement bytes that come at the final end of the instruction data | `"displacementBytes": 0`
clockCost | unsigned int | The number of clock cycles the instruction should take. This can be used to make the emulator complete instructions in semi-realistic time | `"clockCost": 1`
childCodes | array | Another array where new icode elements can be placed for use with prefix instructions to form a "tree" structure | N/A
microcode | array | A list of strings corresponding to microcode instructions the CPU executes in order to form the complete instruction | See above section

## Microcode instructions

All microcode instructions are encoded as string literals (converted internally by Icarus86 to a more efficient format) in the microcode arrays of icode elements. **NOTE: Case Sensitive**

Microcode instruction | Purpose | Notes
 -------------------- | ------- | ----- 
UDEF | None | Undefined microcode instruction. No purpose
NOP | No operation | A valid no op instruction for NOPs
HLT | Halt the CPU | Puts the CPU in a halt state
RMB_8 | Interpret R/M values as 8 bit length | Affects REGOP and MOD of ModRM byte
RMB_16 | Interpret R/M values as 16 bit length | Affects REGOP and MOD of ModRM byte
SE_SRC_B | Sign extend SRC_B field | SRC_B field is sign extended to match length of SRC_A field (in bits, see RMB_8/16 to specify SRC size)
SEG_OVERRIDE_ES | Set segment override to ES | ***TODO***
SRC_REGOP | Interpret REGOP field as pointer to register source | See documentation on Intel 8086 for how REGOP encodes registers
SRC_MODRM | Interpret MODRM field as pointer to register or memory source | See documentation on Intel 8086 for how MODRM encodes registers or memory locations
SRC_IMM | Load a(n) immediate byte(s) | Loads immediate value
SRC_DISP | Load a(n) displacement byte(s) | Loads displacement value
SRC_STACK_POP | Top stack value used as source | Pops value off top of stack as a source. Not placed in register
SRC_R_XX | Loads a register as a source | A, B, C, D (X, L, H) and BP, DI, DS, CS, ES, SS registers
DST_REGOP | Outputs to register indicated by REGOP | See Intel 8086 documentation for how REGOP encodes registers
DST_REGOP_SREG | Outputs to segment register indicated by REGOP | See Intel 8086 documentation for how REGOP encodes registers
DST_MODRM | Outputs to memory/register indicated by MODRM | See Intel 8086 documentation for how MODRM encodes registers
DST_STACK_PUSH | Pushes the output to the stack | None
DST_R_XX | Outputs to register | A, B, C, D (X, L, H) and BP, DI, DS, CS, ES, SS registers

TODO: FN_X
