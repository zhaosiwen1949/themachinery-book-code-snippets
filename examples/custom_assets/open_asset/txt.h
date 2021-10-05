#pragma once
#include <foundation/api_types.h>
//... more code
#define TM_TT_TYPE__MY_ASSET "tm_my_asset"
#define TM_TT_TYPE_HASH__MY_ASSET TM_STATIC_HASH("tm_my_asset", 0x1e12ba1f91b99960ULL)

enum
{
    TM_TT_PROP__MY_ASSET__FILE,
    TM_TT_PROP__MY_ASSET__DATA,
};

enum
{
    TM_TT_PROP__STORY_COMPONENT__ASSET, // reference to asset
};

typedef struct tm_story_component_t
{
    char *text;
    uint64_t size;
} tm_story_component_t;

#define TM_TT_TYPE__STORY_COMPONENT "tm_story_component"
#define TM_TT_TYPE_HASH__STORY_COMPONENT TM_STATIC_HASH("tm_story_component", 0x595be39339ba1ca5ULL)

#define TM_TXT_TAB_VT_NAME "tm_txt_tab"
#define TM_TXT_TAB_VT_NAME_HASH TM_STATIC_HASH("tm_txt_tab", 0x2cd261be98a99bc3ULL)