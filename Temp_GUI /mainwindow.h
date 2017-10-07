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
    void timercallback1();
    void on_pushButton_t1_clicked();

    void on_pushButton_t2_clicked();

    void on_pushButton_t3_clicked();

    void on_pushButton_t4_clicked();

    void on_pushButton_t5_clicked();

    void on_pushButton_save_clicked();

    void on_pushButton_reset_clicked();

private:
    Ui::MainWindow *ui;
    void InitWinParam();
    QVector<QString> cmd_v;
    QVector<QString> fifo_name;
    void update_temp_conf();
    void tempEvent();
};

#endif // MAINWINDOW_H
