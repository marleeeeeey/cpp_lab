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