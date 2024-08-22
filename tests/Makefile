WASI_SDK_PATH?=../wasi-sdk

.PHONY: test test-%
test: test-input test-fail test-config test-var test-strvar test-log

.PRECIOUS: %.wasm
%.wasm: %.cpp util.h ../extism-pdk.hpp
	$(WASI_SDK_PATH)/bin/clang++ -std=c++23 -fno-exceptions -O2 -g -o $*.wasm $*.cpp -mexec-model=reactor

test-%: %.wasm
	extism call $*.wasm --wasi --input "this is a test" run_test

test-fail: fail.wasm
	! extism call fail.wasm --wasi --input "this is a test" run_test
	extism call fail.wasm --wasi --input "this is a test" run_test 2>&1 | grep -Fxq 'Error: Some error message'

test-config: config.wasm
	extism call config.wasm --wasi --config key="this is a test" run_test

test-var: var.wasm
	extism call var.wasm --wasi --loop 100 run_test | grep -Fxq '100'

test-strvar: strvar.wasm
	extism call strvar.wasm --wasi --loop 5 run_test | grep -Fxq 'aaaaa'

test-log: log.wasm
	extism call log.wasm --wasi --input 'debug' --log-level=debug run_test 2>&1 | grep -q 'debug'
	extism call log.wasm --wasi --input 'info' --log-level=debug run_test 2>&1 | grep -q 'info'
	extism call log.wasm --wasi --input 'warn' --log-level=debug run_test 2>&1 | grep -q 'warn'
	extism call log.wasm --wasi --input 'error' --log-level=debug run_test 2>&1 | grep -q 'error'
clean:
	rm -f *.wasm
