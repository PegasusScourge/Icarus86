# Icarus86

Icarus86 is an Intel x86 emulator for the 8086 processor.

### Requirements

#### Libraries
 - SFML 2.5.1

### Capability
 - 8086 emulation. Meets the requirements of the [codegolf](https://codegolf.stackexchange.com/questions/4732/emulate-an-intel-8086-cpu) task
 
### Implementation
 - Uses "[microcode](CPU_JSON.MD)" implementation: CPU core executes microcodes  in external JSON file to reduce code duplication
 - Devices configured with [JSON](DEVICES_JSON.MD) and loaded at run time

### Required files

Icarus86 draws on configuration files to perform.

File | Purpose | Specification
 --- | ------- | ------------- 
Instruction set | List of instructions with microcode steps | [Spec](CPU_JSON.MD) [8086.json example](bin/8086.json)
Device configuration | List of devices and their contexts/configs | [Spec](DEVICES_JSON.MD) [devices.json exmaple](bin/devices.json)
 
### Todo

 - [ ] 8086 codegolf emulation
 - [ ] 8086 full emulation
 - [ ] BIOS emulation
 - [ ] Simple Floppy Disk Controller
 - [ ] 80186 emulation

 
### NASM compiling for x86-16 arch

Use
```BITS 16``` to target 16-bit
 
```nasm -f bin -o output.bin .\input.asm```