// -- api's
static struct tm_the_truth_api *tm_the_truth_api;
// -- inlcudes
#include <foundation/api_registry.h>
#include <foundation/localizer.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <foundation/undo.h>

#include <plugins/editor_views/asset_browser.h>

#include "txt.h"

// -- create truth type
// #code_snippet_begin(create_truth_types)
static void create_truth_types(struct tm_the_truth_o *tt)
{
    // we have properties this is why the last arguments are "0, 0"
    const tm_tt_type_t type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__MY_ASSET, 0, 0);
    tm_the_truth_api->set_aspect(tt, type, TM_TT_ASPECT__FILE_EXTENSION, "my_asset");
}
// #code_snippet_end(create_truth_types)
// #code_snippet_begin(asset_browser_create)
// -- asset browser regsiter interface
static tm_tt_id_t asset_browser_create(struct tm_asset_browser_create_asset_o *inst, tm_the_truth_o *tt, tm_tt_undo_scope_t undo_scope)
{
    const tm_tt_type_t type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__MY_ASSET);
    return tm_the_truth_api->create_object_of_type(tt, type, undo_scope);
}
static tm_asset_browser_create_asset_i asset_browser_create_my_asset = {
    .menu_name = TM_LOCALIZE_LATER("New My Asset"),
    .asset_name = TM_LOCALIZE_LATER("New My Asset"),
    .create = asset_browser_create,
};
// #code_snippet_end(asset_browser_create)
// #code_snippet_begin(tm_load_plugin)
// -- load plugin
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
    tm_add_or_remove_implementation(reg, load, tm_asset_browser_create_asset_i, &asset_browser_create_my_asset);
}
// #code_snippet_end(tm_load_plugin)
