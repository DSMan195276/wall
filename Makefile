
Q := @
CC := gcc
BINDIR := ./bin

PROG := prog

SRCS := $(wildcard ./src/*.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

SHADER_VERT_SRCS := $(wildcard ./src/shaders/*.vert)
SHADER_FRAG_SRCS := $(wildcard ./src/shaders/*.frag)
SHADER_OBJS := $(SHADER_VERT_SRCS:.vert=.vx) $(SHADER_FRAG_SRCS:.frag=.fx)

CFLAGS := -Wall -I./include -O2 -march=native -fno-strict-aliasing -g
LDFLAGS := -lglfw -lGLEW -lGL -lm

CLEAN_LIST += $(OBJS)
CLEAN_LIST += $(SHADER_OBJS)
CLEAN_LIST += $(BINDIR)/$(PROG)
CLEAN_LIST += $(DEPS)

all: $(BINDIR)/$(PROG)

clean:
	$(Q)for file in $(CLEAN_LIST); do \
		if [ -e $$file ]; then \
			echo "rm -rf $$file"; \
			rm -rf $$file; \
		fi \
	done

$(BINDIR)/$(PROG): $(OBJS)
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	gcc $(CFLAGS) $< -c -o $@

%.d: %.c
	$(CC) -MG -MM -MF $@ $(CFLAGS) $< -MT $*.o -MT $@

# We make xxd read from stdin to keep it from outputting a full array declaraction
# This way it just puts out comma-separated bytes, like "0x01, 0x02, 0x03, ..."
%.vx: %.vert
	cat $< | xxd -i - $@

%.fx: %.frag
	cat $< | xxd -i - $@

src/shaders.c: $(SHADER_OBJS)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all clean

