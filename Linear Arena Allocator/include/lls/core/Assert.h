#pragma once

#include <cstdlib>
#include <iostream>

#ifdef LLS_DEBUG

#define LLS_ASSERT(condition, message)           \
    do                                           \
    {                                            \
        if (!(condition))                        \
        {                                        \
            std::cerr << "Assertion Failed: "    \
                      << message << std::endl;   \
            std::abort();                        \
        }                                        \
    } while (false)

#else

#define LLS_ASSERT(condition, message) ((void)0)

#endif