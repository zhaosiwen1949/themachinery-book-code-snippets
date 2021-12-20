// #code_snippet_begin(includes_top)
static struct tm_graph_interpreter_api *tm_graph_interpreter_api;
#include <foundation/api_registry.h> //Is needed for `GGN_GEN_REGISTER_FUNCTION()`
#include <foundation/localizer.h>    // it automatically localizes your category name
#include <foundation/macros.h>
#include <foundation/the_truth_types.h> // is needed for the description of the wire input types
// #code_snippet_end(includes_top)
// #code_snippet_begin(includes)
#include <plugins/editor_views/graph.h>
#include <plugins/graph_interpreter/graph_node_helpers.inl>
#include <plugins/graph_interpreter/graph_node_macros.h>
// #code_snippet_end(includes)
// #code_snippet_begin(node_def)
GGN_BEGIN("Sample/Math/Float");
GGN_GEN_REGISTER_FUNCTION();
GGN_NODE_QUERY();
static inline void sample_float_square(float a, float *res)
{
    *res = a * a;
}
GGN_END();
// #code_snippet_end(node_def)
// #code_snippet_begin(tm_load_plugin)
#include "example_graph_nodes.inl"

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_graph_interpreter_api = tm_get_api(reg, tm_graph_interpreter_api);
    // This is auto generated from the graph node generator, you just need to call it.
    generated__register_example_graph_nodes(reg, load);
}
// #code_snippet_end(tm_load_plugin)
