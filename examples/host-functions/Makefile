WASI_SDK_PATH?=../../wasi-sdk

.PHONY: host-functions
host-functions:
	$(WASI_SDK_PATH)/bin/clang++ -std=c++23 -fno-exceptions -O2 -g -o host-functions.wasm host-functions.cpp -mexec-model=reactor -Wl,--import-undefined
