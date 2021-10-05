static struct tm_entity_api *tm_entity_api;
// beginning of the source file
#include <foundation/api_registry.h>

#include <plugins/entity/entity.h>

#include "esc_component.h"

static void movement_update(tm_engine_o *inst, tm_engine_update_set_t *data, struct tm_entity_commands_o *commands)
{
    //...
}

static void entity_register_engines_i(struct tm_entity_context_o *ctx)
{

    tm_component_type_t keyboard_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__KEYBOARD_COMPONENT);
    tm_component_type_t movement_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__MOVEMENT_COMPONENT);
    tm_component_type_t transform_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);
    tm_component_type_t mover_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__MOVER_COMPONENT);

    const tm_engine_i movement_engine = {
        .ui_name = "movement_engine",
        .hash = TM_STATIC_HASH("movement_engine", 0x336880a23d06646dULL),
        .num_components = 4,
        .components = {keyboard_component, movement_component, transform_component, mover_component},
        .writes = {false, false, true, true},
        .update = movement_update,
        .inst = (tm_engine_o *)ctx,
    };
    tm_entity_api->register_engine(ctx, &movement_engine);
}

static void winning_system_update(struct tm_entity_context_o *ctx, tm_entity_system_o *inst, struct tm_entity_commands_o *commands)
{
    //...
}

static void register_or_system_engine(struct tm_entity_context_o *ctx)
{
    const tm_entity_system_i winning_system = {
        .ui_name = "winning_system_update",
        .hash = TM_STATIC_HASH("winning_system_update", 0x8f8676e599ca5c7aULL),
        .update = winning_system_update,
        .before_me[0] = TM_STATIC_HASH("maze_generation_system", 0x7f1fcbd9ee85c3cfULL),
        .exclusive = true,
    };
    tm_entity_api->register_system(ctx, &winning_system);
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_entity_api = tm_get_api(reg, tm_entity_api);
    tm_add_or_remove_implementation(reg, load, tm_entity_register_engines_simulation_i, &entity_register_engines_i);
    tm_add_or_remove_implementation(reg, load, tm_entity_register_engines_simulation_i, &register_or_system_engine);
}