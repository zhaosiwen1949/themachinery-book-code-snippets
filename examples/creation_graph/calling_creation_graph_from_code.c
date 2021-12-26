static struct tm_allocator_api *tm_allocator_api;
static struct tm_api_registry_api *tm_api_registry_api;
static struct tm_creation_graph_api *tm_creation_graph_api;
static struct tm_entity_api *tm_entity_api;
static struct tm_render_graph_api *tm_render_graph_api;
static struct tm_shader_system_api *tm_shader_system_api;
static struct tm_the_truth_api *tm_the_truth_api;

#include <foundation/allocator.h>
#include <foundation/api_registry.h>
#include <foundation/macros.h>
#include <foundation/the_truth.h>

#include <plugins/creation_graph/creation_graph.h>
#include <plugins/creation_graph/creation_graph_output.inl>
#include <plugins/creation_graph/image_nodes.h>
#include <plugins/editor_views/graph.h>
#include <plugins/entity/entity.h>
#include <plugins/render_graph/render_graph.h>
#include <plugins/render_graph_toolbox/toolbox_common.h>
#include <plugins/renderer/render_backend.h>
#include <plugins/renderer/render_command_buffer.h>
#include <plugins/shader_system/shader_system.h>
#include <plugins/the_machinery_shared/component_interfaces/editor_ui_interface.h>
#include <plugins/the_machinery_shared/component_interfaces/shader_interface.h>
#include <plugins/the_machinery_shared/render_context.h>

#include <string.h>

#define TM_TT_TYPE__CREATION_GRAPH_TEST_COMPONENT "tm_creation_graph_test_component"

enum
{
    TM_TT_PROP__CREATION_GRAPH_TEST_COMPONENT__CREATION_GRAPH
};
// #code_snippet_begin(component)
typedef struct tm_component_t
{
    // The truth ID of the creation graph subobject.
    tm_tt_id_t creation_graph;
    // An instance of the `creation_graph` (created in `shader_ci__init`).
    tm_creation_graph_instance_t instance;

    // The handle to the output image.
    tm_renderer_handle_t image_handle;
    // The resource state of the output image.
    uint32_t resource_state;
    // The description of the output image.
    tm_renderer_image_desc_t desc;
    // The name of the output image.
    tm_strhash_t name;
} tm_component_t;
// #code_snippet_end(component)

typedef struct tm_component_manager_o
{
    tm_allocator_i allocator;
    tm_entity_context_o *ctx;
    tm_renderer_backend_i *rb;
} tm_component_manager_o;

// This function is called when the component is initialized,
// this happens at engine startup for the scene tab,
// at the start of the simulation for the simulate tab,
// or once an entity is selected for the preview tab.
static void shader_ci__init(tm_component_manager_o *manager, const tm_entity_t *entities, const uint32_t *entity_indices, void **data, uint32_t num_components)
{
    tm_component_t **cdata = (tm_component_t **)data;
    // #code_snippet_begin(init_phase)
    // Create the context for the creation graph, only the bare minimum is defined for this tutorial.
    // This is not production-level code.
    tm_creation_graph_context_t ctx = {
        .rb = manager->rb,
        .device_affinity_mask = TM_RENDERER_DEVICE_AFFINITY_MASK_ALL,
        .entity_ctx = manager->ctx,
        .tt = tm_entity_api->the_truth(manager->ctx)};

    for (uint32_t i = 0; i < num_components; ++i)
    {

        // Skip any component that don't have a creation graph defined.
        tm_component_t *cur = cdata[i];
        if (!cur->creation_graph.u64)
            continue;

        // Instantiate the creation graph if this is the first time.
        if (!cur->instance.graph.u64)
            cur->instance = tm_creation_graph_api->create_instance(ctx.tt, cur->creation_graph, &ctx);
        // #code_snippet_exclude_begin()
        // #code_snippet_begin(query_image)
        // Query the creation graph for image outputs, if non are defined then we skip the update step.
        tm_creation_graph_output_t image_outputs = tm_creation_graph_api->output(&cur->instance, TM_CREATION_GRAPH__IMAGE__OUTPUT_NODE_HASH, &ctx, NULL);
        if (image_outputs.num_output_objects > 0)
        {
            const tm_creation_graph_image_data_t *image_data = (const tm_creation_graph_image_data_t *)image_outputs.output;

            cur->image_handle = image_data->handle;
            cur->resource_state = image_data->resource_state;
            cur->desc = image_data->desc;
            cur->name = image_data->resource_name;
        }
        // #code_snippet_end(query_image)
        // #code_snippet_exclude_end(init_phase)
    }
    // #code_snippet_end(init_phase)
}

