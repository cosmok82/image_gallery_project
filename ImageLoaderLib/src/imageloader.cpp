/**
 * @file imageloader.cpp
 * @brief Implementation of the ImageLoader class.
 *
 * This file provides the concrete implementation for the ImageLoader,
 * including methods for discovering image files, generating placeholder images,
 * asynchronously loading images from disk or cache, and emitting signals
 * related to image loading status.
 */
#include "imageloader.h"
#include <QDir>              // For directory operations (listing files)
#include <QFileInfo>         // For file information (checking if it's a file)
#include <QImage>            // For image loading and manipulation
#include <QPainter>          // For drawing text on placeholder images
#include <QDebug>            // For debugging output
#include <QCoreApplication>  // For QCoreApplication::applicationDirPath() etc.
#include <QTimer>            // To simulate asynchronous loading


// Namespace ImageGallery::Loader rimosso

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
ImageLoader::ImageLoader(const QString& imageDirPath, int maxImages, const QSize& maxPreviewSize,
                         ImageCache* cache, QObject* parent) // ImageCache senza namespace
    : QObject(parent),
    m_imageDirPath(imageDirPath),
    m_maxConfiguredImages(maxImages),
    m_maxPreviewSize(maxPreviewSize),
    m_imageCache(cache) // Assign the provided cache instance
{
    if (!m_imageCache) {
        qDebug() << "Warning: ImageCache pointer is null in ImageLoader constructor!";
    }
    populateImagePaths(); // Discover available image files at initialization
    qDebug() << "ImageLoader initialized. Found" << m_imagePaths.size() << "actual images. Max configured images:" << m_maxConfiguredImages;
}

/**
 * @brief Destructor for ImageLoader.
 *
 * Cleans up any resources allocated by the ImageLoader.
 */
ImageLoader::~ImageLoader() {
    qDebug() << "ImageLoader destroyed.";
}

/**
 * @brief Discovers image files in the specified directory and populates m_imagePaths.
 *
 * This method scans the `m_imageDirPath` for common image file extensions
 * and stores their absolute paths in `m_imagePaths`.
 */
void ImageLoader::populateImagePaths() {
    QDir imageDir(m_imageDirPath);
    if (!imageDir.exists()) {
        qDebug() << "Image directory does not exist:" << m_imageDirPath;
        return;
    }

    // Filter for common image file extensions
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.gif" << "*.bmp" << "*.webp";
    QFileInfoList fileList = imageDir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    m_imagePaths.clear();
    for (const QFileInfo& fileInfo : fileList) {
        m_imagePaths.append(fileInfo.absoluteFilePath());
    }
    qDebug() << "Populated image paths. Found" << m_imagePaths.size() << "image files.";
}

/**
 * @brief Generates a placeholder image with the given ID and dimensions.
 *
 * This method creates a dark gray image and draws the image ID in white,
 * bold Arial font, centered within the image.
 *
 * @param id The ID of the image for which to generate a placeholder.
 * @return A QImage representing the placeholder.
 */
QImage ImageLoader::generatePlaceholderImage(int id) const {
    // Creiamo un'immagine con uno sfondo grigio scuro per un look più moderno
    QImage placeholder(m_maxPreviewSize, QImage::Format_RGB32);
    placeholder.fill(QColor("#444444")); // Un bel grigio scuro

    // QPainter è lo strumento per disegnare sull'immagine
    QPainter painter(&placeholder);

    // Migliora la qualità del rendering del testo (antialiasing)
    painter.setRenderHint(QPainter::Antialiasing);

    // Impostiamo il colore della penna su bianco per un buon contrasto
    painter.setPen(Qt::white);

    // Scegliamo un font grande, in grassetto e ben leggibile
    QFont font("Arial", 72, QFont::Bold);
    painter.setFont(font);

    // Disegniamo il numero dell'ID esattamente al centro del rettangolo dell'immagine
    painter.drawText(placeholder.rect(), Qt::AlignCenter, QString::number(id));

    qDebug() << "Generated placeholder for ID:" << id;
    return placeholder;
}

/**
 * @brief Returns the total count of images available (real files + potential placeholders).
 *
 * This method returns the maximum of either the number of actual image files found
 * or the maximum configured number of images.
 *
 * @return The total number of images that can be accessed via their ID.
 */
int ImageLoader::imageCount() const {
    return qMax(m_imagePaths.size(), m_maxConfiguredImages); // Max of actual files or configured max
}

/**
 * @brief Asynchronously loads and emits an image.
 *
 * This method attempts to load an image by its ID. It first checks the cache.
 * If not found, it loads from disk if a real file exists for the ID, or generates
 * a placeholder image otherwise. The `imageLoaded` signal is emitted upon
 * successful completion, or `loadingError` if any issue occurs.
 *
 * @param id The ID of the image to load.
 */
void ImageLoader::loadImageAsync(int id) {
    if (id < 0 || id >= imageCount()) {
        emit loadingError(id, "Image ID out of bounds.");
        return;
    }

    // 1. Check cache first
    if (m_imageCache && m_imageCache->contains(id)) {
        qDebug() << "Image with ID" << id << "found in cache.";
        emit imageLoaded(id, m_imageCache->getImage(id));
        return;
    }

    // Simulate asynchronous loading using a single-shot timer
    // In a real application, you'd use QThreadPool, QFuture, or a separate thread.
    QTimer::singleShot(50, [this, id]() { // 50ms delay to simulate loading time
        QImage loadedImage;
        QString imagePath;

        // 2. Determine if it's a real image or needs a placeholder
        if (id < m_imagePaths.size()) {
            imagePath = m_imagePaths.at(id);
            qDebug() << "Attempting to load image from disk:" << imagePath << "for ID:" << id;
            loadedImage.load(imagePath); // Load image from file

            if (loadedImage.isNull()) {
                qDebug() << "Failed to load image from file:" << imagePath << ". Generating placeholder.";
                loadedImage = generatePlaceholderImage(id); // Fallback to placeholder on failure
            }
        } else {
            // ID is beyond the number of actual images found, generate placeholder
            loadedImage = generatePlaceholderImage(id);
        }

        // 3. Scale the image to the max preview size
        if (!loadedImage.isNull()) {
            loadedImage = loadedImage.scaled(m_maxPreviewSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // 4. Add to cache if loading was successful
            if (m_imageCache) {
                m_imageCache->setImage(id, loadedImage);
            }
            emit imageLoaded(id, loadedImage); // Emit signal with the loaded/generated image
        } else {
            // This should ideally not happen if generatePlaceholderImage works as expected
            emit loadingError(id, "Failed to load or generate image.");
        }
    });
}
