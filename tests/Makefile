WASI_SDK_PATH?=../wasi-sdk

.PHONY: test test-%
test: test-input test-fail test-config test-var test-strvar test-log test-http

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
	extism call log.wasm --wasi --log-level=trace run_test 2>&1 | perl log-matches.pl trace
	extism call log.wasm --wasi --log-level=debug run_test 2>&1 | perl log-matches.pl debug
	extism call log.wasm --wasi --log-level=info run_test 2>&1 | perl log-matches.pl info
	extism call log.wasm --wasi --log-level=warn run_test 2>&1 | perl log-matches.pl warn
	extism call log.wasm --wasi --log-level=error run_test 2>&1 | perl log-matches.pl error

test-http: http.wasm
	extism call http.wasm --wasi --input "" --allow-host "*" --enable-http-response-headers --log-level=debug run_test

clean:
	rm -f *.wasm
