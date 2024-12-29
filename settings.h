#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class settings;
}

class settings : public QDialog
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();
    int n = 9;
    int m = 9;
    int mines = 10;

private slots:
    void on_save_clicked();

    void on_cancel_clicked();

private:
    Ui::settings *ui;
    void retrieveSettings(int &n, int &m, int &mines);
};

#endif // SETTINGS_H
