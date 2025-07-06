# Enable Qt's Meta-Object Compiler (MOC)
set(CMAKE_AUTOMOC ON)

# Adds the library
add_library(ImageCacheLib SHARED
    src/imagecache.cpp
    include/imagecache.h
)

target_compile_definitions(ImageCacheLib PRIVATE IMAGECACHELIB_LIBRARY)

# Add header directories for ImageCacheLib
target_include_directories(ImageCacheLib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link Qt Core and Gui libraries, necessary for QObject and QImage
target_link_libraries(ImageCacheLib PUBLIC
    Qt6::Core
    Qt6::Gui # Necessary for QImage
)

# Set output properties for the DLL
set_target_properties(ImageCacheLib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
