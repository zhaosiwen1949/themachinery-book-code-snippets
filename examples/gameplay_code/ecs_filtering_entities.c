static struct tm_entity_api *tm_entity_api;
static struct tm_entity_commands_api *tm_entity_commands_api;
// beginning of the source file
#include <foundation/api_registry.h>

#include <plugins/entity/entity.h>

#include "esc_component.h"

// Runs on (component a, comnponent b)
static void engine_update__custom_component(tm_engine_o *inst, tm_engine_update_set_t *data)
{
    struct tm_entity_context_o *ctx = (struct tm_entity_context_o *)inst;
    // code
    for (tm_engine_update_array_t *a = data->arrays; a < data->arrays + data->num_arrays; ++a)
    {
        //.. code
        for (uint32_t i = 0; i < a->n; ++i)
        {
            // if the entity has no component it returns a NULL pointer
            if (!tm_entity_api->get_component_by_hash(ctx, a->entities[i], TM_TT_TYPE_HASH__D_COMPONENT))
            {
                //..code
            }
        }
    }
    //...code
}

static void movement_update(tm_engine_o *inst, tm_engine_update_set_t *data, struct tm_entity_commands_o *commands)
{
    //...
}

static void entity_register_engines(struct tm_entity_context_o *ctx)
{

    tm_component_type_t component_a = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__A_COMPONENT);
    tm_component_type_t component_b = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__B_COMPONENT);
    tm_component_type_t component_d = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__D_COMPONENT);

    const tm_engine_i movement_engine = {
        .ui_name = "movement_engine",
        .hash = TM_STATIC_HASH("movement_engine", 0x336880a23d06646dULL),
        .num_components = 2,
        .components = {component_a, component_b},
        .writes = {false, true},
        .excluded = {component_d},
        .num_excluded = 1,
        .update = movement_update,
        .inst = (tm_engine_o *)ctx,
    };
    tm_entity_api->register_engine(ctx, &movement_engine);
}

#define TM_TT_TYPE__PLAYER_NO_MOVE_TAG_COMPONENT "tm_player_no_move_t"
#define TM_TT_TYE_HASH__PLAYER_NO_MOVE_TAG_COMPONENT TM_STATIC_HASH("tm_player_no_move_t", 0xc58cb6ade683ca88ULL)
static void component__create(struct tm_entity_context_o *ctx)
{
    tm_component_i component = (tm_component_i){
        .name = TM_TT_TYPE__PLAYER_NO_MOVE_TAG_COMPONENT,
        .bytes = sizeof(uint64_t), // since we do not care of its content we can just pick any 8 byte type
    };
    tm_entity_api->register_component(ctx, &component);
}

static void alt_entity_register_engines(struct tm_entity_context_o *ctx)
{
    tm_component_type_t transform_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__TRANSFORM_COMPONENT);
    tm_component_type_t mover_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__MOVER_COMPONENT);
    tm_component_type_t movement_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYPE_HASH__MOVEMENT_COMPONENT);
    tm_component_type_t no_movement_tag_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYE_HASH__PLAYER_NO_MOVE_TAG_COMPONENT);

    const tm_engine_i movement_engine = {
        .ui_name = "movement_engine",
        .hash = TM_STATIC_HASH("movement_engine", 0x336880a23d06646dULL),
        .num_components = 3,
        .components = {movement_component, transform_component, mover_component},
        .writes = {false, true, true},
        .excluded = {no_movement_tag_component},
        .num_excluded = 1,
        .update = movement_update,
        .inst = (tm_engine_o *)ctx,
    };
    tm_entity_api->register_engine(ctx, &movement_engine);
}

bool player_should_not_walk_anymore = false;

static void my_other_system(tm_engine_o *inst, tm_engine_update_set_t *data, struct tm_entity_commands_o *commands)
{
    struct tm_entity_context_o *ctx = (struct tm_entity_context_o *)inst;
    tm_component_type_t no_movement_tag_component = tm_entity_api->lookup_component_type(ctx, TM_TT_TYE_HASH__PLAYER_NO_MOVE_TAG_COMPONENT);
    // code ..
    for (tm_engine_update_array_t *a = data->arrays; a < data->arrays + data->num_arrays; ++a)
    {
        // code...
        for (uint32_t x = 0; x < a->n; ++x)
        {
            // code...
            if (player_should_not_walk_anymore)
            {
                tm_entity_commands_api->add_component(commands, a->entities[x], no_movement_tag_component);
            }
        }
    }
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_entity_api = tm_get_api(reg, tm_entity_api);
    tm_entity_commands_api = tm_get_api(reg, tm_entity_commands_api);
    tm_add_or_remove_implementation(reg, load, tm_entity_register_engines_simulation_i, &entity_register_engines_i);
}