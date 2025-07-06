# Enable Qt's Meta-Object Compiler (MOC)
set(CMAKE_AUTOMOC ON)

# Find the necessary Qt packages
find_package(Qt6 COMPONENTS Core REQUIRED)

# Add the library
add_library(UINavigatorLib SHARED
    src/uinavigator.cpp
    include/uinavigator.h
)

target_compile_definitions(UINavigatorLib PRIVATE UINAVIGATORLIB_LIBRARY)

# Set include directories
target_include_directories(UINavigatorLib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# Link necessary Qt libraries
target_link_libraries(UINavigatorLib PUBLIC
    Qt6::Core
)

# Set output properties for the DLL
set_target_properties(UINavigatorLib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
