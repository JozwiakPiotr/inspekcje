#include "inspectionorderdialog.h"
#include "ui_inspectionorderdialog.h"
#include "selectuserdialog.h"
#include <QLabel>
#include <QMessageBox>

InspectionOrderDialog::InspectionOrderDialog(
    QString monumentName,
    int monumentId,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InspectionOrderDialog)
    , client(this)
    , monumentId(monumentId)
{
    ui->setupUi(this);
    this->driver = {-1, ""};
    plannedDate = QDate::currentDate();
    ui->dateEdit_PlannedDate->setDate(QDate::currentDate());

    QWidget* container = new QWidget(this);
    inspectorsLayout = new QVBoxLayout(container);
    container->setLayout(inspectorsLayout);
    ui->scrollArea_inspectors->setWidget(container);


    connect(ui->pushButton_searchDriver, &QPushButton::clicked, this, [this]() {
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
    });

    connect(ui->pushButton_searchInspector, &QPushButton::clicked, this, [this]() {
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
    });
    connect(&client, &BackendClient::createInspectionResult, this, [this](const QString& err){
        if(err.isEmpty()){
            this->accept();
        } else {
            QMessageBox::warning(this,"Błąd",err);
        }
    });
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this](){
        QList<int> inspectorIds;
        for (const Person& person : inspectors) {
            inspectorIds.append(person.id);
        }
        client.createInspection({this->monumentId, this->plannedDate, inspectorIds, this->driver.id});
    });

}

InspectionOrderDialog::~InspectionOrderDialog()
{
    delete ui;
}

void InspectionOrderDialog::setDriver(Person person){
    ui->lineEditDriver->setText(person.name);
    driver = person;
}

void InspectionOrderDialog::addInspector(Person person){
    for (const auto& current : inspectors) {
        if (current.id == person.id) {
            return;
        }
    }
    inspectors.append(person);
    setInspectorsLayout(inspectors);
}

void InspectionOrderDialog::setInspectorsLayout(QList<Person> persons){
    QLayoutItem* item;
    while ((item = inspectorsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    for (const Person& person : persons) {
        QFrame* frame = new QFrame;
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setMaximumHeight(40);
        frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QHBoxLayout* rowLayout = new QHBoxLayout;
        rowLayout->setContentsMargins(10,10,10,10);
        rowLayout->setSpacing(10);

        rowLayout->addWidget(new QLabel(person.name));
        rowLayout->addStretch();
        auto button = new QPushButton("usuń", this);
        connect(button, &QPushButton::clicked, this, [this, id=person.id]{
            removeInspector(id);
        });
        rowLayout->addWidget(button);

        frame->setLayout(rowLayout);
        inspectorsLayout->addWidget(frame);
    }

    // **klucz**: docelowa sprężyna na dole
    inspectorsLayout->addStretch();
}

void InspectionOrderDialog::removeInspector(int id){
    auto it = std::remove_if(inspectors.begin(), inspectors.end(), [id](const Person& person) {
        return person.id == id;
    });
    inspectors.erase(it, inspectors.end());
    setInspectorsLayout(inspectors);
}


void InspectionOrderDialog::on_btn_removeDriver_clicked()
{
    this->driver.id = -1;
    this->driver.name = "";
    ui->lineEditDriver->setText("");
}


void InspectionOrderDialog::on_dateEdit_PlannedDate_userDateChanged(const QDate &date)
{
    plannedDate = date;
}

