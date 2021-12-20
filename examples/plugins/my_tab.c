// #code_snippet_begin(includes)
static struct tm_api_registry_api *tm_global_api_registry;

static struct tm_draw2d_api *tm_draw2d_api;
static struct tm_ui_api *tm_ui_api;
static struct tm_allocator_api *tm_allocator_api;

#include <foundation/allocator.h>
#include <foundation/api_registry.h>

#include <plugins/ui/docking.h>
#include <plugins/ui/draw2d.h>
#include <plugins/ui/ui.h>
#include <plugins/ui/ui_custom.h>

#include <the_machinery/the_machinery_tab.h>

#include <stdio.h>
// #code_snippet_begin(defines)
#define TM_CUSTOM_TAB_VT_NAME "tm_custom_tab"
#define TM_CUSTOM_TAB_VT_NAME_HASH TM_STATIC_HASH("tm_custom_tab", 0xbc4e3e47fbf1cdc1ULL)
// #code_snippet_end(defines)
// #code_snippet_end(includes)
// #code_snippet_begin(tm_tab_o)
struct tm_tab_o
{
    tm_tab_i tm_tab_i;
    tm_allocator_i allocator;
};
// #code_snippet_end(tm_tab_o)
// #code_snippet_begin(tab__ui)
static void tab__ui(tm_tab_o *tab, tm_ui_o *ui, const tm_ui_style_t *uistyle_in, tm_rect_t rect)
{
    // #code_snippet_begin(tm_draw2d_style_t)
    tm_ui_buffers_t uib = tm_ui_api->buffers(ui);
    tm_ui_style_t *uistyle = (tm_ui_style_t[]){*uistyle_in};
    tm_draw2d_style_t *style = &(tm_draw2d_style_t){0};
    tm_ui_api->to_draw_style(ui, style, uistyle);
    // #code_snippet_end(tm_draw2d_style_t)
    // #code_snippet_begin(draw)
    style->color = (tm_color_srgb_t){.a = 255, .r = 255};
    tm_draw2d_api->fill_rect(uib.vbuffer, *uib.ibuffers, style, rect);
    // #code_snippet_end(draw)
}
// #code_snippet_end(tab__create)
// #code_snippet_begin(tab_meta)
static const char *tab__create_menu_name(void)
{
    return "Custom Tab";
}

static const char *tab__title(tm_tab_o *tab, struct tm_ui_o *ui)
{
    return "Custom Tab";
}
// #code_snippet_end(tab_meta)
// #code_snippet_begin(tab__create)
static tm_tab_vt *custom_tab_vt;

static tm_tab_i *tab__create(tm_tab_create_context_t *context, tm_ui_o *ui)
{
    tm_allocator_i allocator = tm_allocator_api->create_child(context->allocator, "Custom Tab");
    uint64_t *id = context->id;
    // #code_snippet_begin(tm_alloc)
    tm_tab_o *tab = tm_alloc(&allocator, sizeof(tm_tab_o));
    *tab = (tm_tab_o){
        .tm_tab_i = {
            .vt = custom_tab_vt,
            .inst = (tm_tab_o *)tab,
            .root_id = *id,
        },
        .allocator = allocator,
    };
    // #code_snippet_end(tm_alloc)
    *id += 1000000;
    return &tab->tm_tab_i;
}
// #code_snippet_end(tab__create)
// #code_snippet_begin(tab__destroy)
static void tab__destroy(tm_tab_o *tab)
{
    tm_allocator_i a = tab->allocator;
    tm_free(&a, tab, sizeof(*tab));
    tm_allocator_api->destroy_child(&a);
}
// #code_snippet_end(tab__destroy)
// #code_snippet_begin(tm_tab_vt)
static tm_tab_vt *custom_tab_vt = &(tm_tab_vt){
    .name = TM_CUSTOM_TAB_VT_NAME,
    .name_hash = TM_CUSTOM_TAB_VT_NAME_HASH,
    .create_menu_name = tab__create_menu_name,
    .create = tab__create,
    .destroy = tab__destroy,
    .title = tab__title,
    .ui = tab__ui};
// #code_snippet_end(tm_tab_vt)
// #code_snippet_begin(tm_load_plugin)
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_global_api_registry = reg;

    tm_draw2d_api = tm_get_api(reg, tm_draw2d_api);
    tm_ui_api = tm_get_api(reg, tm_ui_api);
    tm_allocator_api = tm_get_api(reg, tm_allocator_api);

    tm_add_or_remove_implementation(reg, load, tm_tab_vt, custom_tab_vt);
}
// #code_snippet_end(tm_load_plugin)