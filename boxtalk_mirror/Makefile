TARGET ?= host
ifeq ($(TARGET),toon2)
  CC := /tmp/qt_rebuild/linaro/bin/arm-linux-gnueabihf-gcc
  SR := --sysroot=/tmp/qt_rebuild/linaro/arm-linux-gnueabihf/libc
  AR :=
  OUT := boxtalk_mirror
else ifeq ($(TARGET),toon1)
  CC := /tmp/qt_rebuild/toon1-toolchain/bin/arm-linux-gcc
  SR := --sysroot=/tmp/qt_rebuild/toon1-toolchain/arm-buildroot-linux-gnueabi/sysroot
  AR := -march=armv5te -marm -mfloat-abi=soft
  OUT := boxtalk_mirror-toon1
else
  CC := gcc
  SR :=
  AR :=
  OUT := boxtalk_mirror-host
endif
CFLAGS := $(SR) $(AR) -O2 -Wall -Wextra
$(OUT): boxtalk_mirror.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f boxtalk_mirror boxtalk_mirror-toon1 boxtalk_mirror-host
