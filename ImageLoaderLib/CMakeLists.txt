set(CMAKE_AUTOMOC ON)

add_library(ImageLoaderLib SHARED
    src/imageloader.cpp
    include/imageloader.h
)

target_compile_definitions(ImageLoaderLib PRIVATE IMAGELOADERLIB_LIBRARY)

# Add header directories for ImageLoaderLib
target_include_directories(ImageLoaderLib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/ImageCacheLib/include>
)

# Link Qt and ImageCacheLib libraries
target_link_libraries(ImageLoaderLib PUBLIC
    Qt6::Core # For QObject, QString, QDebug
    Qt6::Gui  # For QImage, QPixmap
    ImageCacheLib # Link the cache library
)

# Set output properties for the DLL
set_target_properties(ImageLoaderLib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
