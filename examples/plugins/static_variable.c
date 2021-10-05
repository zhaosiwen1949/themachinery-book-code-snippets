#include <foundation/api_registry.h>

uint64_t *count_ptr;

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    count_ptr = (uint64_t *)reg->static_variable(TM_STATIC_HASH("my_count", 0xa287d4b3ec9c2109ULL), sizeof(uint64_t), __FILE__, __LINE__);
}

void f()
{
    ++*count_ptr;
}