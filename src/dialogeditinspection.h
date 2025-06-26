#ifndef DIALOGEDITINSPECTION_H
#define DIALOGEDITINSPECTION_H

#include "backendclient.h"
#include <QDate>
#include <QDialog>

namespace Ui {
class DialogEditInspection;
}

class DialogEditInspection : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditInspection(
        int inspectionId,
        const QString& monumentName,
        QWidget *parent = nullptr);
    ~DialogEditInspection();

private slots:
    void on_plannedDateCalendar_clicked(const QDate &date);

    void on_btnCancel_clicked();

    void on_btnGetDriver_clicked();

    void on_btnOk_clicked();

    void on_btnGetInspector_clicked();

private:
    Ui::DialogEditInspection *ui;

    int inspectionId;
    QDate plannedDate;
    User driver;
    QList<User> inspectors;

    BackendClient client;

    void loadInspection(const GetInspectionByIdResponse& inspection, const QString& err);

    void setInspectorsLayout(QList<User> users);
    void removeInspector(int id);
    void setDriver(User user);
    void addInspector(User user);
    void removeDriver(int id);
};

#endif // DIALOGEDITINSPECTION_H
