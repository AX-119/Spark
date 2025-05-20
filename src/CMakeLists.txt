file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)

add_executable(Spark
    ${SRC_FILES}
    ${IMGUI_SRC}
)
target_include_directories(Spark PUBLIC ../include ${IMGUI_INCLUDE_DIRS})
target_link_libraries(Spark PRIVATE SDL3::SDL3 glm::glm lua::lua sol2)

# Copy SDL3 DLLs to output after build (Windows only)
add_custom_command(TARGET Spark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:Spark>)

# Copy res folder to output
add_custom_command(TARGET Spark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:Spark>/res)