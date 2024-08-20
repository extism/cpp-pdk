WASI_SDK_PATH?=../../wasi-sdk

.PHONY: globals
globals:
	$(WASI_SDK_PATH)/bin/clang++ -std=c++23 -fno-exceptions -O2 -g -o globals.wasm globals.cpp -mexec-model=reactor

run: globals
	extism call ./globals.wasm globals --loop 100 --wasi
