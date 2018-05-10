CC		= clang
CFLAGS	= -g -std=c11 -Iinclude $(WFLAGS) $(OPFLAGS)
WFLAGS	= -Wall -Wextra -Wpedantic -Wformat=2 -Wunused-parameter -Wshadow \
		  -Wwrite-strings -Wstrict-prototypes -Wredundant-decls \
		  -Wnested-externs -Wmissing-include-dirs -Werror
# OPFLAGS =
RLFLAGS	= -march=native -O2 -pipe

ifeq ($(CC), gcc)
    CFLAGS += -Wjump-misses-init -Wlogical-op
endif

SRCS 	= $(wildcard src/*.c src/**/*.c)
HDRS	= $(wildcard include/*.h include/**/*.h)
OBJS	= $(patsubst %.c, build/%.o, $(notdir $(SRCS)))

EXEC	= great-chip-8
TRGT	= bin/$(EXEC)

all: $(SRCS) $(HDRS) $(TRGT)

release: CFLAGS=$(RLFLAGS) all

$(TRGT): build $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

build:
	@mkdir build bin

$(OBJS): build/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf bin build

print:
	@echo $(SRCS)
	@echo $(HDRS)
	@echo $(OBJS)
