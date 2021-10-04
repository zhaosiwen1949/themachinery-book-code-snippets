// beginning of the source file
static struct tm_the_truth_api *tm_the_truth_api;
static struct tm_logger_api *tm_logger_api;
#include <foundation/api_registry.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_types.h>
#include <foundation/log.h>

#include "my_type.h"

static void create_truth_types(tm_the_truth_o *tt)
{
    tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__MY_TYPE, 0, 0);
}

void log_with(tm_the_truth_o *tt, tm_tt_id_t my_object)
{
    const tm_tt_type_t type = tm_tt_type(my_object);
    const tm_tt_type_t expected_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__RECT);

    if (type.u64 != expected_type.u64)
    {
        TM_LOG("The provided type does not mmatch! %p{tm_tt_type_t} != %p{tm_tt_type_t}", &type, &expected_type);
        return;
    }

    const float width = tm_the_truth_api->get_float(tt, tm_tt_read(tt, my_object), TM_TT_PROP__RECT__W);
    TM_LOG("the width is %f", width);
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_logger_api = tm_get_api(reg, tm_logger_api);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
}