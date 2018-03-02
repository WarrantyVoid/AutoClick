#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QTimer>
#include "MouseRobot.hpp"
#include "MouseRuleConfig.hpp"


namespace Ui
{
class MainWindow;
class MainWidget;
class MainDialog;
}
class UGlobalHotkeys;
class GlassWindow;

class MainWindow : public QDialog, MouseRuleObserver
{
    Q_OBJECT
public:
    enum ShortCuts
    {
        Load = 1,
        Save = 2,
        Exit = 3,
        ToggleClicks = 4
    };

public:
    explicit MainWindow(QWidget *parent = 0, GlassWindow *glass = 0);
    ~MainWindow();

public slots:
    void setAlwaysOnTop(bool isAlwaysOnTop);
    void loadMouseRules();
    void saveMouseRules();

protected slots:
    void addMouseRule();
    void removeMouseRule();
    void triggerMouseRules();
    void triggerHotkey(size_t id);
    void toggleTimer();
    void updateTimer();
    void quit();
    void startPosCapture();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void ruleAdded(MouseRule &rule);
    void ruleRemoved(MouseRule &rule);

private:
    Ui::MainDialog *mUi;
    GlassWindow *mGlassWindow;
    QPoint mDragWinPos;
    QPoint mDragMousePos;
    bool mIsDragging;
    QTimer mTimer;
    UGlobalHotkeys *mHotkeyManager;
    MouseRobot mMouseRobot;
    MouseRuleConfig mMouseRules;
};

#endif // MAINWINDOW_H
