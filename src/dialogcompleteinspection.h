#ifndef DIALOGCOMPLETEINSPECTION_H
#define DIALOGCOMPLETEINSPECTION_H

#include "backendclient.h"
#include <QDialog>

namespace Ui {
class DialogCompleteInspection;
}

class DialogCompleteInspection : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCompleteInspection(
        int inspectionId,
        QWidget *parent = nullptr);
    ~DialogCompleteInspection();

private slots:
    void on_btnAddProtocol_clicked();

    void on_btnOk_clicked();
    void on_checkBox_orderIssued_checkStateChanged(const Qt::CheckState &state);

    void on_checkBox_recommendationIssued_checkStateChanged(const Qt::CheckState &arg1);

    void on_dateEdit_date_userDateChanged(const QDate &date);

    void on_dateEdit_orderDeadline_userDateChanged(const QDate &date);

private:
    Ui::DialogCompleteInspection *ui;
    BackendClient client;

    InspectionForm form;
};

#endif // DIALOGCOMPLETEINSPECTION_H
