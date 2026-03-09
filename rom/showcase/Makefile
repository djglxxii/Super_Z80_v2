SJASMPLUS ?= sjasmplus
ROOT_DIR ?= ../..
BUILD_DIR ?= $(ROOT_DIR)/build
ROM_OUT ?= $(BUILD_DIR)/showcase_validation.bin

.PHONY: all clean

all: $(ROM_OUT)

$(ROM_OUT): src/main.asm | $(BUILD_DIR)
	$(SJASMPLUS) --nologo -I $(ROOT_DIR) --raw=$@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(ROM_OUT)
