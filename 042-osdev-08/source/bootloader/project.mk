include $(BASE_DIRECTORY)/make/macros.mk

build.path.stage1 = $(global.path.build)/bootloader/stage1
build.path.stage2 = $(global.path.build)/bootloader/stage2

$(build.path.stage1): source/stage1.asm
	$(noecho)$(call make-dir,$(@))
	nasm $(<) -o $(@)

$(build.path.stage2): source/stage2.asm
	$(noecho)$(call make-dir,$(@))
	nasm $(<) -o $(@)

clean:
	$(host.rm) $(build.path.stage1)
	$(host.rm) $(build.path.stage2)


build: $(build.path.stage1) $(build.path.stage2)

