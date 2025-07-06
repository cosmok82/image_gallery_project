cmake_minimum_required(VERSION 3.16)

project(ImageGalleryProject
    VERSION 1.0
    LANGUAGES CXX
)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set MSVC DLL runtime policy (useful for Windows)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)
endif()

# Find required Qt6 packages
find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    Svg # For triangular button icons or SVG rendering
REQUIRED)

# Add subdirectories for libraries and the application
add_subdirectory(ImageCacheLib)
add_subdirectory(ImageLoaderLib)
add_subdirectory(UINavigatorLib)
add_subdirectory(ImageGalleryApp)
