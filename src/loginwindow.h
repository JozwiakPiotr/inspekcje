#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "backendclient.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_pushButtonSubmit_clicked();
    void handleLoginResult(bool success, const QString &role, const QString &errorMessage);

    void on_btnSettings_clicked();

private:
    Ui::LoginWindow *ui;
    BackendClient* backend;
};

#endif // LOGINWINDOW_H
