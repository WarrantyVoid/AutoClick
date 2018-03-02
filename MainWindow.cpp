#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "ui_MainWidget.h"
#include "ui_MainDialog.h"
#include "MouseRobot.hpp"
#include "MouseRule.hpp"
#include "GlassWindow.hpp"
#include "uglobalhotkeys.h"
#include <QDebug>
#include <QFileDialog>
#include <QListWidgetItem>
#include <cmath>

MainWindow::MainWindow(QWidget *parent, GlassWindow *glass)
    : QDialog(parent)
    , mUi(new Ui::MainDialog)
    , mGlassWindow(glass)
    , mDragWinPos()
    , mDragMousePos()
    , mIsDragging(false)
    , mTimer()
    , mHotkeyManager(new UGlobalHotkeys())
    , mMouseRobot(winId())
    , mMouseRules(this)
{
    mUi->setupUi(this);
    setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
    setFixedSize(size());
    setAlwaysOnTop(true);

    mHotkeyManager->registerHotkey("ctrl+shift+l", MainWindow::Load);
    mHotkeyManager->registerHotkey("ctrl+shift+s", MainWindow::Save);
    mHotkeyManager->registerHotkey("ctrl+shift+x", MainWindow::Exit);
    mHotkeyManager->registerHotkey("ctrl+shift+c", MainWindow::ToggleClicks);
    mMouseRules.load("/home/thomas/mouse_rules.ini");

    connect(mHotkeyManager, &UGlobalHotkeys::activated, this, &MainWindow::triggerHotkey);
    connect(mUi->loadButton, SIGNAL(clicked(bool)), this, SLOT(loadMouseRules()));
    connect(mUi->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveMouseRules()));
    connect(mUi->timerButton, SIGNAL(toggled(bool)), this, SLOT(updateTimer()));
    connect(mUi->quitButton, SIGNAL(clicked()), this, SLOT(quit()));
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(triggerMouseRules()));
}


MainWindow::~MainWindow()
{
    if (mHotkeyManager)
    {
        delete mHotkeyManager;
        mHotkeyManager = 0;
    }
    if (mUi)
    {
        delete mUi;
        mUi = 0;
    }
}


void MainWindow::setAlwaysOnTop(bool isAlwaysOnTop)
{
    Qt::WindowFlags flags = this->windowFlags();
    if (isAlwaysOnTop)
    {
        this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        this->show();
    }
}


void MainWindow::loadMouseRules()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Mouse Rules"), ".", tr("Mouse Rule Ini (*.ini)"));
    if (!fileName.isEmpty())
    {
        mMouseRules.load(fileName);
    }
}


void MainWindow::saveMouseRules()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Mouse Rules"), ".", tr("Mouse Rule Ini (*.ini)"));
    if (!fileName.isEmpty())
    {
        mMouseRules.save(fileName);
    }
}


void MainWindow::addMouseRule()
{
    mMouseRules.addRule(dynamic_cast<MouseRule*>(QObject::sender()));
}


void MainWindow::removeMouseRule()
{
    mMouseRules.removeRule(dynamic_cast<MouseRule*>(QObject::sender()));
}


void MainWindow::triggerMouseRules()
{
    // Move window out of way
    Qt::KeyboardModifiers mods = QApplication::queryKeyboardModifiers();
    if ((mods & Qt::ShiftModifier) == 0 && (mods & Qt::ControlModifier) == 0 && !mIsDragging)
    {
        //move(std::max(QCursor::pos().x() - width() -10, 0), std::max(QCursor::pos().y() - height() - 10, 0));
        mMouseRules.invoke(mMouseRobot);
    }
}


void MainWindow::triggerHotkey(size_t id)
{
    qDebug() << "Activated: " << QString::number(id);
    switch(id)
    {
    case MainWindow::Load:
        loadMouseRules();
        break;
    case MainWindow::Save:
        saveMouseRules();
        break;
    case MainWindow::Exit:
        quit();
        break;
    case MainWindow::ToggleClicks:
        toggleTimer();
        break;
    default:
        break;
    }
}


void MainWindow::toggleTimer()
{
    mUi->timerButton->setChecked(!mUi->timerButton->isChecked());
}


void MainWindow::updateTimer()
{
    if (mUi->timerButton->isChecked())
    {
        mTimer.start(50);
    }
    else
    {
        mTimer.stop();
    }
}


void MainWindow::quit()
{
  QApplication::quit();
}


void MainWindow::startPosCapture()
{
    //mMouseRobot.startMouseCapture();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mDragWinPos = pos();
    mDragMousePos = event->globalPos();
    mIsDragging = true;
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton && mIsDragging)
    {
        QPoint dragged = event->globalPos() - mDragMousePos;
        QPoint newpos = dragged + mDragWinPos;
        move(newpos);
    }
}


void MainWindow::mouseReleaseEvent(QMouseEvent */*event*/)
{
    mIsDragging = false;
    mDragWinPos = QPoint();
    mDragMousePos = QPoint();
}

void MainWindow::ruleAdded(MouseRule &rule)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(rule.sizeHint());
    rule.setItem(item);
    mUi->ruleList->addItem(item);
    mUi->ruleList->setItemWidget(item, &rule);
    mUi->ruleList->setCurrentItem(item);
    mGlassWindow->addDrawable(&rule);
    connect(&rule, SIGNAL(removeClicked()), this, SLOT(removeMouseRule()));
    connect(&rule, SIGNAL(addClicked()), this, SLOT(addMouseRule()));
    connect(&rule, SIGNAL(posPressed()), this, SLOT(startPosCapture()));
}


void MainWindow::ruleRemoved(MouseRule &rule)
{
    Q_ASSERT(rule.item());
    if (rule.predecessor())
    {
         mUi->ruleList->setCurrentItem(rule.predecessor()->item());
    }
    mGlassWindow->removeDrawable(&rule);
    delete rule.item();
    rule.setItem(0);
}

