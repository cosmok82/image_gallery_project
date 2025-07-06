/**
 * @file imageloader.h
 * @brief Declaration of the ImageLoader class, responsible for loading and managing image data.
 *
 * This file defines the ImageLoader class, which handles asynchronous image loading
 * from a specified directory or generates placeholder images if real images are not found.
 * It interacts with an ImageCache to store and retrieve images efficiently and emits
 * signals upon successful image loading or errors.
 */
#ifndef IMAGELOADERLIB_IMAGELOADER_H
#define IMAGELOADERLIB_IMAGELOADER_H

#include <QObject>      // Base class for Qt objects
#include <QImage>       // For image data
#include <QString>      // For string handling
#include <QVector>      // For the lookup table of image paths
#include <QSize>        // For image dimensions

#include "imagecache.h" // Include the ImageCacheLib header (ora senza namespace)

// Macro standard per l'esportazione/importazione
#if defined(IMAGELOADERLIB_LIBRARY)
#  define IMAGELOADERLIB_EXPORT Q_DECL_EXPORT
#else
#  define IMAGELOADERLIB_EXPORT Q_DECL_IMPORT
#endif

// Namespace ImageGallery::Loader rimosso

/**
 * @brief A structure to hold image name and image data.
 *
 * This structure is used internally by the ImageLoader to manage
 * information about individual images, primarily their name and QImage data.
 */
struct ImageData {
    /**
     * @brief The name or identifier of the image.
     */
    QString name;
    /**
     * @brief The actual image data.
     */
    QImage image; // The actual image data
};

/**
 * @brief The ImageLoader class handles asynchronous loading and management of images.
 *
 * This class inherits from QObject to utilize Qt's signal and slot mechanism.
 * It is responsible for:
 * - Discovering image files in a specified directory.
 * - Loading images from disk or generating placeholder images.
 * - Caching images using an ImageCache instance to improve performance.
 * - Emitting signals when an image is successfully loaded or if an error occurs.
 */
class IMAGELOADERLIB_EXPORT ImageLoader : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor for ImageLoader.
     *
     * Initializes the ImageLoader with the path to the image directory,
     * the maximum number of images to handle, the maximum preview size for scaling,
     * and a pointer to a shared ImageCache instance.
     *
     * @param imageDirPath The path to the directory containing image files.
     * @param maxImages The maximum total number of images (real + placeholder) to manage.
     * @param maxPreviewSize The maximum dimensions (width, height) to scale images to.
     * @param cache Pointer to the ImageCache instance used for caching images.
     * @param parent Pointer to the parent QObject.
     */
    ImageLoader(const QString& imageDirPath, int maxImages, const QSize& maxPreviewSize,
                ImageCache* cache, QObject* parent = nullptr); // ImageCache senza namespace
    /**
     * @brief Destructor for ImageLoader.
     *
     * Cleans up any resources allocated by the ImageLoader.
     */
    ~ImageLoader(); // Destructor for cleanup

    /**
     * @brief Returns the total number of images available.
     *
     * This count includes both real images found in the directory and
     * potential placeholder images up to the configured maximum.
     *
     * @return The total number of images.
     */
    int imageCount() const;

    /**
     * @brief Asynchronously loads an image by its ID.
     *
     * This method first checks if the image is in the cache. If not,
     * it attempts to load it from disk or generates a placeholder.
     * The `imageLoaded` signal is emitted upon successful completion,
     * or `loadingError` if an issue occurs.
     *
     * @param id The ID (index) of the image to load.
     */
    void loadImageAsync(int id);

signals:
    /**
     * @brief Signal emitted when an image is successfully loaded or retrieved from cache.
     *
     * @param id The ID of the loaded image.
     * @param image The loaded QImage data.
     */
    void imageLoaded(int id, const QImage& image);
    /**
     * @brief Signal emitted if an error occurs during image loading.
     *
     * @param id The ID of the image that failed to load.
     * @param errorMessage A string describing the error.
     */
    void loadingError(int id, const QString& errorMessage);

private:
    /**
     * @brief Discovers and stores paths to image files in the specified directory.
     *
     * This method populates `m_imagePaths` with the file paths of actual images.
     */
    void populateImagePaths(); // Discovers image files in the directory
    /**
     * @brief Generates a dummy placeholder image.
     *
     * This method creates a generic QImage to be used when a real image
     * corresponding to the requested ID is not found.
     *
     * @param id The ID of the image for which to generate a placeholder.
     * @return A QImage representing the placeholder.
     */
    QImage generatePlaceholderImage(int id) const; // Generates a dummy image

    /**
     * @brief Path to the directory containing actual image files.
     */
    QString m_imageDirPath;       // Path to the directory containing actual images
    /**
     * @brief The maximum number of images (real + placeholder) the loader is configured to handle.
     */
    int m_maxConfiguredImages;    // Max number of images (real + placeholder)
    /**
     * @brief The maximum dimensions to which loaded images will be scaled for preview.
     */
    QSize m_maxPreviewSize;       // Max dimensions for scaled images

    /**
     * @brief A lookup table storing paths to actual image files.
     * The index corresponds to the image ID. If an index beyond existing images
     * is requested, a placeholder will be generated.
     */
    QVector<QString> m_imagePaths; // Lookup table: stores paths to actual images by their ID/index
    // If an index beyond existing images is requested, we generate a placeholder.

    /**
     * @brief Pointer to the shared ImageCache instance.
     * This cache is used to store and retrieve images efficiently.
     */
    ImageCache* m_imageCache; // Pointer to the shared image cache instance (senza namespace)
};

#endif // IMAGELOADERLIB_IMAGELOADER_H
