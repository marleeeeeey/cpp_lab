#pragma once

#include <functional>
#include <span>
#include <system_error>

using MessageHandler = std::function<void(std::span<const std::uint8_t> data)>;
using ErrorHandler = std::function<void(const std::error_code& ec)>;
