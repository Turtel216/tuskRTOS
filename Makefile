#
# Makefile for building an ARM Cortex-M4 project and running it in QEMU.
#

# --- Toolchain Definition ---
PREFIX    = arm-none-eabi-
CC        = $(PREFIX)gcc
AS        = $(PREFIX)as
LD        = $(PREFIX)gcc
OBJCOPY   = $(PREFIX)objcopy
GDB       = $(PREFIX)gdb

# --- Project Files ---
C_SOURCES = src/main.c src/tusk.c src/uart.c src/m_queue.c src/mem.c
ASM_SOURCES = src/rtos_asm.s src/startup.s

OBJECTS   = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)
TARGET_ELF= rtos_project.elf
TARGET_BIN= rtos_project.bin

# --- Build Flags ---
CPU_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
SPECS = --specs=nosys.specs
CFLAGS    = $(CPU_FLAGS) -g -O0 -Wall -Iinclude $(SPECS)
LDFLAGS   = $(CPU_FLAGS) -nostdlib -Tqemu.ld -Wl,-Map=$(TARGET_ELF:.elf=.map) $(SPECS)

# --- QEMU Settings ---
QEMU_MACHINE  = netduinoplus2
QEMU          = qemu-system-arm
QEMU_FLAGS    = -M $(QEMU_MACHINE) -kernel $(TARGET_ELF) -nographic -serial mon:stdio

# --- Build Rules ---
all: $(TARGET_BIN)

$(TARGET_ELF): $(OBJECTS)
	@echo "[LD] Linking to create $(TARGET_ELF)"
	$(LD) $(LDFLAGS) -o $@ $^

$(TARGET_BIN): $(TARGET_ELF)
	@echo "[OBJCOPY] Creating $(TARGET_BIN)"
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	@echo "[CC] Compiling $<"
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	@echo "[AS] Assembling $<"
	$(CC) $(CFLAGS) -c -o $@ $<

# --- QEMU & Debugging Rules ---
run: all
	@echo "[QEMU] Starting emulation. Press Ctrl+A, then X to exit."
	$(QEMU) $(QEMU_FLAGS)

# In 'debug' mode, we use a separate port for GDB and leave stdio for the monitor.
debug: all
	@echo "[QEMU-GDB] Starting GDB server on tcp::1234. Waiting for connection..."
	$(QEMU) -M $(QEMU_MACHINE) -kernel $(TARGET_ELF) -nographic -S -gdb tcp::1234

gdb:
	@echo "[GDB] Connecting to QEMU GDB server..."
	$(GDB) -ex "target remote localhost:1234" $(TARGET_ELF)

# --- Housekeeping ---
clean:
	@echo "[CLEAN] Removing build artifacts."
	rm -f src/*.o $(TARGET_ELF) $(TARGET_BIN) *.map

.PHONY: all run debug gdb clean
