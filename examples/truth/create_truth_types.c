// beginning of the source file
static struct tm_the_truth_api *tm_the_truth_api;
#include <foundation/api_registry.h>
// include [macros.h](https://ourmachinery.com/apidoc/foundation/macros.h.html#macros.h) to access TM_ARRAY_COUNT for convinace:
#include <foundation/macros.h>
#include <foundation/the_truth.h>

#include "my_type.h"

static void create_truth_types(tm_the_truth_o *tt)
{
    tm_the_truth_property_definition_t properties[] = {
        {"my_bool", TM_THE_TRUTH_PROPERTY_TYPE_BOOL},
        {"my_string", TM_THE_TRUTH_PROPERTY_TYPE_STRING},
    };
    tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__MY_TYPE, properties, TM_ARRAY_COUNT(properties));
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
}