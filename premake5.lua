-- premake5.lua
-- version: premake-5.0.0-alpha14

-- %TM_SDK_DIR% should be set to the directory of The Machinery SDK

-- premake5.lua
-- version: premake-5.0.0-alpha14

-- %TM_SDK_DIR% should be set to the directory of The Machinery SDK

-- helper functions:

-- Returns a snake-case version of the kebab-case string `name`.
function snake_case(name)
    return string.gsub(name, "-", "_")
end

-- Include all project files from specified folder
function folder(t)
    if type(t) ~= "table" then t = {t} end
    for _,f in ipairs(t) do
        files {f .. "/**.h",  f .. "/**.c", f .. "/**.inl", f .. "/**.cpp", f .. "/**.m", f .. "/**.tmsl"}
    end
end

function folder_headers(t)
    if type(t) ~= "table" then t = {t} end
    for _,f in ipairs(t) do
        files {f .. "/**.h",f .. "/**.inl"}
    end
end



function check_env(env)
    local env_var = os.getenv(env)

    if env_var == nil then
        return false
    end
    return true
end

function tm_lib_dir(path)
    local lib_dir = os.getenv("TM_LIB_DIR")

    if not check_env("TM_LIB_DIR") then
        error("TM_LIB_DIR not set")
        return nil
    end

    return lib_dir .. "/" .. path
end

oldlibdirs = libdirs
function libdirs(path)
    if not check_env("TM_SDK_DIR") then
        error("TM_SDK_DIR not set")
        return
    end
    oldlibdirs { 
        "$(TM_SDK_DIR)/lib/" .. _ACTION .. "/%{cfg.buildcfg}",
        "$(TM_SDK_DIR)/bin/%{cfg.buildcfg}",
        dirs
    }
    oldlibdirs { 
        "$(TM_SDK_DIR)/lib/" .. _ACTION .. "/%{cfg.buildcfg}",
        "$(TM_SDK_DIR)/bin/%{cfg.buildcfg}",
        dirs
    }
end

-- Make incluedirs() also call sysincludedirs()
oldincludedirs = includedirs
function includedirs(dirs)
    if not check_env("TM_SDK_DIR") then
        error("TM_SDK_DIR not set")
        return
    end
    oldincludedirs { 
        "$(TM_SDK_DIR)/headers",
        "$(TM_SDK_DIR)",
            dirs
    }
    sysincludedirs { 
        "$(TM_SDK_DIR)/headers",
        "$(TM_SDK_DIR)",
        dirs
    }
end

-- Base for all projects
function base(name)
    project(name)
        language "C++"
        includedirs { "" }
end

-- Project type for plugins

function example(name,file)
    local sn = snake_case(name)
    base(name.."__"..file)
        location("build/examples/" .. sn .. "/"..file)
        kind "SharedLib"
        targetdir "bin/%{cfg.buildcfg}/examples"
        targetname("tm_" .. sn .. "__"..file)
        defines {"TM_LINKS_" .. string.upper(sn)}
        folder_headers {"examples/" .. sn}
        files {"examples/" .. sn .. "/" ..file..".c"}
end

function example_all(folder_name,name)
    local sn = snake_case(folder_name)
    base(name)
        location("build/examples/" .. sn .. "/"..name)
        kind "SharedLib"
        targetdir "bin/%{cfg.buildcfg}/examples"
        targetname("tm_"..name)
        defines {"TM_LINKS_" .. string.upper(name)}
        folder{"examples/" .. sn}
end


newoption {
    trigger     = "clang",
    description = "Force use of CLANG for Windows builds"
}

workspace "the-machinery-book-snippets"
    configurations {"Debug", "Release"}
    language "C++"
    cppdialect "C++11"
    filter { "options:not clang" }
        flags {"MultiProcessorCompile" }
    filter {}
    flags { "FatalWarnings" }
    warnings "Extra"
    inlining "Auto"
    editandcontinue "Off"
    -- Enable this to test compile with strict aliasing.
    -- strictaliasing "Level3"
    -- Enable this to report build times, see: http://aras-p.info/blog/2019/01/21/Another-cool-MSVC-flag-d1reportTime/
    -- buildoptions { "/Bt+", "/d2cgsummary", "/d1reportTime" }

filter { "system:windows" }
    platforms { "x64" }
    systemversion("latest")

filter { "system:windows", "options:clang" }
    toolset("msc-clangcl")
    buildoptions {
        "-Wno-missing-field-initializers",   -- = {0} is OK.
        "-Wno-unused-parameter",             -- Useful for documentation purposes.
        "-Wno-unused-local-typedef",         -- We don't always use all typedefs.
        "-Wno-missing-braces",               -- = {0} is OK.
        "-Wno-microsoft-anon-tag",           -- Allow anonymous structs.
        "-Wshadow",                          -- Warn about shadowed variables.
        "-Wpadded",                           -- Require explicit padding.
        "-fcommon",                          -- Allow tentative definitions
    }
    buildoptions {
        "-fms-extensions",                   -- Allow anonymous struct as C inheritance.
        "-mavx",                             -- AVX.
        "-mfma",                             -- FMA.
    }

    removeflags {"FatalLinkWarnings"}        -- clang linker doesn't understand /WX

filter { "system:macosx" }
    platforms { "MacOSX-x64", "MacOSX-ARM" }

