#include "GlassWindow.hpp"
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>


GlassWindow::GlassWindow()
    : QMainWindow()
    , mDrawables()
{
    QRect screenSize = QApplication::desktop()->screenGeometry();
    setGeometry(screenSize);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWindowFlags( Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_NoSystemBackground );
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowState(Qt::WindowFullScreen);
}


void GlassWindow::addDrawable(const Drawable *drawable)
{
    if (drawable)
    {
        mDrawables.insert(drawable);
        drawable->setParent(this);
        update();
    }
}


void GlassWindow::removeDrawable(const Drawable *drawable)
{
    auto drawIt = mDrawables.find(drawable);
    if (drawIt != mDrawables.end())
    {
        (*drawIt)->setParent(0);
        mDrawables.erase(drawIt);
        update();
    }
}


void GlassWindow::removeAllDrawables()
{
    for (auto drawIt = mDrawables.begin(); drawIt != mDrawables.end(); ++drawIt)
    {
        (*drawIt)->setParent(0);
        drawIt = mDrawables.erase(drawIt);

    }
    update();
}


void GlassWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for(const auto e : mDrawables)
    {
        e->draw(painter);
    }

    QMainWindow::paintEvent(event);
}

