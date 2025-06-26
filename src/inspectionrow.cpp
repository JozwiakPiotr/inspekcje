#include "dialogeditinspection.h"
#include "dialogshowreport.h"
#include "inspectionrow.h"
#include "ui_inspectionrow.h"

#include <QMessageBox>

InspectionRow::InspectionRow(
    int inspectionId,
    const QString &monumentName,
    const QDate &plannedDate,
    const QString &orderedBy,
    const std::optional<QDate>& dateOfCompletion,
    QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::InspectionRow)
    , inspectionId(inspectionId)
    , plannedDate(plannedDate)
    , orderedBy(orderedBy)
    , monumentName(monumentName)
    , client(BackendClient(this))
{
    ui->setupUi(this);
    ui->labelName->setText(monumentName);
    ui->lableOrderedBy->setText("Zlecił: " + orderedBy);
    this->setFrameShape(QFrame::StyledPanel);
    this->setFrameShadow(QFrame::Raised);

    if(dateOfCompletion.has_value()){
        ui->btnCancel->hide();
        ui->btnEdit->hide();
        ui->labelDate->setText("data wykonania: " + dateOfCompletion.value().toString());
    } else {
        ui->labelDate->setText("planowana data: " + plannedDate.toString());
        ui->btnReport->hide();
    }

    connect(&client, &BackendClient::cancelInspectionResult, this, [this](const QString& err){
        if(!err.isEmpty()){
            QMessageBox::warning(this, "Błąd", err);
            return;
        }
        this->deleteLater();
    });
}

InspectionRow::~InspectionRow()
{
    delete ui;
}


void InspectionRow::on_btnEdit_clicked()
{
    auto dialog = new DialogEditInspection(inspectionId, monumentName, this);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->exec();
}


void InspectionRow::on_btnCancel_clicked()
{
    client.cancelInspection(inspectionId);
}


void InspectionRow::on_btnReport_clicked()
{
    auto dialog = new DialogShowReport(inspectionId, this);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->exec();
}

