#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <camera_thread.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QImage img;

signals:
    void capture();

private slots:
    void on_ptnStart_clicked();
    void show_picture(const unsigned char *rgb);
    void on_ptnCap_clicked();
    void on_ptnStop_clicked();

    void on_radio5640_toggled(bool checked);

    void on_radio5645_toggled(bool checked);

private:
    Ui::Widget *ui;
    video_thread *video;
};

#endif // WIDGET_H
