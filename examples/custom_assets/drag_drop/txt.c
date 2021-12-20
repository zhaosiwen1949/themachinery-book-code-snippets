// -- api's
static struct tm_api_registry_api *tm_global_api_registry;
static struct tm_the_truth_api *tm_the_truth_api;
static struct tm_properties_view_api *tm_properties_view_api;
static struct tm_os_api *tm_os_api;
static struct tm_path_api *tm_path_api;
static struct tm_temp_allocator_api *tm_temp_allocator_api;
static struct tm_logger_api *tm_logger_api;
static struct tm_localizer_api *tm_localizer_api;
static struct tm_asset_io_api *tm_asset_io_api;
static struct tm_task_system_api *task_system;
static struct tm_allocator_api *tm_allocator_api;
static struct tm_sprintf_api *tm_sprintf_api;

// important fro drag and drop
static struct tm_entity_api *tm_entity_api;
static struct tm_scene_common_api *tm_scene_common_api;

// -- inlcudes

#include <foundation/api_registry.h>
#include <foundation/asset_io.h>
#include <foundation/buffer.h>
#include <foundation/carray_print.inl>
#include <foundation/localizer.h>
#include <foundation/log.h>
#include <foundation/macros.h>
#include <foundation/os.h>
#include <foundation/path.h>
#include <foundation/sprintf.h>
#include <foundation/string.inl>
#include <foundation/task_system.h>
#include <foundation/temp_allocator.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_assets.h>
#include <foundation/undo.h>

#include <plugins/editor_views/asset_browser.h>
#include <plugins/editor_views/properties.h>
#include <plugins/entity/entity.h>
// #code_snippet_begin(scene_api_def)
#include <plugins/the_machinery_shared/asset_aspects.h>
// #code_snippet_end(scene_api_def)
#include <plugins/the_machinery_shared/component_interfaces/editor_ui_interface.h>

#include <plugins/entity/transform_component.h>
#include <plugins/the_machinery_shared/scene_common.h>

#include "txt.h"
// -- struct definitions
struct task__import_txt
{
    uint64_t bytes;
    struct tm_asset_io_import args;
    char file[8];
};
/////
// -- functions:
////
// --- importer
static void task__import_txt(void *data, uint64_t task_id)
{
    struct task__import_txt *task = (struct task__import_txt *)data;
    const struct tm_asset_io_import *args = &task->args;
    const char *txt_file = task->file;
    tm_the_truth_o *tt = args->tt;
    tm_file_stat_t stat = tm_os_api->file_system->stat(txt_file);
    if (stat.exists)
    {
        tm_buffers_i *buffers = tm_the_truth_api->buffers(tt);
        void *buffer = buffers->allocate(buffers->inst, stat.size, false);
        tm_file_o f = tm_os_api->file_io->open_input(txt_file);
        const int64_t read = tm_os_api->file_io->read(f, buffer, stat.size);
        tm_os_api->file_io->close(f);
        if (read == (int64_t)stat.size)
        {
            const uint32_t buffer_id = buffers->add(buffers->inst, buffer, stat.size, 0);
            const tm_tt_type_t plugin_asset_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__MY_ASSET);
            const tm_tt_id_t asset_id = tm_the_truth_api->create_object_of_type(tt, plugin_asset_type, TM_TT_NO_UNDO_SCOPE);
            tm_the_truth_object_o *asset_obj = tm_the_truth_api->write(tt, asset_id);
            tm_the_truth_api->set_buffer(tt, asset_obj, TM_TT_PROP__MY_ASSET__DATA, buffer_id);
            tm_the_truth_api->set_string(tt, asset_obj, TM_TT_PROP__MY_ASSET__FILE, txt_file);
            if (args->reimport_into.u64)
            {
                tm_the_truth_api->retarget_write(tt, asset_obj, args->reimport_into);
                tm_the_truth_api->commit(tt, asset_obj, args->undo_scope);
                tm_the_truth_api->destroy_object(tt, asset_id, args->undo_scope);
            }
            else
            {
                tm_the_truth_api->commit(tt, asset_obj, args->undo_scope);
                const char *asset_name = tm_path_api->base(tm_str(txt_file)).data;
                struct tm_asset_browser_add_asset_api *add_asset = tm_get_api(tm_global_api_registry, tm_asset_browser_add_asset_api);
                const tm_tt_id_t current_dir = add_asset->current_directory(add_asset->inst, args->ui);
                const bool should_select = args->asset_browser.u64 && tm_the_truth_api->version(tt, args->asset_browser) == args->asset_browser_version_at_start;
                add_asset->add(add_asset->inst, current_dir, asset_id, asset_name, args->undo_scope, should_select, args->ui, 0, 0);
            }
        }
        else
        {
            tm_logger_api->printf(TM_LOG_TYPE_INFO, "import txt:cound not read %s\n", txt_file);
        }
    }
    else
    {
        tm_logger_api->printf(TM_LOG_TYPE_INFO, "import txt:cound not find %s \n", txt_file);
    }
    tm_free(args->allocator, task, task->bytes);
}

