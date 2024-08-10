#include <array>

#ifdef INDIRECT
#include "ind.hpp"
#else
#include "func.hpp"

static auto direct(uint32_t a, uint32_t b) -> uint32_t
{
    return func(a, b);
}

#endif


template <typename T, std::size_t S>
class g
{
public:
    constexpr auto operator ()() -> std::array<T, S>
    {
        std::array<T, S> ret = { 0 };

        for (int i = 0; i < S; i++)
        {
            ret[i] = (i * 3) + 9;
        }

        return ret;
    }
};

#define LIST_SIZE 100000

int main()
{
    g<uint32_t, LIST_SIZE> gen;
    auto data = gen();

    for (int i = 0; i < LIST_SIZE; i++)
    {
#ifdef INDIRECT
    indirect(data[i], data[LIST_SIZE - i]);
#else
    direct(data[i], data[LIST_SIZE - i]);
#endif   
    }

    return 0;
}