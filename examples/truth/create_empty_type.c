// beginning of the source file
static struct tm_the_truth_api *tm_the_truth_api;
#include <foundation/api_registry.h>
#include <foundation/the_truth.h>
#include "my_type.h"

static void create_truth_types(tm_the_truth_o *tt)
{
    tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__MY_TYPE, 0, 0);
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
}