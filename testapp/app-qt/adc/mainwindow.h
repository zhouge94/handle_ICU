#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBasicTimer>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void timerEvent(QTimerEvent *);
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);
    
private:
    Ui::MainWindow *ui;
    QBasicTimer timer;
    QString boardname;
};

#endif // MAINWINDOW_H
