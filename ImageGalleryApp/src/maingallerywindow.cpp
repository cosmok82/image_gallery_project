/**
 * @file maingallerywindow.cpp
 * @brief Implementation of the MainGalleryWindow class.
 *
 * This file provides the concrete implementation for the MainGalleryWindow,
 * handling the setup of the user interface, connecting signals and slots
 * for interaction with ImageLoader and UINavigator, and managing the display
 * of images and navigation elements.
 */
#include "maingallerywindow.h"
#include "ui_maingallerywindow.h" // Generato da uic da maingallerywindow.ui

#include "imageloader.h"          // Include completo per ImageLoader (ora senza namespace)
#include "uinavigator.h"          // Include completo per UINavigator (ora senza namespace)

#include <QDebug>                 // Per debugging
#include <QPixmap>                // Per visualizzare QImage in QLabel
#include <QMessageBox>            // Per messaggi di errore
#include <QScreen>                // Per ottenere la risoluzione dello schermo per lo scaling
#include <QDir>                   // Per controllare l'esistenza della directory delle immagini

// Per disegnare icone triangolari personalizzate (in alternativa, usa file SVG)
#include <QPainter>
#include <QPolygonF>
#include <QIcon>
#include <QPainterPath>

/**
 * @brief Constructor for MainGalleryWindow.
 *
 * Initializes the main gallery window, setting up its dependencies
 * on an ImageLoader and a UINavigator instance.
 *
 * @param loader Pointer to the ImageLoader instance responsible for loading images.
 * @param navigator Pointer to the UINavigator instance responsible for image ID management.
 * @param parent Pointer to the parent QObject, typically nullptr for the main window.
 */
MainGalleryWindow::MainGalleryWindow(ImageLoader* loader,
                                     UINavigator* navigator,
                                     QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainGalleryWindow()), // Inizializza l'UI dal file .ui (il namespace Ui è di Qt)
    m_imageLoader(loader),
    m_uiNavigator(navigator),
    m_maxImageId(0) // Sarà aggiornato dal navigatore
{
    ui->setupUi(this); // Configura gli elementi UI definiti nel file .ui

    // Controlli di base per i componenti passati
    if (!m_imageLoader) {
        QMessageBox::critical(this, "Error", "ImageLoader non fornito a MainGalleryWindow!");
        return;
    }
    if (!m_uiNavigator) {
        QMessageBox::critical(this, "Error", "UINavigator non fornito a MainGalleryWindow!");
        return;
    }

    // Collega segnali e slot
    // Segnali di ImageLoader
    /**
     * @brief Connects the imageLoaded signal from ImageLoader to onImageLoaded slot.
     */
    connect(m_imageLoader, &ImageLoader::imageLoaded,
            this, &MainGalleryWindow::onImageLoaded);
    /**
     * @brief Connects the loadingError signal from ImageLoader to onLoadingError slot.
     */
    connect(m_imageLoader, &ImageLoader::loadingError,
            this, &MainGalleryWindow::onLoadingError);

    // Segnali di UINavigator
    /**
     * @brief Connects the imageIdChanged signal from UINavigator to onImageIdChanged slot.
     */
    connect(m_uiNavigator, &UINavigator::imageIdChanged,
            this, &MainGalleryWindow::onImageIdChanged);

    // Configurazione iniziale
    /**
     * @brief Retrieves the initial maximum image ID from the UINavigator.
     * This value defines the upper bound for image navigation.
     */
    m_maxImageId = m_uiNavigator->maxImageId(); // Ottieni l'ID massimo iniziale
    /**
     * @brief Updates the image ID label with the current and maximum image IDs.
     */
    updateIdLabel(m_uiNavigator->currentImageId(), m_maxImageId);
    /**
     * @brief Updates the enabled state of navigation buttons based on current and maximum image IDs.
     */
    updateNavigationButtons(m_uiNavigator->currentImageId(), m_maxImageId);
    /**
     * @brief Initiates asynchronous loading of the first image based on the current image ID from UINavigator.
     */
    m_imageLoader->loadImageAsync(m_uiNavigator->currentImageId()); // Carica la prima immagine

    /**
     * @brief Sets up custom icons for the navigation buttons.
     */
    setupUiIcons(); // Imposta le icone per i pulsanti
}

