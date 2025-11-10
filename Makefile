#
# Makefile for building an ARM Cortex-M4 project and running it in QEMU.
#

# --- Toolchain Definition ---
# Assumes 'arm-none-eabi-' toolchain is in your PATH.
PREFIX    = arm-none-eabi-
CC        = $(PREFIX)gcc
AS        = $(PREFIX)as
LD        = $(PREFIX)gcc
OBJCOPY   = $(PREFIX)objcopy
GDB       = $(PREFIX)gdb

# --- Project Files ---
# Add your C source files here.
C_SOURCES = src/main.c src/rtos.c
# Add your assembly source files here.
ASM_SOURCES = src/rtos_asm.s

OBJECTS   = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)
TARGET_ELF= rtos_project.elf
TARGET_BIN= rtos_project.bin

# --- Build Flags ---
# CPU flags for Cortex-M4. Adjust FPU usage if needed.
# 'netduinoplus2' is a QEMU machine with an STM32F4 (Cortex-M4).
CPU_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft

# Compiler flags:
# -g: Generate debug information.
# -O0: No optimization, makes debugging easier.
# -Wall: Turn on all warnings.
# -I.: Include the current directory for header files.
CFLAGS    = $(CPU_FLAGS) -g -O0 -Wall -I.

# Linker flags:
# -nostdlib: We are building a bare-metal application without a standard library.
# -Tqemu.ld: Use our custom linker script for memory layout.
# -Wl,-Map: Generate a map file for inspecting memory layout.
LDFLAGS   = $(CPU_FLAGS) -nostdlib -Tqemu.ld -Wl,-Map=$(TARGET_ELF:.elf=.map)

# --- QEMU Settings ---
QEMU_MACHINE  = netduinoplus2
QEMU          = qemu-system-arm
QEMU_FLAGS    = -M $(QEMU_MACHINE) -kernel $(TARGET_ELF) -nographic -serial stdio

# --- Build Rules ---

# The default target when you just run 'make'.
all: $(TARGET_BIN)

# Rule to link the final ELF file.
$(TARGET_ELF): $(OBJECTS)
	@echo "[LD] Linking to create $(TARGET_ELF)"
	$(LD) $(LDFLAGS) -o $@ $^

# Rule to create a flat binary file from the ELF (for flashing to hardware).
$(TARGET_BIN): $(TARGET_ELF)
	@echo "[OBJCOPY] Creating $(TARGET_BIN)"
	$(OBJCOPY) -O binary $< $@

# Rule to compile a C source file into an object file.
%.o: %.c
	@echo "[CC] Compiling $<"
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to assemble an assembly source file into an object file.
%.o: %.s
	@echo "[AS] Assembling $<"
	$(CC) $(CFLAGS) -c -o $@ $<


# --- QEMU & Debugging Rules ---

# 'make run': Builds the project and runs it in QEMU.
# Press Ctrl+A then X to exit QEMU.
run: all
	@echo "[QEMU] Starting emulation. Press Ctrl+A, then X to exit."
	$(QEMU) $(QEMU_FLAGS)

# 'make debug': Builds and starts QEMU in a GDB server mode, paused.
# You can then connect a GDB client to it.
debug: all
	@echo "[QEMU-GDB] Starting GDB server on tcp::1234. Waiting for connection..."
	$(QEMU) $(QEMU_FLAGS) -S -gdb tcp::1234

# 'make gdb': A convenience target to launch GDB and connect to the QEMU session.
# Run 'make debug' in one terminal, then 'make gdb' in another.
gdb:
	@echo "[GDB] Connecting to QEMU GDB server..."
	$(GDB) -ex "target remote localhost:1234" $(TARGET_ELF)

# --- Housekeeping ---
clean:
	@echo "[CLEAN] Removing build artifacts."
	rm -f $(OBJECTS) $(TARGET_ELF) $(TARGET_BIN) *.map

# Phony targets are commands that don't represent actual files.
.PHONY: all run debug gdb clean
