#ifndef MOUSEROBOT_H
#define MOUSEROBOT_H

#include <QtGui/qwindowdefs.h>
#include <QPoint>


class MouseRobot
{
    class MouseRobotImpl;

public:
    enum Button
    {
        Button1	= 1,
        Button2	= 2,
        Button3	= 3,
        Button4	= 4,
        Button5	= 5
    };

public:
    MouseRobot(WId parentWindow);
    ~MouseRobot();

public:
    void setMouseCursor();
    void mouseMove(quint32 x, quint32 y);
    void mouseClick(Button button);
    void keyType(quint32 key);

 private:
    MouseRobotImpl *mImpl;
};

#endif // MOUSEROBOT_H
