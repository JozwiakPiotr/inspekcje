#include "managerwindow.h"
#include "ui_managerwindow.h"
#include <QDesktopServices>
#include <QLabel>
#include <QPushButton>
#include "inspectionorderdialog.h"
#include "inspectionrow.h"

ManagerWindow::ManagerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ManagerWindow)
    , client(new BackendClient(this))
    , monuments(QList<Monument>())
    , inspections(QList<GetInspectionsResponse>())
    , start(QDate::currentDate().year(), QDate::currentDate().month(), 1)
    , end(start.addMonths(1).addDays(-1))
{
    ui->setupUi(this);
    ui->dateFrom->setDate(start);
    ui->dateTo->setDate(end);
    ui->comboRange->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);

    QWidget* container = new QWidget(this);
    monumentsLayout = new QVBoxLayout(container);
    container->setLayout(monumentsLayout);
    ui->scrollAreaMonuments->setWidget(container);

    connect(client, &BackendClient::getMonumentsResult, this, &ManagerWindow::loadMonuments);
    connect(client, &BackendClient::getInspectionsResult, this, &ManagerWindow::loadInspections);
    connect(client, &BackendClient::getOverallReportResult, this, [this](const QString& filePath, const QString& err){
        if(!err.isEmpty()){
            QMessageBox::warning(this, "Błąd", err);
            return;
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    });

    client->getMonuments();
}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}

void ManagerWindow::loadMonuments(const QList<Monument>& monuments, const QString &err)
{
    if(!err.isEmpty()){
        QMessageBox::warning(this, "Błąd", err);
        return;
    }
    this->monuments = monuments;
    setMonumentsLayout(monuments);
}

void ManagerWindow::loadInspections(const QList<GetInspectionsResponse> &inspections, const QString &err)
{
    if(!err.isEmpty()){
        QMessageBox::warning(this, "Błąd", err);
        return;
    }
    this->inspections=inspections;
    setInspectionsLayout(inspections);
}

void ManagerWindow::setMonumentsLayout(const QList<Monument> &monuments)
{
    QLayoutItem* child;
    while ((child = monumentsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    for (const Monument& monument : monuments) {
        QFrame* frame = new QFrame;
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setMinimumHeight(100);
        frame->setMaximumHeight(1000);
        frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QHBoxLayout* layout = new QHBoxLayout(frame);

        auto infoLayout = new QVBoxLayout;
        auto infoContainer = new QWidget;
        infoContainer->setLayout(infoLayout);
        QLabel* labelName = new QLabel(monument.name);
        QString lastInspectionText = monument.lastInspection.isValid()
                                         ? monument.lastInspection.toString(Qt::ISODate)
                                         : QStringLiteral("brak");

        QString nextInspectionText = monument.nextInspection.isValid()
                                         ? monument.nextInspection.toString(Qt::ISODate)
                                         : QStringLiteral("brak");

        QLabel* labelLastInspection = new QLabel("Ostatnia inspekcja: " + lastInspectionText);
        QLabel* labelNextInspection = new QLabel("Następna inspekcja: " + nextInspectionText);

        infoLayout->addWidget(labelName);
        infoLayout->addWidget(labelLastInspection);
        infoLayout->addWidget(labelNextInspection);

        QPushButton* button = new QPushButton("Zleć inspekcję");

        connect(button, &QPushButton::clicked, this,
                [this, name = monument.name, id = monument.id](){
                    auto dialog = new InspectionOrderDialog(name, id, this);

            connect(dialog, &QDialog::accepted, this, [this]{
                        client->getMonuments();
                    });

                    dialog->setWindowModality(Qt::ApplicationModal);
                    dialog->show();
                });

        layout->addWidget(infoContainer);
        layout->addStretch();
        layout->addWidget(button);

        monumentsLayout->addWidget(frame);
    }
    monumentsLayout->addStretch();
}

void ManagerWindow::setInspectionsLayout(const QList<GetInspectionsResponse> &inspections)
{
    QLayoutItem *item;
    auto layout = ui->layoutInspections->layout();
    while ((item = layout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    for (auto inspection : inspections){
        if(inspection.plannedDate >= start && inspection.plannedDate <= end){
            auto row = new InspectionRow(
                inspection.inspectionId,
                inspection.monumentName,
                inspection.plannedDate,
                inspection.orderedBy,
                inspection.completionDate
                );
            layout->addWidget(row);
        }
    }
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void ManagerWindow::on_lineEdit_textEdited(const QString &text)
{
    QList<Monument> filteredList;
    QString lowerText = text.toLower();

    for (auto &m : monuments) {
        if (m.name.toLower().contains(lowerText)) {
            filteredList.append(m);
        }
    }
    setMonumentsLayout(filteredList);
}


void ManagerWindow::on_btn_index0_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->btn_index0->setEnabled(false);
    ui->btn_index1->setEnabled(true);
}


void ManagerWindow::on_btn_index1_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->btn_index0->setEnabled(true);
    ui->btn_index1->setEnabled(false);
}


void ManagerWindow::on_stackedWidget_currentChanged(int index)
{
    switch(index){
    case 0:
        client->getMonuments();
        break;
    case 1:
        client->getInspections();
        break;
    default:
        qDebug() << "Nieznany indeks: " <<index;
        break;
    }
}

void ManagerWindow::on_dateFrom_userDateChanged(const QDate &date)
{
    ui->comboRange->setCurrentIndex(3);
    start = date;
    setInspectionsLayout(inspections);
}


void ManagerWindow::on_comboRange_currentIndexChanged(int index)
{
    QDate today = QDate::currentDate();
    switch(index){
    case 0:
        start = QDate((today).year(), today.month(), 1);
        end = QDate(start.addMonths(1).addDays(-1));
        break;
    case 1:
        start = today.addDays(-(today.dayOfWeek() - 1));
        end = start.addDays(6);
        break;
    case 2:
        start = QDate(today.year(), 1, 1);
        end = today;
        break;
    default:
        break;
    }
    ui->dateFrom->setDate(start);
    ui->dateTo->setDate(end);
    setInspectionsLayout(inspections);
}


void ManagerWindow::on_dateTo_userDateChanged(const QDate &date)
{
    ui->comboRange->setCurrentIndex(3);
    end = date;
    setInspectionsLayout(inspections);
}


void ManagerWindow::on_actionWyjd_triggered()
{
    QApplication::quit();
}


void ManagerWindow::on_actionGeneruj_triggered()
{
    auto from = QDate(QDate::currentDate().year(), 1,1);
    client->getOverallReport(from, from.addYears(1).addDays(-1));
}

