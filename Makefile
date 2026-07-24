# Snake Game - Cross-platform Makefile
# Requires raylib to be installed

CC ?= clang

# Detect OS
UNAME_S := $(shell uname -s)

# Allow environment variables to override defaults
CFLAGS ?= -Wall -Wextra -std=c99
LIBS ?= -lm
TARGET ?= snake

ifeq ($(UNAME_S),Darwin)
    # macOS - use pkg-config if available, otherwise use environment vars
    CFLAGS += $(shell pkg-config --cflags raylib 2>/dev/null || echo "")
    LIBS += $(shell pkg-config --libs raylib 2>/dev/null || echo "")
else ifeq ($(UNAME_S),Linux)
    # Linux - use pkg-config if available, otherwise use environment vars
    CFLAGS += $(shell pkg-config --cflags raylib 2>/dev/null || echo "")
    LIBS += $(shell pkg-config --libs raylib 2>/dev/null || echo "")
else
    # Windows (MinGW)
    CFLAGS += -I./raylib/include
    LIBS += -L./raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := snake.exe
endif

SRCDIR := src
OBJDIR := obj

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)