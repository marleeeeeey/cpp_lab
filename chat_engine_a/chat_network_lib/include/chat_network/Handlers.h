#pragma once

#include <functional>
#include <string>

using MessageHandler = std::function<void(const std::string&)>;
using ErrorHandler = std::function<void(const std::string&)>;
