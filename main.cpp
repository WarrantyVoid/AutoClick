#include "MainWindow.hpp"
#include "GlassWindow.hpp"
#include <QApplication>
#include <QDialog>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlassWindow glass;
    glass.show();

    MainWindow window(&glass, &glass);
    window.show();

    return a.exec();
}
