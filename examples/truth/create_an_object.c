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
// #code_snippet_begin(create_my_type_object)
tm_tt_id_t create_my_type_object(tm_the_truth_o *tt)
{
    const tm_tt_type_t my_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__MY_TYPE);
    const tm_tt_id_t my_type_object = tm_the_truth_api->create_object_of_type(tt, my_type, TM_TT_NO_UNDO_SCOPE);
    return my_type_object;
}
// #code_snippet_end(create_my_type_object)
// #code_snippet_begin(quick_create_my_type_object)
tm_tt_id_t quick_create_my_type_object(tm_the_truth_o *tt)
{
    return tm_the_truth_api->quick_create_object(tt, TM_TT_NO_UNDO_SCOPE, TM_TT_TYPE_HASH__MY_TYPE, -1);
}
// #code_snippet_end(quick_create_my_type_object)
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_logger_api = tm_get_api(reg, tm_logger_api);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
}