// This function is called every frame and allows us to update our shader variables.
static void shader_ci__update(tm_component_manager_o *manager, tm_render_args_t *args, const tm_entity_t *entities,
                              const struct tm_transform_component_t *transforms, const uint32_t *entity_indices, void **data,
                              uint32_t num_components, const uint8_t *frustum_visibilty)
{
    // #code_snippet_begin(itr)
    // Loop through all components until we find one that has a valid image output.
    uint32_t i;
    const tm_component_t **cdata = (const tm_component_t **)data;
    for (i = 0; i < num_components; ++i)
    {
        const tm_component_t *cur = cdata[i];
        if (!cur->image_handle.resource)
            continue;

        tm_render_graph_api->register_gpu_image(args->render_graph, cur->name, cur->image_handle, cur->resource_state, &cur->desc);
        break;
    }

    // None of the components had a valid image output, so skip the copy step.
    if (i == num_components)
        return;

    // Instead of making our own copy call, the debug visualization pass is used to copy to the viewport.
    // This is not a proper copy, but it's good enough for this tutorial.
    tm_render_graph_blackboard_value value;
    tm_render_graph_api->read_blackboard(args->render_graph, TM_STATIC_HASH("debug_visualization_resources", 0xd0d50436a0f3fcb9ULL), &value);
    tm_debug_visualization_resources_t *resources = (tm_debug_visualization_resources_t *)value.data;

    const uint32_t slot = resources->num_resources;
    resources->resources[slot].name = cdata[i]->name,
    resources->resources[slot].contents = CONTENT_COLOR_RGB;
    ++resources->num_resources;
    // #code_snippet_end(itr)
}

static void create_truth_types(struct tm_the_truth_o *tt)
{
    static tm_ci_editor_ui_i editor_aspect = {0};

    static tm_ci_shader_i shader_aspect = {
        .init = shader_ci__init,
        .update = shader_ci__update};
    // #code_snippet_begin(gc_asset)
    static const tm_the_truth_property_definition_t properties[] = {
        [TM_TT_PROP__CREATION_GRAPH_TEST_COMPONENT__CREATION_GRAPH] = {"creation_graph", TM_THE_TRUTH_PROPERTY_TYPE_SUBOBJECT, .type_hash = TM_TT_TYPE_HASH__CREATION_GRAPH}};
    // #code_snippet_end(gc_asset)
    const tm_tt_type_t component_type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__CREATION_GRAPH_TEST_COMPONENT, properties, TM_ARRAY_COUNT(properties));
    tm_creation_graph_api->create_truth_types(tt);
    tm_the_truth_api->set_default_object_to_create_subobjects(tt, component_type);

    // The editor aspect has to be defined if we want our component to be usable in the editor.
    // The shader aspect is used to update the creation graph and our final output.
    tm_tt_set_aspect(tt, component_type, tm_ci_editor_ui_i, &editor_aspect);
    tm_tt_set_aspect(tt, component_type, tm_ci_shader_i, &shader_aspect);
}

static bool component__load_asset(tm_component_manager_o *manager, struct tm_entity_commands_o *commands, tm_entity_t e, void *data, const tm_the_truth_o *tt, tm_tt_id_t asset)
{
    tm_component_t *c = data;
    tm_tt_id_t creation_graph = tm_the_truth_api->get_subobject(tt, tm_tt_read(tt, asset), TM_TT_PROP__CREATION_GRAPH_TEST_COMPONENT__CREATION_GRAPH);

    // We only want update if the creation graph has changed,
    // Note that we set the entire component to zero if this happens,
    // this is because all fields are dependent on the creation graph.
    if (c->creation_graph.u64 != creation_graph.u64)
    {
        memset(c, 0, sizeof(tm_component_t));
        c->creation_graph = creation_graph;
        return true;
    }

    return false;
}

static void component__create_manager(tm_entity_context_o *ctx)
{
    tm_allocator_i a;
    tm_entity_api->create_child_allocator(ctx, TM_TT_TYPE__CREATION_GRAPH_TEST_COMPONENT, &a);

    tm_renderer_backend_i *backend = tm_single_implementation(tm_api_registry_api, tm_renderer_backend_i);

    tm_component_manager_o *manager = tm_alloc(&a, sizeof(tm_component_manager_o));
    *manager = (tm_component_manager_o){
        .allocator = a,
        .ctx = ctx,
        .rb = backend,
    };

    const tm_component_i component = {
        .name = TM_TT_TYPE__CREATION_GRAPH_TEST_COMPONENT,
        .bytes = sizeof(tm_component_t),
        .manager = manager,
        .load_asset = component__load_asset,
    };

    tm_entity_api->register_component(ctx, &component);
}

TM_DLL_EXPORT void load_plugin(struct tm_api_registry_api *reg, bool load)
{

    tm_allocator_api = tm_get_api(reg, tm_allocator_api);
    tm_api_registry_api = tm_get_api(reg, tm_api_registry_api);
    tm_creation_graph_api = tm_get_api(reg, tm_creation_graph_api);
    tm_entity_api = tm_get_api(reg, tm_entity_api);
    tm_render_graph_api = tm_get_api(reg, tm_render_graph_api);
    tm_shader_system_api = tm_get_api(reg, tm_shader_system_api);
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);

    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, create_truth_types);
    tm_add_or_remove_implementation(reg, load, tm_entity_create_component_i, component__create_manager);
}
