cmake_minimum_required(VERSION 3.10)

# Set the target architecture.
if(NOT DEFINED ENABLE_SSE2)
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i386|x86_64|AMD64")
        set(ENABLE_SSE2 True)
    else()
        set(ENABLE_SSE2 False)
    endif()
endif()

if(NOT DEFINED ENABLE_AVX2)
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i386|x86_64|AMD64")
        set(ENABLE_AVX2 True)
    else()
        set(ENABLE_AVX2 False)
    endif()
endif()

#####################

# Use CMake -DLGPL=ON if you opt for the LGPL license
option(LGPL "LGPL" OFF)

# set the project name
project(cdt)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(${PROJECT_NAME} 
    app/main.cpp
    src/cdt_api.cpp
    src/giftWrap.cpp
    src/PLC.cpp
    src/delaunay.cpp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# optimize for speed
	target_compile_options(${PROJECT_NAME} PUBLIC /Ot)
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC "/fp:strict")
# use intrinsic functions
	target_compile_options(${PROJECT_NAME} PUBLIC "/Oi")
# turn off annoying warnings
	target_compile_options(${PROJECT_NAME} PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
	# set target architecture
	if(ENABLE_AVX2)
		target_compile_options(${PROJECT_NAME} PUBLIC "/arch:AVX2")
	elseif(ENABLE_SSE2)
		target_compile_options(${PROJECT_NAME} PUBLIC "/arch:SSE2")
	endif()
# reserve enough stack size
	target_link_options(${PROJECT_NAME} PUBLIC "/STACK:8421376")
	if(NOT LGPL)
		target_compile_options(${PROJECT_NAME} PUBLIC "/D USE_MAROTS_METHOD")
	endif()
else()
# set standard optimization level
	target_compile_options(${PROJECT_NAME} PUBLIC -O2)
# grant IEEE 754 compliance
	target_compile_options(${PROJECT_NAME} PUBLIC -frounding-math)
	# set target architecture
	if(ENABLE_AVX2)
		target_compile_options(${PROJECT_NAME} PUBLIC "-mavx2")
                target_compile_options(${PROJECT_NAME} PUBLIC "-mfma")
	elseif(ENABLE_SSE2)
		target_compile_options(${PROJECT_NAME} PUBLIC "-msse2")
	endif()
# reserve enough stack size	
    if(APPLE)
        target_link_options(${PROJECT_NAME} PUBLIC "-Wl,-stack_size,0x808000")
    else()
        target_compile_options(${PROJECT_NAME} PUBLIC "-Wl,-z,stacksize=8421376")
    endif()
	if(NOT LGPL)
		target_compile_options(${PROJECT_NAME} PUBLIC "-DUSE_MAROTS_METHOD")
	endif()
endif()

# Public include directory
target_include_directories(${PROJECT_NAME} PUBLIC
	src
	include
)

# Find Assimp
find_package(assimp REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC /opt/homebrew/Cellar/assimp/6.0.5/include)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp::assimp)

# Add tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

