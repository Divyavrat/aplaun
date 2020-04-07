#!/bin/sh

# This script assembles the TachyonOS bootloader, kernel and programs
# with NASM, and then creates floppy and CD images (on Linux)

# Only the root user can mount the floppy disk image as a virtual
# drive (loopback mounting), in order to copy across the files

# (If you need to blank the floppy image: 'mkdosfs disk_images/TachyonOS.flp')


if test "`whoami`" != "root" ; then
	echo "You must be logged in as root to build (for loopback mounting)"
	echo "Enter 'su' or 'sudo bash' to switch to root"
	exit
fi


if [ ! -e disk_images/tachyonos.flp ]
then
	echo ">>> Creating new TachyonOS floppy image..."
	mkdosfs -C disk_images/tachyonos.flp 1440 || exit
fi


echo ">>> Assembling bootloader..."

nasm -O0 -w+orphan-labels -f bin -o source/bootload/bootload.bin source/bootload/bootload.asm || exit


echo ">>> Assembling TachyonOS kernel..."

cd source
nasm -O0 -w+orphan-labels -f bin -o kernel.bin kernel.asm || exit
nasm -O0 -w+orphan-labels -f bin -o zkernel.sys zkernel.asm || exit
cd ..


echo ">>> Assembling programs..."

cd programs

for i in *.asm
do
	nasm -O0 -w+orphan-labels -f bin $i -o `basename $i .asm`.bin || exit
done

cd ..


echo ">>> Adding bootloader to floppy image..."

dd status=noxfer conv=notrunc if=source/bootload/bootload.bin of=disk_images/tachyonos.flp || exit


echo ">>> Copying TachyonOS kernel and programs..."

rm -rf tmp-loop

mkdir tmp-loop && mount -o loop -t vfat disk_images/tachyonos.flp tmp-loop && cp source/kernel.bin tmp-loop/ && cp source/zkernel.sys tmp-loop/

cp programs/*.bin programs/*.bas diskfiles/*.* tmp-loop

sleep 0.2

echo ">>> Unmounting loopback floppy..."

umount tmp-loop || exit

rm -rf tmp-loop

echo ">>> Creating CD-ROM ISO image..."

rm -f disk_images/tachyonos.iso
mkisofs -quiet -V 'TACHYON' -input-charset iso8859-1 -o disk_images/tachyonos.iso -b tachyonos.flp disk_images/ || exit

echo '>>> Done!'

