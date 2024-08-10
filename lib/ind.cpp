#include <cstdint>
#include "func.hpp"
#include "ind.hpp"

auto indirect(uint32_t a, uint32_t b) -> uint32_t
{
    return func(a, b);
}
