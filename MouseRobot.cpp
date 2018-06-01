#include "MouseRobot.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/XTest.h>
#include <string.h>
#include <ctime>
#include <cmath>
#include <vector>

class MouseRobot::MouseRobotImpl
{
public:
    MouseRobotImpl(WId parentWindow)
        : mParentWindow(parentWindow)
        , mDisplay(XOpenDisplay(NULL))
    {

    }

    ~MouseRobotImpl()
    {
        if(mDisplay != NULL)
        {
            XCloseDisplay(mDisplay);
            mDisplay = NULL;
        }
    }

    XButtonEvent queryCurrentWindow() const
    {
        XButtonEvent event;
        memset(&event, 0x00, sizeof(event));
        event.subwindow = DefaultRootWindow(mDisplay);

        while(event.subwindow)
        {
            event.window = event.subwindow;
            XQueryPointer(
                mDisplay,
                event.window,
                &event.root, &event.subwindow,
                &event.x_root, &event.y_root,
                &event.x, &event.y,
                &event.state);
        }
        return event;
    }

    void setMouseCursor()
    {
        if(mDisplay != NULL)
        {
            XButtonEvent event(queryCurrentWindow());

            // Change cursor
            Cursor cursor(XCreateFontCursor(mDisplay, XC_crosshair));
            XDefineCursor(mDisplay, event.window, cursor);
        }
    }

    void mouseMove(quint32 x, quint32 y)
    {
        if(mDisplay != NULL)
        {
            XButtonEvent event;
            memset(&event, 0x00, sizeof(event));
            event.subwindow = DefaultRootWindow(mDisplay);
            event.window = event.subwindow;
            XQueryPointer(
                mDisplay,
                event.window,
                &event.root, &event.subwindow,
                &event.x_root, &event.y_root,
                &event.x, &event.y,
                &event.state);

            int x1 = static_cast<int>(x);
            int y1 = static_cast<int>(y);
            int x2 = event.x_root;
            int y2 = event.y_root;
            int dx = x2 - x1;
            int dy = y2 - y1;
            int d = std::sqrt(dx * dx + dy * dy);
            int n = std::max(1, std::min(static_cast<int>(std::sqrt(d / 10)), 10));
            quint64 delay = 10000000ull;
            for (int i = 0; i <= n; ++i)
            {
                quint32 curX = x2 + ((x1 - x2) * i) / n;
                quint32 curY = y2 + ((y1 - y2) * i) / n;

                // Send mouse move
                XTestFakeMotionEvent(mDisplay, 0, curX, curY, CurrentTime);
                XFlush(mDisplay);

                // Delay (10ms)
                timespec time;
                time.tv_sec = 0;
                time.tv_nsec = delay;
                nanosleep(&time, 0);
            }
        }
    }

    bool mouseClick(Button button)
    {
        if(mDisplay != NULL)
        {
            XButtonEvent event(queryCurrentWindow());

            // Never sent events to own window
            if (event.window != mParentWindow)
            {
                // Send mouse down
                XTestFakeButtonEvent(mDisplay, button, True, CurrentTime);
                XFlush(mDisplay);

                // Delay (1ms)
                timespec time;
                time.tv_sec = 0;
                time.tv_nsec = 1000000;
                nanosleep(&time, 0);

                // Send mouse up
                XTestFakeButtonEvent(mDisplay, button, False, CurrentTime);
                XFlush(mDisplay);

                return true;
            }
        }
        return false;
    }

    void keyType(quint32 key)
    {
        Window focusWindow;
        int revert;
        XGetInputFocus(mDisplay, &focusWindow, &revert);

        // Never sent events to own window
        if (focusWindow != mParentWindow)
        {
            // Convert modifiers
            quint32 keyMod = key & Qt::KeyboardModifierMask;
            std::vector<KeyCode> modCodes;
            if (keyMod & Qt::ShiftModifier)
            {
                modCodes.push_back(XKeysymToKeycode(mDisplay, XK_Shift_L));
            }
            if (keyMod & Qt::ControlModifier)
            {
                modCodes.push_back(XKeysymToKeycode(mDisplay, XK_Control_L));
            }
            if (keyMod & Qt::MetaModifier)
            {
                modCodes.push_back(XKeysymToKeycode(mDisplay, XK_Meta_L));
            }
            if (keyMod & Qt::AltModifier)
            {
                modCodes.push_back(XKeysymToKeycode(mDisplay, XK_Alt_L));
            }

            // Convert key
            key = key & ~Qt::KeyboardModifierMask;
            if (key >= Qt::Key_F1 && key <= Qt::Key_F35)
            {
                key += XK_F1 - Qt::Key_F1;
            }
            else if (key >= Qt::Key_Left && key <= Qt::Key_Down)
            {
                key -= 0xff00c1;
            }
            else if (key >= Qt::Key_Space && key <= Qt::Key_QuoteLeft)
            {
                //no conversion
            }
            else
            {
                // Ignore
                key = 0;
            }
            KeyCode keyCode = XKeysymToKeycode(mDisplay, key);

            // Send mods down
            for (auto m = modCodes.begin(); m != modCodes.end(); ++m)
            {
                XTestFakeKeyEvent(mDisplay, *m, True, CurrentTime);
            }
            XFlush(mDisplay);

            // Send key down
            XTestFakeKeyEvent(mDisplay, keyCode, True, CurrentTime);
            XFlush(mDisplay);

            // Delay (1ms)
            timespec time;
            time.tv_sec = 0;
            time.tv_nsec = 1000000;
            nanosleep(&time, 0);

            // Send key up
            XTestFakeKeyEvent(mDisplay, keyCode, False, CurrentTime);
            XFlush(mDisplay);

            // Send mods up
            for (auto m = modCodes.rbegin(); m != modCodes.rend(); ++m)
            {
                XTestFakeKeyEvent(mDisplay, *m, False, CurrentTime);
            }
            XFlush(mDisplay);
        }
    }

private:
    WId mParentWindow;
    Display *mDisplay;
};


MouseRobot::MouseRobot(WId parentWindow)
    : mImpl(new MouseRobotImpl(parentWindow))
{

}


MouseRobot::~MouseRobot()
{
    if (mImpl)
    {
        delete mImpl;
        mImpl = 0;
    }
}


void MouseRobot::setMouseCursor()
{
    mImpl->setMouseCursor();
}


void MouseRobot::mouseMove(quint32 x, quint32 y)
{
    return mImpl->mouseMove(x, y);
}


void MouseRobot::mouseClick(Button button)
{
    (void)mImpl->mouseClick(button);
}


void MouseRobot::keyType(quint32 key)
{
    mImpl->keyType(key);
}

