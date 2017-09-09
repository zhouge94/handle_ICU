#include "widget.h"
#include "ui_widget.h"
#include "common.h"
#include <QTime>

extern char flag;


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    //on_ptnStart_clicked();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_ptnStart_clicked()
{
    if(ui->radio5640->isChecked())
        flag = 0;
    else
        flag = 1;

    video = new video_thread();
    connect(video, SIGNAL(image_data(const unsigned char *)), this, SLOT(show_picture(const unsigned char *)));
    connect(this, SIGNAL(capture()), video, SLOT(capture()));
    video->start();
}

void Widget::show_picture(const unsigned char *rgb)
{
    printf("show_picture\n");
    img = QImage(rgb, WIDTH, HEIGHT,QImage::Format_RGB16);
    img.save(tr("%1.jpg").arg(QTime::currentTime().toString("hh-mm-ss")),"JPG");
}

void Widget::on_ptnCap_clicked()
{
    printf("on_ptnCap_clicked\n");
    emit capture();
}

void Widget::on_ptnStop_clicked()
{
    delete video;
}

void Widget::on_radio5640_toggled(bool checked)
{
    if(checked)
        ui->radio5645->setChecked(0);
    else
        ui->radio5645->setChecked(1);
}

void Widget::on_radio5645_toggled(bool checked)
{
    if(checked)
        ui->radio5640->setChecked(0);
    else
        ui->radio5640->setChecked(1);
}
