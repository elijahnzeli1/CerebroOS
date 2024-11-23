CC = gcc
CFLAGS = -I../os -I../apps -IC:/SDL2/include
LDFLAGS = -LC:/SDL2/lib -lSDL2main -lSDL2

EMULATOR_SRCS = emulator/emulator.c
TEST_SRCS = test_apps/clock_test.c

all: test_clock

test_clock: $(EMULATOR_SRCS) $(TEST_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f test_clock.exe
