#include "dialogcompleteinspection.h"
#include "ui_dialogcompleteinspection.h"
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

DialogCompleteInspection::DialogCompleteInspection(
    int inspectionId,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogCompleteInspection)
    , client(BackendClient(this))
    , form({inspectionId,
            QDate::currentDate(),
            false,
            false,
            std::nullopt,
            ""})
{
    ui->setupUi(this);
    connect(&client, &BackendClient::completeInspectionResult, this, [this](const QString& err){
        if(!err.isEmpty()){
            QMessageBox::warning(this, "Błąd", err);
            return;
        }
        this->accept();
    });
    ui->dateEdit_date->setDate(QDate::currentDate());
    ui->dateEdit_orderDeadline->setDate(QDate::currentDate());
}

DialogCompleteInspection::~DialogCompleteInspection()
{
    delete ui;
}

void DialogCompleteInspection::on_btnAddProtocol_clicked()
{
    form.reportFilePath = QFileDialog::getOpenFileName(
        this,
        "Wybierz plik PDF",
        QDir::homePath(),
        "PDF Files (*.pdf);;All Files (*)"
        );
    ui->lineEditFilePath->setText(form.reportFilePath);
}

void DialogCompleteInspection::on_btnOk_clicked()
{
    client.completeInspection(form);
}

void DialogCompleteInspection::on_checkBox_orderIssued_checkStateChanged(const Qt::CheckState &state)
{
    form.orderIssued = state == Qt::Checked;
    if(form.orderIssued) {
        ui->dateEdit_orderDeadline->setEnabled(true);
        form.orderDeadline = ui->dateEdit_orderDeadline->date();
    } else {
        ui->dateEdit_orderDeadline->setEnabled(false);
        form.orderDeadline.reset();
    }
}

void DialogCompleteInspection::on_checkBox_recommendationIssued_checkStateChanged(const Qt::CheckState &state)
{
    form.recommendationIssued = state == Qt::Checked;
}

void DialogCompleteInspection::on_dateEdit_date_userDateChanged(const QDate &date)
{
    form.reportDate = date;
}

void DialogCompleteInspection::on_dateEdit_orderDeadline_userDateChanged(const QDate &date)
{
    form.orderDeadline = date;
}

