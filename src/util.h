#ifndef D2ITEMREADER_UTIL_H
#define D2ITEMREADER_UTIL_H

#include <stdlib.h>
#include "d2err.h"

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CHECK_RESULT __attribute__ ((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define CHECK_RESULT _Check_return_
#else
#define CHECK_RESULT
#endif

CHECK_RESULT d2err d2util_read_file(const char* filepath, unsigned char** data_out, size_t* bytesRead);

#endif