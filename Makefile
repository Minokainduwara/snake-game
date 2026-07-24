# Snake Game - Cross-platform Makefile
# Requires raylib to be installed

CC := clang

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    # macOS
    CFLAGS := $(shell pkg-config --cflags raylib) -Wall -Wextra -std=c99
    LIBS := $(shell pkg-config --libs raylib) -lm
    TARGET := snake
else ifeq ($(UNAME_S),Linux)
    # Linux
    CFLAGS := $(shell pkg-config --cflags raylib) -Wall -Wextra -std=c99
    LIBS := $(shell pkg-config --libs raylib) -lm
    TARGET := snake
else
    # Windows (MinGW)
    CFLAGS := -I./raylib/include -Wall -Wextra -std=c99
    LIBS := -L./raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lm
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
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)