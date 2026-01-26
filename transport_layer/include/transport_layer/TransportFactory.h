#pragma once
#include <transport_layer/Transport.h>

#include <memory>

std::unique_ptr<ITransport> createTransport();