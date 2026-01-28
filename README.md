# cpp_lab

My C++ playground.

### Configure and build 

```shell
cmake -S . -B build && cmake --build build
```

**[Emscripten](https://emscripten.org/docs/getting_started/downloads.html)** build requires options `CMAKE_TOOLCHAIN_FILE` and
`FLATC_HOST_EXECUTABLE` to be set.

`FLATC_HOST_EXECUTABLE` - path to the flatbuffer compiler executable `flatc.exe` from the flatbuffers package.
It can be downloaded from https://github.com/google/flatbuffers/releases/tag/v25.12.19.

Example:

```powershell
cd .\cpp_lab
cmake -S . -B build `
  -DCMAKE_TOOLCHAIN_FILE="$env:EMSDK\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake" `
  -DFLATC_HOST_EXECUTABLE="C:\path\to\flatc.exe" `
  -DCMAKE_C_COMPILER="$env:EMSDK\upstream\emscripten\emcc.bat" `
  -DCMAKE_CXX_COMPILER="$env:EMSDK\upstream\emscripten\em++.bat" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_MAKE_PROGRAM="C:\path\to\ninja.exe" `
  -G Ninja
cmake --build build
```
