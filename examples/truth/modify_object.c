// beginning of the source file
static struct tm_the_truth_api *tm_the_truth_api;
#include <foundation/api_registry.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_types.h>
#include <foundation/log.h>

#include "my_type.h"

void modify_my_object(tm_the_truth_o *tt, tm_tt_id_t my_object)
{
    const tm_tt_type_t type = tm_tt_type(my_object);
    const tm_tt_type_t expected_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__RECT);

    if (type.u64 != expected_type.u64)
        return;

    float wdith = tm_the_truth_api->get_float(tt, tm_tt_read(tt, my_object), TM_TT_PROP__RECT__W);
    wdith += 100;
    tm_the_truth_object_o *my_object_w = tm_the_truth_api->write(tt, my_object);
    tm_the_truth_api->set_float(tt, my_object_w, TM_TT_PROP__RECT__W, wdith);
    const tm_tt_undo_scope_t undo_scope = tm_the_truth_api->create_undo_scope(tt, "My Undo Scope");
    tm_the_truth_api->commit(tt, my_object_w, undo_scope);
}

void alt_modify_my_object(tm_the_truth_o *tt, tm_tt_id_t my_object)
{
    tm_the_truth_object_o *my_object_w = tm_the_truth_api->write(tt, my_object);
    tm_the_truth_api->set_float(tt, my_object_w, TM_TT_PROP__RECT__W, 100);
    const tm_tt_undo_scope_t undo_scope = tm_the_truth_api->create_undo_scope(tt, "My Undo Scope");
    tm_the_truth_api->commit(tt, my_object_w, undo_scope);
}

void no_undo_modify_my_object(tm_the_truth_o *tt, tm_tt_id_t my_object)
{
    tm_the_truth_object_o *my_object_w = tm_the_truth_api->write(tt, my_object);
    tm_the_truth_api->set_float(tt, my_object_w, TM_TT_PROP__RECT__W, 100);
    tm_the_truth_api->commit(tt, my_object_w, TM_TT_NO_UNDO_SCOPE);
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
}