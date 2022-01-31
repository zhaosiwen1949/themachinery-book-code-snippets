# themachinery-book-code-snippets
Repo holding all snippets of the machinery book. They are being used like this within the book:

### When adding source code snippets:

Please when you intend to add source code snippets please push them first (or make a PR) to the https://github.com/OurMachinery/themachinery-book-code-snippets. After that you can make use of the auto include preprocessor:

```
{{insert_code(env.TM_BOOK_CODE_SNIPPETS/gameplay_code/ecs_component_example.c,tag_name)}} // will include what ever is in this context
{{insert_code(env.TM_BOOK_CODE_SNIPPETS/gameplay_code/ecs_component_example.c)}} // will include the whole file
{{insert_code(env.TM_BOOK_CODE_SNIPPETS/gameplay_code/ecs_component_example.c,tag,off)}} // ignores all exclude statements
```

in code they are defined as following:

**begin context and end context:**
```c
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
```
**Contextual exclude:**

> *Note*: If the context (tag) `tab__toolbars` shall include all (also whats excluded) one can call: `{{insert_code(env.TM_BOOK_CODE_SNIPPETS/toolbar/toolbar_example.c,tab__toolbars,off)}}`

```c
// #code_snippet_begin(tab__toolbars)
static struct tm_toolbar_i *tab__toolbars(tm_tab_o *tab, tm_temp_allocator_i *ta)
{
    // #code_snippet_exclude_begin(tab__toolbars)
    struct tm_toolbar_i *toolbars = 0;
    tm_carray_temp_push(toolbars,
                        ((tm_toolbar_i){
                            .id = TM_STRHASH_U64(TM_STATIC_HASH("my_tab", 0x833aa53d363283b5ULL)),
                            .ui = toolbar__ui,
                            .draw_mode_mask = TM_TOOLBAR_DRAW_MODE_HORIZONTAL | TM_TOOLBAR_DRAW_MODE_VERTICAL,
                        }),
                        ta);
    return toolbars;
    // #code_snippet_exclude_end(tab__toolbars)
}
// #code_snippet_end(tab__toolbars)
```

> **Note**: You can also have a none contextual exclude: `// #code_snippet_exclude_end()`.
