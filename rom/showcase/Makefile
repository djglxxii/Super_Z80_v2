SJASMPLUS ?= sjasmplus
BUILD_DIR ?= build
ROM_OUT ?= $(BUILD_DIR)/showcase.bin

SOURCE_FILES := \
	src/main.asm \
	src/init.asm \
	src/vdp_init.asm \
	src/text.asm \
	src/main_loop.asm

INCLUDE_FILES := \
	inc/constants.inc \
	inc/memory_map.inc \
	inc/vdp_helpers.inc

ASSET_FILES := \
	assets/demo_sprite.asm \
	assets/font_8x8.asm \
	assets/splash_tiles.asm \
	assets/splash_tilemap.asm

.PHONY: all clean

all: $(ROM_OUT)

$(ROM_OUT): $(SOURCE_FILES) $(INCLUDE_FILES) $(ASSET_FILES) | $(BUILD_DIR)
	$(SJASMPLUS) --nologo -I . -I ../../sdk --raw=$@ src/main.asm

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(ROM_OUT)
