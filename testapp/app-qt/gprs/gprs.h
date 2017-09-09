#ifndef GPRS_H
#define GPRS_H

#include <QWidget>
#include <qsocketnotifier.h>
#include<QProcess>


#define PPPPATH           "/"
namespace Ui {
class Gprs;
}

class Gprs : public QWidget
{
    Q_OBJECT
    
public:
    explicit Gprs(QWidget *parent = 0);
    ~Gprs();

private:
     int openSerialPort();


private slots:
    void on_pbt_set_clicked();

    void on_pbt_call_clicked();

    void on_pbt_send_clicked();

    void on_pbt_gprs_clicked();

    void remoteDataIncoming();

    void readOutput();

    void on_ping_clicked();

    void result();

protected:
    void closeEvent(QCloseEvent *);
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);

private:
    Ui::Gprs *ui;
     int fd;
     QString telnum;
     QSocketNotifier *m_notifier;
     QString portName;
     QProcess* m_startgprs;
     QProcess* myprocess;
     QTimer *m_timer;


};

#endif // GPRS_H
