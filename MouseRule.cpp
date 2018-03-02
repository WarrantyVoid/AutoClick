#include "MouseRule.hpp"
#include "MouseRobot.hpp"
#include "ui_MouseRule.h"
#include <QDesktopWidget>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QPainter>


MouseRule::MouseRule(QWidget *parent,
                     QPoint position, EPositionMode positionMode,
                     quint32 interval, EIntervalMode intervalMode,
                     quint32 action, EActionMode actionMode)
    : QWidget(parent)
    , Drawable()
    , mUi(new Ui::MouseRule)
    , mIsDragging(false)
    , mPredecessor(0)
    , mItem(0)
    , mPosIconAbs()
    , mPosIconRel()
    , mTimer()
    , mPosition()
    , mPositionOffset()
    , mBasePosition(QApplication::desktop()->screenGeometry().center())
    , mPositionMode(positionMode)
    , mIntervalMode(intervalMode)
    , mActionMode(actionMode)
{
    mUi->setupUi(this);
    setPosition(position, positionMode);
    setInterval(interval, intervalMode);
    setAction(action, actionMode);

    connect(mUi->removeButton, SIGNAL(clicked()), this, SIGNAL(removeClicked()));
    connect(mUi->addButton, SIGNAL(clicked()), this, SIGNAL(addClicked()));
    connect(mUi->absButton, SIGNAL(pressed()), this, SLOT(grabMouse()));
    connect(mUi->relButton, SIGNAL(pressed()), this, SLOT(grabMouse()));
    connect(mUi->positionSelect, SIGNAL(activated(int)), this, SLOT(changePositionMode(int)));
    connect(mUi->intervalSelect, SIGNAL(activated(int)), this, SLOT(changeIntervalMode(int)));
    connect(mUi->actionSelect, SIGNAL(activated(int)), this, SLOT(changeActionMode(int)));
    connect(mUi->absEdit, SIGNAL(textEdited(const QString&)), this, SLOT(ui2pos()));
    connect(mUi->relEdit, SIGNAL(textEdited(const QString&)), this, SLOT(ui2pos()));
    mPosIconAbs = mUi->absButton->icon();
    mPosIconRel = mUi->relButton->icon();
    mTimer.start();
}


MouseRule::~MouseRule()
{
    if (mUi)
    {
        delete mUi;
        mUi = 0;
    }
}


void MouseRule::invoke(MouseRobot &robot)
{
    quint32 v = interval();
    if (static_cast<quint32>(mTimer.elapsed()) >= v)
    {
        // Move rule
        switch (mPositionMode)
        {
        case CurrentPosition:
        {
            break;
        }
        case AbsolutePosition:
        {
            QPointF pos = position();
            robot.mouseMove(pos.x(), pos.y());
            break;
        }
        case RelativePosition:
        {
            QPointF posOff = position();
            QPoint curPos = QCursor::pos();
            robot.mouseMove(curPos.x() + posOff.x(), curPos.y() + posOff.y());
            break;
        }
        }

        // Action rule
        switch(mActionMode)
        {
        case ButtonAction:
            robot.mouseClick(static_cast<MouseRobot::Button>(action()));
            break;
        case KeyAction:
            robot.keyType(static_cast<quint32>(action()));
            break;
        case NoAction:
            break;
        }
        mTimer.restart();
    }
    mUi->progressBar->setValue((mTimer.elapsed() * 100) / v);
}


void MouseRule::setButtonState(bool isRemoveEnabled, bool isAddEnabled)
{
    mUi->removeButton->setEnabled(isRemoveEnabled);
    mUi->addButton->setEnabled(isAddEnabled);
}


int MouseRule::number() const
{
    return mPredecessor ? mPredecessor->number() + 1 : 0;
}


void MouseRule::setPredecessor(MouseRule *predRule)
{
    mPredecessor = predRule;
}


MouseRule *MouseRule::predecessor()
{
    return mPredecessor;
}


QPoint MouseRule::absolutePosition() const
{
    switch(mPositionMode)
    {
    case CurrentPosition:
        return mPredecessor ? mPredecessor->absolutePosition() : QApplication::desktop()->screenGeometry().center();
    case AbsolutePosition:
        return position();
    case RelativePosition:
        return mPredecessor ? mPredecessor->absolutePosition() + position() : QApplication::desktop()->screenGeometry().center() + position();
    }
    return QPoint();
}


void MouseRule::setItem(QListWidgetItem *item)
{
    mItem = item;
}


QListWidgetItem *MouseRule::item()
{
    return mItem;
}


void MouseRule::setPosition(QPoint position, EPositionMode positionMode)
{   
    mPositionMode = positionMode;
    mUi->positionWidget->setCurrentIndex(static_cast<int>(positionMode));
    mUi->positionSelect->setCurrentIndex(static_cast<int>(positionMode));
    switch(positionMode)
    {
    case CurrentPosition:
        break;
    case AbsolutePosition:
        mPosition = position;
        pos2ui();
        break;
    case RelativePosition:
        mPositionOffset = position;
        pos2ui();
        break;
    }
}


