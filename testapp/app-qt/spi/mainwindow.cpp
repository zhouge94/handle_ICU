#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <QMessageBox>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int pd;
    char buf[32];

    fd = ::open("/dev/spidev0.0",O_RDWR);

    if(fd < 0)
    {
        QMessageBox::about(this,"error","open spidev failure");
        exit(0);
    }

    ::close(pd);

    ui->setupUi(this);

    update_params();
}

void MainWindow::update_params()
{
    unsigned char mode = 0,bits;
    unsigned int speed,ret;
    unsigned short delay;

    speed = ui->speed->text().toInt();
    delay = ui->delay->text().toInt();
    bits  = ui->bits->text().toInt();

    if(ui->loop->isChecked())
        mode |= SPI_LOOP;

    if(ui->clockphase->isChecked())
        mode |= SPI_CPHA;

    if(ui->clockpolarity->isChecked())
        mode |= SPI_CPOL;

    if(ui->lsb->isChecked())
        mode |= SPI_LSB_FIRST;

    if(ui->cshigh->isChecked())
        mode |= SPI_CS_HIGH;

    if(ui->wire->isChecked())
        mode |= SPI_3WIRE;

    if(ui->nocs->isChecked())
        mode |= SPI_NO_CS;

    if(ui->ready->isChecked())
        mode |= SPI_READY;

    ret = ::ioctl(fd,SPI_IOC_WR_MODE,&mode);
    if(ret < 0)
    {
        QMessageBox::about(this,"error","SPI_IOC_WR_MODE failure");
        exit(0);
    }

    ret = ::ioctl(fd,SPI_IOC_WR_BITS_PER_WORD,&bits);
    if(ret < 0)
    {
        QMessageBox::about(this,"error","SPI_IOC_WR_BITS_PER_WORD failure");
        exit(0);
    }

    ret = ::ioctl(fd,SPI_IOC_WR_MAX_SPEED_HZ,&speed);
    if(ret < 0)
    {
        QMessageBox::about(this,"error","SPI_IOC_WR_MAX_SPEED_HZ failure");
        exit(0);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    ::close(fd);
}

void MainWindow::on_send_clicked()
{
    std::string str = ui->editsend->text().toStdString();
    struct spi_ioc_transfer tr;
    char buf[str.length()+1];
    memset(buf,0,str.length()+1);

    tr.tx_buf = (__u64)str.c_str();
    tr.rx_buf = (__u64)buf;
    tr.len = str.length();
    tr.delay_usecs = ui->delay->text().toInt();
    tr.speed_hz = ui->speed->text().toInt();
    tr.bits_per_word = ui->bits->text().toInt();

    ::ioctl(fd,SPI_IOC_MESSAGE(1),&tr);

    ui->editrecv->append(QString(buf));

}

void MainWindow::on_loop_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_clockphase_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_clockpolarity_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_lsb_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_cshigh_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_wire_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_nocs_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_ready_clicked(bool checked)
{
    update_params();
}

void MainWindow::on_speed_textChanged(const QString &arg1)
{
    update_params();
}

void MainWindow::on_delay_textChanged(const QString &arg1)
{
    update_params();
}

void MainWindow::on_bits_textChanged(const QString &arg1)
{
    update_params();
}

void MainWindow::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    exit(0);
}

