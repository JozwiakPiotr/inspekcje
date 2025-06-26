#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include "dialogsettings.h"
#include "inspectorwindow.h"
#include "managerwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->setFixedSize(240, 320);
    ui->label->pixmap().scaledToWidth(ui->label->height(), Qt::SmoothTransformation);
    QIcon icon(":res/settings.png");
    ui->btnSettings->setIcon(icon);
    backend = new BackendClient(this);
    connect(backend, &BackendClient::loginResult, this, &LoginWindow::handleLoginResult);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_pushButtonSubmit_clicked()
{
    auto login = ui->lineEditLogin->text();
    auto password = ui->lineEditPassword->text();

    if(login.isEmpty() || password.isEmpty())
        return;

    backend->login(login, password);
}

void LoginWindow::handleLoginResult(bool success, const QString &role, const QString &errorMessage)
{
    if (!success) {
        QMessageBox::critical(this, "Błąd logowania", errorMessage);
        return;
    }

    if (role == "inspector") {
        auto *win = new InspectorWindow();
        win->show();
    } else if (role == "manager") {
        auto *win = new ManagerWindow();
        win->show();
    } else {
        QMessageBox::warning(this, "Nieznana rola", "Rola użytkownika nie jest obsługiwana: " + role);
        return;
    }

    this->close();
}

void LoginWindow::on_btnSettings_clicked()
{
    auto dialog = new DialogSettings(this);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->exec();
}

