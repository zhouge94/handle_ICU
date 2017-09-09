#ifndef KEYTEST_H
#define KEYTEST_H

#include <QWidget>

#define  KEYMAXNUM  6

class QSocketNotifier; //need this class

namespace Ui
{
class Keytest;
}


struct InputData
{
    unsigned int dummy1;
    unsigned int dummy2;
    unsigned short type;
    unsigned short code;
    unsigned int value;
    unsigned int dummy3;
    unsigned int dummy4;
    unsigned int dummy5;
    unsigned int dummy6;
};

#define KEY_UP	 	256 	//		103   // key 1
#define KEY_DOWN	257 	//		108   // key 2
#define KEY_LEFT 	258	//		105   // key 3
#define KEY_RIGHT	259	//		106   // key 4
#define KEY_ENTER	260	//	28    // key 5
#define KEY_ESC	 	261	//		1     // key 6

//#define HOME  256
//#define MENU  257
//#define ESC   258
//#define CALL  259
//#define END   260
//#define OTHER 261


class Keytest : public QWidget
{
    Q_OBJECT
    
public:
    explicit Keytest(QWidget *parent = 0);
    ~Keytest();
     QWidget *parent;

protected:
  void paintEvent(QPaintEvent* e);
  void moveEvent(QMoveEvent *);
  void resizeEvent(QResizeEvent *);

private:
    Ui::Keytest *ui;
    volatile int m_fd;
    QSocketNotifier* m_notifyObject;
    bool m_oldButtonsState[KEYMAXNUM];

private slots:
  void keyEvent();

};

#endif // KEYTEST_H
