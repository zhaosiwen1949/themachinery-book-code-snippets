static struct tm_error_api *tm_error_api;
static struct tm_random_api *tm_random_api;
static struct tm_creation_graph_interpreter_api *tm_creation_graph_interpreter_api;

#include <foundation/api_registry.h>
#include <foundation/error.h>
#include <foundation/random.h>
#include <foundation/the_truth_types.h>

#include <plugins/creation_graph/creation_graph.h>
#include <plugins/creation_graph/creation_graph_interpreter.h>
#include <plugins/creation_graph/creation_graph_node_type.h>

// #code_snippet_begin(custom_cpu_node_fn)
static void random_node__run(tm_creation_graph_interpreter_context_t *ctx)
{
    tm_creation_graph_interpreter_wire_content_t max_wire = tm_creation_graph_interpreter_api->read_wire(ctx->instance, ctx->wires[0]);
    if (!TM_ASSERT(max_wire.n, "Max wire was not connected to random node!"))
        return;

    uint32_t *res = (uint32_t *)tm_creation_graph_interpreter_api->write_wire(ctx->instance, ctx->wires[1], TM_TT_TYPE_HASH__UINT32_T, 1, sizeof(uint32_t));
    *res = tm_random_to_uint32_t(tm_random_api->next()) % *(uint32_t *)max_wire.data;
}
// #code_snippet_end(custom_cpu_node_fn)

// #code_snippet_begin(custom_cpu_node_node)
static tm_creation_graph_node_type_i random_node = {
    .name = "tm_random",
    .display_name = "Random Uint",
    .static_connectors.in = {
        {.name = "max", .display_name = "Max", .type_hash = TM_TT_TYPE_HASH__UINT32_T},
    },
    .static_connectors.num_in = 1,
    .static_connectors.out = {
        {.name = "res", .display_name = "Result", .type_hash = TM_TT_TYPE_HASH__UINT32_T},
    },
    .static_connectors.num_out = 1,
    .run = random_node__run,
};
// #code_snippet_end(custom_cpu_node_node)

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_error_api = tm_get_api(reg, tm_error_api);
    tm_random_api = tm_get_api(reg, tm_random_api);
    tm_creation_graph_interpreter_api = tm_get_api(reg, tm_creation_graph_interpreter_api);
    // #code_snippet_begin(custom_cpu_node_register)
    tm_add_or_remove_implementation(reg, load, tm_creation_graph_node_type_i, &random_node);
    // #code_snippet_end(custom_cpu_node_register)
}