#ifndef D2ITEMREADER_UTIL_H
#define D2ITEMREADER_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include "d2err.h"

#if defined(__GNUC__) && (__GNUC__ >= 4)
# define CHECK_RESULT __attribute__ ((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
# define CHECK_RESULT _Check_return_
#else
# define CHECK_RESULT
#endif

#ifdef __GNUC__
# define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
# define UNUSED(x) UNUSED_ ## x
#endif

CHECK_RESULT d2err d2util_read_file(const char* filepath, uint8_t** data_out, size_t* bytesRead);

#ifdef __cplusplus
}
#endif

#endif
