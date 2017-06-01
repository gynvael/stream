
project.name		= kernel64
project.type		= executable
project.kind		= target
project.path		= kernel
project.component.major	= boot
project.component.minor	= kernel
project.emit-metadata	= true
project.configuration	= debug

project.sources		= \
	$(wildcard source/*.S) \
	$(wildcard source/*.c)

project.flags.cc	= -ffreestanding -nostdlib -nostartfiles -masm=intel 
project.flags.as	= -ffreestanding -nostdlib -nostartfiles -masm=intel 
project.flags.cxx	= -ffreestanding -nostdlib -nostartfiles -masm=intel 

project.flags.link = \
	-ffreestanding \
	-nostdlib \
	-Wl,-z,max-page-size=0x1000 \
	-Wl,-z,common-page-size=0x1000

project.flags.link.x86_64	= -mno-red-zone -mcmodel=kernel
project.flags.cc.x86_64		= -mno-red-zone -mcmodel=kernel
project.flags.cxx.x86_64	= -mno-red-zone -mcmodel=kernel

project.libraries	= gcc

project.includes	= \
	include

include $(BASE_DIRECTORY)/make/build.mk
