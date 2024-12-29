#include "startscreen.h"
#include "mainwindow.h"
#include "settings.h"
#include "ui_startscreen.h"
#include <QLabel>
#include <QPixmap>
#include <QDebug>

// Constructor for the startScreen class
startScreen::startScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::startScreen) // Initialize UI
{
    ui->setupUi(this); // Set up the user interface

    // Set window flags to allow minimize and close buttons
    this->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    // Create a QLabel to hold the background image
    QLabel* backgroundLabel = new QLabel(this);

    // Load the background image from resources
    QPixmap pixmap(":/icons/bg.png");
    backgroundLabel->setPixmap(pixmap); // Set the image to the label

    // Ensure the background image scales to the window size
    backgroundLabel->setScaledContents(true);

    // Position the background image to cover the entire window
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());

    // Ensure the background image is behind all other widgets
    backgroundLabel->lower();
}

// Destructor for the startScreen class
startScreen::~startScreen()
{
    delete ui; // Clean up the UI
}

// Slot triggered when the settings button is clicked
void startScreen::on_settings_clicked()
{
    settings s; // Create a settings dialog
    s.exec(); // Execute the dialog modally (blocks interaction with other windows)
}

// Slot triggered when the play button is clicked
void startScreen::on_play_clicked()
{
    this->hide(); // Hide the start screen

    // Create the main game window
    MainWindow *w = new MainWindow();

    // Ensure the main window is deleted automatically when closed
    w->setAttribute(Qt::WA_DeleteOnClose);

    // Show the main game window
    w->show();
}
