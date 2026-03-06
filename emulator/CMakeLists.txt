find_package(SDL2)

add_library(super_z80_core
    src/emulator_core.cpp
)

target_include_directories(super_z80_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(super_z80
    src/main.cpp
)

target_link_libraries(super_z80
    PRIVATE
        super_z80_core
)

# Keep runtime path simple for milestone verification commands.
set_target_properties(super_z80 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
