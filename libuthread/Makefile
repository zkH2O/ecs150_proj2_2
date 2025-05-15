# Target library
lib := libuthread.a

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Werror
CFLAGS += -g
# Add _XOPEN_SOURCE for ucontext.h on macOS
CFLAGS += -D_XOPEN_SOURCE
# Disable deprecated declaration warnings for ucontext.h
CFLAGS += -Wno-deprecated-declarations

# Source files
SRCS := queue.c uthread.c context.c sem.c preempt.c

# Object files
OBJS := $(SRCS:.c=.o)

# Include path
INCLUDES := -I.

# Arhiver
AR := ar
ARFLAGS := rcs

# Default rule
all: $(lib)

# Rule for making the .a library 
$(lib): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Clean up rule
clean:
	rm -f $(OBJS) $(lib)

.PHONY: clean all
