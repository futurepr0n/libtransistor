include libtransistor.mk

# for building newlib and sdl
export LD
export CC
export CXX
export AS
export AR
export LD_FOR_TARGET = $(LD)
export CC_FOR_TARGET = $(CC)
export AS_FOR_TARGET = $(AS) -arch=aarch64 -mattr=+neon
export AR_FOR_TARGET = $(AR)
export RANLIB_FOR_TARGET = $(RANLIB)
export CFLAGS_FOR_TARGET = $(CC_FLAGS) -Wno-unused-command-line-argument -Wno-error-implicit-function-declaration

.SUFFIXES: # disable built-in rules
.SECONDARY: # don't delete intermediate files

.PHONY: all

default:

include mk/lib.mk
include mk/tests.mk
include mk/pthread.mk
include mk/newlib.mk
include mk/compiler-rt.mk
include mk/sdl2.mk
include mk/liblzma.mk

clean: clean_lib clean_test
	rm -rf $(LIBTRANSISTOR_HOME)/docs

distclean: clean clean_newlib clean_compiler-rt clean_pthread clean_sdl2

fs.squashfs.o: fs.squashfs
    mkdir -p $(@D)
    $(LD) -s -r -b binary -m aarch64elf -T $(LIBTRANSISTOR_HOME)/fs.T -o $@ $<
