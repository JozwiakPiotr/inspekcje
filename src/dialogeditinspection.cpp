#include "dialogeditinspection.h"
#include "selectuserdialog.h"
#include "ui_dialogeditinspection.h"

#include <QLabel>
#include <QMessageBox>

DialogEditInspection::DialogEditInspection(
    int inspectionId,
    const QString& monumentName,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogEditInspection)
    , inspectionId(inspectionId)
    , plannedDate(QDate::currentDate())
    , driver{-1,""}
    , inspectors(QList<User>())
    , client(this)
{
    ui->setupUi(this);
    this->setWindowTitle("Edycja - " + monumentName);
    connect(&client, &BackendClient::getInspectionByIdResult, this, &DialogEditInspection::loadInspection);
    connect(&client, &BackendClient::editInspectionResult, this, [this](const QString& err){
        if(!err.isEmpty()){
            QMessageBox::warning(this,"Błąd", err);
            return;
        }
        this->accept();
    });
    client.getInspectionById(inspectionId);
}

DialogEditInspection::~DialogEditInspection()
{
    delete ui;
}

void DialogEditInspection::on_plannedDateCalendar_clicked(const QDate &date)
{
    plannedDate = date;
}

void DialogEditInspection::setInspectorsLayout(QList<User> users){
    QLayoutItem* item;
    auto layout = ui->inspectorsContent->layout();

    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    for (const User& user : users) {
        QFrame* frame = new QFrame;
        frame->setMaximumHeight(40);

        QHBoxLayout* rowLayout = new QHBoxLayout;
        rowLayout->addWidget(new QLabel(user.username,this));
        rowLayout->addStretch();

        auto button = new QPushButton("usuń", this);
        connect(button, &QPushButton::clicked, this, [this, id=user.id]{
            removeInspector(id);
        });
        rowLayout->addWidget(button);

        frame->setLayout(rowLayout);
        layout->addWidget(frame);
    }
}

void DialogEditInspection::removeInspector(int id)
{
    auto it = std::remove_if(inspectors.begin(), inspectors.end(), [id](const User& user) {
        return user.id == id;
    });
    inspectors.erase(it, inspectors.end());
    setInspectorsLayout(inspectors);
}

void DialogEditInspection::setDriver(User user)
{
    ui->driverLineEdit->setText(user.username);
    driver = user;
}

void DialogEditInspection::addInspector(User user)
{
    for (const auto& current : inspectors) {
        if (current.id == user.id) {
            return;
        }
    }
    inspectors.append(user);
    setInspectorsLayout(inspectors);
}

void DialogEditInspection::removeDriver(int id)
{
    ui->driverLineEdit->setText("");
    driver = {-1,""};
}

void DialogEditInspection::on_btnCancel_clicked()
{
    this->reject();
}


void DialogEditInspection::on_btnGetDriver_clicked()
{
    auto dialog = new SelectUserDialog("driver", this);

    connect (dialog, &SelectUserDialog::UserSelected, this,
            [this](int userId, const QString &username, const QString &role){
                if(role == "inspector"){
                    addInspector({userId, username});
                } else {
                    setDriver({userId, username});
                }
            });

    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
}


void DialogEditInspection::on_btnOk_clicked()
{
    QList<int> inspectorsIds;
    for(auto ins: inspectors){
        inspectorsIds.append(ins.id);
    }
    client.editInspection(
        {
            inspectionId,
            plannedDate,
            inspectorsIds,
            driver.id
        });
}

void DialogEditInspection::loadInspection(const GetInspectionByIdResponse &inspection, const QString &err)
{
    if(!err.isEmpty()){
        QMessageBox::warning(this, "Błąd", err);
        return;
    }
    plannedDate = inspection.PlannedDate;
    ui->plannedDateCalendar->setSelectedDate(plannedDate);
    if(inspection.Driver.has_value()){
        driver = inspection.Driver.value();
        ui->driverLineEdit->setText(driver.username);
    }
    inspectors = inspection.Inspectors;
    setInspectorsLayout(inspectors);
}


void DialogEditInspection::on_btnGetInspector_clicked()
{
    auto dialog = new SelectUserDialog("inspector", this);

    connect (dialog, &SelectUserDialog::UserSelected, this,
            [this](int userId, const QString &username, const QString &role){
                if(role == "inspector"){
                    addInspector({userId, username});
                } else {
                    setDriver({userId, username});
                }
            });

    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
}

