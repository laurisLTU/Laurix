#!/bin/sh
set -e

echo "[*] Building kernel..."
./build.sh

echo "[*] Preparing ISO directory..."
rm -rf iso
mkdir -p iso/boot/grub

echo "[*] Writing GRUB config..."
cat > iso/boot/grub/grub.cfg <<EOF
set timeout=0
set default=0

menuentry "Laurix OS" {
    multiboot /boot/kernel.elf
    boot
}
EOF

echo "[*] Copying kernel..."
cp build/kernel.elf iso/boot/kernel.elf

echo "[*] Creating ISO..."
grub-mkrescue -o laurix.iso iso

echo "[+] ISO created: laurix.iso"
