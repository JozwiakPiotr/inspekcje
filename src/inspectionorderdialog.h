#ifndef INSPECTIONORDERDIALOG_H
#define INSPECTIONORDERDIALOG_H

#include "backendclient.h"

#include <QDialog>
#include <QDate>
#include <QVBoxLayout>

struct Person {
    int id;
    QString name;
};

namespace Ui {
class InspectionOrderDialog;
}

class InspectionOrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InspectionOrderDialog(
        QString monumentName,
        int monumentId,
        QWidget *parent = nullptr);
    ~InspectionOrderDialog();

private slots:
    void on_btn_removeDriver_clicked();

    void on_dateEdit_PlannedDate_userDateChanged(const QDate &date);

private:
    Ui::InspectionOrderDialog *ui;
    QVBoxLayout* inspectorsLayout;
    BackendClient client;

    QString monumentName;
    int monumentId;
    QDate plannedDate;
    Person driver;
    QList<Person> inspectors;

    void setDriver(Person person);
    void addInspector(Person person);
    void setInspectorsLayout(QList<Person> person);
    void removeInspector(int id);

};

#endif // INSPECTIONORDERDIALOG_H
