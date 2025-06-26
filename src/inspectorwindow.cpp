#include "inspectionrowforinspector.h"
#include "inspectorwindow.h"
#include "ui_inspectorwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

InspectorWindow::InspectorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::InspectorWindow)
    , start(QDate::currentDate().year(), QDate::currentDate().month(), 1)
    , end(start.addMonths(1).addDays(-1))
    , client(BackendClient(this))
{
    ui->setupUi(this);
    this->setWindowTitle("Inspekcje");
    ui->dateEdit_from->setDate(start);
    ui->dateEdit_to->setDate(end);
    ui->comboBox_range->setCurrentIndex(0);

    connect(&client, &BackendClient::getInspectionsResult, this, &InspectorWindow::loadInspections);
    client.getInspections();
}

InspectorWindow::~InspectorWindow()
{
    delete ui;
}

void InspectorWindow::setScrollLayout()
{
    QLayoutItem *item;
    auto layout = ui->scrollContent->layout();
    while ((item = layout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    for (auto inspection : inspections){
        if(inspection.plannedDate >= start && inspection.plannedDate <= end){
            auto row = new InspectionRowForInspector(
                inspection.inspectionId,
                inspection.monumentName,
                inspection.plannedDate,
                inspection.completionDate,
                this);
            layout->addWidget(row);
        }
    }
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void InspectorWindow::loadInspections(const QList<GetInspectionsResponse> &inspections, const QString &err)
{
    if(!err.isEmpty()){
        QMessageBox::warning(this, "Błąd", err);
        return;
    }
    this->inspections = inspections;
    setScrollLayout();
}


void InspectorWindow::on_btn_refresh_clicked()
{
    client.getInspections();
}


void InspectorWindow::on_dateEdit_from_userDateChanged(const QDate &date)
{
    ui->comboBox_range->setCurrentIndex(1);
    start = date;
    setScrollLayout();
}


void InspectorWindow::on_dateEdit_to_userDateChanged(const QDate &date)
{
    ui->comboBox_range->setCurrentIndex(1);
    end = date;
    setScrollLayout();
}


void InspectorWindow::on_comboBox_range_currentIndexChanged(int index)
{
    switch(index){
    case 0:
        start = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
        end = start.addMonths(1).addDays(-1);
        ui->dateEdit_from->blockSignals(true);
        ui->dateEdit_to  ->blockSignals(true);
        ui->dateEdit_from->setDate(start);
        ui->dateEdit_to->setDate(end);
        ui->dateEdit_from->blockSignals(false);
        ui->dateEdit_to  ->blockSignals(false);
        setScrollLayout();
        break;
    case 1:
        break;
    }
}


void InspectorWindow::on_actionWyjd_triggered()
{
    QApplication::quit();
}