static bool asset_io__enabled(struct tm_asset_io_o *inst)
{
    return true;
}
static bool asset_io__can_import(struct tm_asset_io_o *inst, const char *extension)
{
    return tm_strcmp_ignore_case(extension, "txt") == 0;
}
static bool asset_io__can_reimport(struct tm_asset_io_o *inst, struct tm_the_truth_o *tt, tm_tt_id_t asset)
{
    const tm_tt_id_t object = tm_the_truth_api->get_subobject(tt, tm_tt_read(tt, asset), TM_TT_PROP__ASSET__OBJECT);
    return object.type == tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__MY_ASSET).u64;
}
static void asset_io__importer_extensions_string(struct tm_asset_io_o *inst, char **output, struct tm_temp_allocator_i *ta, const char *separator)
{
    tm_carray_temp_printf(output, ta, "txt");
}
static void asset_io__importer_description_string(struct tm_asset_io_o *inst, char **output, struct tm_temp_allocator_i *ta, const char *separator)
{
    tm_carray_temp_printf(output, ta, ".txt");
}
static uint64_t asset_io__import_asset(struct tm_asset_io_o *inst, const char *file, const struct tm_asset_io_import *args)
{
    const uint64_t bytes = sizeof(struct task__import_txt) + strlen(file);
    struct task__import_txt *task = tm_alloc(args->allocator, bytes);
    *task = (struct task__import_txt){
        .bytes = bytes,
        .args = *args,
    };
    strcpy(task->file, file);
    return task_system->run_task(task__import_txt, task, "Import Text File");
}
static struct tm_asset_io_i txt_asset_io = {
    .enabled = asset_io__enabled,
    .can_import = asset_io__can_import,
    .can_reimport = asset_io__can_reimport,
    .importer_extensions_string = asset_io__importer_extensions_string,
    .importer_description_string = asset_io__importer_description_string,
    .import_asset = asset_io__import_asset};

// -- asset on its own

