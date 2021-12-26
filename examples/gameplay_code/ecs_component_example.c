// #code_snippet_begin(includes)
static struct tm_entity_api *tm_entity_api;
static struct tm_transform_component_api *tm_transform_component_api;
static struct tm_temp_allocator_api *tm_temp_allocator_api;
static struct tm_the_truth_api *tm_the_truth_api;
static struct tm_localizer_api *tm_localizer_api;

#include <plugins/entity/entity.h>
#include <plugins/entity/transform_component.h>
#include <plugins/the_machinery_shared/component_interfaces/editor_ui_interface.h>

#include <foundation/api_registry.h>
#include <foundation/carray.inl>
#include <foundation/localizer.h>
#include <foundation/math.inl>
#include <foundation/the_truth.h>
// #code_snippet_end(includes)
// #code_snippet_begin(comp_def)
#define TM_TT_TYPE__CUSTOM_COMPONENT "tm_custom_component"
#define TM_TT_TYPE_HASH__CUSTOM_COMPONENT TM_STATIC_HASH("tm_custom_component", 0x355309758b21930cULL)

enum
{
    TM_TT_PROP__CUSTOM_COMPONENT__FREQUENCY, // float
    TM_TT_PROP__CUSTOM_COMPONENT__AMPLITUDE, // float
};

struct tm_custom_component_t
{
    float y0;
    float frequency;
    float amplitude;
};
// #code_snippet_end(comp_def)
// #code_snippet_begin(comp_meta)
static const char *component__category(void)
{
    return TM_LOCALIZE("Samples");
}

static tm_ci_editor_ui_i *editor_aspect = &(tm_ci_editor_ui_i){
    .category = component__category};
