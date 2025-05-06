include Makefile.inc

KERNEL=kernel.bin

# Inicialmente, incluimos todos los archivos .c excepto los de memory/
# Los archivos de memory/ los añadiremos condicionalmente después
SOURCES=$(filter-out memory/bitMap.c memory/buddy.c, $(wildcard *.c drivers/*.c interruptions/*.c lib/*.c memory/*.c pipes/*.c processes/*.c scheduler/*.c semaphores/*.c syscalls/*.c ))
SOURCES_ASM=$(wildcard asm/*.asm)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

TEST_CFLAGS =

ifeq ($(TEST), mm)
    TEST_CFLAGS += -DTEST_MM

endif


MM_TYPE ?= buddy

ifeq ($(MM_TYPE), bitmap)
    SOURCES += memory/bitMap.c
    TEST_CFLAGS += -DMM_BITMAP
    $(info INFO: Usando Administrador de Memoria: BitMap) 
else ifeq ($(MM_TYPE), buddy)
    SOURCES += memory/buddy.c
    TEST_CFLAGS += -DMM_BUDDY
    $(info INFO: Usando Administrador de Memoria: Buddy)
else
    $(error ERROR: Tipo de Administrador de Memoria invalido especificado: $(MM_TYPE). Debe ser 'bitmap' o 'buddy'.)
endif
# --- Fin Selección del Administrador de Memoria ---


LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL):	$(LOADEROBJECT)	$(OBJECTS)	$(STATICLIBS)	$(OBJECTS_ASM)
	$(LD)	$(LDFLAGS) -T kernel.ld -o	$(KERNEL)	$(LOADEROBJECT)	$(OBJECTS)	$(OBJECTS_ASM)	$(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) $(TEST_CFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
    # Limpia los archivos .o en el directorio actual y en los subdirectorios
	rm -rf asm/*.o *.o *.bin memory/*.o drivers/*.o interruptions/*.o lib/*.o pipes/*.o processes/*.o scheduler/*.o semaphores/*.o syscalls/*.o

.PHONY: all clean
