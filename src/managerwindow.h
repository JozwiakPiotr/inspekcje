#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QMainWindow>
#include "monument.h"
#include "backendclient.h"
#include <QVBoxLayout>
#include <qmessagebox.h>

namespace Ui {
class ManagerWindow;
}

class ManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManagerWindow(QWidget *parent = nullptr);
    ~ManagerWindow();

private slots:
    void on_lineEdit_textEdited(const QString &text);
    void on_btn_index0_clicked();
    void on_btn_index1_clicked();
    void on_stackedWidget_currentChanged(int index);
    void on_dateFrom_userDateChanged(const QDate &date);

    void on_comboRange_currentIndexChanged(int index);

    void on_dateTo_userDateChanged(const QDate &date);

    void on_actionWyjd_triggered();

    void on_actionGeneruj_triggered();

private:
    Ui::ManagerWindow *ui;
    QList<Monument> monuments;
    QList<GetInspectionsResponse> inspections;
    QVBoxLayout* monumentsLayout = nullptr;
    BackendClient* client;

    QDate start;
    QDate end;

    void loadMonuments(const QList<Monument>& monuments, const QString &err);
    void loadInspections(const QList<GetInspectionsResponse>& inspections, const QString &err);
    void setMonumentsLayout(const QList<Monument>& monuments);
    void setInspectionsLayout(const QList<GetInspectionsResponse>& inspections);
    void setFrom(const QDate& date);
    void setTo(const QDate& date);
};

#endif // MANAGERWINDOW_H
