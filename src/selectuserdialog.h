#ifndef SELECTUSERDIALOG_H
#define SELECTUSERDIALOG_H

#include <QDialog>
#include "inspectionorderdialog.h"
#include "backendclient.h"
#include <QVBoxLayout>

namespace Ui {
class SelectUserDialog;
}

class SelectUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectUserDialog(const QString &role = "", QWidget *parent = nullptr);
    ~SelectUserDialog();

signals:
    void UserSelected(int userId, const QString &username, const QString &role);

private slots:
    void on_lineEdit_textEdited(const QString &text);

private:
    QString _role;
    Ui::SelectUserDialog *ui;
    QVBoxLayout* usersLayout;
    BackendClient* client;
    void LoadUsers(const QList<User>& users, const QString &err);
    QList<User> Users;
    void SetUsers(const QList<User>& users);
};

#endif // SELECTUSERDIALOG_H
