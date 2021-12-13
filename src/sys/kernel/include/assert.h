#pragma once

#include <kernel/log.h>
#include <kernel/config.h>

#ifdef KERNEL_ASSERT_ENABLED
struct source_location {
    const char* file_name;
    unsigned    line_number;
    const char* function_name;
};

#define CUR_SOURCE_LOCATION \
    source_location { __FILE__, __LINE__, __func__ }

void do_assert(bool expr, const source_location& loc, const char* expression) noexcept;

#define assert(Expr) do_assert(Expr, CUR_SOURCE_LOCATION, #Expr)
#else
#define assert(Expr)
#endif