// #code_snippet_end(comp_meta)
// #code_snippet_begin(truth__create_types)
static void truth__create_types(struct tm_the_truth_o *tt)
{
    tm_the_truth_property_definition_t custom_component_properties[] = {
        [TM_TT_PROP__CUSTOM_COMPONENT__FREQUENCY] = {"frequency", TM_THE_TRUTH_PROPERTY_TYPE_FLOAT},
        [TM_TT_PROP__CUSTOM_COMPONENT__AMPLITUDE] = {"amplitude", TM_THE_TRUTH_PROPERTY_TYPE_FLOAT},
    };

    const tm_tt_type_t custom_component_type = tm_the_truth_api->create_object_type(tt, TM_TT_TYPE__CUSTOM_COMPONENT, custom_component_properties, TM_ARRAY_COUNT(custom_component_properties));
    const tm_tt_id_t default_object = tm_the_truth_api->quick_create_object(tt, TM_TT_NO_UNDO_SCOPE, TM_TT_TYPE_HASH__CUSTOM_COMPONENT, TM_TT_PROP__CUSTOM_COMPONENT__FREQUENCY, 1.0f, TM_TT_PROP__CUSTOM_COMPONENT__AMPLITUDE, 1.0f, -1);
    tm_the_truth_api->set_default_object(tt, custom_component_type, default_object);
    tm_tt_set_aspect(tt, custom_component_type, tm_ci_editor_ui_i, editor_aspect);
}
// #code_snippet_end(truth__create_types)
// #code_snippet_begin(component__load_asset)
static bool component__load_asset(tm_component_manager_o *man, struct tm_entity_commands_o *commands, tm_entity_t e, void *c_vp, const tm_the_truth_o *tt, tm_tt_id_t asset)
{
    struct tm_custom_component_t *c = c_vp;
    const tm_the_truth_object_o *asset_r = tm_tt_read(tt, asset);
    c->y0 = 0;
    c->frequency = tm_the_truth_api->get_float(tt, asset_r, TM_TT_PROP__CUSTOM_COMPONENT__FREQUENCY);
    c->amplitude = tm_the_truth_api->get_float(tt, asset_r, TM_TT_PROP__CUSTOM_COMPONENT__AMPLITUDE);
    return true;
}
// #code_snippet_end(component__load_asset)
// #code_snippet_begin(component__create)
static void component__create(struct tm_entity_context_o *ctx)
{
    tm_component_i component = {
        .name = TM_TT_TYPE__CUSTOM_COMPONENT,
        .bytes = sizeof(struct tm_custom_component_t),
        .load_asset = component__load_asset,
    };

    tm_entity_api->register_component(ctx, &component);
}
// #code_snippet_end(component__create)
// #code_snippet_begin(update)
// Runs on (custom_component, transform_component)
static void engine_update__custom_component(tm_engine_o *inst, tm_engine_update_set_t *data, struct tm_entity_commands_o *commands)
{
    TM_INIT_TEMP_ALLOCATOR(ta);

    tm_entity_t *mod_transform = 0;

    struct tm_entity_context_o *ctx = (struct tm_entity_context_o *)inst;
    // #code_snippet_begin(blackboard)
    double t = 0;
    for (const tm_entity_blackboard_value_t *bb = data->blackboard_start; bb != data->blackboard_end; ++bb)
    {
        if (TM_STRHASH_EQUAL(bb->id, TM_ENTITY_BB__TIME))
            t = bb->double_value;
    }
    // #code_snippet_end(blackboard)
    // #code_snippet_begin(for)
    for (tm_engine_update_array_t *a = data->arrays; a < data->arrays + data->num_arrays; ++a)
    {
        struct tm_custom_component_t *custom_component = a->components[0];
        tm_transform_component_t *transform = a->components[1];

        for (uint32_t i = 0; i < a->n; ++i)
        {
            if (!custom_component[i].y0)
                custom_component[i].y0 = transform[i].world.pos.y;
            const float y = custom_component[i].y0 + custom_component[i].amplitude * sinf((float)t * custom_component[i].frequency);

            transform[i].world.pos.y = y;
            ++transform[i].version;
            tm_carray_temp_push(mod_transform, a->entities[i], ta);
        }
    }
    // #code_snippet_end(for)
    // #code_snippet_begin(notify)
    tm_entity_api->notify(ctx, data->engine->components[1], mod_transform, (uint32_t)tm_carray_size(mod_transform));
    // #code_snippet_end(notify)
    TM_SHUTDOWN_TEMP_ALLOCATOR(ta);
}
// #code_snippet_end(update)
// #code_snippet_begin(engine_filter)
static bool engine_filter__custom_component(tm_engine_o *inst, const tm_component_type_t *components, uint32_t num_components, const tm_component_mask_t *mask)
{
    return tm_entity_mask_has_component(mask, components[0]) && tm_entity_mask_has_component(mask, components[1]);
}
// #code_snippet_end(engine_filter)
// #code_snippet_begin(register_engine)
static void component__register_engine(struct tm_entity_context_o *ctx)
{
    const tm_component_type_t custom_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__CUSTOM_COMPONENT);
    const tm_component_type_t transform_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);

    const tm_engine_i custom_component_engine = {
        .ui_name = "Custom Component",
        .hash = TM_STATIC_HASH("CUSTOM_COMPONENT", 0xe093a8316a6c2d29ULL),
        .num_components = 2,
        .components = {custom_component, transform_component},
        .writes = {false, true},
        .update = engine_update__custom_component,
        .filter = engine_filter__custom_component,
        .inst = (tm_engine_o *)ctx,
    };
    tm_entity_api->register_engine(ctx, &custom_component_engine);
}
// #code_snippet_end(register_engine)

// #code_snippet_begin(load_plugin)
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_entity_api = tm_get_api(reg, tm_entity_api);
    tm_transform_component_api = tm_get_api(reg, tm_transform_component_api);
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_temp_allocator_api = tm_get_api(reg, tm_temp_allocator_api);
    tm_localizer_api = tm_get_api(reg, tm_localizer_api);

    tm_add_or_remove_implementation(reg, load, tm_the_truth_create_types_i, truth__create_types);
    tm_add_or_remove_implementation(reg, load, tm_entity_create_component_i, component__create);
    tm_add_or_remove_implementation(reg, load, tm_entity_register_engines_simulation_i, component__register_engine);
}
// #code_snippet_end(load_plugin)