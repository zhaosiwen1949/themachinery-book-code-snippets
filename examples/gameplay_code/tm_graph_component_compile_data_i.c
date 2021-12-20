static struct tm_api_registry_api *tm_global_api_registry;
static struct tm_the_truth_api *tm_the_truth_api;
static struct tm_graph_interpreter_api *tm_graph_interpreter_api;

#include <foundation/api_registry.h>
#include <foundation/api_types.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_types.h>

#include <plugins/editor_views/graph.h>
#include <plugins/graph_interpreter/graph_component_node_type.h>
#include <plugins/graph_interpreter/graph_component.h>
#include <plugins/graph_interpreter/graph_interpreter.h>
#include <plugins/animation/animation_state_machine.h>

#define TM_TT_TYPE__ASM_EVENT_REFERENCE "tm_asm_event_reference"
#define TM_TT_TYPE_HASH__ASM_EVENT_REFERENCE TM_STATIC_HASH("tm_asm_event_reference", 0x60cbc051e2b37c38ULL)

static bool compile_data_to_wire(tm_graph_interpreter_o *gr, uint32_t wire, const tm_the_truth_o *tt, tm_tt_id_t data_id, tm_strhash_t to_type_hash)
{
    // #code_snippet_begin(data_id)
    const tm_tt_type_t type = tm_tt_type(data_id);
    const tm_strhash_t type_hash = tm_the_truth_api->type_name_hash(tt, type);
    const tm_the_truth_object_o *data_r = tm_tt_read(tt, data_id);
    // #code_snippet_end(data_id)
    // #code_snippet_begin(translation)
    // #code_snippet_begin(compare)
    if (TM_STRHASH_EQUAL(type_hash, TM_TT_TYPE_HASH__ASM_EVENT_REFERENCE) && TM_STRHASH_EQUAL(to_type_hash, TM_TT_TYPE_HASH__STRING_HASH))
    // #code_snippet_end(compare)
    {
        // #code_snippet_begin(tt_extract)
        const tm_tt_id_t event_id = tm_the_truth_api->get_reference(tt, data_r, 0);
        const tm_strhash_t event_name_hash = tm_the_truth_api->get_string_hash(tt, tm_tt_read(tt, event_id), TM_TT_PROP__ASM_EVENT__NAME);
        // #code_snippet_end(tt_extract)
        // #code_snippet_begin(data_extract)
        tm_strhash_t *v = (tm_strhash_t *)tm_graph_interpreter_api->write_wire(gr, wire, 1, sizeof(*v));
        // #code_snippet_end(data_extract)
        *v = event_name_hash;
        return true;
    }
    // #code_snippet_end(translation)
    //...
    return false;
}

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_global_api_registry = reg;
    tm_the_truth_api = tm_get_api(reg, tm_the_truth_api);
    tm_graph_interpreter_api = tm_get_api(reg, tm_graph_interpreter_api);
    tm_add_or_remove_implementation(reg, load, tm_graph_component_compile_data_i, compile_data_to_wire);
}