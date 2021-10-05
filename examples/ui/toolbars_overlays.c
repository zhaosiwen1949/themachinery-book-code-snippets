static struct tm_api_registry_api *tm_global_api_registry;

static struct tm_draw2d_api *tm_draw2d_api;
static struct tm_ui_api *tm_ui_api;
static struct tm_allocator_api *tm_allocator_api;
static struct tm_temp_allocator_api *tm_temp_allocator_api;

#include <foundation/allocator.h>
#include <foundation/api_registry.h>
#include <foundation/temp_allocator.h>
#include <foundation/carray.inl>

#include <plugins/ui/docking.h>
#include <plugins/ui/draw2d.h>
#include <plugins/ui/ui.h>
#include <plugins/ui/ui_custom.h>
#include <plugins/ui/toolbar.h>

#include <the_machinery/the_machinery_tab.h>

#include <stdio.h>

#define TM_CUSTOM_TAB_VT_NAME "tm_custom_tab"
#define TM_CUSTOM_TAB_VT_NAME_HASH TM_STATIC_HASH("tm_custom_tab", 0xbc4e3e47fbf1cdc1ULL)

struct tm_tab_o
{
    tm_tab_i tm_tab_i;
    tm_allocator_i allocator;
};

static void tab__ui(tm_tab_o *tab, tm_ui_o *ui, const tm_ui_style_t *uistyle_in, tm_rect_t rect)
{
    tm_ui_buffers_t uib = tm_ui_api->buffers(ui);
    tm_ui_style_t *uistyle = (tm_ui_style_t[]){*uistyle_in};
    tm_draw2d_style_t *style = &(tm_draw2d_style_t){0};
    tm_ui_api->to_draw_style(ui, style, uistyle);

    style->color = (tm_color_srgb_t){.a = 255, .r = 255};
    tm_draw2d_api->fill_rect(uib.vbuffer, *uib.ibuffers, style, rect);
}

static const char *tab__create_menu_name(void)
{
    return "Custom Tab";
}

static const char *tab__title(tm_tab_o *tab, struct tm_ui_o *ui)
{
    return "Custom Tab";
}

static tm_tab_vt *vt = 0;

static tm_tab_i *tab__create(tm_tab_create_context_t *context, tm_ui_o *ui)
{
    tm_allocator_i allocator = tm_allocator_api->create_child(context->allocator, "Custom Tab");
    uint64_t *id = context->id;

    tm_tab_o *tab = tm_alloc(&allocator, sizeof(tm_tab_o));
    *tab = (tm_tab_o){
        .tm_tab_i = {
            .vt = vt,
            .inst = (tm_tab_o *)tab,
            .root_id = *id,
        },
        .allocator = allocator,
    };

    *id += 1000000;
    return &tab->tm_tab_i;
}

static void tab__destroy(tm_tab_o *tab)
{
    tm_allocator_i a = tab->allocator;
    tm_free(&a, tab, sizeof(*tab));
    tm_allocator_api->destroy_child(&a);
}

static tm_tab_vt *custom_tab_vt = &(tm_tab_vt){
    .name = TM_CUSTOM_TAB_VT_NAME,
    .name_hash = TM_CUSTOM_TAB_VT_NAME_HASH,
    .create_menu_name = tab__create_menu_name,
    .create = tab__create,
    .destroy = tab__destroy,
    .title = tab__title,
    .ui = tab__ui,
};

// -- toolbars

static tm_rect_t toolbar__ui(tm_toolbar_i *toolbar, struct tm_ui_o *ui, const struct tm_ui_style_t *uistyle, tm_rect_t toolbar_r, enum tm_toolbar_draw_mode dm)
{
    // ui code...
    return toolbar_r;
}

static struct tm_toolbar_i *tab__toolbars(tm_tab_o *tab, tm_temp_allocator_i *ta)
{
    struct tm_toolbar_i *toolbars = 0;
    tm_carray_temp_push(toolbars,
                        ((tm_toolbar_i){
                            .id = TM_STRHASH_U64(TM_STATIC_HASH("my_tab", 0x833aa53d363283b5ULL)),
                            .ui = toolbar__ui,
                            .draw_mode_mask = TM_TOOLBAR_DRAW_MODE_HORIZONTAL | TM_TOOLBAR_DRAW_MODE_VERTICAL,
                        }),
                        ta);
    return toolbars;
}

static tm_tab_vt *custom_tab_vt_toolbars = &(tm_tab_vt){
    .name = TM_CUSTOM_TAB_VT_NAME,
    .name_hash = TM_CUSTOM_TAB_VT_NAME_HASH,
    .create_menu_name = tab__create_menu_name,
    .create = tab__create,
    .destroy = tab__destroy,
    .title = tab__title,
    .ui = tab__ui,
    .toolbars = tab__toolbars, // we added this line
};

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_global_api_registry = reg;

    tm_draw2d_api = tm_get_api(reg, tm_draw2d_api);
    tm_ui_api = tm_get_api(reg, tm_ui_api);
    tm_allocator_api = tm_get_api(reg, tm_allocator_api);
    tm_temp_allocator_api = tm_get_api(reg, tm_temp_allocator_api);

    tm_add_or_remove_implementation(reg, load, tm_tab_vt, custom_tab_vt);
    tm_add_or_remove_implementation(reg, load, tm_tab_vt, custom_tab_vt_toolbars);
}