//custom ui
static float properties__custom_ui(struct tm_properties_ui_args_t *args, tm_rect_t item_rect, tm_tt_id_t object, uint32_t indent)
{
    tm_the_truth_o *tt = args->tt;
    bool picked = false;
    item_rect.y = tm_properties_view_api->ui_open_path(args, item_rect, TM_LOCALIZE_LATER("Import Path"), TM_LOCALIZE_LATER("Path that the text file was imported from."), object, TM_TT_PROP__MY_ASSET__FILE, "txt", "text files", &picked);
    if (picked)
    {
        const char *file = tm_the_truth_api->get_string(tt, tm_tt_read(tt, object), TM_TT_PROP__MY_ASSET__FILE);
        {
            tm_allocator_i *allocator = tm_allocator_api->system;
            const uint64_t bytes = sizeof(struct task__import_txt) + strlen(file);
            struct task__import_txt *task = tm_alloc(allocator, bytes);
            *task = (struct task__import_txt){
                .bytes = bytes,
                .args = {
                    .allocator = allocator,
                    .tt = tt,
                    .reimport_into = object}};
            strcpy(task->file, file);
            task_system->run_task(task__import_txt, task, "Import Text File");
        }
    }
    return item_rect.y;
}
// #code_snippet_begin(component_custom_ui)
static float properties__component_custom_ui(struct tm_properties_ui_args_t *args, tm_rect_t item_rect, tm_tt_id_t object, uint32_t indent)
{
    TM_INIT_TEMP_ALLOCATOR(ta);
    tm_tt_type_t asset_type = tm_the_truth_api->object_type_from_name_hash(args->tt, TM_TT_TYPE_HASH__ASSET);
    tm_tt_id_t *ids = tm_the_truth_api->all_objects_of_type(args->tt, tm_the_truth_api->object_type_from_name_hash(args->tt, TM_TT_TYPE_HASH__MY_ASSET), ta);
    tm_tt_id_t *items = 0;
    const char **names = 0;
    tm_carray_temp_push(names, "Select", ta);
    tm_carray_temp_push(items, (tm_tt_id_t){0}, ta);
    for (uint32_t i = 0; i < tm_carray_size(ids); ++i)
    {
        tm_tt_id_t owner = tm_the_truth_api->owner(args->tt, ids[i]);
        if (tm_tt_type(owner).u64 == asset_type.u64)
        {
            tm_carray_temp_push(names, tm_the_truth_api->get_string(args->tt, tm_tt_read(args->tt, owner), TM_TT_PROP__ASSET__NAME), ta);
            tm_carray_temp_push(items, ids[i], ta);
        }
    }
    item_rect.y = tm_properties_view_api->ui_reference_popup_picker(args, item_rect, "Asset", NULL, object, TM_TT_PROP__STORY_COMPONENT__ASSET, names, items, (uint32_t)tm_carray_size(items));
    TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
    return item_rect.y;
}
// #code_snippet_end(component_custom_ui)
// #code_snippet_begin(droppable)
bool droppable(struct tm_asset_scene_o *inst, struct tm_the_truth_o *tt, tm_tt_id_t asset)
{
    return true;
}
// #code_snippet_end(droppable)
// #code_snippet_begin(create_entity_fn)
// #code_snippet_begin(create_entity)
// #code_snippet_begin(create_entity_transform)
// #code_snippet_begin(create_entity_story)
tm_tt_id_t create_entity(struct tm_asset_scene_o *inst, struct tm_the_truth_o *tt,
                         tm_tt_id_t asset, const char *name, const tm_transform_t *local_transform,
                         tm_tt_id_t parent_entity, tm_tt_id_t asset_root, struct tm_undo_stack_i *undo_stack)
{
    const tm_tt_undo_scope_t undo_scope = tm_the_truth_api->create_undo_scope(tt, TM_LOCALIZE("Create Entity From Creation Graph"));
    const tm_tt_type_t entity_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__ENTITY);
    const tm_tt_id_t entity = tm_the_truth_api->create_object_of_type(tt, entity_type, undo_scope);
    tm_the_truth_object_o *entity_w = tm_the_truth_api->write(tt, entity);
    tm_the_truth_api->set_string(tt, entity_w, TM_TT_PROP__ENTITY__NAME, name);
    // #code_snippet_end(create_entity)
    // add transform:
    {
        const tm_tt_type_t transform_component_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);
        const tm_tt_id_t component = tm_the_truth_api->create_object_of_type(tt, transform_component_type, undo_scope);
        tm_the_truth_object_o *component_w = tm_the_truth_api->write(tt, component);
        tm_the_truth_api->add_to_subobject_set(tt, entity_w, TM_TT_PROP__ENTITY__COMPONENTS, &component_w, 1);
        tm_the_truth_api->commit(tt, component_w, undo_scope);
    }
    // #code_snippet_end(create_entity_transform)
    // add story:
    {
        tm_tt_type_t asset_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__STORY_COMPONENT);
        const tm_tt_id_t component = tm_the_truth_api->create_object_of_type(tt, asset_type, undo_scope);
        tm_the_truth_object_o *component_w = tm_the_truth_api->write(tt, component);
        tm_the_truth_api->set_reference(tt, component_w, TM_TT_PROP__STORY_COMPONENT__ASSET, asset);
        tm_the_truth_api->add_to_subobject_set(tt, entity_w, TM_TT_PROP__ENTITY__COMPONENTS, &component_w, 1);
        tm_the_truth_api->commit(tt, component_w, undo_scope);
    }
    // #code_snippet_end(create_entity_story)

    tm_the_truth_api->commit(tt, entity_w, undo_scope);

    tm_scene_common_api->place_entity(tt, entity, local_transform, parent_entity, undo_scope);

    undo_stack->add(undo_stack->inst, tt, undo_scope);

    return entity;
}
// #code_snippet_end(create_entity_fn)
// #code_snippet_begin(scene_api_def)
tm_asset_scene_api scene_api = {
    .droppable = droppable,
    .create_entity = create_entity,
};
// #code_snippet_end(scene_api_def)
// #code_snippet_begin(comp_meta)
static const char *component__category(void)
{
    return TM_LOCALIZE("Story");
}

