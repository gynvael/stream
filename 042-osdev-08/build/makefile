include ../make/macros.mk

clean:
	$(host.rm) ./floppy.bin

floppy.bin:
	python ./build.py


QEMU_FLAGS = \
	-m 2048 \
	-cpu Haswell \
	-D qemu.log \
	-d cpu_reset,int,guest_errors \
	-vga std \
	-smp 4

QEMU_FLAGS_DEBUG = \
	-serial file:serial.log

run-qemu: floppy.bin
	qemu-system-x86_64 --fda $(<) $(QEMU_FLAGS) $(QEMU_FLAGS_DEBUG)
	cat serial.log

run-bochs: floppy.bin

.PHONY: run-qemu run-bochs
build: run-qemu