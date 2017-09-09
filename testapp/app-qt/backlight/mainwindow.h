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
    
private:
    Ui::MainWindow *ui;
    int fd;

private slots:
    void sliderchanged(int v);
protected:
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);

};

#endif // MAINWINDOW_H
