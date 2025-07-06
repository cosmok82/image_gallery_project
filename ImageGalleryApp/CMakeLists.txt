set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

# === EXPLICIT UI FILE HANDLING WITH UIC ===
# Define the path of the input .ui file
set(UI_FILE "${CMAKE_CURRENT_SOURCE_DIR}/forms/maingallerywindow.ui")
# Define the path of the output .h header file (we place it in the target's build directory)
set(UI_HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/ui_maingallerywindow.h")

# Add a custom command to execute uic
# This will ensure that ui_maingallerywindow.h is generated
add_custom_command(
    OUTPUT ${UI_HEADER_FILE}
    # *** THIS LINE WAS MODIFIED ***
    # Use the file property of the Qt6::uic target to get the correct path to uic.exe
    COMMAND "$<TARGET_FILE:Qt6::uic>" -o "${UI_HEADER_FILE}" "${UI_FILE}"
    # Specify that this command depends on the input .ui file
    MAIN_DEPENDENCY ${UI_FILE}
    COMMENT "Generating ${UI_HEADER_FILE} via UIC"
)

# Add a custom target that depends on the generation of the UI header
# This ensures that the custom command is executed before the application compilation
add_custom_target(GenerateUiHeaders ALL
    DEPENDS ${UI_HEADER_FILE}
)
# === END EXPLICIT UI FILE HANDLING ===


# Create the application executable
# We no longer specify the .ui file here, as we handle it manually with add_custom_command.
# Now we directly include the generated header.
qt_add_executable(ImageGalleryApp
    src/main.cpp
    src/maingallerywindow.cpp
    include/maingallerywindow.h
    ${UI_HEADER_FILE} # Include the explicitly generated UI header
)

# Ensure the application depends on the target that generates the UI headers
add_dependencies(ImageGalleryApp GenerateUiHeaders)

# Add header directories for the application
target_include_directories(ImageGalleryApp PUBLIC
    # This line is crucial: adds your app's 'include' directory
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # The 'src' directory if it contains other internally used headers
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    # The directory where ui_maingallerywindow.h will be explicitly generated
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    # Final corrections to generator expressions for libraries:
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/ImageCacheLib/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/ImageLoaderLib/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/UINavigatorLib/include>
)

# Link necessary Qt libraries and your custom libraries
target_link_libraries(ImageGalleryApp PRIVATE
    Qt6::Widgets          # Crucial for UI applications
    Qt6::Gui              # Necessary for QImage, QPixmap and other graphical functionalities
    Qt6::Core             # Basic Qt functionalities
    Qt6::Svg              # If the application uses SVG files
    ImageCacheLib         # Your image cache library
    ImageLoaderLib        # Your image loading library
    UINavigatorLib        # Your UI navigation library
)

# Set output properties for the executable
set_target_properties(ImageGalleryApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # For the application executable
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # For shared libraries (DLLs on Windows)
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" # For static libraries
)
