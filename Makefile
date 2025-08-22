PROJECT_NAME := panorama
BUILD_DIR := build

# CLI arg variables (override on command line):
INPUT ?= images
OUTPUT ?= output
FILE ?= panorama.jpg
# Or pass ARGS directly to override everything
ARGS ?= --input $(INPUT) --output $(OUTPUT) --file $(FILE)

.PHONY: all debug release clean run help

all: release

$(BUILD_DIR):
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

release: $(BUILD_DIR)
	cmake --build $(BUILD_DIR) --config Release

debug:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR) --config Debug

clean:
	rm -rf $(BUILD_DIR)

run: release
	./$(BUILD_DIR)/$(PROJECT_NAME) $(ARGS)

help:
	@echo "Targets:"
	@echo "  make            -> same as 'make release'"
	@echo "  make release    -> build Release"
	@echo "  make debug      -> build Debug"
	@echo "  make run        -> build then run with defaults or provided vars"
	@echo "  make clean      -> remove build/"
	@echo ""
	@echo "Usage examples:"
	@echo "  make run                                  # uses defaults"
	@echo "  make run INPUT=images/set1 OUTPUT=out1    # override via vars"
	@echo "  make run FILE=pano.jpg                    # change output filename"
	@echo "  make run ARGS=\"--input images/x --output out/x --file pano.jpg\"  # full control"

