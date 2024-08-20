WASI_SDK_PATH?=../../wasi-sdk

.PHONY: infinite-loop
infinite-loop:
	$(WASI_SDK_PATH)/bin/clang++ -std=c++23 -fno-exceptions -O0 -g -o infinite-loop.wasm infinite-loop.cpp -mexec-model=reactor -Wl,--export=infinite_loop

run: infinite-loop
	extism call --manifest ./manifest.json infinite_loop --wasi
