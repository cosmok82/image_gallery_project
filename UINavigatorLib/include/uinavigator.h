/**
 * @file uinavigator.h
 * @brief Declaration of the UINavigator class, which manages image ID navigation in the UI.
 *
 * This file defines the UINavigator class, responsible for tracking the current
 * and maximum image IDs, and providing methods to navigate between images.
 * It emits a signal when the current image ID changes.
 */
#ifndef UINAVIGATOR_H
#define UINAVIGATOR_H

#include <QObject>

// Macro standard per l'esportazione/importazione
#if defined(UINAVIGATORLIB_LIBRARY)
#  define UINAVIGATORLIB_EXPORT Q_DECL_EXPORT
#else
#  define UINAVIGATORLIB_EXPORT Q_DECL_IMPORT
#endif

/**
 * @brief The UINavigator class manages the current image ID and navigation logic.
 *
 * This class inherits from QObject to utilize Qt's signal and slot mechanism.
 * It is responsible for:
 * - Storing and providing the current image ID.
 * - Storing and providing the maximum available image ID.
 * - Handling navigation requests (next and previous image).
 * - Emitting a signal when the current image ID changes, allowing other UI components to react.
 */
class UINAVIGATORLIB_EXPORT UINavigator : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for UINavigator.
     *
     * Initializes the navigator with a starting image ID and the maximum available ID.
     *
     * @param initialImageId The starting image ID.
     * @param maxImageId The maximum valid image ID in the gallery.
     * @param parent Pointer to the parent QObject.
     */
    explicit UINavigator(int initialImageId, int maxImageId, QObject *parent = nullptr);

    /**
     * @brief Returns the current image ID.
     *
     * @return The ID of the currently selected image.
     */
    int currentImageId() const;

    /**
     * @brief Sets the maximum available image ID.
     *
     * This method is typically called by the ImageLoader once the total
     * number of images has been determined.
     *
     * @param maxId The new maximum valid image ID.
     */
    void setMaxImageId(int maxId);

    /**
     * @brief Returns the maximum available image ID.
     *
     * @return The maximum valid image ID.
     */
    int maxImageId() const;

    /**
     * @brief Attempts to navigate to the next image.
     *
     * Increments the current image ID if it's not already at the maximum.
     * Emits `imageIdChanged` signal if the ID is successfully incremented.
     *
     * @return True if navigation to the next image was successful, false otherwise (e.g., already at max ID).
     */
    bool next();

    /**
     * @brief Attempts to navigate to the previous image.
     *
     * Decrements the current image ID if it's not already at zero.
     * Emits `imageIdChanged` signal if the ID is successfully decremented.
     *
     * @return True if navigation to the previous image was successful, false otherwise (e.g., already at ID 0).
     */
    bool previous();

signals:
    /**
     * @brief Signal emitted when the current image ID changes.
     *
     * This signal notifies connected objects (e.g., the UI window)
     * that a new image should be displayed.
     *
     * @param newId The new current image ID.
     */
    void imageIdChanged(int newId);

private:
    /**
     * @brief The current image ID being displayed or navigated to.
     */
    int m_currentImageId;
    /**
     * @brief The maximum valid image ID in the gallery.
     * This is used to prevent navigation beyond the last image.
     */
    int m_maxImageId;
};

#endif // UINAVIGATOR_H
