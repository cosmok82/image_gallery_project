/**
 * @file main.cpp
 * @brief Main entry point for the ImageGalleryApp application.
 *
 * This file contains the main function which initializes the Qt application,
 * sets up the core components for image loading, caching, and UI navigation,
 * and then creates and displays the MainGalleryWindow. It orchestrates the
 * startup of the entire image gallery application.
 */
#include <QApplication> // Required for GUI applications
#include <QLocale>      // For localization settings (optional but good practice)
#include <QDebug>       // For debugging output

#include "maingallerywindow.h" // Our main application window class
#include "imagecache.h"        // ImageCacheLib (ora senza namespace)
#include "imageloader.h"       // ImageLoaderLib (ora senza namespace)
#include "uinavigator.h"       // UINavigatorLib (ora senza namespace)

/**
 * @brief The main entry point for the ImageGalleryApp application.
 *
 * This function initializes the Qt application, sets up core components
 * like ImageCache, ImageLoader, and UINavigator, and then creates
 * and displays the MainGalleryWindow. It also manages the application's
 * event loop.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return The application's exit code.
 */
int main(int argc, char *argv[]) {
    // Set up high DPI scaling for better appearance on high-resolution displays
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    /**
     * @brief The QApplication instance, managing the GUI application's control flow and settings.
     */
    QApplication App(argc, argv); // Create the QApplication instance

    // Set up the locale (useful for number formatting, etc.)
    /**
     * @brief Sets the default locale for the application.
     * This affects formatting of numbers, dates, and other locale-dependent data.
     */
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates)); // Or your preferred locale

    qDebug() << "Starting ImageGalleryApp...";

    // --- Instantiate our DLL components ---
    // These objects will be managed by QApplication's object tree (or manually if needed).
    // They derive from QObject, so they can live on the heap and have parents.

    // 1. ImageCache: The cache where images will be stored
    // It's created here and passed to the ImageLoader. Its lifetime is tied to main().
    /**
     * @brief Instance of ImageCache for storing loaded and generated images.
     * This object manages the caching mechanism for the image gallery.
     */
    ImageCache imageCache; // Nessun namespace

    // 2. ImageLoader: Handles loading images (from disk or generating placeholders)
    // We define parameters here: image directory, max images, max preview size.
    /**
     * @brief Path to the directory containing image files.
     * Constructed relative to the application's executable path.
     */
    QString imageDirPath = App.applicationDirPath() + "/../images"; // Relative path to images folder
    // You can adjust MAX_GALLERY_IMAGES to your desired total number of images (real + placeholders)
    /**
     * @brief Constant defining the maximum number of images (real + placeholders) in the gallery.
     */
    const int MAX_GALLERY_IMAGES = 15; // Example: display 15 images total (real + placeholders)
    /**
     * @brief Maximum resolution for scaled images.
     * Images will be scaled down to fit within these dimensions while maintaining aspect ratio.
     */
    QSize maxPreviewSize(1920, 1080); // Maximum resolution for scaled images

    /**
     * @brief Instance of ImageLoader responsible for loading images and interacting with the ImageCache.
     * It is initialized with the image directory, maximum image count, maximum preview size, and a reference to the image cache.
     */
    ImageLoader imageLoader(imageDirPath, MAX_GALLERY_IMAGES, maxPreviewSize, &imageCache); // Nessun namespace

    // 3. UINavigator: Manages current image ID and navigation logic
    // Initial ID is 0, Max ID will be set once imageLoader knows total count.
    /**
     * @brief Instance of UINavigator for managing the current image ID and navigation logic.
     * Initialized with a starting ID of 0 and the maximum image ID derived from the ImageLoader.
     */
    UINavigator uiNavigator(0, imageLoader.imageCount() - 1); // Nessun namespace
    // Important: Update the navigator with the actual max count determined by the loader
    /**
     * @brief Updates the maximum image ID in the UINavigator based on the actual count determined by the ImageLoader.
     */
    uiNavigator.setMaxImageId(imageLoader.imageCount() - 1);


    // --- Create and show the main window ---
    // Pass references to our core logic components to the UI window.
    // The MainGalleryWindow will be the parent of these components in a logical sense (though not QObject parent-child)
    // or it will just hold pointers to them. For simplicity, we pass pointers.
    /**
     * @brief The main gallery window instance.
     * It is created with pointers to the ImageLoader and UINavigator, establishing their dependency.
     */
    MainGalleryWindow GalleryWindow(&imageLoader,
                                    &uiNavigator,
                                    nullptr);
    /**
     * @brief Displays the main application window to the user.
     */
    GalleryWindow.show(); // Display the main window

    /**
     * @brief Starts the Qt event loop.
     * This function transfers control to Qt, which then listens for and dispatches events.
     * The application remains running until QApplication::exit() is called or the last window is closed.
     */
    return App.exec(); // Start the Qt event loop
}
