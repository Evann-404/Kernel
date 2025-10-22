# Simple x86 Kernel

A minimal 32-bit x86 operating system kernel with VGA text mode output and keyboard input support. 

## Prerequisites

### Required Tools

1. **Cross-Compiler Toolchain** (i686-elf)
   - `i686-elf-gcc` - C compiler for bare metal x86
   - `i686-elf-as` - Assembler
   - `i686-elf-ld` - Linker

2. **QEMU** - x86 emulator for testing
   - `qemu-system-i386`

3. **Build Tools**
   - `make`
   - `binutils`

### Installing Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install qemu-system-x86

# Fedora
sudo dnf install qemu-system-x86

# Arch Linux
sudo pacman -S qemu-system-x86
yay -S i686-elf-gcc i686-elf-binutils
```

## Building and Running

Build and run in one command:

```bash
make
```

Or build individual steps:
```bash
make link
make run
make clean
make debug
```

## Usage

Once running, type characters with your keyboard.