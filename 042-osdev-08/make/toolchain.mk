#---------------------------------------------------------------------------------------------------
#
#	By Karol Grzybowski, 2017
#
#---------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------------
#
# Toolchain cross-compiler path.
#
#---------------------------------------------------------------------------------------------------

toolchain.target.path		:= $(BASE_DIRECTORY)/build


#---------------------------------------------------------------------------------------------------
#
# Native toolchain.
#
#---------------------------------------------------------------------------------------------------

toolchain.native.ar		:= ar
toolchain.native.as		:= gcc
toolchain.native.cc		:= gcc
toolchain.native.cxx		:= g++
toolchain.native.link		:= g++
toolchain.native.nm		:= nm
toolchain.native.objcopy	:= objcopy
toolchain.native.objdump	:= objdump
toolchain.native.ranlib		:= ranlib
toolchain.native.readelf	:= readelf
toolchain.native.strings	:= strings
toolchain.native.strip		:= strip

toolchain.native.cflags		:=
toolchain.native.cxxflags	:=
toolchain.native.defines	:=


#---------------------------------------------------------------------------------------------------
#
# Select proper target toolchain.
#
#---------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------------
#
# Crosscompile x86_64
#
#---------------------------------------------------------------------------------------------------

toolchain.target.prefix		:= 
toolchain.target.flags.as	:= -Wa,--divide
toolchain.target.flags.cc	:= -m64 -mtune=corei7
toolchain.target.flags.cxx	:= -m64 -mtune=corei7
toolchain.target.defines	:= 

#---------------------------------------------------------------------------------------------------
#
# Apply target toolchain.
#
#---------------------------------------------------------------------------------------------------

toolchain.target.ar		:= $(toolchain.target.prefix)ar
toolchain.target.as		:= $(toolchain.target.prefix)gcc
toolchain.target.cc		:= $(toolchain.target.prefix)gcc
toolchain.target.cxx		:= $(toolchain.target.prefix)g++
toolchain.target.link		:= $(toolchain.target.prefix)g++
toolchain.target.nm		:= $(toolchain.target.prefix)nm
toolchain.target.objcopy	:= $(toolchain.target.prefix)objcopy
toolchain.target.objdump	:= $(toolchain.target.prefix)objdump
toolchain.target.ranlib		:= $(toolchain.target.prefix)ranlib
toolchain.target.readelf	:= $(toolchain.target.prefix)readelf
toolchain.target.strings	:= $(toolchain.target.prefix)strings
toolchain.target.strip		:= $(toolchain.target.prefix)strip

#---------------------------------------------------------------------------------------------------
#
# Configuration specific flags.
#
#---------------------------------------------------------------------------------------------------

toolchain.flags.cc.debug	:= -D_DEBUG -O0 -ggdb
toolchain.flags.cc.checked	:= -D_DEBUG -O3
toolchain.flags.cc.release	:= -DNDEBUG -O3

toolchain.flags.cxx.debug	:= -D_DEBUG -O0 -ggdb
toolchain.flags.cxx.checked	:= -D_DEBUG -O3
toolchain.flags.cxx.release	:= -DNDEBUG -O3

toolchain.flags.as.debug	:=
toolchain.flags.as.checked	:=
toolchain.flags.as.release	:=

toolchain.flags.link.debug	:=
toolchain.flags.link.checked	:=
toolchain.flags.link.release	:=

#---------------------------------------------------------------------------------------------------
#
# Global toolchain flags.
#
#---------------------------------------------------------------------------------------------------

toolchain.flags.cc := \
	-std=c11 \
	-Wall \
	-Wextra \
	-Werror=implicit-function-declaration \
	-Wdouble-promotion \
	-Wbad-function-cast \
	-Wundef \
	-Wconversion \
	-Wwrite-strings \
	-Wno-conversion \
	-fstrict-aliasing \
	-Wstrict-aliasing=1 \
	-Werror=strict-aliasing \
	-Werror=address \
	-Werror \
	-Wmissing-format-attribute \
	-fdiagnostics-color \
	-fmax-errors=20 \
	-Wvla \
	-flto-odr-type-merging \
	-Wparentheses \
	-Wmissing-braces \
	-Wsequence-point \
	-Wlogical-op \
	-Wmissing-field-initializers \
	-Wundef \
	-Wfloat-equal \
	-Wshadow \
	-Wswitch \
	-Wunused \
	-Wuninitialized \
	-Wstrict-overflow=2 \
	-Wpointer-arith \
	-Wcast-align -Wformat=2 \
	-Winit-self \
	-Wunreachable-code \
	-fstack-protector-strong \
	-fvisibility=hidden \
	-Wno-format-zero-length

#	
#	-Wno-implicit-fallthrough \
#	-Wno-bool-operation \

toolchain.flags.cxx := \
	-std=c++14 \
	-fmax-errors=20 \
	-fdiagnostics-color \
	-Wall \
	-Wextra \
	-Werror \
	-Werror=strict-aliasing \
	-Werror=address \
	-Werror=implicit-function-declaration \
	-Wdouble-promotion \
	-Wundef \
	-Wconversion \
	-Wwrite-strings \
	-Wstrict-aliasing=2 \
	-Wmissing-format-attribute \
	-Wfloat-equal \
	-Wshadow \
	-Wvla \
	-Wdisabled-optimization \
	-Wparentheses \
	-Wmissing-braces \
	-Wsequence-point \
	-Wswitch \
	-Wunused \
	-Wuninitialized \
	-Wstrict-overflow=2 \
	-Wpointer-arith \
	-Wlogical-op \
	-Wmissing-field-initializers \
	-Wcast-align \
	-Wcast-qual \
	-Wformat=2 \
	-Winit-self \
	-Winline \
	-Wunreachable-code \
	-flto-odr-type-merging \
	-fstack-protector-strong \
	-fvisibility=hidden \
	-Wno-format-zero-length

#---------------------------------------------------------------------------------------------------
#
# Predefined system includes
#
#---------------------------------------------------------------------------------------------------
