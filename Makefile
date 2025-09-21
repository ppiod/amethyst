TARGET = amethyst
ASSEMBLY_TARGET = output
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g
AS = nasm
ASFLAGS = -f elf64
LD = ld

SRCDIR = src
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all clean run asm run-asm

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET)

run-asm: asm
	./$(ASSEMBLY_TARGET)

asm: all
	./$(TARGET)
	$(AS) $(ASFLAGS) -o $(ASSEMBLY_TARGET).o $(ASSEMBLY_TARGET).asm
	$(LD) -o $(ASSEMBLY_TARGET) $(ASSEMBLY_TARGET).o

clean:
	rm -rf $(OBJDIR) $(TARGET) $(ASSEMBLY_TARGET) *.o *.asm