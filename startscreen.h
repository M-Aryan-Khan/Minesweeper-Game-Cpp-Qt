#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QMainWindow>
#include "mainwindow.h"

namespace Ui {
class startScreen;
}

class startScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit startScreen(QWidget *parent = nullptr);
    ~startScreen();

private slots:
    void on_settings_clicked();
    void on_play_clicked();

private:
    Ui::startScreen *ui;
    MainWindow *mainWindow; // Pointer to the MainWindow
};

#endif // STARTSCREEN_H
