# CPU JSON SPEC

### Example Snippet

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
		}
    ]
}
```

### icode Elements

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

### Microcode instructions

All microcode instructions are encoded as string literals (converted internally by Icarus86 to a more efficient format) in the microcode arrays of icode elements. **NOTE: Case Sensitive**

Microcode instruction | Purpose | Notes
 -------------------- | ------- | ----- 
UDEF | None | Undefined microcode instruction. No purpose

TODO
