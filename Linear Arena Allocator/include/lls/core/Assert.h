#pragma once

#include <cassert>

#ifdef _DEBUG
#define LLS_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            assert((condition) && (message)); \
        } \
    } while (0)
#else
#define LLS_ASSERT(condition, message) ((void)0)
#endif
