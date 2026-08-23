CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic

BUILD = build

BITTER= bin/bitter
BITTER_SRC = $(wildcard bitter/*.c)
BITTER_OBJ = $(patsubst bitter/%.c,$(BUILD)/bitter/%.o,$(BITTER_SRC))

.PHONY: all bitter clean

all: bitter

bitter: $(BITTER)

$(BITTER): $(BITTER_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@

$(BUILD)/bitter/%.o: bitter/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) $(BITTER)