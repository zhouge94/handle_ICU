#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sys.h"
#include"QTimer"
#include <QKeyEvent>
#include "QMessageBox"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyEvent(void);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();
    void syscmd();
    void timercallback1();
private:
    Ui::MainWindow *ui;
    void InitWinParam();
    QVector<QString> cmd_v;
    QVector<QString> fifo_name;
};

#endif // MAINWINDOW_H
