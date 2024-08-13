WASI_SDK_PATH?=../../wasi-sdk

.PHONY: count-vowels
count-vowels:
	$(WASI_SDK_PATH)/bin/clang++ -std=c++23 -fno-exceptions -O2 -g -o count-vowels.wasm count-vowels.cpp -mexec-model=reactor

run: count-vowels
	extism call ./count-vowels.wasm count_vowels --wasi --input "this is a test"