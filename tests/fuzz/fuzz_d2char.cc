extern "C" {
#include "d2itemreader.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    d2data_use_default(&g_d2data);
    d2char character;
    uint32_t bytesRead;
    d2err err = d2char_parse(data, size, &character, &bytesRead);
    if (err == D2ERR_OK)
    {
        d2char_destroy(&character);
    }
    d2data_destroy(&g_d2data);
    return 0;
}
