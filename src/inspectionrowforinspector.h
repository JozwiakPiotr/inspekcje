#ifndef INSPECTIONROWFORINSPECTOR_H
#define INSPECTIONROWFORINSPECTOR_H

#include <QDate>
#include <QWidget>

namespace Ui {
class InspectionRowForInspector;
}

class InspectionRowForInspector : public QWidget
{
    Q_OBJECT

public:
    explicit InspectionRowForInspector(
        int inspectionId,
        QString monument,
        QDate plannedDate,
        std::optional<QDate> completionDate = std::nullopt,
        QWidget *parent = nullptr);
    ~InspectionRowForInspector();

private slots:
    void on_btn_complete_clicked();

private:
    Ui::InspectionRowForInspector *ui;

    int inspectionId;
};

#endif // INSPECTIONROWFORINSPECTOR_H
