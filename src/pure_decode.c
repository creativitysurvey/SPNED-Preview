#include <stdint.h>
__attribute__((export_name("decode")))
void decode(int32_t* in, int32_t* out, int32_t len) {
    for (int32_t i = 0; i < len; ++i) {
        out[i] = in[i];
    }
}
