# Fuzzing d2itemreader

d2itemreader uses [libFuzzer](https://llvm.org/docs/LibFuzzer.html), and requires both clang and clang++.

## Building and running the fuzzers

To create the fuzzers, first execute the following in the root of the cloned d2itemreader repository:
```
mkdir build.fuzz && cd build.fuzz
cmake .. -DENABLE_FUZZING=On -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
```
Then, from inside `build.fuzz`:

- To build a fuzzer, use `make fuzz_<name>`
- To run a fuzzer, use `make fuzz_<name>_run`
- To minimize a fuzzer's corpus, use `make fuzz_<name>_minimize`

where `<name>` is one of `d2char`, `d2atmastash`, `d2personalstash` or `d2sharedstash`.

More info:
- https://github.com/google/fuzzer-test-suite/blob/master/tutorial/libFuzzerTutorial.md
- https://llvm.org/docs/LibFuzzer.html

## Checking coverage of a fuzzer

To build the fuzzers for coverage analysis, execute the following in the root of the cloned d2itemreader repository:
```
mkdir build.cov && cd build.cov
cmake .. -DENABLE_FUZZING=On -DCOVERAGE=On -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
```
Then, from inside `build.cov`:

- To build a fuzzer, use `make fuzz_<name>`
- To generate coverage information, use `make fuzz_<name>_cov`

where `<name>` is one of `d2char`, `d2atmastash`, `d2personalstash` or `d2sharedstash`.

The `.profraw` will be in `build.cov/tests/fuzz/`. Navigate to that directory and execute the following to show the coverage information (substituting `<name>` with the name of the fuzzer used in the previous step):

```
cd tests/fuzz/
llvm-profdata merge -sparse default.profraw -o default.profdata
llvm-cov show ./fuzz_<name> -instr-profile=default.profdata
```

More info:
- https://github.com/google/fuzzer-test-suite/blob/master/tutorial/libFuzzerTutorial.md#visualizing-coverage
- http://clang.llvm.org/docs/SourceBasedCodeCoverage.html

## Bugs found via fuzzing so far

- [`4324bd0568aefb35d077fd91af12078930f295de`](https://github.com/squeek502/d2itemreader/commit/4324bd0568aefb35d077fd91af12078930f295de)
- [`ec40071115efd50f0a64c1d910c71d8307059c31`](https://github.com/squeek502/d2itemreader/commit/ec40071115efd50f0a64c1d910c71d8307059c31)
- [`c59b9db6c7ae010f7fb9e5d471c778b090f94835`](https://github.com/squeek502/d2itemreader/commit/c59b9db6c7ae010f7fb9e5d471c778b090f94835)
- [`9b1ca7883e0f4589203871ff7f649821c4ce3c62`](https://github.com/squeek502/d2itemreader/commit/9b1ca7883e0f4589203871ff7f649821c4ce3c62)
- [`5ac6f44d8078481e3ce4c437b9450d0026c03a5f`](https://github.com/squeek502/d2itemreader/commit/5ac6f44d8078481e3ce4c437b9450d0026c03a5f)
- [`341956eb23bc8dedc685712a48130c6e29b43d45`](https://github.com/squeek502/d2itemreader/commit/341956eb23bc8dedc685712a48130c6e29b43d45)
- [`1fa7512e8eed7d30c28d6e7df026c765f91615d9`](https://github.com/squeek502/d2itemreader/commit/1fa7512e8eed7d30c28d6e7df026c765f91615d9)
- [`133f6e04a47b57ba7c4c6e7c9b283eca84f18fd4`](https://github.com/squeek502/d2itemreader/commit/133f6e04a47b57ba7c4c6e7c9b283eca84f18fd4)
- [`29c8fd5f14ea3f327e64dacbf2616ff47b533f87`](https://github.com/squeek502/d2itemreader/commit/29c8fd5f14ea3f327e64dacbf2616ff47b533f87)
- [`6621fa33fa76c8d9023787913eb90860aa2229ee`](https://github.com/squeek502/d2itemreader/commit/6621fa33fa76c8d9023787913eb90860aa2229ee)
