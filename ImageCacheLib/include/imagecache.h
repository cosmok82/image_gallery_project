/**
 * @file imagecache.h
 * @brief Declaration of the ImageCache class, a component for storing and retrieving QImage objects.
 *
 * This file defines the ImageCache class, which provides a simple in-memory
 * caching mechanism for images identified by an integer ID. It is designed
 * to be a shared library (DLL) component.
 */
#ifndef IMAGECACHELIB_IMAGECACHE_H
#define IMAGECACHELIB_IMAGECACHE_H

#include <QObject>   // Base class for Qt objects, enables signals/slots
#include <QImage>    // Class for image data
#include <QHash>     // Hash table for efficient key-value storage

/**
 * @brief Standard macro for export/import of symbols for the ImageCacheLib.
 *
 * This macro handles the platform-specific directives required to correctly
 * export symbols when building the ImageCacheLib as a shared library (DLL)
 * and import them when using the library in another project.
 */
#if defined(IMAGECACHELIB_LIBRARY)
#  define IMAGECACHELIB_EXPORT Q_DECL_EXPORT
#else
#  define IMAGECACHELIB_EXPORT Q_DECL_IMPORT
#endif

/**
 * @class ImageCache
 * @brief Manages a cache of QImage objects identified by an integer ID.
 *
 * The ImageCache class provides functionality to add, retrieve, check for existence,
 * remove, and clear images from an in-memory hash-based cache. It inherits
 * from QObject to allow for Qt's meta-object system features, although it
 * does not currently emit signals or have slots.
 */
class IMAGECACHELIB_EXPORT ImageCache : public QObject {
    Q_OBJECT // Required for QObject-derived classes to use Qt's meta-object system (signals/slots, properties)

public:
    /**
     * @brief Constructs an ImageCache object.
     * @param parent A pointer to the parent QObject. Defaults to nullptr.
     */
    explicit ImageCache(QObject* parent = nullptr);

    /**
     * @brief Adds an image to the cache or updates an existing one.
     *
     * If an image with the specified ID already exists in the cache, it will be
     * overwritten with the new image.
     *
     * @param id The unique integer ID for the image.
     * @param image The QImage object to be stored in the cache.
     */
    void setImage(int id, const QImage& image);

    /**
     * @brief Retrieves an image from the cache by its ID.
     * @param id The ID of the image to retrieve.
     * @return The QImage associated with the ID, or a null QImage if the ID is not found.
     */
    QImage getImage(int id) const;

    /**
     * @brief Checks if an image with the given ID exists in the cache.
     * @param id The ID of the image to check for.
     * @return True if an image with the ID is found, false otherwise.
     */
    bool contains(int id) const;

    /**
     * @brief Removes an image from the cache by its ID.
     * @param id The ID of the image to remove.
     */
    void removeImage(int id);

    /**
     * @brief Clears all images from the cache.
     */
    void clear();

private:
    /**
     * @brief The internal hash table storing images.
     *
     * Images are stored as QImage objects, with their unique integer ID
     * serving as the key for efficient lookup, insertion, and removal.
     */
    QHash<int, QImage> m_imageCache;
};

#endif // IMAGECACHELIB_IMAGECACHE_H
