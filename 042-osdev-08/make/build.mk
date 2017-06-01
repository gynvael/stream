#---------------------------------------------------------------------------------------------------
#
#	By Karol Grzybowski, 2017
#
#---------------------------------------------------------------------------------------------------
#
#	Parameters:
#
#		$(project.name) : [required]
#
#			Provides a name of project in build system.
#
#		$(project.uuid) : [optional]
#
#			Provides project UUID to identify sub-builds.
#
#		$(project.path) : [required]
#
#			Provides path in build directory to where build target file.
#
#		$(project.configuration) : [optional]
#
#			Provides project specific configuration type.
#
#			Available configuration types are:
#
#			- `debug`
#
#				Specifies a build configuration with optimizations disabled and
#				assertions enabled.
#
#			- `checked`
#
#				Specifies a build configuration with both optimizations and
#				assertions enabled.
#
#			- `release`
#
#				Specifies a build configuration with optimizations enabled and
#				assertions disabled.
#
#
#		$(project.component.major) : [required]
#
#			Provides major component ID.
#
#		$(project.component.minor) : [required]
#
#			Provides minor component ID.
#
#		$(project.type) : [required]
#
#			Provides a project type.
#
#			Available project types are:
#
#			- `executable`
#
#				Specifies an executable file project type.
#
#			- `static`
#
#				Specifies a static library project type.
#
#			- `shared`
#
#				Specifies a shared library project type.
#
#			- `module`
#
#				Specifies a shared kernel module project type.
#
#		$(project.kind) : [required]
#
#			Provides a project kind.
#
#			- `target`
#
#				Specifies that project uses target OS toolchain.
#
#			- `native`
#
#				Specifies that project uses native OS toolchain.
#
#		$(project.sources) : [required]
#
#			Provides a list of source files.
#
#		$(project.defines) : [optional]
#
#			Provides a list of compiler defines.
#
#		$(project.includes) : [optional]
#
#			Provides a list of additional project-specific include directories.
#
#		$(project.libraries.static) : [optional]
#
#			Provides a list of static libraries required to link project.
#
#		$(project.libraries.shared) : [optional]
#
#			Provides a list of shared libraries required to link project.
#
#		$(project.flags.cc)
#		$(project.flags.cc.{machine})
#
#			Provides a list of additional flags for C compiler.
#
#		$(project.flags.cxx)
#		$(project.flags.cxx.{machine})
#
#			Provides a list of additional flags for C++ compiler.
#
#		$(project.flags.as)
#		$(project.flags.as.{machine})
#
#			Provides a list of additional flags for assembler.
#
#		$(project.flags.rc)
#		$(project.flags.rc.{machine})
#
#			Provides a list of additional flags for resource compiler.
#
#		$(project.flags.link)
#
#			Provides a list of additional flags for linker.
#
#---------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------------
#
# Include base toolchain makefiles.
#
#---------------------------------------------------------------------------------------------------
include $(BASE_DIRECTORY)/make/macros.mk
include $(BASE_DIRECTORY)/make/toolchain.mk

#---------------------------------------------------------------------------------------------------
#
# Validate required project parameters.
#
#---------------------------------------------------------------------------------------------------

ifndef project.name
$(error Variable project.name is required)
endif

ifndef project.path
$(error Variable project.path is required)
endif

ifndef project.component.major
$(error Variable project.component.minor is required)
endif

ifndef project.component.minor
$(error Variable project.component.minor is required)
endif

ifndef project.type
$(error Variable project.type is required)
endif

ifndef project.sources
$(error Variable project.sources is required)
endif

