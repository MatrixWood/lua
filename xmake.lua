set_project("lua")
set_languages("c11")
set_warnings("allextra")

set_config("cc", "clang")
set_config("cxx", "clang++")
set_config("ld", "clang++")

add_rules("mode.debug", "mode.release")

add_includedirs("./lua/common")
add_includedirs("./lua/vm")
add_includedirs("./lua/clib")
add_includedirs("./lua/test")

target("lua")
    set_kind("binary")
    add_files("./lua/common/*.c")
    add_files("./lua/vm/*.c")
    add_files("./lua/clib/*.c")
    add_files("./lua/test/*.c")