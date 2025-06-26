#ifndef DIALOGSHOWREPORT_H
#define DIALOGSHOWREPORT_H

#include "backendclient.h"

#include <QDialog>

namespace Ui {
class DialogShowReport;
}

class DialogShowReport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogShowReport(
        int inspectionId,
        QWidget *parent = nullptr);
    ~DialogShowReport();

private slots:
    void on_btnGetProtocolFile_clicked();
    void on_btnBack_clicked();

private:
    Ui::DialogShowReport *ui;
    BackendClient client;

    int inspectionId;
    int reportId;

    void loadReport(const GetReportResponse& report, const QString& err);
};

#endif // DIALOGSHOWREPORT_H
