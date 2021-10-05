#include "foundation/api_types.h"

struct my_api
{
    void (*foo)(void);
};

#define my_api_version TM_VERSION(1, 0, 0)