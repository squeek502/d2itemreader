#include "d2itemreader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    d2data_use_default(&g_d2data);
    d2atmastash stash;
    uint32_t bytesRead;
    d2err err = d2atmastash_parse(data, size, &stash, &bytesRead);
    if (err == D2ERR_OK)
    {
        d2atmastash_destroy(&stash);
    }
    d2data_destroy(&g_d2data);
    return 0;
}