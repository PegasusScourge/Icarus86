# Devices JSON Spec

**All devices inherit from type GENERIC**

**NOTE**: JSON config variables and Range Names are case sensitive

### Range declarations

TODO

## Device: GENERIC

### JSON Configs

Variable | Type | Purpose | Example
 ------- | ---- | ------- | ------- 
human | string | Human-readable string name | `"human": "test123"`
serial | string | Hexadecimal number-as-string for a serial number | `"serial": "FE55092"`
type | string | The type of device. See headers for devices (e.g. Device **GENERIC**) | `"type": "GENERIC"`
memoryspace | array | List of memory ranges | See range declarations
iospace | array | List of io ranges | See range declarations

## Device: RAM_CHIP

### JSON Configs

Variable | Type | Purpose | Example
 ------- | ---- | ------- | ------- 
size | unsigned int | The size of the memory chip, in bytes (typically a power of 2) | `"size": 1024`

### Range Names: Memory Space

Name | Purpose | Notes
 --- | ------- | ----- 
RAM | RAM content | This is the R/W random access memory area

#### Range Names: IO Space

None

## Device: ROM_CHIP

### JSON Configs

Variable | Type | Purpose | Example
 ------- | ---- | ------- | ------- 
size | unsigned int | The size of the memory chip, in bytes (typically a power of 2) | `"size": 1024`
content | string | File name containing the binary data to load into this chip on start up. Must be the same size as `size` parameter | `"content": "bios.bin"`

### Range Names: Memory Space

Name | Purpose | Notes
 --- | ------- | ----- 
ROM | ROM content | This is the read only random access memory area

### Range Names: IO Space

None