CC = gcc
STRIP = strip
CFLAGS = -Wall -O2
LDFLAGS = -lm

build_dir = build
sources = $(wildcard *.c)
objects = $(patsubst %.c,$(build_dir)/%.o,$(sources))

bin = kmalloc

all: dir $(objects)
	@echo CC $(bin)
	@$(CC) $(LDFLAGS) $(objects) -o $(build_dir)/$(bin)

run: all
	$(build_dir)/$(bin)

$(build_dir)/%.o: %.c
	@echo CC $@
	@$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: dir clean
dir:
	@if [ ! -d $(build_dir) ]; then mkdir "$(build_dir)"; fi 
clean:
	-rm -rf $(build_dir)

