#include "mainwindow.h"
#include "startscreen.h"
#include <QDebug>
#include <QDateTime>
#include <QInputDialog>
#include <QMapIterator>
#include <cstdlib>
#include <fstream>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QShowEvent>

// Constructor definition
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *w = new QWidget;
    setCentralWidget(w);
    setWindowTitle("Minesweeper");
    w->setStyleSheet("background-color:grey");

    // Set up layouts
    font.setFamily("Comic Sans MS");
    mainLayout = new QVBoxLayout;
    firstLayout = new QHBoxLayout;
    firstLayout->setSpacing(50);
    gameLayout = new QGridLayout;
    gameLayout->setSpacing(0);
    gameLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(firstLayout);
    mainLayout->addLayout(gameLayout);
    w->setLayout(mainLayout);

    // Initialize member variables
    time = 0;
    disabledButtons = 0;
    endOfGame = false;

    // Set up firstLayout: game time, reset button
    // Set up game board
    //w:16 -> 43, w:21 -> 33, w:30 -> 23
    int n, m, mines;
    retrieveSettings(n, m, mines);
    if(n < 17){
        setButtons(n,m,43);
    }
    else if(n < 22){
        setButtons(n,m,33);
    }
    else{
        setButtons(n,m,23);
    }

    numberOfBombs = mines;
    numberOfFlags = mines; // Initialize number of flags to number of bombs

    flags = new QLabel;
    font = flags->font();
    font.setPointSize(font.pointSize() + 8);
    flags->setFont(font);
    flags->setText("Flags: " + QString::number(mines));


    firstLayout->addWidget(flags);

    //setBombs();
    //setNumbers();
    firstClick = false;
    bombsPlaced = false;

    timeLabel = new QLabel("0:0");
    timeLabel->setFont(font);
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
    firstLayout->addWidget(timeLabel);

    resetButton = new QPushButton("Reset");
    resetButton->setFont(font);
    connect(resetButton, &QPushButton::clicked,  this, [&](){reset();} );
    firstLayout->addWidget(resetButton);


}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event); // Call base class implementation

    // Center the window on the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        int windowWidth = this->width();
        int windowHeight = this->height();

        int x = (screenWidth - windowWidth) / 2;
        int y = (screenHeight - windowHeight) / 2;

        this->setGeometry(x, y, windowWidth, windowHeight);
    }
}


void MainWindow::retrieveSettings(int &j, int &k, int &l) {
    std::ifstream inFile("settings.txt");

    if (inFile.is_open()) {
        std::string line;
        try {
            if (std::getline(inFile, line) && !line.empty()) {
                j = std::stoi(line); // Read n
            }
            if (std::getline(inFile, line) && !line.empty()) {
                k = std::stoi(line); // Read m
            }
            if (std::getline(inFile, line) && !line.empty()) {
                l = std::stoi(line); // Read mines
            }
        } catch (const std::exception &e) {
            // If there's an issue with reading or conversion, set defaults
            j = 9;
            k = 9;
            l = 10;
            inFile.close();
            std::ofstream outFile("settings.txt");
            if (outFile.is_open()) {
                outFile << j << '\n'; // Write n to the file
                outFile << k << '\n'; // Write m to the file
                outFile << l << '\n'; // Write mines to the file
                outFile.close();
            }
        }
    } else {
        // If the file cannot be opened, set defaults
        j = 9;
        k = 9;
        l = 10;

        std::ofstream outFile("settings.txt");
        if (outFile.is_open()) {
            outFile << j << '\n'; // Write n to the file
            outFile << k << '\n'; // Write m to the file
            outFile << l << '\n'; // Write mines to the file
            outFile.close();
        }
    }
}

