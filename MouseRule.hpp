#ifndef MOUSERULE_HPP
#define MOUSERULE_HPP

#include <QWidget>
#include <QTime>
#include <QIcon>
#include "GlassWindow.hpp"


namespace Ui
{
class MouseRule;
}
class QListWidgetItem;
class MouseRobot;


enum EPositionMode
{
    CurrentPosition,
    AbsolutePosition,
    RelativePosition
};


enum EIntervalMode
{
    MillisecondsInterval,
    SecondsInterval,
    MinutesInterval,
    HoursInterval
};


enum EActionMode
{
    ButtonAction,
    KeyAction,
    NoAction
};


class MouseRule : public QWidget, public Drawable
{
    Q_OBJECT

public:
    MouseRule(QWidget *parent = 0,
              QPoint position = QPoint(), EPositionMode positionMode = CurrentPosition,
              quint32 interval = 50, EIntervalMode intervalMode = MillisecondsInterval,
              quint32 action = 1, EActionMode actionMode = ButtonAction);
    virtual ~MouseRule();

signals:
    void removeClicked();
    void addClicked();
    void posPressed();

public:
    void invoke(MouseRobot &robot);
    void setButtonState(bool isRemoveEnabled, bool isAddEnabled);
    int number() const;

    void setPredecessor(MouseRule *predRule);
    MouseRule *predecessor();
    QPoint absolutePosition() const;

    void setItem(QListWidgetItem *item);
    QListWidgetItem *item();

    void setPosition(QPoint position, EPositionMode positionMode);
    QPoint position() const;
    EPositionMode positionMode() const;

    void setInterval(quint32 interval, EIntervalMode intervalMode);
    quint32 interval() const;
    EIntervalMode intervalMode() const;

    void setAction(quint32 action, EActionMode actionMode);
    quint32 action() const;
    EActionMode actionMode() const;

protected slots:
    void grabMouse();
    void ungrabMouse();
    void changePositionMode(int positionIndex);
    void changeIntervalMode(int intervalIndex);
    void changeActionMode(int actionIndex);
    void ui2pos();
    void pos2ui();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void draw(QPainter &painter) const;

private:
    Ui::MouseRule *mUi;
    bool mIsDragging;
    MouseRule *mPredecessor;
    QListWidgetItem *mItem;
    QIcon mPosIconAbs;
    QIcon mPosIconRel;
    QTime mTimer;
    QPoint mPosition;
    QPoint mPositionOffset;
    QPoint mBasePosition;
    EPositionMode mPositionMode;
    EIntervalMode mIntervalMode;
    EActionMode mActionMode;
};

#endif // MOUSERULE_HPP
