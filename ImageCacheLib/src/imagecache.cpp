/**
 * @file imagecache.cpp
 * @brief Implementation of the ImageCache class.
 *
 * This file provides the definitions for the methods of the ImageCache class,
 * handling the storage and retrieval of QImage objects in an in-memory cache.
 */
#include "imagecache.h"
#include <QDebug> // For debugging output

/**
 * @brief Constructs an ImageCache object.
 * @param parent A pointer to the parent QObject.
 *
 * Initializes the internal image cache and prints a debug message.
 */
ImageCache::ImageCache(QObject* parent)
    : QObject(parent) {
    qDebug() << "ImageCache initialized.";
}

/**
 * @brief Adds an image to the cache or updates an existing one.
 *
 * If the provided image is null, a warning is logged, and the function returns.
 * Otherwise, the image is inserted into the hash table. If an image with the
 * same ID already exists, it is overwritten. A debug message indicates the
 * success and current cache size.
 *
 * @param id The unique integer ID for the image.
 * @param image The QImage object to be stored in the cache.
 */
void ImageCache::setImage(int id, const QImage& image) {
    if (image.isNull()) {
        qDebug() << "Warning: Attempted to add a null image to cache with ID:" << id;
        return;
    }
    m_imageCache.insert(id, image);
    qDebug() << "Image with ID" << id << "added to cache. Current cache size:" << m_imageCache.size();
}

/**
 * @brief Retrieves an image from the cache by its ID.
 *
 * Checks if an image with the given ID exists in the cache.
 *
 * @param id The ID of the image to retrieve.
 * @return The QImage associated with the ID. Returns a null QImage if the ID is not found.
 */
QImage ImageCache::getImage(int id) const {
    if (m_imageCache.contains(id)) {
        qDebug() << "Image with ID" << id << "retrieved from cache.";
        return m_imageCache.value(id);
    }
    qDebug() << "Image with ID" << id << "not found in cache.";
    return QImage(); // Return a null QImage if not found
}

/**
 * @brief Checks if an image with the given ID exists in the cache.
 * @param id The ID of the image to check for.
 * @return True if an image with the ID is found, false otherwise.
 */
bool ImageCache::contains(int id) const {
    return m_imageCache.contains(id);
}

/**
 * @brief Removes an image from the cache by its ID.
 *
 * Logs a message indicating whether the image was successfully removed or if
 * it was not found in the cache.
 *
 * @param id The ID of the image to remove.
 */
void ImageCache::removeImage(int id) {
    if (m_imageCache.remove(id)) {
        qDebug() << "Image with ID" << id << "removed from cache. Current cache size:" << m_imageCache.size();
    } else {
        qDebug() << "Warning: Image with ID" << id << "not found in cache for removal.";
    }
}

/**
 * @brief Clears all images from the cache.
 *
 * Removes all key-value pairs from the internal hash table and logs a debug message.
 */
void ImageCache::clear() {
    m_imageCache.clear();
    qDebug() << "ImageCache cleared.";
}