QPoint MouseRule::position() const
{
    switch(mPositionMode)
    {
    case CurrentPosition:
        break;
    case AbsolutePosition:
        return mPosition;
    case RelativePosition:
        return mPositionOffset;
    }
    return QPoint();
}


EPositionMode MouseRule::positionMode() const
{
    return mPositionMode;
}


void MouseRule::setInterval(quint32 interval, EIntervalMode intervalMode)
{
    mIntervalMode = intervalMode;
    mUi->intervalWidget->setCurrentIndex(static_cast<int>(intervalMode));
    mUi->intervalSelect->setCurrentIndex(static_cast<int>(intervalMode));
    switch(intervalMode)
    {
    case MillisecondsInterval:
        mUi->milliSecondsEdit->setValue(interval);
        break;
    case SecondsInterval:
        mUi->secondsEdit->setValue(interval / 1000.0);
        break;
    case MinutesInterval:
    case HoursInterval:
    {
        QTimeEdit *edit = mUi->intervalWidget->currentWidget()->findChild<QTimeEdit*>();
        edit->setTime(QTime::fromMSecsSinceStartOfDay(interval));
        break;
    }
    }
}


quint32 MouseRule::interval() const
{
    switch(mIntervalMode)
    {
    case MillisecondsInterval:
        return mUi->milliSecondsEdit->value();
    case SecondsInterval:
        return static_cast<quint32>(mUi->secondsEdit->value() * 1000.0 + 0.5);
    case MinutesInterval:
    case HoursInterval:
    {
        const QTimeEdit *edit = mUi->intervalWidget->currentWidget()->findChild<QTimeEdit*>();
        return QTime(0, 0, 0, 0).msecsTo(edit->time());
    }
    }

    return 50;
}


EIntervalMode MouseRule::intervalMode() const
{
    return mIntervalMode;
}


void MouseRule::setAction(quint32 action, EActionMode actionMode)
{
    mActionMode = actionMode;
    mUi->actionWidget->setCurrentIndex(static_cast<int>(actionMode));
    mUi->actionSelect->setCurrentIndex(static_cast<int>(actionMode));
    switch(actionMode)
    {
    case ButtonAction:
        mUi->buttonSelect->setCurrentIndex(static_cast<int>(action) - 1);
        break;
    case KeyAction:
        mUi->keyEdit->setKeySequence(QKeySequence(action));
        mUi->keyEdit->setFocus(Qt::OtherFocusReason);
        break;
    case NoAction:
        break;
    }
}


quint32 MouseRule::action() const
{
    switch(mActionMode)
    {
    case ButtonAction:
        return mUi->buttonSelect->currentIndex() + 1;
    case KeyAction:
        if (mUi->keyEdit->keySequence().count() > 0)
        {
            return mUi->keyEdit->keySequence()[0];
        }
        break;
    case NoAction:
        break;
    }
    return 0;
}


EActionMode MouseRule::actionMode() const
{
    return mActionMode;
}


