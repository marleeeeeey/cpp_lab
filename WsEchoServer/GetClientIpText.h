#pragma once
#include <string>
#include <string_view>

namespace uWS {
struct HttpRequest;
}

std::string getClientIpText(uWS::HttpRequest* req, std::string_view socketIpText);