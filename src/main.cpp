#include "loginwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings("inspekcje");
    if (!settings.contains("baseUrl")) {
        settings.setValue("baseUrl", "http://localhost:5110/");
    }

    LoginWindow w;
    w.show();
    return a.exec();
}
