#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_btnQuit_clicked();

    void on_txtPath_textChanged(const QString &arg1);

    void on_btnPath_clicked();

    void on_btnInstall_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
