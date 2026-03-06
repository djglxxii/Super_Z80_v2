find_package(SDL2)

add_library(super_z80_crc32
    src/crc32.cpp
)

add_library(super_z80_memory
    src/memory.cpp
)

add_library(super_z80_bus
    src/bus.cpp
)

target_include_directories(super_z80_crc32
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(super_z80_bus
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(super_z80_bus
    PUBLIC
        super_z80_memory
)

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

add_executable(super_z80_test_crc32
    tests/unit/test_crc32.cpp
)

add_executable(super_z80_test_memory
    tests/unit/test_memory.cpp
)

add_executable(super_z80_test_bus
    tests/unit/test_bus.cpp
)

target_link_libraries(super_z80_test_crc32
    PRIVATE
        super_z80_crc32
)

target_link_libraries(super_z80_test_memory
    PRIVATE
        super_z80_memory
)

target_link_libraries(super_z80_test_bus
    PRIVATE
        super_z80_bus
)

enable_testing()
add_test(NAME super_z80_test_crc32 COMMAND super_z80_test_crc32)
add_test(NAME super_z80_test_memory COMMAND super_z80_test_memory)
add_test(NAME super_z80_test_bus COMMAND super_z80_test_bus)

# Keep runtime path simple for milestone verification commands.
set_target_properties(super_z80 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
