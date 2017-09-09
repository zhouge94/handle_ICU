#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    system(". /forlinx/web/lighttpd/webpages/shell/movie-once.sh");
    this->close();
}

void Dialog::on_pushButton_2_clicked()
{
    system(". /forlinx/web/lighttpd/webpages/shell/movie-loop.sh");
    this->close();
}
