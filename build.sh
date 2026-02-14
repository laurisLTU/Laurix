#!/bin/sh
set -e

echo "[*] Cleaning and preparing build directory..."
rm -rf build
mkdir -p build

CC=i686-linux-gnu-gcc
LD=i686-linux-gnu-ld
CFLAGS="-m32 -ffreestanding -O2 -Wall -Wextra"
LDFLAGS="-m elf_i386 -T linker.ld -nostdlib"

echo "[*] Assembling all .s files with NASM..."
for asm in *.s; do
    echo "    assembling $asm"
    nasm -f elf32 "$asm" -o "build/${asm%.s}.o"
done

echo "[*] Compiling all .c files..."
for src in *.c; do
    echo "    compiling $src"
    $CC $CFLAGS -c "$src" -o "build/${src%.c}.o"
done

echo "[*] Linking kernel..."
$LD $LDFLAGS -o build/kernel.elf build/*.o

echo "[+] Build complete: build/kernel.elf"
