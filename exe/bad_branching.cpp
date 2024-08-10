#include <cstdint>

#define TIMES 1000000000

auto func(uint32_t a, uint32_t b) -> uint32_t
{
    return a + b;
}

auto goodcall(uint32_t a, uint32_t b) -> uint32_t
{
    return func(a, b);
}

static uint32_t (*pfunc)(uint32_t, uint32_t) = func;

auto badcall(uint32_t a, uint32_t b) -> uint32_t
{
    return pfunc(a, b);
}

int main()
{
    for (int i = 0; i < TIMES; i++)
    {
#ifdef GOOD
        goodcall(i, TIMES - i);
#endif

#ifdef BAD
        badcall(i, TIMES - i);
#endif  
    }

    return 0;
}