static tm_ci_editor_ui_i *editor_aspect = &(tm_ci_editor_ui_i){
    .category = component__category};
// #code_snippet_end(comp_meta)
// -- create truth type
// #code_snippet_begin(create_truth_types)
static void create_truth_types(struct tm_the_truth_o *tt)
{
    static tm_the_truth_property_definition_t my_asset_properties[] = {
        {"import_path", TM_THE_TRUTH_PROPERTY_TYPE_STRING},
        {"data", TM_THE_TRUTH_PROPERTY_TYPE_BUFFER},
    };
    const tm_tt_type_t type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__MY_ASSET, my_asset_properties, TM_ARRAY_COUNT(my_asset_properties));
    tm_the_truth_api->set_aspect(tt, type, TM_TT_ASPECT__FILE_EXTENSION, "txt");
    static tm_properties_aspect_i properties_aspect = {
        .custom_ui = properties__custom_ui,
    };
    tm_the_truth_api->set_aspect(tt, type, TM_TT_ASPECT__PROPERTIES, &properties_aspect);
    // #code_snippet_exclude_begin()
    tm_the_truth_api->set_aspect(tt, type, TM_TT_ASPECT__ASSET_SCENE, &scene_api); // <<---
    // #code_snippet_exclude_end()
    // #code_snippet_begin(none_asset_picker)
    tm_the_truth_property_definition_t story_component_properties[] = {
        [TM_TT_PROP__STORY_COMPONENT__ASSET] = {"story_asset", .type = TM_THE_TRUTH_PROPERTY_TYPE_REFERENCE, .type_hash = TM_TT_TYPE_HASH__MY_ASSET}};
    // #code_snippet_end(none_asset_picker)

    // #code_snippet_exclude_begin()
    // #code_snippet_begin(story_component)
    static tm_properties_aspect_i properties_component_aspect = {
        .custom_ui = properties__component_custom_ui,
    };

    const tm_tt_type_t story_component_type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__STORY_COMPONENT, story_component_properties, TM_ARRAY_COUNT(story_component_properties));
    tm_the_truth_api->set_aspect(tt, story_component_type, TM_CI_EDITOR_UI, editor_aspect);
    tm_the_truth_api->set_aspect(tt, story_component_type, TM_TT_ASPECT__PROPERTIES, &properties_component_aspect);
    // #code_snippet_end(story_component)
    // #code_snippet_exclude_end()
}
// #code_snippet_end(create_truth_types)
// -- asset browser regsiter interface
static tm_tt_id_t asset_browser_create(struct tm_asset_browser_create_asset_o *inst, tm_the_truth_o *tt, tm_tt_undo_scope_t undo_scope)
{
    const tm_tt_type_t type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__MY_ASSET);
    return tm_the_truth_api->create_object_of_type(tt, type, undo_scope);
}
static tm_asset_browser_create_asset_i asset_browser_create_my_asset = {
    .menu_name = TM_LOCALIZE_LATER("New Text File"),
    .asset_name = TM_LOCALIZE_LATER("New Text File"),
    .create = asset_browser_create,
};
// #code_snippet_begin(component_def)
// #code_snippet_begin(component_manager)
struct tm_component_manager_o
{
    tm_entity_context_o *ctx;
    tm_allocator_i allocator;
};
// #code_snippet_end(component_manager)
// #code_snippet_begin(component__load_asset)
static bool component__load_asset(tm_component_manager_o *man, struct tm_entity_commands_o *commands, tm_entity_t e, void *c_vp, const tm_the_truth_o *tt, tm_tt_id_t asset)
{
    struct tm_story_component_t *c = c_vp;
    const tm_the_truth_object_o *asset_r = tm_tt_read(tt, asset);
    tm_tt_id_t id = tm_the_truth_api->get_reference(tt, asset_r, TM_TT_PROP__STORY_COMPONENT__ASSET);
    if (id.u64)
    {
        tm_tt_buffer_t buffer = tm_the_truth_api->get_buffer(tt, tm_tt_read(tt, id), TM_TT_PROP__MY_ASSET__DATA);
        c->text = tm_alloc(&man->allocator, buffer.size);
        c->size = buffer.size;
        memcpy(c->text, buffer.data, buffer.size);
    }
    return true;
}
// #code_snippet_end(component__load_asset)
// #code_snippet_begin(component__remove_destroy)
static void component__remove(tm_component_manager_o *manager, struct tm_entity_commands_o *commands, tm_entity_t e, void *data)
{
    tm_story_component_t *sc = (tm_story_component_t *)data;
    tm_free(&manager->allocator, sc->text, sc->size);
}

