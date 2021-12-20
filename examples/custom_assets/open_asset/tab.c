static struct tm_api_registry_api *tm_global_api_registry;

static struct tm_ui_api *tm_ui_api;
static struct tm_temp_allocator_api *tm_temp_allocator_api;

extern struct tm_the_truth_api *tm_the_truth_api;
static struct tm_localizer_api *tm_localizer_api;

#include <foundation/allocator.h>
#include <foundation/api_registry.h>
#include <foundation/string.inl>
#include <foundation/temp_allocator.h>
#include <foundation/the_truth.h>

#include <plugins/ui/docking.h>
#include <plugins/ui/ui.h>

#include <the_machinery/the_machinery_tab.h>

#include "txt.h"

// #code_snippet_begin(tm_tab_o)
struct tm_tab_o
{
    tm_tab_i tm_tab_i;
    tm_allocator_i *allocator;

    tm_tt_id_t asset;
    tm_the_truth_o *tt;
};
// #code_snippet_end(tm_tab_o)
// #code_snippet_begin(tab_ui)
// #code_snippet_begin(tab_ui_style)
// #code_snippet_begin(tab_ui_alloc)
static void tab__ui(tm_tab_o *tab, tm_ui_o *ui, const tm_ui_style_t *uistyle_in, tm_rect_t rect)
{
    tm_ui_style_t *uistyle = (tm_ui_style_t[]){*uistyle_in};
    if (tab->asset.u64)
    {
        // #code_snippet_exclude_begin(tab_ui_style)
        TM_INIT_TEMP_ALLOCATOR(ta);
        // #code_snippet_exclude_begin(tab_ui_alloc)
        // #code_snippet_begin(tab_ui_buffer)
        tm_tt_buffer_t buffer = tm_the_truth_api->get_buffer(tab->tt, tm_tt_read(tab->tt, tab->asset), TM_TT_PROP__MY_ASSET__DATA);
        char *content = tm_temp_alloc(ta, buffer.size + 1);
        // #code_snippet_begin(tm_strncpy_safe)
        tm_strncpy_safe(content, buffer.data, buffer.size);
        // #code_snippet_end(tm_strncpy_safe)
        // #code_snippet_end(tab_ui_buffer)
        // #code_snippet_begin(wrapped_text)
        tm_ui_text_t *text = &(tm_ui_text_t){.text = content, .rect = rect};
        tm_ui_api->wrapped_text(ui, uistyle, text);
        // #code_snippet_end(wrapped_text)
        // #code_snippet_exclude_end(tab_ui_alloc)
        TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
        // #code_snippet_exclude_end(tab_ui_style)
    }
    else
    {
        rect.h = 20;
        tm_ui_text_t *text = &(tm_ui_text_t){.align = TM_UI_ALIGN_CENTER, .text = "Please open a .txt asset.", .rect = rect};
        tm_ui_api->text(ui, uistyle, text);
    }
}
// #code_snippet_end(tab_ui)
// #code_snippet_end(tab_ui_alloc)
// #code_snippet_end(tab_ui_style)
// #code_snippet_begin(tab_create)
static const char *tab__create_menu_name(void)
{
    return "Text Tab";
}

static const char *tab__title(tm_tab_o *tab, struct tm_ui_o *ui)
{
    return "Text Tab";
}

static tm_tab_vt *tab_vt;

static tm_tab_i *tab__create(tm_tab_create_context_t *context, tm_ui_o *ui)
{
    tm_allocator_i *allocator = context->allocator;
    uint64_t *id = context->id;
    // #code_snippet_begin(tm_tab_o_def)
    tm_tab_o *tab = tm_alloc(allocator, sizeof(tm_tab_o));
    *tab = (tm_tab_o){
        .tm_tab_i = {
            .vt = (tm_tab_vt *)tab_vt,
            .inst = (tm_tab_o *)tab,
            .root_id = *id,
        },
        .allocator = allocator,
    };
    // #code_snippet_end(tm_tab_o_def)

    *id += 1000000;
    return &tab->tm_tab_i;
}
// #code_snippet_end(tab_create)

static void tab__destroy(tm_tab_o *tab)
{
    tm_free(tab->allocator, tab, sizeof(*tab));
}
// #code_snippet_begin(root)
void tab__set_root(tm_tab_o *inst, struct tm_the_truth_o *tt, tm_tt_id_t root)
{
    inst->asset = root;
    inst->tt = tt;
}

static tm_tab_vt_root_t tab__root(tm_tab_o *tab)
{
    return (tm_tab_vt_root_t){tab->tt, tab->asset};
}
// #code_snippet_end(root)

static tm_tab_vt *tab_vt = &(tm_tab_vt){
    .name = TM_TXT_TAB_VT_NAME,
    .name_hash = TM_TXT_TAB_VT_NAME_HASH,
    .create_menu_name = tab__create_menu_name,
    .create = tab__create,
    .destroy = tab__destroy,
    .title = tab__title,
    .ui = tab__ui,
    .set_root = tab__set_root,
    .root = tab__root,
};

void load_txt_tab(struct tm_api_registry_api *reg, bool load)
{
    // #code_snippet_begin(load_txt_tab)
    tm_global_api_registry = reg;

    tm_ui_api = tm_get_api(reg, tm_ui_api);
    tm_temp_allocator_api = tm_get_api(reg, tm_temp_allocator_api);

    tm_add_or_remove_implementation(reg, load, tm_tab_vt, tab_vt);
    // #code_snippet_end(load_txt_tab)
}