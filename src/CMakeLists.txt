# /src
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(Spark
    ${SRC_FILES}
    ${IMGUI_SRC}
)

target_include_directories(Spark PUBLIC ../include ${IMGUI_INCLUDE_DIRS})
target_link_libraries(Spark PRIVATE SDL3::SDL3 glm::glm lua::lua sol2)

if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    # Only for 64-bit builds with MSVC
    target_compile_options(Spark PRIVATE /bigobj)
endif()


if(EMSCRIPTEN)
 # Emscripten-specific settings
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    
    target_compile_options(Spark PRIVATE
        "SHELL:-s WASM=1"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s NO_EXIT_RUNTIME=1"
    )
    
    target_link_options(Spark PRIVATE
        "SHELL:-s WASM=1"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1" 
        "SHELL:-s NO_EXIT_RUNTIME=1"
        "SHELL:-sEXPORTED_RUNTIME_METHODS=ccall,requestFullscreen"
        "SHELL:-s EXPORTED_FUNCTIONS=_main,_free"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/res@/res"
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/spark-shell.html"
    )
    
    # Debug/Release specific flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(Spark PRIVATE "SHELL:-g" "SHELL:-O0" "SHELL:-s ASSERTIONS=1")
        target_link_options(Spark PRIVATE "SHELL:-s ASSERTIONS=1")
    else()
        target_compile_options(Spark PRIVATE "SHELL:-O3")
        target_link_options(Spark PRIVATE "SHELL:-O3")
    endif()
else()
 # Native build settings
    
    # Copy SDL3 DLL on Windows
    if(WIN32)
        add_custom_command(TARGET Spark POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:Spark>
        )
    endif()
    
    # Copy resources
    add_custom_command(TARGET Spark POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:Spark>/res
    )
    
endif()