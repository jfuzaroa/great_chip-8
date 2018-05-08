CC		= gcc
CFLAGS	= -g -Wall -Wextra -Iinclude $(OPTFLAGS)

SRCS 	= $(wildcard src/*.c src/**/*.c)
HDRS	= $(wildcard include/*.h include/**/*.h)
OBJS	= $(patsubst %.c, build/%.o, $(notdir $(SRCS)))

EXEC	= great-chip-8
TRGT	= bin/$(EXEC)

all: $(SRCS) $(HDRS) $(TRGT)

release: CFLAGS=-march=native -O2 -pipe all

$(TRGT): build $(OBJS)
	$(CC) $(OBJS) -o $@

build:
	@mkdir build bin

$(OBJS): build/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin build

print:
	@echo $(SRCS)
	@echo $(HDRS)