/**
 * @brief Destructor for MainGalleryWindow.
 *
 * Cleans up resources allocated by the window.
 * The QScopedPointer 'ui' automatically handles the deletion of Ui::MainGalleryWindow.
 * m_imageLoader and m_uiNavigator are not owned by this class (passed as pointers),
 * so they are not deleted here. They are owned by main().
 */
MainGalleryWindow::~MainGalleryWindow() {
    qDebug() << "MainGalleryWindow distrutta.";
}

/**
 * @brief Updates the image displayed in the UI.
 *
 * Scales the provided image to fit the display area and sets it on the image label.
 *
 * @param image The QImage to be displayed.
 */
void MainGalleryWindow::updateImageDisplay(const QImage& image) {
    if (image.isNull()) {
        ui->imageLabel->setText("Immagine non disponibile.");
        ui->imageLabel->setPixmap(QPixmap()); // Cancella qualsiasi immagine precedente
        return;
    }

    // Scala l'immagine per adattarsi al frame, mantenendo le proporzioni
    QPixmap pixmap = QPixmap::fromImage(image);
    // Ottieni la dimensione corrente dello schermo per assicurarti che lo scaling sia appropriato per la visualizzazione
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->size();

    // Scala il pixmap per adattarsi alla dimensione del genitore di imageLabel (o dimensione approssimativa dello schermo)
    // Una soluzione più robusta sarebbe scalare alla dimensione effettiva disponibile di imageLabel
    // o a una dimensione massima di visualizzazione fissa. Per semplicità, usiamo un massimo definito in ImageLoader.
    // Tuttavia, per la visualizzazione nell'UI, lo scaling alla dimensione dell'etichetta è spesso migliore.
    // Scaliamola per adattarsi allo spazio disponibile in imageFrame, che è il genitore di imageLabel
    QSize targetSize = ui->imageFrame->size() - QSize(20, 20); // Alcuni padding
    if (targetSize.isEmpty() || targetSize.width() <= 0 || targetSize.height() <= 0) {
        targetSize = QSize(800, 600); // Dimensione di fallback se il frame non è ancora stato disposto
    }
    ui->imageLabel->setPixmap(pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->imageLabel->setAlignment(Qt::AlignCenter);
    ui->imageLabel->setText(""); // Cancella il testo "Caricamento Immagine..."
}

/**
 * @brief Updates the enabled/disabled state of navigation buttons.
 *
 * Based on the current image ID and the maximum image ID, this method
 * enables or disables the "Previous" and "Next" buttons appropriately.
 *
 * @param currentId The current image ID being displayed.
 * @param maxId The maximum available image ID.
 */
void MainGalleryWindow::updateNavigationButtons(int currentId, int maxId) {
    /**
     * @brief Flag indicating whether navigation buttons should be enabled.
     * Buttons are disabled only if there are 0 or 1 images available.
     */
    bool buttonsEnabled = (maxId > 0); // Disabilita solo se c'è 1 o 0 immagini
    ui->prevButton->setEnabled(buttonsEnabled);
    ui->nextButton->setEnabled(buttonsEnabled);
}

/**
 * @brief Updates the label showing the current image ID and total count.
 *
 * Formats and sets the text for the image ID display label.
 *
 * @param currentId The current image ID.
 * @param maxId The maximum available image ID.
 */
void MainGalleryWindow::updateIdLabel(int currentId, int maxId) {
    ui->idLabel->setText(QString("ID: %1/%2").arg(currentId).arg(maxId));
}

/**
 * @brief Sets up icons for the navigation buttons.
 *
 * This method is responsible for creating and assigning appropriate icons
 * (e.g., triangular arrows) to the "Previous" and "Next" buttons using QPainter.
 * For more complex or scalable icons, SVG files are recommended.
 */
void MainGalleryWindow::setupUiIcons() {
    // Usando QPainter per disegnare semplici triangoli.
    // Per icone più complesse o scalabili, considera l'uso di file SVG.

    // Icona pulsante precedente
    /**
     * @brief Pixmap for the "Previous" button icon.
     * A transparent pixmap is created, and a black triangle pointing left is drawn onto it.
     */
    QPixmap prevPixmap(60, 60);
    prevPixmap.fill(Qt::transparent);
    QPainter prevPainter(&prevPixmap);
    prevPainter.setRenderHint(QPainter::Antialiasing);
    prevPainter.setPen(Qt::NoPen);
    prevPainter.setBrush(Qt::black); // Colore del triangolo
    QPolygonF prevTriangle;
    prevTriangle << QPointF(50, 10) << QPointF(10, 30) << QPointF(50, 50); // Triangolo che punta a destra
    prevPainter.drawPolygon(prevTriangle);
    ui->prevButton->setIcon(QIcon(prevPixmap));

    // Icona pulsante successivo
    /**
     * @brief Pixmap for the "Next" button icon.
     * A transparent pixmap is created, and a black triangle pointing right is drawn onto it.
     */
    QPixmap nextPixmap(60, 60);
    nextPixmap.fill(Qt::transparent);
    QPainter nextPainter(&nextPixmap);
    nextPainter.setRenderHint(QPainter::Antialiasing);
    nextPainter.setPen(Qt::NoPen);
    nextPainter.setBrush(Qt::black); // Colore del triangolo
    QPolygonF nextTriangle;
    nextTriangle << QPointF(10, 10) << QPointF(50, 30) << QPointF(10, 50); // Triangolo che punta a sinistra
    nextPainter.drawPolygon(nextTriangle);
    ui->nextButton->setIcon(QIcon(nextPixmap));
}


/**
 * @brief Slot to receive loaded images from ImageLoader.
 *
 * This slot is connected to the ImageLoader::imageLoaded signal.
 * It updates the displayed image when a new image is successfully loaded,
 * but only if the loaded image's ID matches the currently expected image ID.
 *
 * @param id The ID of the loaded image.
 * @param image The loaded QImage data.
 */
void MainGalleryWindow::onImageLoaded(int id, const QImage& image) {
    qDebug() << "MainGalleryWindow: Ricevuta immagine ID" << id;
    if (id == m_uiNavigator->currentImageId()) {
        // Aggiorna la visualizzazione solo se è l'immagine che ci aspettiamo attualmente
        updateImageDisplay(image);
    }
    // Anche se non è l'immagine corrente, potrebbe essere in cache ora per un uso futuro
}

/**
 * @brief Slot to handle image loading errors.
 *
 * This slot is connected to the ImageLoader::loadingError signal.
 * It displays an error message on the image label if the failed ID matches the current ID,
 * and also shows a QMessageBox to the user.
 *
 * @param id The ID of the image that failed to load.
 * @param errorMessage A string describing the error.
 */
void MainGalleryWindow::onLoadingError(int id, const QString& errorMessage) {
    qDebug() << "MainGalleryWindow: Errore di caricamento per ID" << id << ":" << errorMessage;
    if (id == m_uiNavigator->currentImageId()) {
        ui->imageLabel->setText(QString("Errore caricamento immagine %1:\n%2").arg(id).arg(errorMessage));
        ui->imageLabel->setPixmap(QPixmap());
    }
    QMessageBox::warning(this, "Errore di caricamento", QString("Impossibile caricare l'immagine ID %1: %2").arg(id).arg(errorMessage));
}

/**
 * @brief Slot to react to image ID changes from UINavigator.
 *
 * This slot is connected to the UINavigator::imageIdChanged signal.
 * It updates the ID label, navigation button states, and triggers
 * the loading of the new image.
 *
 * @param newId The new current image ID.
 */
void MainGalleryWindow::onImageIdChanged(int newId) {
    qDebug() << "MainGalleryWindow: ID immagine cambiato in" << newId;
    updateIdLabel(newId, m_maxImageId);
    updateNavigationButtons(newId, m_maxImageId);
    m_imageLoader->loadImageAsync(newId); // Richiede il caricamento della nuova immagine
}

/**
 * @brief Slot for the "Previous" button click.
 *
 * This slot is connected to the clicked() signal of the previous button.
 * It requests the UINavigator to move to the previous image.
 */
void MainGalleryWindow::on_prevButton_clicked() {
    qDebug() << "Pulsante precedente cliccato.";
    m_uiNavigator->previous(); // Tenta di spostarsi all'immagine precedente
}

/**
 * @brief Slot for the "Next" button click.
 *
 * This slot is connected to the clicked() signal of the next button.
 * It requests the UINavigator to move to the next image.
 */
void MainGalleryWindow::on_nextButton_clicked() {
    qDebug() << "Pulsante successivo cliccato.";
    m_uiNavigator->next(); // Tenta di spostarsi all'immagine successiva
}
