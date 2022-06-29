#pragma once

#include <stddef.h>

/// result_t is a type meant to be a return type. If less then zero, it dictates an error. 
/// If greater or equal to zero, it dictates a user specified quantity.
typedef intptr_t result_t;

#define result_error(x) (x < 0)

#define RESULT_OK 0
#define RESULT_UNKNOWN_ERROR -1
#define RESULT_NULL_ARGUMENT -2
#define RESULT_INVALID_OPERATION -3
#define RESULT_OOM -4
#define RESULT_WRONG_TYPE -5
#define RESULT_HANDLE_INVALID -6