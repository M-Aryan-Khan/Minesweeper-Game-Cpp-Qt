#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>
#include <QMap>
#include <QMessageBox>
#include "MyHeader.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QHBoxLayout* firstLayout; // here you can select the difficulty, shows the game time, etc.
    QGridLayout* gameLayout; // contains the game buttons
    QVBoxLayout* mainLayout; // contains the previous two layouts
    QComboBox* difficulties; // for selecting the game difficulty
    QFont font; // for setting the font of buttons, labels
    MyButton*** buttons; // the game buttons
    QPushButton* resetButton; // reset button for a new game
    QPushButton* showHighscores; // button to show the top 10 scores
    QString** buttonText; // texts corresponding to the buttons, e.g., bomb, empty, 1, 2, etc.
    QLabel* flags; // shows how many flags are left
    QLabel* timeLabel; // shows the game time
    QTimer* timer; // measures the game time
    double time; // necessary for time measurement
    int buttonRows, buttonColumns; // store the current size of the game area
    int numberOfBombs; // number of bombs, depends on difficulty
    int disabledButtons; // stores how many buttons are no longer usable during the game, needed to determine the end of the game
    int numberOfFlags; // initially equals the number of bombs, stores the remaining number of flags
    bool endOfGame; // false during the game, true after winning/losing
    bool firstClick;
    bool bombsPlaced;

    void clearButtons(); // removes all buttons from the game area
    void setButtons(int n, int m, int size); // sets up the buttons
    void setBombs(); // randomly places the bombs (issue: the seed for random generation depends on the current time, but it still produces the same result every run)
    void setNumbers(); // sets the numbers on the buttons based on the bombs
    void onFreeButtonClicked(int i, int j); // when an empty button is clicked; recursive
    void bombClicked(); // when a bomb is clicked
    void win(); // when the player wins
    void reset(); // resets everything to the default state
    void retrieveSettings(int &n, int &m, int &mines);
    void showEvent(QShowEvent *event) override;


private slots:
    void buttonClicked(); // when a button on the game area is clicked
    void rightButtonClicked(); // when a right-click is pressed
    void tick(); // necessary for time measurement

};
#endif // MAINWINDOW_H
