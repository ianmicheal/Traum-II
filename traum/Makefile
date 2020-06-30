#
# Basic KallistiOS skeleton / test program
# Copyright (C)2001-2004 Dan Potter
#   

# Put the filename of the output binary here
TARGET = ../../traum/traum.elf

# List all of your C files here, but change the extension to ".o"
# Include "romdisk.o" if you want a rom disk.
OBJS = ../system/kos.o
OBJS += ../maya/math.o ../maya/mat.o ../maya/vec.o
OBJS += ../engine/gfx.o ../engine/gfx_image.o ../engine/gfx_material.o \
		../engine/gfx_model.o ../engine/gfx_debug.o ../engine/iface.o ../engine/lex.o
OBJS += main.o

CFLAGS += -I../headers -DUSING_KOS -std=c11

# The rm-elf step is to remove the target before building, to force the
# re-creation of the rom disk.
all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS) 

rm-elf:
	-rm -f $(TARGET) 

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) -L$(KOS_BASE)/lib  -lGL -lm $(KOS_LIBS)
		
run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS)
	$(KOS_STRIP) $(TARGET)
