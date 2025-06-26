#include "dialogshowreport.h"
#include "ui_dialogshowreport.h"

#include <QMessageBox>
#include <QDesktopServices>

DialogShowReport::DialogShowReport(
    int inspectionId,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogShowReport)
    , inspectionId(inspectionId)
    , client(BackendClient(this))
{
    ui->setupUi(this);

    connect(&client, &BackendClient::getReportResult, this, &DialogShowReport::loadReport);
    connect(&client, &BackendClient::getProtocolFileResult, this, [this](const QString& filePath, const QString& err){
        if(!err.isEmpty()){
            QMessageBox::warning(this,"Błąd",err);
            return;
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    });
    client.getReport(inspectionId);
}

DialogShowReport::~DialogShowReport()
{
    delete ui;
}

void DialogShowReport::on_btnGetProtocolFile_clicked()
{
    client.getProtocolFile(reportId);
}

void DialogShowReport::loadReport(const GetReportResponse &report, const QString &err)
{
    reportId = report.Id;
    ui->labelReportDate->setText("Data przekazania protokołu kontroli: " + report.Date.toString(Qt::DateFormat::ISODate));
    ui->labelPerformedBy->setText("Wykonał: " + report.PerformedBy);
    auto rec = report.RecommendationIssued ? "Czy wydano zalecenia: TAK" : " Czy wydano zalecenia: NIE";
    ui->labelRecomendations->setText(rec);
    auto order = report.OrderIssued ? "Czy wydano nakaz: TAK" : "Czy wydano nakaz: NIE";
    ui->labelOrder->setText(order);
    if(report.OrderDeadline.has_value()){
        ui->labelOrderDeadline->setText(report.OrderDeadline.value().toString(Qt::DateFormat::ISODate));
    } else{
        ui->labelOrderDeadline->hide();
    }
}


void DialogShowReport::on_btnBack_clicked()
{
    this->reject();
}

