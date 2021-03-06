# Icarus86

Icarus86 is an Intel x86 emulator for the 8086 processor.

### Requirements

Requires 64-bit OS and the [Microsoft Visual C++ Redistributable](https://aka.ms/vs/16/release/vc_redist.x64.exe) [(VS 2015,17,19)](https://support.microsoft.com/en-gb/help/2977003/the-latest-supported-visual-c-downloads)

### Capability
 - 8086 emulation. Meets the requirements of the [codegolf](https://codegolf.stackexchange.com/questions/4732/emulate-an-intel-8086-cpu) task
 
### Implementation
 - Uses "microcode" implementation: CPU core executes microcodes specified in external JSON file to reduce code duplication
 
### Todo

 - [x] 8086 codegolf emulation
 - [ ] 8086 full emulation
 - [ ] BIOS emulation
 - [ ] Simple Floppy Disk Controller
 - [ ] 80186 emulation

 
### NASM compiling for x86-16 arch
 
```nasm -f bin -o output.bin .\input.asm```