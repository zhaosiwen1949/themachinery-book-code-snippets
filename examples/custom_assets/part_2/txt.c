// -- api's
static struct tm_the_truth_api *tm_the_truth_api;
// #code_snippet_begin(tm_properties_view_api)
static struct tm_properties_view_api *tm_properties_view_api;
// #code_snippet_end(tm_properties_view_api)
static struct tm_os_api *tm_os_api;
// -- inlcudes
#include <foundation/api_registry.h>
#include <foundation/buffer.h>
#include <foundation/localizer.h>
#include <foundation/macros.h>
#include <foundation/os.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <foundation/undo.h>

#include <plugins/editor_views/asset_browser.h>
// #code_snippet_begin(tm_properties_view_api)
#include <plugins/editor_views/properties.h>
// #code_snippet_end(tm_properties_view_api)

#include "txt.h"

//custom ui
// #code_snippet_begin(custom_ui)
static float properties__custom_ui(struct tm_properties_ui_args_t *args, tm_rect_t item_rect, tm_tt_id_t object)
{
    // #code_snippet_exclude_begin(custom_ui)
    tm_the_truth_o *tt = args->tt;
    // #code_snippet_begin(ui_open_path)
    bool picked = false;
    item_rect.y = tm_properties_view_api->ui_open_path(args, item_rect, TM_LOCALIZE_LATER("Import Path"), TM_LOCALIZE_LATER("Path that the text file was imported from."), object, TM_TT_PROP__MY_ASSET__FILE, "txt", "text files", &picked);
    if (picked)
    {
        // #code_snippet_exclude_begin(ui_open_path)
        const char *file = tm_the_truth_api->get_string(tt, tm_tt_read(tt, object), TM_TT_PROP__MY_ASSET__FILE);
        tm_file_stat_t stat = tm_os_api->file_system->stat(file);
        tm_buffers_i *buffers = tm_the_truth_api->buffers(tt);
        void *buffer = buffers->allocate(buffers->inst, stat.size, false);
        tm_file_o f = tm_os_api->file_io->open_input(file);
        tm_os_api->file_io->read(f, buffer, stat.size);
        tm_os_api->file_io->close(f);
        const uint32_t buffer_id = buffers->add(buffers->inst, buffer, stat.size, 0);
        tm_the_truth_object_o *asset_obj = tm_the_truth_api->write(tt, object);
        tm_the_truth_api->set_buffer(tt, asset_obj, TM_TT_PROP__MY_ASSET__DATA, buffer_id);
        tm_the_truth_api->commit(tt, asset_obj, TM_TT_NO_UNDO_SCOPE);
        // #code_snippet_exclude_end(ui_open_path)
    }
    // #code_snippet_end(ui_open_path)
    // #code_snippet_exclude_end(custom_ui)
    return item_rect.y;
}
// #code_snippet_end(custom_ui)
// -- create truth type
// #code_snippet_begin(create_truth_types)
// #code_snippet_begin(create_truth_types_custom_ui)
static void create_truth_types(struct tm_the_truth_o *tt)
{
    // #code_snippet_exclude_begin(create_truth_types)
    // #code_snippet_exclude_begin(create_truth_types_custom_ui)
    // #code_snippet_begin(register_aspect)
    static tm_the_truth_property_definition_t my_asset_properties[] = {
        {"import_path", TM_THE_TRUTH_PROPERTY_TYPE_STRING},
        {"data", TM_THE_TRUTH_PROPERTY_TYPE_BUFFER},
    };
    // #code_snippet_end(register_aspect)
    const tm_tt_type_t type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__MY_ASSET, my_asset_properties, TM_ARRAY_COUNT(my_asset_properties));
    // #code_snippet_exclude_end(create_truth_types)
    tm_tt_set_aspect(tt, type, tm_tt_assets_file_extension_aspect_i, "txt");
    // #code_snippet_exclude_begin(create_truth_types)
    // #code_snippet_begin(custom_ui)
    // #code_snippet_exclude_end(create_truth_types_custom_ui)
    static tm_properties_aspect_i properties_aspect = {
        .custom_ui = properties__custom_ui,
    };
    // #code_snippet_end(custom_ui)
    tm_tt_set_aspect(tt, type, tm_properties_aspect_i, &properties_aspect);
    // #code_snippet_exclude_end(create_truth_types)
}
// #code_snippet_end(create_truth_types)
// #code_snippet_end(create_truth_types_custom_ui)

// -- asset browser regsiter interface
static tm_tt_id_t asset_browser_create(struct tm_asset_browser_create_asset_o *inst, tm_the_truth_o *tt, tm_tt_undo_scope_t undo_scope)
{
    const tm_tt_type_t type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__MY_ASSET);
    return tm_the_truth_api->create_object_of_type(tt, type, undo_scope);
}
// #code_snippet_begin(asset_browser_create_my_asset)
static tm_asset_browser_create_asset_i asset_browser_create_my_asset = {
    .menu_name = TM_LOCALIZE_LATER("New Text File"),
    .asset_name = TM_LOCALIZE_LATER("New Text File"),
    .create = asset_browser_create,
};
// #code_snippet_end(asset_browser_create_my_asset)
// -- load plugin
// #code_snippet_begin(tm_properties_view_api)
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    // #code_snippet_exclude_begin()
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    // #code_snippet_exclude_end()
    tm_properties_view_api = tm_get_api(reg, tm_properties_view_api);
    // #code_snippet_exclude_begin()
    tm_os_api = tm_get_api(reg, tm_os_api);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
    tm_add_or_remove_implementation(reg, load, tm_asset_browser_create_asset_i, &asset_browser_create_my_asset);
    // #code_snippet_exclude_end()
}
// #code_snippet_end(tm_properties_view_api)