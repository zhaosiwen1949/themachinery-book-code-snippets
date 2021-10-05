static struct tm_api_registry_api *tm_global_api_registry;
static struct tm_error_api *tm_error_api;
static struct tm_logger_api *tm_logger_api;

#include "my_api.h"

#include "foundation/api_registry.h"
#include "foundation/error.h"
#include "foundation/log.h"
#include "foundation/unit_test.h"

static void foo(void)
{
    // ...
}

static struct my_api *my_api = &(struct my_api){
    .foo = foo,
};

static void my_unit_test_function(tm_unit_test_runner_i *tr, struct tm_allocator_i *a)
{
    // ...
}

static struct tm_unit_test_i *my_unit_test = &(struct tm_unit_test_i){
    .name = "my_api",
    .test = my_unit_test_function,
};

TM_DLL_EXPORT void tm_load_plugin(struct tm_api_registry_api *reg, bool load)
{
    tm_global_api_registry = reg;

    tm_error_api = tm_get_api(reg, tm_error_api);
    tm_logger_api = tm_get_api(reg, tm_logger_api);

    tm_set_or_remove_api(reg, load, my_api, my_api);

    tm_add_or_remove_implementation(reg, load, tm_unit_test_i, my_unit_test);
}