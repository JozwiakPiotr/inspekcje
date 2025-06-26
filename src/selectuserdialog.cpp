#include "selectuserdialog.h"
#include "ui_selectuserdialog.h"
#include <QLabel>
#include <QPushButton>

SelectUserDialog::SelectUserDialog(const QString &role, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SelectUserDialog)
{
    ui->setupUi(this);

    client = new BackendClient(this);
    _role = role;

    QWidget* container = new QWidget(this);
    usersLayout = new QVBoxLayout(container);
    container->setLayout(usersLayout);
    ui->scrollArea->setWidget(container);

    connect(client, &BackendClient::getUsersResult, this, [this](const QList<User>& list, const QString &err){
        LoadUsers(list, err);
    });
    client->getUsers(role);
}

SelectUserDialog::~SelectUserDialog()
{
    delete ui;
}

void SelectUserDialog::LoadUsers(const QList<User>& users, const QString &err){
    if(!err.isEmpty()){
        qDebug() << err;
        return;
    }
    Users = users;
    SetUsers(users);
}

void SelectUserDialog::SetUsers(const QList<User>& users){
    QLayoutItem* child;
    while ((child = usersLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    for (const User& user : users) {
        QFrame* frame = new QFrame;
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setMaximumHeight(40);
        frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QHBoxLayout* rowLayout = new QHBoxLayout;
        rowLayout->setContentsMargins(10,10,10,10);
        rowLayout->setSpacing(10);

        rowLayout->addWidget(new QLabel(user.username));
        rowLayout->addStretch();
        auto button = new QPushButton("wybierz", this);
        connect(button, &QPushButton::clicked, this, [this, user = user, role=_role]{
            emit UserSelected(user.id, user.username, _role);
            this->accept();
        });
        rowLayout->addWidget(button);

        frame->setLayout(rowLayout);
        usersLayout->addWidget(frame);
    }

    usersLayout->addStretch();
}

void SelectUserDialog::on_lineEdit_textEdited(const QString &text)
{
    QList<User> filteredList;
    QString lowerText = text.toLower();

    for (const User &user : Users) {
        if (user.username.toLower().contains(lowerText)) {
            filteredList.append(user);
        }
    }
    SetUsers(filteredList);
}

