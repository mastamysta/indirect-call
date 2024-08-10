#include <array>

#ifdef INDIRECT
#include "ind.hpp"
#else
#include "func.hpp"

    #ifdef PINDIRECT

    static uint32_t (*pfunc)(uint32_t, uint32_t) = func;

    static auto pindirect(uint32_t a, uint32_t b) -> uint32_t
    {
        return pfunc(a, b);
    }

    #else

    static auto direct(uint32_t a, uint32_t b) -> uint32_t
    {
        return func(a, b);
    }

    #endif

#endif


#define LIST_SIZE 100000

int main()
{

    for (int j = 0; j < 10000; j++)
    {
        for (int i = 0; i < LIST_SIZE; i++)
        {
    #ifdef INDIRECT
        indirect(i, LIST_SIZE - i);
    #else
        #ifdef PINDIRECT
            pindirect(i, LIST_SIZE - i);
        #else
            direct(i, LIST_SIZE - i);
        #endif
    #endif   
        }
    }

    return 0;
}