void MainWindow::setButtons(int n, int m, int size){
    buttonRows = n;
    buttonColumns = m;
    buttons = new MyButton**[n];
    buttonText = new QString*[n];
    for(int i=0; i<n; ++i){
        buttons[i] = new MyButton*[m];
        buttonText[i] = new QString[m];
    }
    for(int i=0; i<n; ++i){
        for(int j=0; j<m; ++j){
            buttonText[i][j] = '-';
        }
    }
    int fontsize = 12;

    // Create buttons and add them to the game board
    for(int i=0; i<n; ++i){
        for(int j=0; j<m; ++j){
            buttons[i][j] = new MyButton(this);
            buttons[i][j]->setMinimumSize(size, size);
            buttons[i][j]->setMaximumWidth(size+7);
            buttons[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            font = buttons[i][j]->font();
            font.setPointSize(font.pointSize() + fontsize);
            buttons[i][j]->setFont(font);
            buttons[i][j]->installEventFilter(this);
            connect(buttons[i][j], SIGNAL(clicked()), this, SLOT(buttonClicked()));
            connect(buttons[i][j], SIGNAL(rightClick()), this, SLOT(rightButtonClicked()));
            buttons[i][j]->setStyleSheet("background-color:silver");
            gameLayout->addWidget(buttons[i][j], i, j);
        }
    }
}

void MainWindow::setBombs() {
    if (bombsPlaced) {
        return; // Do nothing if bombs have already been placed
    }

    int nB = numberOfBombs;
    // Clear existing bombs
    for (int i = 0; i < buttonRows; ++i) {
        for (int j = 0; j < buttonColumns; ++j) {
            if (buttonText[i][j] == "X") {
                buttonText[i][j] = "-";
            }
        }
    }

    while (nB > 0) {
        QTime time = QTime::currentTime();
        std::srand(static_cast<unsigned>(time.msec())); // Seed based on current time
        int x = std::rand() % buttonRows;
        int y = std::rand() % buttonColumns;
        if (buttonText[x][y] == "X") { // If there's already a bomb at that position
            continue;
        }
        buttonText[x][y] = "X";
        --nB;
    }
}


void MainWindow::setNumbers(){
    for(int i=0; i<buttonRows; ++i){
        for(int j=0; j<buttonColumns; ++j){
            if(buttonText[i][j] == 'X'){
                continue;
            }
            int db = 0; // Number of bombs around the button
            for(int x=i-1; x<i+2; ++x){
                for(int y=j-1; y<j+2; ++y){
                    if(x<0 || x>buttonRows-1 || y<0 || y>buttonColumns-1){ // If we're outside the game board
                        continue;
                    }
                    if(buttonText[x][y] == "X"){
                        ++db;
                    }
                }
            }
            if(db != 0){
                buttonText[i][j] = QString::number(db);
            }
        }
    }
}

void MainWindow::onFreeButtonClicked(int i, int j) {
    if (i < 0 || i >= buttonRows || j < 0 || j >= buttonColumns || buttonText[i][j] == "X" || !buttons[i][j]->isEnabled()) {
        // Exit condition: out of bounds, bomb, or already revealed
        return;
    }

    if (buttonText[i][j] == '-') { // If the button is empty
        buttons[i][j]->setStyleSheet("background-color:azure");
        buttons[i][j]->setEnabled(false);
        ++disabledButtons;

        // Reveal adjacent cells in the four cardinal directions
        onFreeButtonClicked(i - 1, j); // Up
        onFreeButtonClicked(i + 1, j); // Down
        onFreeButtonClicked(i, j - 1); // Left
        onFreeButtonClicked(i, j + 1); // Right
    } else { // If there's a number on the button
        buttons[i][j]->setStyleSheet("background-color:wheat");
        buttons[i][j]->setText(buttonText[i][j]);
        buttons[i][j]->setEnabled(false);
        ++disabledButtons;
    }
}

void MainWindow::bombClicked() { // When a bomb is clicked
    endOfGame = true;
    timer->stop();
    flags->setText("You lost!");

    for (int i = 0; i < buttonRows; ++i) { // Highlight all buttons containing bombs
        for (int j = 0; j < buttonColumns; ++j) {
            if (buttonText[i][j] == "X") {
                buttons[i][j]->setIcon(QIcon(":/icons/bomb.png")); // Set bomb icon
                buttons[i][j]->setIconSize(buttons[i][j]->size());   // Adjust icon size to button size
                buttons[i][j]->setStyleSheet("background-color:red"); // Set background to red
            }
        }
    }
}

void MainWindow::win(){ // In case of winning
    endOfGame = true;
    timer->stop();
    flags->setText("You win!");

    for(int i=0; i<buttonRows; ++i){ // Highlight buttons according to their content
        for(int j=0; j<buttonColumns; ++j){
            if(buttonText[i][j] == "X"){
                buttons[i][j]->setStyleSheet("background-color:pink");
                buttons[i][j]->setText(buttonText[i][j]);
            }
            else{
                if(buttonText[i][j] == "-"){
                    buttons[i][j]->setStyleSheet("background-color:powderblue");
                }
                else{
                    buttons[i][j]->setStyleSheet("background-color:powderblue");
                    buttons[i][j]->setText(buttonText[i][j]);
                }
            }
        }
    }
}

void MainWindow::reset(){ // Resets everything according to the current difficulty level
    endOfGame = false;
    time = 0;
    timer->stop();
    timeLabel->setText("0:0");
    disabledButtons = 0;
    int n,m,mines;
    retrieveSettings(n,m,mines);
    clearButtons();
    if(n < 17){
        setButtons(n,m,43);
    }
    else if(n < 22){
        setButtons(n,m,33);
    }
    else{
        setButtons(n,m,23);
    }
    setBombs();
    setNumbers();
    flags->setText("Flags: " + QString::number(mines));
    numberOfFlags = mines;
    firstClick = false;
    bombsPlaced = false;
}

void MainWindow::clearButtons(){ // Removes all buttons from the game board
    while(gameLayout->count()){ // For some reason, it doesn't work with a single for-loop
        for(int i=0; i<gameLayout->count(); ++i){
            QWidget *tempWidget = gameLayout->itemAt(i++)->widget();
            mainLayout->removeWidget(tempWidget);
            delete tempWidget;
        }
    }
    delete buttons;
    delete buttonText;
}

void MainWindow::buttonClicked() {
    if (endOfGame) {
        return; // Do nothing if the game has ended
    }

    if (!timer->isActive()) {
        timer->start(80); // Start the timer if it's not running
    }

    QPushButton* tmp = qobject_cast<QPushButton*>(sender());
    if (tmp->styleSheet() == "background-color:transparent") {
        return; // Ignore if button has a flag
    }

    int x, y;
    for (int i = 0; i < buttonRows; ++i) {
        for (int j = 0; j < buttonColumns; ++j) {
            if (buttons[i][j] == tmp) {
                x = i;
                y = j;
                break;
            }
        }
    }

    if (!firstClick) {
        firstClick = true;
        if (!bombsPlaced) {
            // Place bombs ensuring the first click is not on a bomb
            do {
                setBombs();
            } while (buttonText[x][y] == "X"); // Ensure first click is not on a bomb
            setNumbers(); // Set numbers after placing bombs
            bombsPlaced = true; // Set flag to true after bombs are placed
        }
    }

    if (buttonText[x][y] != "X") {
        onFreeButtonClicked(x, y);
        if (disabledButtons == (buttonRows * buttonColumns - numberOfBombs)) {
            win(); // Player wins if all non-bomb buttons are revealed
        }
    } else {
        bombClicked(); // Game over if a bomb is clicked
    }
}


void MainWindow::rightButtonClicked() {
    if (!timer->isActive()) {
        timer->start(80);
    }

    QPushButton* tmp = qobject_cast<QPushButton*>(sender());
    int x, y;

    // Check if a flag is already set
    if (tmp->styleSheet() == "background-color:transparent" && numberOfFlags >=0 && numberOfFlags < numberOfBombs) {
        for (int i = 0; i < buttonRows; ++i) {
            for (int j = 0; j < buttonColumns; ++j) {
                if (buttons[i][j] == tmp) {
                    x = i;
                    y = j;
                    break;
                }
            }
        }
        tmp->setIcon(QIcon());
        tmp->setStyleSheet("background-color:silver");
        ++numberOfFlags;
    } else if(numberOfFlags >0 && numberOfFlags <= numberOfBombs) {
        tmp->setStyleSheet("background-color:transparent");
        tmp->setIcon(QIcon(":/icons/flag.png")); // Set flag icon
        tmp->setIconSize(tmp->size());            // Adjust icon size to button size
        --numberOfFlags;
    }

    flags->setText("Flags: " + QString::number(numberOfFlags));
}



void MainWindow::tick(){ // Used for time measurement
    time += 0.1;
    timeLabel->setText(QString::number(time));
}


MainWindow::~MainWindow()
{
    startScreen *start = new startScreen();
    start->show();
}