ifdef project.filename
$(error Project result filename can't be overriden)
endif

#---------------------------------------------------------------------------------------------------
#
# Apply defaults for optional project parameters.
#
#---------------------------------------------------------------------------------------------------

project.kind		?= target
project.uuid		?= $(project.name)
project.configuration	?= debug

#---------------------------------------------------------------------------------------------------
#
# Acquire C compiler flags.
#
#---------------------------------------------------------------------------------------------------

build.flags.cc		:= $(project.flags.cc)
build.flags.cc		+= $(project.flags.cc.${project.configuration})
build.flags.cc		+= $(toolchain.flags.cc)
build.flags.cc		+= $(toolchain.flags.cc.${project.configuration})
build.flags.cc		+= $(toolchain.${project.kind}.flags.cc)

#---------------------------------------------------------------------------------------------------
#
# Acquire C++ compiler flags.
#
#---------------------------------------------------------------------------------------------------

build.flags.cxx		:= $(project.flags.cxx)
build.flags.cxx		+= $(project.flags.cxx.${project.configuration})
build.flags.cxx		+= $(toolchain.flags.cxx)
build.flags.cxx		+= $(toolchain.flags.cxx.${project.configuration})
build.flags.cxx		+= $(toolchain.${project.kind}.flags.cxx)

#---------------------------------------------------------------------------------------------------
#
# Acquire Assembler flags.
#
#---------------------------------------------------------------------------------------------------

build.flags.as		:= $(project.flags.as)
build.flags.as		+= $(project.flags.as.${project.configuration})
build.flags.as		+= $(toolchain.flags.as)
build.flags.as		+= $(toolchain.flags.as.${project.configuration})
build.flags.as		+= $(toolchain.${project.kind}.flags.as)

#---------------------------------------------------------------------------------------------------
#
# Acquire resource compiler flags.
#
#---------------------------------------------------------------------------------------------------

build.flags.rc		:= $(project.flags.rc)
build.flags.rc		+= $(project.flags.rc.${project.configuration})
build.flags.rc		+= $(toolchain.flags.rc)
build.flags.rc		+= $(toolchain.flags.rc.${project.configuration})

#---------------------------------------------------------------------------------------------------
#
# Acquire link flags.
#
#---------------------------------------------------------------------------------------------------

build.flags.link	:= $(project.flags.link)
build.flags.link	+= $(project.flags.link.${project.configuration})
build.flags.link	+= $(toolchain.flags.link)
build.flags.link	+= $(toolchain.flags.link.${project.configuration})

#---------------------------------------------------------------------------------------------------
#
# Prepare include paths.
#
#---------------------------------------------------------------------------------------------------

build.includes		:= $(project.includes)
build.includes		+= $(toolchain.${project.kind}.includes)

#---------------------------------------------------------------------------------------------------
#
# Prepare compiler defines.
#
#---------------------------------------------------------------------------------------------------

build.defines		:= $(project.defines)
build.defines		+= $(toolchain.${project.kind}.defines)

#---------------------------------------------------------------------------------------------------
#
# Combine build path.
#
#---------------------------------------------------------------------------------------------------

build.path.target	:= $(global.path.build)/$(project.path)
build.path.object	:= obj/$(project.uuid)

#---------------------------------------------------------------------------------------------------
#
# Validate project type and choose type specific flags.
#
#---------------------------------------------------------------------------------------------------
ifeq ($(project.type),executable)

#
# Regular executable.
#

build.filename		:= $(project.name).elf

else ifeq ($(project.type),static)

#
# Static library or kernel module.
#

build.filename		:= lib$(project.name).a
build.flags.link	+= -static

else ifeq ($(project.type),shared)

#
# Shared library.
#

build.filename		:= lib$(project.name).so
build.flags.link	+= -fPIC -shared
build.flags.cc		+= -fPIC
build.flags.as		+= -fPIC
build.flags.cxx		+= -fPIC
build.flags.rc		+= -fPIC

else ifeq ($(project.type),module)

#
# Loadable kernel module.
#

build.filename		:= $(project.name).ko
build.flags.link	+= -r

else

$(error Unknown project type: $(project.type))

endif

#---------------------------------------------------------------------------------------------------
#
# This build system will use feature of compiler on which we specify directly which library we want
# to link instead of passing -l option.
#
# Process list of static link dependencies.
#
#---------------------------------------------------------------------------------------------------

build.libraries.static	:= $(foreach lib,$(project.libraries.static),$(dir $(global.path.build)/$(lib))lib$(notdir $(lib)).a)

#---------------------------------------------------------------------------------------------------
#
# Process list of shared link dependencies
#
#---------------------------------------------------------------------------------------------------

build.libraries.shared	:= $(foreach lib,$(project.libraries.shared),$(dir $(global.path.build)/$(lib))lib$(notdir $(lib)).so)

build.libraries		:= $(addprefix -l,$(project.libraries))

#---------------------------------------------------------------------------------------------------
#
# Prepare compiler list of include paths and defines.
#
#---------------------------------------------------------------------------------------------------

build.includes		:= $(addprefix -I,$(build.includes))
build.defines		:= $(addprefix -D,$(build.defines))

#---------------------------------------------------------------------------------------------------
#
# Clear list of objects.
#
#---------------------------------------------------------------------------------------------------

build.objects		:=

#---------------------------------------------------------------------------------------------------
#
# Convenient macro for making object file path from source path.
#
#---------------------------------------------------------------------------------------------------

build.make-object-path	= $(strip $(subst ../,_/,$(build.path.object)/$(1).o))

#---------------------------------------------------------------------------------------------------
#
#	Description:
#
#		Define rule for building .c files using toolchain C compiler.
#
#
#	Parameters:
#
#		$(1) - Provides a source file path.
#
#		$(2) - Provides a target file path.
#
#---------------------------------------------------------------------------------------------------
define rule.build.cc
build.objects += $(2)
$(2): $(1)
	$(noecho)$(call make-dir,$(2))
	$(noecho)echo -e "CC: $(call echo.success,$(2))"
	$(noecho)$(MAKEDEPEND)
	$(noecho)$(toolchain.${project.kind}.cc) \
		-c $(1) \
		-o $(2) \
		$(build.flags.cc) \
		$(build.defines) \
		$(build.includes) \
		-MMD \
		-MP \
		-MT $(2) \
		-MF $(2:%o=%d)
endef

#---------------------------------------------------------------------------------------------------
#
#	Description:
#
#		Define rule for assembling .S files using toolchain assembler.
#
#
#	Parameters:
#
#		$(1) - Provides a source file path.
#
#		$(2) - Provides a target file path.
#
#---------------------------------------------------------------------------------------------------

define rule.build.as
build.objects += $(2)
$(2): $(1)
	$(noecho)$(call make-dir,$(2))
	$(noecho)echo -e "AS: $(call echo.success,$(2))"
	$(noecho)$(MAKEDEPEND)
	$(noecho)$(toolchain.${project.kind}.as) \
		-c $(1) \
		-o $(2) \
		$(build.flags.as) \
		$(build.defines) \
		$(build.includes) \
		-MMD \
		-MP \
		-MT $(2) \
		-MF $(2:%o=%d)
endef

#---------------------------------------------------------------------------------------------------
#
#	Description:
#
#		Define rule for assembling .S files using toolchain assembler.
#
#
#	Parameters:
#
#		$(1) - Provides a source file path.
#
#		$(2) - Provides a target file path.
#
#---------------------------------------------------------------------------------------------------

define rule.build.asm
build.objects += $(2)
$(2): $(1)
	$(noecho)$(call make-dir,$(2))
	$(noecho)echo -e "AS: $(call echo.success,$(2))"
	$(noecho)$(MAKEDEPEND)
	nasm \
		$(1) \
		-o $(2) \
		-f bin
endef

#---------------------------------------------------------------------------------------------------
#
#	Description:
#
#		Define rule for compiling .cxx files using C++ compiler.
#
#
#	Parameters:
#
#		$(1) - Provides a source file path.
#
#		$(2) - Provides a target file path.
#
#---------------------------------------------------------------------------------------------------

define rule.build.cxx
build.objects += $(2)
$(2): $(1)
	$(noecho)$(call make-dir,$(2))
	$(noecho)echo -e "CXX: $(call echo.success,$(2))"
	$(noecho)$(MAKEDEPEND)
	$(noecho)$(toolchain.${project.kind}.cxx) \
		-c $(1) \
		-o $(2) \
		$(build.flags.cxx) \
		$(build.defines) \
		$(build.includes) \
		-MMD \
		-MP \
		-MT $(2) \
		-MF $(2:%o=%d)
endef

#---------------------------------------------------------------------------------------------------
#
# TODO: Define rule for compiling .rc files.
#
#---------------------------------------------------------------------------------------------------


#---------------------------------------------------------------------------------------------------
#
# Filter source files by type.
#
#---------------------------------------------------------------------------------------------------

build.sources.cc	:= $(filter %.c,$(project.sources))
build.sources.as	:= $(filter %.S,$(project.sources))
build.sources.asm	:= $(filter %.asm,$(project.sources))
build.sources.cxx	:= $(filter %.cxx,$(project.sources))
build.sources.rc	:= $(filter %.rc,$(project.sources))

#---------------------------------------------------------------------------------------------------
#
# Apply build rules.
#
#---------------------------------------------------------------------------------------------------

$(foreach src,$(build.sources.cc),$(eval $(call rule.build.cc,$(src),$(call build.make-object-path,$(src)))))
$(foreach src,$(build.sources.as),$(eval $(call rule.build.as,$(src),$(call build.make-object-path,$(src)))))
$(foreach src,$(build.sources.asm),$(eval $(call rule.build.asm,$(src),$(call build.make-object-path,$(src)))))
$(foreach src,$(build.sources.cxx),$(eval $(call rule.build.cxx,$(src),$(call build.make-object-path,$(src)))))

#---------------------------------------------------------------------------------------------------
#
# Combine build target.
#
#---------------------------------------------------------------------------------------------------

build.target		:= $(build.path.target)/$(build.filename)
build.target.hash	:= $(build.target).hash
build.target.sym	:= $(build.target).sym
build.target.rde	:= $(build.target).rde
build.target.lst	:= $(build.target).lst
build.target.dbg	:= $(build.target).dbg

#---------------------------------------------------------------------------------------------------
#
# Define main target.
#
#---------------------------------------------------------------------------------------------------

#
# Include dependencies for this project.
#
build.dependencies	= $(build.objects:.o=.d)
-include $(build.dependencies)


#
# Use proper linker to build project.
#
ifeq ($(project.type),static)

$(build.target): $(build.objects)
	$(noecho)$(call make-dir,$(@))
	$(noecho)echo -e "LINK: $(call echo.success,$(@))"
	$(noecho)$(toolchain.${project.kind}.ar) rcs $(@) $(^)

else

$(build.target): $(build.objects)
	$(noecho)$(call make-dir,$(@))
	$(noecho)echo -e "LINK: $(call echo.success,$(@))"
	$(noecho)$(toolchain.${project.kind}.link) $(^) -o $(@) $(build.libraries.static) $(build.libraries.shared) $(build.flags.link) $(build.libraries)
	$(noecho)$(toolchain.${project.kind}.objcopy) --only-keep-debug $(@) $(@).dbg
	$(noecho)$(toolchain.${project.kind}.objcopy) --strip-debug $(@)
	$(noecho)$(toolchain.${project.kind}.objcopy) --add-gnu-debuglink=$(@).dbg $(@)

endif

#---------------------------------------------------------------------------------------------------
#
# Define list of optional metadata targets.
#
#---------------------------------------------------------------------------------------------------

build.metadata-targets	:=

ifeq ($(project.emit-metadata),true)

build.metadata-targets += $(build.target.hash)
build.metadata-targets += $(build.target.sym)
build.metadata-targets += $(build.target.rde)
build.metadata-targets += $(build.target.lst)

endif

#---------------------------------------------------------------------------------------------------
#
# Define other optional metadata targets builders.
#
#---------------------------------------------------------------------------------------------------

$(build.target.hash): $(build.target)
	$(noecho)echo -e "SHA512: $(call echo.success,$(@))"
	$(noecho)$(host.sha512sum) $(<) > $(@)

$(build.target.sym): $(build.target)
	$(noecho)echo -e "Symbols: $(call echo.success,$(@))"
	$(noecho)$(toolchain.${project.kind}.nm) -C $(<) | sort > $(@)

$(build.target.rde): $(build.target)
	$(noecho)echo -e "Fileinfo: $(call echo.success,$(@))"
	$(noecho)$(toolchain.${project.kind}.readelf) -aW $(<) > $(@)

$(build.target.lst): $(build.target)
	$(noecho)echo -e "Listing: $(call echo.success,$(@))"
	$(noecho)$(toolchain.${project.kind}.objdump) --insn-width=14 -d $(<) > $(@)

#---------------------------------------------------------------------------------------------------
#
# Cleaning rule definition.
#
#---------------------------------------------------------------------------------------------------

clean:
	$(noecho)echo -e "Removing: $(call echo.success,$(CURDIR)/obj)"
	$(noecho)$(host.rmdir) ./obj
	$(noecho)echo -e "Removing: $(call echo.success,$(build.target))"
	$(noecho)$(host.rm) $(build.target)
	$(noecho)echo -e "Removing: $(call echo.success,$(build.target.dbg))"
	$(noecho)$(host.rm) $(build.target.dbg)
	$(noecho)$(host.rm) $(build.metadata-targets)

#---------------------------------------------------------------------------------------------------
#
# Project configure rule definition.
#
#---------------------------------------------------------------------------------------------------

configure:

#---------------------------------------------------------------------------------------------------
#
# Build rule definition.
#
#---------------------------------------------------------------------------------------------------

build: $(build.target) $(build.metadata-targets)