static void component__destroy(tm_component_manager_o *manager)
{
    tm_entity_api->call_remove_on_all_entities(manager->ctx, tm_entity_api->lookup_component_type(manager->ctx, TM_TT_TYPE_HASH__STORY_COMPONENT));
    // Free the actual manager struct and the allocator used to allocate it.
    tm_entity_context_o *ctx = manager->ctx;
    tm_allocator_i allocator = manager->allocator;
    tm_free(&allocator, manager, sizeof(tm_component_manager_o));
    tm_entity_api->destroy_child_allocator(ctx, &allocator);
}
// #code_snippet_end(component__remove_destroy)
// #code_snippet_begin(component_manager)
static void component__create(struct tm_entity_context_o *ctx)
{
    // Allocate a new manager for this component type (freed in component__destroy).
    tm_allocator_i allocator;
    tm_entity_api->create_child_allocator(ctx, TM_TT_TYPE__STORY_COMPONENT, &allocator);
    tm_component_manager_o *story_manager = tm_alloc(&allocator, sizeof(tm_component_manager_o));

    *story_manager = (tm_component_manager_o){
        .ctx = ctx,
        .allocator = allocator};

    tm_component_i component = {
        .name = TM_TT_TYPE__STORY_COMPONENT,
        .bytes = sizeof(struct tm_story_component_t),
        .load_asset = component__load_asset,
        .destroy = component__destroy,
        .remove = component__remove,
        .manager = (tm_component_manager_o *)story_manager};
    tm_entity_api->register_component(ctx, &component);
};
// #code_snippet_end(component_manager)
// #code_snippet_end(component_def)
// #code_snippet_begin(plugin_load)
// -- load plugin
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_properties_view_api = tm_get_api(reg, tm_properties_view_api);
    tm_os_api = tm_get_api(reg, tm_os_api);
    tm_path_api = tm_get_api(reg, tm_path_api);
    tm_temp_allocator_api = tm_get_api(reg, tm_temp_allocator_api);
    tm_allocator_api = tm_get_api(reg, tm_allocator_api);
    tm_logger_api = tm_get_api(reg, tm_logger_api);
    tm_localizer_api = tm_get_api(reg, tm_localizer_api);
    tm_asset_io_api = tm_get_api(reg, tm_asset_io_api);
    task_system = tm_get_api(reg, tm_task_system_api);
    tm_sprintf_api = tm_get_api(reg, tm_sprintf_api);
    tm_entity_api = tm_get_api(reg, tm_entity_api);
    tm_scene_common_api = tm_get_api(reg, tm_scene_common_api);
    tm_global_api_registry = reg;
    if (load)
        tm_asset_io_api->add_asset_io(&txt_asset_io);
    else
        tm_asset_io_api->remove_asset_io(&txt_asset_io);

    tm_add_or_remove_implementation(reg, load, tm_entity_create_component_i, component__create);
    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
    tm_add_or_remove_implementation(reg, load, tm_asset_browser_create_asset_i, &asset_browser_create_my_asset);
}
// #code_snippet_end(plugin_load)

// #code_snippet_begin(create_truth_types_modify)
// -- create truth type
static void create_truth_types_modify(struct tm_the_truth_o *tt)
{
    tm_tt_type_t asset_type = tm_the_truth_api->object_type_from_name_hash(tt, TM_TT_TYPE_HASH__MY_ASSET);
    if (asset_type.u64)
    {
        if (tm_the_truth_api->get_aspect(tt, asset_type, TM_TT_ASPECT__ASSET_SCENE))
            tm_the_truth_api->set_aspect(tt, asset_type, TM_TT_ASPECT__ASSET_SCENE, &scene_api);
    }
}
// #code_snippet_end(create_truth_types_modify)