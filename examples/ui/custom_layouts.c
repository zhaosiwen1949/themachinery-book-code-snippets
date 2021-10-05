static struct tm_the_machinery_api *tm_the_machinery_api;
static struct tm_temp_allocator_api *tm_temp_allocator_api;
static struct tm_tab_layout_api *tm_tab_layout_api;
static struct tm_the_truth_api *tm_the_truth_api;
#include <foundation/api_registry.h>
#include <foundation/application.h>
#include <foundation/temp_allocator.h>
#include <foundation/the_truth.h>
#include <foundation/carray.inl>

#include <the_machinery/the_machinery.h>
#include <plugins/ui/layouts.h>
#include <plugins/ui/ui_icon.h>

// tabs:
#include <the_machinery/asset_browser_tab.h>
#include <the_machinery/scene_tab.h>
#include <the_machinery/preview_tab.h>
#include <the_machinery/properties_tab.h>
#include <the_machinery/console_tab.h>
#include <the_machinery/entity_tree_tab.h>

#define TM_LAYOUT_NAME_HASH TM_STATIC_HASH("my_layout", 0xc1b38d6389074e53ULL)
#define TM_LAYOUT_NAME "my_layout"

static void create_layout(tm_application_o *app)
{
    TM_INIT_TEMP_ALLOCATOR(ta);

    // Query the settings object and Truth from The Machinery API.
    tm_tt_id_t app_settings_id;
    tm_the_truth_o *tt = tm_the_machinery_api->settings(app, &app_settings_id);
    const tm_tt_id_t window_layouts_id = tm_the_truth_api->get_subobject(tt, tm_tt_read(tt, app_settings_id), TM_TT_PROP__APPLICATION_SETTINGS__WINDOW_LAYOUTS);

    // Check whether our layout already exists.
    const tm_tt_id_t *window_layouts = tm_the_truth_api->get_subobject_set(tt, tm_tt_read(tt, window_layouts_id), TM_TT_PROP__WINDOW_LAYOUTS__LAYOUTS, ta);
    const uint32_t num_window_layouts = (uint32_t)tm_carray_size(window_layouts);
    for (uint32_t i = 0; i < num_window_layouts; ++i)
    {
        const tm_strhash_t name_hash = tm_the_truth_api->get_string_hash(tt, tm_tt_read(tt, window_layouts[i]), TM_TT_PROP__WINDOW_LAYOUT__NAME);
        if (TM_STRHASH_EQUAL(name_hash, TM_LAYOUT_NAME_HASH))
        {
            TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
            return;
        }
    }
    tm_tab_layout_t layout = {
        .split = TM_TAB_LAYOUT_SPLIT_TYPE__HORIZONTAL,
        .bias = 0.25f,
        .top = &(tm_tab_layout_t){
            .split = TM_TAB_LAYOUT_SPLIT_TYPE__VERTICAL,
            .bias = 0.67f,
            .left = &(tm_tab_layout_t){
                .split = TM_TAB_LAYOUT_SPLIT_TYPE__VERTICAL,
                .bias = -0.67f,
                .right = &(tm_tab_layout_t){.tab = {TM_SCENE_TAB_VT_NAME_HASH}},
                .left = &(tm_tab_layout_t){.tab = {TM_TREE_TAB_VT_NAME_HASH}},
            },
            .right = &(tm_tab_layout_t){.tab = {TM_PROPERTIES_TAB_VT_NAME_HASH}},
        },
        .bottom = &(tm_tab_layout_t){
            .split = TM_TAB_LAYOUT_SPLIT_TYPE__VERTICAL,
            .bias = 0.5f,
            .left = &(tm_tab_layout_t){
                .split = TM_TAB_LAYOUT_SPLIT_TYPE__VERTICAL,
                .bias = -0.5f,
                .right = &(tm_tab_layout_t){.tab = {TM_ASSET_BROWSER_TAB_VT_NAME_HASH}},
                .left = &(tm_tab_layout_t){.tab = {TM_CONSOLE_TAB_VT_NAME_HASH}},
            },
            .right = &(tm_tab_layout_t){.tab = {TM_PREVIEW_TAB_VT_NAME_HASH}},
        },
    };

    const tm_tt_id_t layout_id = tm_the_truth_api->create_object_of_hash(tt, TM_TT_TYPE_HASH__WINDOW_LAYOUT, TM_TT_NO_UNDO_SCOPE);
    tm_the_truth_object_o *layout_w = tm_the_truth_api->write(tt, layout_id);

    tm_the_truth_object_o *layouts_w = tm_the_truth_api->write(tt, window_layouts_id);
    tm_the_truth_api->add_to_subobject_set(tt, layouts_w, TM_TT_PROP__WINDOW_LAYOUTS__LAYOUTS, &layout_w, 1);
    tm_the_truth_api->commit(tt, layouts_w, TM_TT_NO_UNDO_SCOPE);

    tm_the_truth_api->set_string(tt, layout_w, TM_TT_PROP__WINDOW_LAYOUT__NAME, TM_LAYOUT_NAME);
    tm_the_truth_api->set_uint32_t(tt, layout_w, TM_TT_PROP__WINDOW_LAYOUT__ICON, TM_UI_ICON__COLOR_WAND);
    tm_the_truth_api->set_float(tt, layout_w, TM_TT_PROP__WINDOW_LAYOUT__WINDOW_X, 0.0f);
    tm_the_truth_api->set_float(tt, layout_w, TM_TT_PROP__WINDOW_LAYOUT__WINDOW_Y, 0.0f);
    tm_the_truth_api->set_float(tt, layout_w, TM_TT_PROP__WINDOW_LAYOUT__WINDOW_WIDTH, 1920.0f);
    tm_the_truth_api->set_float(tt, layout_w, TM_TT_PROP__WINDOW_LAYOUT__WINDOW_HEIGHT, 1080.0f);

    const tm_tt_id_t tabwell_id = tm_tab_layout_api->save_layout(tt, &layout, false, TM_TT_NO_UNDO_SCOPE);
    tm_the_truth_api->set_subobject_id(tt, layout_w, TM_TT_PROP__WINDOW_LAYOUT__TABWELL, tabwell_id, TM_TT_NO_UNDO_SCOPE);

    tm_the_truth_api->commit(tt, layout_w, TM_TT_NO_UNDO_SCOPE);
    TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{

    tm_temp_allocator_api = tm_get_api(reg, tm_temp_allocator_api);
    tm_the_machinery_api = tm_get_api(reg, tm_the_machinery_api);
    tm_tab_layout_api = tm_get_api(reg, tm_tab_layout_api);
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);

    tm_add_or_remove_implementation(reg, load, tm_the_machinery_create_layout_i, create_layout);
}