void MouseRule::grabMouse()
{
    switch (mPositionMode)
    {
    case CurrentPosition:
        break;
    case AbsolutePosition:
    {
        mIsDragging = true;
        mUi->absButton->setIcon(QIcon());
        QPixmap p = mPosIconAbs.pixmap(QSize(24, 24));
        QWidget::grabMouse(QCursor(p.scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        break;
    }
    case RelativePosition:
    {
        mIsDragging = true;
        mBasePosition = mPredecessor ? mPredecessor->absolutePosition() : QApplication::desktop()->screenGeometry().center();
        mUi->relButton->setIcon(QIcon());
        QPixmap p = mPosIconRel.pixmap(QSize(24, 24));
        QWidget::grabMouse(QCursor(p.scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        break;
    }
    }
}


void MouseRule::ungrabMouse()
{
    mIsDragging = false;
    QWidget::releaseMouse();
}


void MouseRule::changePositionMode(int positionIndex)
{
    mUi->positionWidget->setCurrentIndex(positionIndex);
    mPositionMode = static_cast<EPositionMode>(positionIndex);
    requestRepaint();
}


void MouseRule::changeIntervalMode(int intervalIndex)
{
    mUi->intervalWidget->setCurrentIndex(intervalIndex);
    mIntervalMode = static_cast<EIntervalMode>(intervalIndex);
    requestRepaint();
}


void MouseRule::changeActionMode(int actionIndex)
{
    mUi->actionWidget->setCurrentIndex(actionIndex);
    mActionMode = static_cast<EActionMode>(actionIndex);
    if (mActionMode ==  KeyAction)
    {
        mUi->keyEdit->setFocus(Qt::OtherFocusReason);
    }
    requestRepaint();
}


void MouseRule::ui2pos()
{
    switch (mPositionMode)
    {
    case CurrentPosition:
    {
        break;
    }
    case AbsolutePosition:
    {
        QStringList coords = mUi->absEdit->text().split(",");
        if (coords.size() == 2)
        {
            bool okx(true);
            bool oky(true);
            quint32 x = coords.front().toUInt(&okx);
            quint32 y = coords.back().toUInt(&oky);
            if (okx && oky)
            {
                mPosition.setX(x);
                mPosition.setY(y);
            }
        }
        break;
    }
    case RelativePosition:
    {
        QStringList coords = mUi->relEdit->text().split(",");
        if (coords.size() == 2)
        {
            bool okx(true);
            bool oky(true);
            quint32 x = coords.front().toInt(&okx);
            quint32 y = coords.back().toInt(&oky);
            if (okx && oky)
            {
                mPositionOffset.setX(x);
                mPositionOffset.setY(y);
            }
        }
        break;
    }
    }
    requestRepaint();
}


void MouseRule::pos2ui()
{
    switch (mPositionMode)
    {
    case CurrentPosition:
        break;
    case AbsolutePosition:
        mUi->absEdit->setText(QString("%1, %2").arg(mPosition.x(), 4, 10, QChar(' ')).arg(mPosition.y()));
        break;
    case RelativePosition:
        mUi->relEdit->setText(QString().sprintf("%+5d, %+d", (mPositionOffset.x()), (mPositionOffset.y())));
        break;
    }
}


void MouseRule::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton && mIsDragging)
    {
        switch (mPositionMode)
        {
        case CurrentPosition:
            break;
        case AbsolutePosition:
            mPosition = event->globalPos();
            pos2ui();
            break;
        case RelativePosition:
            mPositionOffset = event->globalPos() - mBasePosition;
            pos2ui();
            break;
        }
        requestRepaint();
    }
}


void MouseRule::mouseReleaseEvent(QMouseEvent */*event*/)
{
    switch (mPositionMode)
    {
    case CurrentPosition:
        break;
    case AbsolutePosition:
        mUi->absButton->setIcon(mPosIconAbs);
        mUi->absButton->setDown(false);
        break;
    case RelativePosition:
        mUi->relButton->setIcon(mPosIconRel);
        mUi->relButton->setDown(false);
        break;
    }

    ungrabMouse();
    requestRepaint();
}


void MouseRule::draw(QPainter &painter) const
{
    switch (mPositionMode)
    {
    case CurrentPosition:
        break;
    case AbsolutePosition:
    {
        QPoint pos = position();

        // Draw crosshair
        QPixmap p = mPosIconAbs.pixmap(QSize(24, 24));
        painter.drawImage(pos.x() - 12, pos.y() - 12, p.toImage());

        // Draw number
        QFont font=painter.font() ;
        font.setPointSize (12);
        font.setWeight(QFont::DemiBold);
        painter.setFont(font);
        painter.setPen(Qt::black);  
        int id = number();
        for(int x = -1; x < 2; ++x)
        {
            for(int y = -1; y < 2; ++y)
            {
                painter.drawText(pos.x() + x + 12, pos.y() + y + 12, QString("%1").arg(id));
            }
        }
        painter.setPen(Qt::white);
        painter.drawText(pos.x() + 12, pos.y() + 12, QString("%1").arg(id));
        break;
    }
     case RelativePosition:
        QPoint basePos(mPredecessor ? mPredecessor->absolutePosition() : QApplication::desktop()->screenGeometry().center());
        QPoint pos = basePos + position();

        // Draw line
        QVector<qreal> dashes;
        dashes << 1 << 4;
        QPen linePen(Qt::black, 4.0f, Qt::DashLine);
        linePen.setDashPattern(dashes);
        linePen.setDashOffset(0.0);
        painter.setPen(linePen);

        painter.drawLine(basePos, pos);

        linePen.setColor(Qt::white);
        linePen.setWidthF(2.0f);
        dashes[0] = 2;
        dashes[1] = 8;
        linePen.setDashPattern(dashes);
        linePen.setDashOffset(0.0);
        painter.setPen(linePen);
        painter.drawLine(basePos, pos);

        // Draw crosshair
        QPixmap p = mPosIconRel.pixmap(QSize(24, 24));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawImage(pos.x() - 12, pos.y() - 12, p.toImage());

        // Draw number
        QFont font=painter.font() ;
        font.setPointSize (12);
        font.setWeight(QFont::DemiBold);
        painter.setFont(font);
        painter.setPen(Qt::black);
        int id = number();
        for(int x = -1; x < 2; ++x)
        {
            for(int y = -1; y < 2; ++y)
            {
                painter.drawText(pos.x() + x + 12, pos.y() + y + 12, QString("%1").arg(id));
            }
        }
        painter.setPen(Qt::white);
        painter.drawText(pos.x() + 12, pos.y() + 12, QString("%1").arg(id));
        break;
    }
}

