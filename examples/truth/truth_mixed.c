// beginning of the source file
static struct tm_the_truth_api *tm_the_truth_api;

#include <foundation/api_registry.h>
#include <foundation/the_truth.h>
#include <foundation/the_truth_types.h>
#include <foundation/log.h>

#include "my_type.h"

// --- aspects.md
// #code_snippet_begin(debug_aspect)
#define TM_TT_ASPECT__DEBUG_PRINT TM_STATIC_HASH("tm_debug_print_aspect_i", 0x39821c78639e0773ULL)

typedef struct tm_debug_print_aspect_i
{
    void (*debug_print)(tm_the_truth_o *tt, tm_tt_id_t object);
} tm_debug_print_aspect_i;
// #code_snippet_end(debug_aspect)
// #code_snippet_begin(example_use_case)
static void example_use_case(tm_the_truth_o *tt, tm_tt_id_t object)
{
    tm_debug_print_aspect_i *dp = tm_the_truth_api->get_aspect(tt, tm_tt_type(object), TM_TT_ASPECT__DEBUG_PRINT);
    if (dp)
        dp->debug_print(tt, object);
}
// #code_snippet_end(example_use_case)
//--- end

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
}