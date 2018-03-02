#ifndef GLASSWINDOW_HPP
#define GLASSWINDOW_HPP

#include <QMainWindow>
#include <set>
class Drawable;


class GlassWindow : public QMainWindow
{
public:
    GlassWindow();

    void addDrawable(const Drawable *drawable);
    void removeDrawable(const Drawable *drawable);
    void removeAllDrawables();

protected:
    void paintEvent(QPaintEvent *event);

private:
    std::set<const Drawable*> mDrawables;
};


class Drawable
{
    friend class GlassWindow;

public:
    Drawable() : mParent(0) { }
    virtual ~Drawable() { }
    virtual void draw(QPainter &painter) const = 0;

protected:
    void requestRepaint() { if (mParent) { mParent->update(); } }
    void setParent(GlassWindow *parent) const { mParent = parent; }

private:
    mutable GlassWindow *mParent;
};


#endif // GLASSWINDOW_HPP
