/**
 * @file uinavigator.cpp
 * @brief Implementation of the UINavigator class.
 *
 * This file provides the concrete implementation for the UINavigator,
 * including its constructor, methods for getting and setting the current and
 * maximum image IDs, and functions for navigating to the next and previous images.
 * It also handles emitting the imageIdChanged signal.
 */
#include "uinavigator.h"
#include <QDebug>

// Namespace ImageGallery::UI rimosso

/**
 * @brief Constructor for UINavigator.
 *
 * Initializes the navigator with a starting image ID and the maximum available ID.
 * Ensures that the initial ID is within valid bounds (0 to maxImageId).
 *
 * @param initialImageId The starting image ID.
 * @param maxImageId The maximum valid image ID in the gallery.
 * @param parent Pointer to the parent QObject.
 */
UINavigator::UINavigator(int initialImageId, int maxImageId, QObject* parent)
    : QObject(parent),
    m_currentImageId(initialImageId),
    m_maxImageId(maxImageId)
{
    // Ensure initial ID is within valid bounds
    if (m_currentImageId < 0) {
        m_currentImageId = 0;
    }
    if (m_currentImageId > m_maxImageId) {
        m_currentImageId = m_maxImageId;
    }
    qDebug() << "UINavigator initialized. Current ID: " << m_currentImageId << ", Max ID: " << m_maxImageId;

}

/**
 * @brief Returns the current image ID.
 *
 * @return The ID of the currently selected image.
 */
int UINavigator::currentImageId() const {
    return m_currentImageId;
}

/**
 * @brief Sets the maximum available image ID.
 *
 * This method is typically called by the ImageLoader once the total
 * number of images has been determined. If the new `maxId` is less than 0,
 * it defaults to 0. If the `m_currentImageId` exceeds the new `m_maxImageId`,
 * `m_currentImageId` is adjusted to `m_maxImageId` and the `imageIdChanged`
 * signal is emitted.
 *
 * @param maxId The new maximum valid image ID.
 */
void UINavigator::setMaxImageId(int maxId) {
    if (maxId < 0) { // Max ID cannot be negative
        maxId = 0;
    }
    if (m_maxImageId != maxId) {
        m_maxImageId = maxId;
        qDebug() << "UINavigator: Max ID updated to: " << m_maxImageId;
        // Adjust current ID if it's now out of bounds
        if (m_currentImageId > m_maxImageId) {
            m_currentImageId = m_maxImageId;
            emit imageIdChanged(m_currentImageId); // Emit signal if current ID changes
        }
    }
}

/**
 * @brief Returns the maximum available image ID.
 *
 * @return The maximum valid image ID.
 */
int UINavigator::maxImageId() const {
    return m_maxImageId;
}

/**
 * @brief Attempts to navigate to the next image.
 *
 * Increments the current image ID using a modulo operation to loop back
 * to 0 if the end of the gallery is reached.
 * Emits `imageIdChanged` signal if the ID is successfully incremented.
 *
 * @return True if navigation to the next image was successful, false if no images exist (m_maxImageId < 0).
 */
bool UINavigator::next() {
    if (m_maxImageId < 0) return false; // Nessuna immagine
    m_currentImageId = (m_currentImageId + 1) % (m_maxImageId + 1);
    qDebug() << "UINavigator: Moved to next image. New ID: " << m_currentImageId;
    emit imageIdChanged(m_currentImageId);
    return true;
}

/**
 * @brief Attempts to navigate to the previous image.
 *
 * Decrements the current image ID. If the current ID is 0, it wraps around
 * to the `m_maxImageId`.
 * Emits `imageIdChanged` signal if the ID is successfully decremented.
 *
 * @return True if navigation to the previous image was successful, false if no images exist (m_maxImageId < 0).
 */
bool UINavigator::previous() {
    if (m_maxImageId < 0) return false; // Nessuna immagine

    if (m_currentImageId == 0) {
        m_currentImageId = m_maxImageId; // Da 0 vai all'ultimo ID
    } else {
        m_currentImageId--;
    }
    qDebug() << "UINavigator: Moved to previous image. New ID: " << m_currentImageId;
    emit imageIdChanged(m_currentImageId);
    return true;
}
