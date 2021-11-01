CFLAGS += $(shell yed --print-cflags)
CFLAGS += $(shell yed --print-ldflags)
install:
	gcc $(CFLAGS) rv64asm.c -o rv64asm.so
	cp ./rv64asm.so ~/.config/yed/mpy/plugins/lang/syntax/.
