#include "dialogcompleteinspection.h"
#include "inspectionrowforinspector.h"
#include "ui_inspectionrowforinspector.h"

InspectionRowForInspector::InspectionRowForInspector(
    int inspectionId,
    QString monument,
    QDate plannedDate,
    std::optional<QDate> completionDate,
    QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InspectionRowForInspector)
    , inspectionId(inspectionId)
{
    ui->setupUi(this);

    ui->label_monument->setText(monument);

    ui->label_plannedDate->setText("Planowana data"+ plannedDate.toString(Qt::DateFormat::ISODate));

    if(completionDate.has_value()){
        ui->label_completionDate->setText("Data wykonania: "+ completionDate.value().toString(Qt::DateFormat::ISODate));
        ui->label_plannedDate->hide();
        ui->btn_complete->hide();
    } else {
        ui->label_completionDate->hide();
    }
}

InspectionRowForInspector::~InspectionRowForInspector()
{
    delete ui;
}

void InspectionRowForInspector::on_btn_complete_clicked()
{
    auto dialog = new DialogCompleteInspection(inspectionId, this);
    dialog->setWindowModality(Qt::WindowModal);
    dialog->exec();
}

