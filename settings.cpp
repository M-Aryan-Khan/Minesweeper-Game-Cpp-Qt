#include "settings.h"
#include "ui_settings.h"
#include <string>
#include <QString>
#include <sstream>
#include <QDebug>
#include <stdexcept>
#include <fstream>

// Constructor for the settings class
settings::settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settings) // Initialize UI
{
    ui->setupUi(this); // Set up the user interface

    // Retrieve saved settings or apply defaults if necessary
    retrieveSettings(n, m, mines);

    // Update the UI with the retrieved or default values
    ui->boardSize->setText(QString::number(n) + "x" + QString::number(m));
    ui->NoOfMines->setText(QString::number(mines));
}

// Destructor for the settings class
settings::~settings()
{
    delete ui; // Clean up the UI
}

// Function to retrieve settings from the settings.txt file
void settings::retrieveSettings(int &j, int &k, int &l) {
    std::ifstream inFile("settings.txt"); // Open settings file for reading

    if (inFile.is_open()) { // Check if file opened successfully
        std::string line;
        try {
            // Read and convert the first line to the 'n' variable
            if (std::getline(inFile, line) && !line.empty()) {
                j = std::stoi(line);
            }

            // Read and convert the second line to the 'm' variable
            if (std::getline(inFile, line) && !line.empty()) {
                k = std::stoi(line);
            }

            // Read and convert the third line to the 'mines' variable
            if (std::getline(inFile, line) && !line.empty()) {
                l = std::stoi(line);
            }
        } catch (const std::exception &e) {
            // If an error occurs during reading or conversion, apply default values
            j = 9;
            k = 9;
            l = 10;

            // Close the input file
            inFile.close();

            // Create and write default values to a new settings.txt file
            std::ofstream outFile("settings.txt");
            if (outFile.is_open()) {
                outFile << j << '\n';
                outFile << k << '\n';
                outFile << l << '\n';
                outFile.close(); // Close the output file
            }
        }
    } else {
        // If the file does not exist or cannot be opened, apply default values
        j = 9;
        k = 9;
        l = 10;

        // Create and write default values to a new settings.txt file
        std::ofstream outFile("settings.txt");
        if (outFile.is_open()) {
            outFile << j << '\n';
            outFile << k << '\n';
            outFile << l << '\n';
            outFile.close(); // Close the output file
        }
    }
}

// Slot triggered when the save button is clicked
void settings::on_save_clicked()
{
    try {
        // Get the board size and number of mines from the UI
        QString size = ui->boardSize->text();
        std::string stdSize = size.toStdString();

        QString nmines = ui->NoOfMines->text();
        std::string minestd = nmines.toStdString();

        // Find the position of the 'x' in the board size string
        size_t xPos = stdSize.find('x');
        if (xPos != std::string::npos) {
            // Extract the board dimensions from the string
            std::string nStr = stdSize.substr(0, xPos);
            std::string mStr = stdSize.substr(xPos + 1);

            // Ensure that both dimensions are valid numbers
            if (nStr.empty() || mStr.empty() || !std::all_of(nStr.begin(), nStr.end(), ::isdigit) || !std::all_of(mStr.begin(), mStr.end(), ::isdigit)) {
                throw std::invalid_argument("Enter correct format for board size (e.g., 9x9)");
            }

            // Convert the dimensions to integers
            n = std::stoi(nStr);
            m = std::stoi(mStr);

            // Validate that the dimensions are within the allowed range
            if (n < 9 || m < 9) {
                throw std::invalid_argument("Board size must be at least 9x9");
            }

            if (n > 30 || m > 30) {
                throw std::invalid_argument("Board size must be less than or equal to 30x30");
            }

            // Validate the number of mines
            if (minestd.empty() || !std::all_of(minestd.begin(), minestd.end(), ::isdigit)) {
                throw std::invalid_argument("Invalid number of mines");
            }
            mines = std::stoi(minestd);

            // Open the settings file for writing
            std::ofstream outFile("settings.txt");
            if (outFile.is_open()) {
                // Write the board dimensions and number of mines to the file
                outFile << n << '\n';
                outFile << m << '\n';
                outFile << mines << '\n';
                outFile.close(); // Close the file

                // Close the settings dialog
                this->close();
            } else {
                throw std::runtime_error("Unable to open file for writing");
            }
        } else {
            // Display an error message if the board size format is incorrect
            ui->error->setText("Enter correct format for board size (e.g., 9x9)");
        }
    } catch (const std::invalid_argument &e) {
        // Display the error message for invalid input
        ui->error->setText(e.what());
    } catch (const std::out_of_range &e) {
        // Display an error message for numbers out of range
        ui->error->setText("Number out of range");
    } catch (const std::runtime_error &e) {
        // Display an error message for file writing errors
        ui->error->setText(e.what());
    } catch (...) {
        // Display a generic error message for any other exceptions
        ui->error->setText("An unexpected error occurred");
    }
}

// Slot triggered when the cancel button is clicked
void settings::on_cancel_clicked()
{
    this->close(); // Close the settings dialog without saving
}
