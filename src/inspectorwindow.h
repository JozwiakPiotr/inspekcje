#ifndef INSPECTORWINDOW_H
#define INSPECTORWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVBoxLayout>
#include "backendclient.h"

namespace Ui {
class InspectorWindow;
}

class InspectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit InspectorWindow(QWidget *parent = nullptr);
    ~InspectorWindow();

private slots:

    void on_btn_refresh_clicked();

    void on_dateEdit_from_userDateChanged(const QDate &date);

    void on_dateEdit_to_userDateChanged(const QDate &date);

    void on_comboBox_range_currentIndexChanged(int index);

    void on_actionWyjd_triggered();

private:
    Ui::InspectorWindow *ui;
    BackendClient client;

    QList<GetInspectionsResponse> inspections;
    QDate start;
    QDate end;

    void setScrollLayout();
    void loadInspections(const QList<GetInspectionsResponse>& inspections, const QString& err);

};

#endif // INSPECTORWINDOW_H
