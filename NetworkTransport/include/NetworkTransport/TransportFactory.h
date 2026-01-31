#pragma once
#include <NetworkTransport/Transport.h>

#include <memory>

std::unique_ptr<ITransport> createTransport();