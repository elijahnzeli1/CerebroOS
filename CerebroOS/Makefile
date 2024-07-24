CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

# Directories
KERNEL_DIR = kernel
OS_DIR = os
DRIVERS_DIR = drivers
UTILS_DIR = utils
SECURITY_DIR = security
APPS_DIR = apps
AI_MODELS_DIR = ai_models
NETWORK_DIR = network
UI_DIR = ui
UPDATE_DIR = update

# Source Files (explicitly listed per directory)
KERNEL_SOURCES = $(wildcard $(KERNEL_DIR)/*.c)
OS_SOURCES = $(wildcard $(OS_DIR)/*.c)
DRIVER_SOURCES = $(wildcard $(DRIVERS_DIR)/*.c)
UTILS_SOURCES = $(wildcard $(UTILS_DIR)/*.c)
SECURITY_SOURCES = $(wildcard $(SECURITY_DIR)/*.c)
APPS_SOURCES = $(wildcard $(APPS_DIR)/*/*.c)
AI_MODELS_SOURCES = $(wildcard $(AI_MODELS_DIR)/*/*.c)
NETWORK_SOURCES = $(wildcard $(NETWORK_DIR)/*.c)
UI_SOURCES = $(wildcard $(UI_DIR)/*.c)
UPDATE_SOURCES = $(wildcard $(UPDATE_DIR)/*.c)
SOURCES = $(KERNEL_SOURCES) $(OS_SOURCES) $(DRIVER_SOURCES) $(UTILS_SOURCES) \
          $(SECURITY_SOURCES) $(APPS_SOURCES) $(AI_MODELS_SOURCES) \
          $(NETWORK_SOURCES) $(UI_SOURCES) $(UPDATE_SOURCES) main.c

# Object Files and Dependency Files
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(OBJECTS:.o=.d)

# Executable
EXECUTABLE = cerebro_os_emulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
    $(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

-include $(DEPS) # Include dependency files

%.o: %.c
    $(CC) $(CFLAGS) -MMD -c $< -o $@ # Generate dependencies

clean:
    rm -f $(OBJECTS) $(EXECUTABLE) $(DEPS)