filter {"system:linux"}
    platforms { "Linux" }

filter { "platforms:x64" }
    defines { "TM_OS_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
    staticruntime "On"
    architecture "x64"
    defines {"TM_CPU_X64", "TM_CPU_AVX", "TM_CPU_SSE"}
    buildoptions {
        "/we4121", -- Padding was added to align structure member on boundary.
        "/we4820", -- Padding was added to end of struct.
        "/utf-8",  -- Source encoding is UTF-8.
    }
    disablewarnings {
        "4057", -- Slightly different base types. Converting from type with volatile to without.
        "4100", -- Unused formal parameter. I think unusued parameters are good for documentation.
        "4152", -- Conversion from function pointer to void *. Should be ok.
        "4200", -- Zero-sized array. Valid C99.
        "4201", -- Nameless struct/union. Valid C11.
        "4204", -- Non-constant aggregate initializer. Valid C99.
        "4206", -- Translation unit is empty. Might be #ifdefed out.
        "4214", -- Bool bit-fields. Valid C99.
        "4221", -- Pointers to locals in initializers. Valid C99.
        "4702", -- Unreachable code. We sometimes want return after exit() because otherwise we get an error about no return value.
    }
    -- ITERATOR_DEBUG_LEVEL kills performance, we don't want it in our builds.
    defines {'_ITERATOR_DEBUG_LEVEL=0'}

filter {"platforms:MacOSX-x64"}
    architecture "x64"
    defines {"TM_CPU_X64", "TM_CPU_AVX", "TM_CPU_SSE"}
    buildoptions {
        "-mavx",                            -- AVX.
        "-mfma"                             -- FMA.
    }

filter {"platforms:MacOSX-ARM"}
    architecture "ARM"
    defines {"TM_CPU_ARM", "TM_CPU_NEON"}

filter { "platforms:MacOSX-x64 or MacOSX-ARM" }
    defines { "TM_OS_MACOSX", "TM_OS_POSIX", "TM_NO_MAIN_FIBER" }
    buildoptions {
        "-fms-extensions",                   -- Allow anonymous struct as C inheritance.
    }
    enablewarnings {
        "shadow",
         "padded"
    }
    disablewarnings {
        "missing-field-initializers",   -- = {0} is OK.
        "unused-parameter",             -- Useful for documentation purposes.
        "unused-local-typedef",         -- We don't always use all typedefs.
        "missing-braces",               -- = {0} is OK.
        "microsoft-anon-tag",           -- Allow anonymous structs.
    }
    -- Needed, because Xcode project generation does not respect `disablewarnings` (premake-5.0.0-alpha13)
    xcodebuildsettings {
        WARNING_CFLAGS = "-Wall -Wextra " ..
            "-Wno-missing-field-initializers " ..
            "-Wno-unused-parameter " ..
            "-Wno-unused-local-typedef " ..
            "-Wno-missing-braces " ..
            "-Wno-microsoft-anon-tag "
    }

filter {"platforms:Linux"}
    defines { "TM_OS_LINUX", "TM_OS_POSIX" }
    architecture "x64"
    defines {"TM_CPU_X64", "TM_CPU_AVX", "TM_CPU_SSE"}
    toolset "clang"
    buildoptions {
        "-fms-extensions",                   -- Allow anonymous struct as C inheritance.
        "-g",                                -- Debugging.
        "-mavx",                             -- AVX.
        "-mfma",                             -- FMA.
        "-fcommon",                          -- Allow tentative definitions
    }
    enablewarnings {
        "shadow",
        "padded"
    }
    disablewarnings {
        "missing-field-initializers",   -- = {0} is OK.
        "unused-parameter",             -- Useful for documentation purposes.
        "unused-local-typedef",         -- We don't always use all typedefs.
        "missing-braces",               -- = {0} is OK.
        "microsoft-anon-tag",           -- Allow anonymous structs.
        "unused-function",
    }

filter "configurations:Debug"
    defines { "TM_CONFIGURATION_DEBUG", "DEBUG" }
    symbols "On"

filter "configurations:Release"
    defines { "TM_CONFIGURATION_RELEASE" }
    optimize "On"


group "the_machinery_book"
    example ("truth","create_empty_type")
    example ("truth","create_truth_types")
    example ("truth","access_values")
    example ("truth","create_an_object")
    example ("truth","modify_object")
    example ("truth","truth_mixed")

    example ("plugins","get_api_demo")
    example ("plugins","my_plugin")
    example ("plugins","static_variable")
    example ("plugins","my_tab")

    example ("gameplay_code","simulation_entry")
    example ("gameplay_code","ecs_system_engine")
    example ("gameplay_code","ecs_component_example")
    example ("gameplay_code","ecs_filtering_entities")

group "tutorials"

    example ("creation_graph","custom_cpu_nodes")
    example ("creation_graph","custom_geometry_node")
    example ("creation_graph","calling_creation_graph_from_code")    

    example ("gameplay_code","example_graph_nodes")
    example ("gameplay_code","tm_graph_component_compile_data_i")

    example ("ui","toolbars_overlays")
    example ("ui","custom_layouts")

    example_all("custom_assets/part_1","part1")
    example_all("custom_assets/part_2","part2")
    example_all("custom_assets/part_3","part3")
    example_all("custom_assets/drag_drop","drag_drop")
    example_all("custom_assets/open_asset","open_asset")
