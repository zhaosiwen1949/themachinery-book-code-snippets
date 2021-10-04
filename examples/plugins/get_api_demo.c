static struct tm_unicode_api *tm_unicode_api;
#include <foundation/api_registry.h>
#include <foundation/unicode.h>

static void demo_usage(char *utf8, uint32_t codepoint)
{
    tm_unicode_api->utf8_encode(utf8, codepoint);
    //more code...
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_unicode_api = tm_get_api(reg, tm_unicode_api);
}