# cpp_lab

My C++ playground. Where code is born, tested, and sometimes survives.

### Configure and build 

Default

```shell
cmake -S . -B build && cmake --build build
```

Preset clang-debug

```shell
cmake --preset clang-debug && cmake --build --preset clang-debug
```

Preset clang-release

```shell
cmake --preset clang-release && cmake --build --preset clang-release
```