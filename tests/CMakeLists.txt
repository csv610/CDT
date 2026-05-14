# No project() call here, it's a subdirectory

# Add all test source files except test_main.cpp if needed, 
# but here we want to include it.
file(GLOB TEST_SOURCES "*.cpp")
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test_no_new_surface_triangles.cpp")

# Create the test executable
add_executable(run_tests 
    ${TEST_SOURCES}
    ../src/PLC.cpp
    ../src/giftWrap.cpp
    ../src/delaunay.cpp
)

# Inherit includes and settings
target_include_directories(run_tests PRIVATE
    ../src
    ../include
    .
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(run_tests PUBLIC "/fp:strict" "/Oi" "/D _CRT_SECURE_NO_WARNINGS")
else()
    target_compile_options(run_tests PUBLIC -frounding-math)
    if(APPLE)
        target_link_options(run_tests PUBLIC "-Wl,-stack_size,0x808000")
    else()
        target_compile_options(run_tests PUBLIC "-Wl,-z,stacksize=8421376")
    endif()
endif()

# Enable testing
include(CTest)
add_test(NAME AllTests COMMAND run_tests)
