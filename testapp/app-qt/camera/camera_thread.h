#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H
#include <QThread>
#include <QByteArray>
#include <QImage>
#include "v4l2.h"
class video;

class video_thread:public QThread
{
    Q_OBJECT
public:
    explicit video_thread();
    ~video_thread();
    char needCapture;

protected :
    void run();

signals:
    void image_data(const unsigned char *rgb);

public slots:
    void capture();

private:
    video m_video;
    bool quit_flag;//线程释放标志否则线程一直会释放不了（在调用quit（）和 wait()函数后）
};
#endif // CAMERA_THREAD_H
