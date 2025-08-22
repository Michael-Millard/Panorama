PROJECT_NAME := panorama
BUILD_DIR := build

.PHONY: all debug release clean run

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
	./$(BUILD_DIR)/$(PROJECT_NAME) --input images --output output --file panorama.jpg

