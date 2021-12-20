static struct tm_localizer_api *tm_localizer_api;
static struct tm_allocator_api *tm_allocator_api;
// beginning of the source file
#include <foundation/api_registry.h>
#include <foundation/localizer.h>
#include <foundation/allocator.h>

#include <plugins/simulation/simulation_entry.h>

struct tm_simulation_state_o
{
    tm_allocator_i *allocator;
    //..
};

// Starts a new simulation session. Called just before `tick` is called for the first
// time. The return value will later be fed to later calls to `stop` and `update`.
tm_simulation_state_o *start(tm_simulation_start_args_t *args)
{
    tm_simulation_state_o *state = tm_alloc(args->allocator, sizeof(*state));
    *state = (tm_simulation_state_o){
        .allocator = args->allocator,
        //...
    };
    //...
    return state;
}

// Called when the entity containing the Simulation Entry Component is destroyed.
void stop(tm_simulation_state_o *state, struct tm_entity_commands_o *commands)
{
    //...
    tm_allocator_i a = *state->allocator;
    tm_free(&a, state, sizeof(*state));
}

// Called each frame. Implement logic such as gameplay here. See `args` for useful
// stuff like duration of the frame etc.
void tick(tm_simulation_state_o *state, tm_simulation_frame_args_t *args)
{
    //...
}

// Called whenever a code hot reload has occurred. Note that the start, tick and
// stop functions will be updated to any new version automatically, this  callback is for other
// hot reload related tasks such as updating function pointers within the simulation code.
void hot_reload(tm_simulation_state_o *state, struct tm_entity_commands_o *commands)
{
    //...
}
// #code_snippet_begin(simulation_entry_i)
static tm_simulation_entry_i simulation_entry_i = {
    .id = TM_STATIC_HASH("tm_my_game_simulation_entry", 0x2d5f7dad50097045ULL),
    .display_name = TM_LOCALIZE_LATER("My Game Simulate Entry"),
    .start = start,
    .stop = stop,
    .tick = tick,
    .hot_reload = hot_reload,
};
// #code_snippet_end(simulation_entry_i)
TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_localizer_api = tm_get_api(reg, tm_localizer_api);
    tm_allocator_api = tm_get_api(reg, tm_allocator_api);
    tm_add_or_remove_implementation(reg, load, tm_simulation_entry_i, &simulation_entry_i);
}