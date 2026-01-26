# cpp_lab

My C++ playground.

The codebase is self-documenting. To understand the library interfaces, check the headers in the include folder of each subproject.

**List of projects**:

- [debug_log_lib](./debug_log_lib): A header-only library for console debug logging. Messages can be toggled using a preprocessor macro.
- [network_connection_lib](./network_connection_lib): A library for symmetrical data exchange via TCP sockets. It follows a "
  length-prefixed" protocol: it sends/receives the data size first, followed by the actual payload.
- [chat_network_lib](./chat_network_lib): Provides an Asio-based chat server and client. It handles networking logic by utilizing the
  `network_connection_lib`.
- [chat_engine_a](./chat_engine_a): A chat application engine. Features a UI powered by ImGui and networking powered by Asio via
  `chat_network_lib`.
- [game_engine_b](./game_engine_b): A game engine built with SDL3 and ImGui.

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
