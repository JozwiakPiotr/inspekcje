#ifndef INSPECTIONROW_H
#define INSPECTIONROW_H

#include "backendclient.h"

#include <QDate>
#include <QFrame>
#include <QWidget>

namespace Ui {
class InspectionRow;
}

class InspectionRow : public QFrame
{
    Q_OBJECT

public:
    explicit InspectionRow(
        int inspectionId,
        const QString& monumentName,
        const QDate& plannedDate,
        const QString& orderedBy,
        const std::optional<QDate>& dateOfCompletion,
        QWidget *parent = nullptr);
    ~InspectionRow();

private slots:
    void on_btnEdit_clicked();

    void on_btnCancel_clicked();

    void on_btnReport_clicked();

private:
    Ui::InspectionRow *ui;
    BackendClient client;

    int inspectionId;
    QString monumentName;
    QDate plannedDate;
    QString orderedBy;
};

#endif // INSPECTIONROW_H
