#include "dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QSettings>

DialogSettings::DialogSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    QSettings settings("inspekcje");
    auto value = settings.value("baseUrl").toString();

    ui->tableWidget->setRowCount(1);
    QTableWidgetItem* keyItem = new QTableWidgetItem("baseUrl");
    keyItem->setFlags(keyItem->flags() & ~Qt::ItemIsEditable);
    QTableWidgetItem* valueItem = new QTableWidgetItem(value);
    ui->tableWidget->setItem(0, 0, keyItem);
    ui->tableWidget->setItem(0, 1, valueItem);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::on_buttonBox_accepted()
{
    QSettings settings("inspekcje");
    auto table = ui->tableWidget;
    for (int row = 0; row < table->rowCount(); ++row) {
        auto keyItem = table->item(row, 0);
        auto valueItem = table->item(row, 1);
        if (keyItem && valueItem) {
            QString key = keyItem->text();
            QString value = valueItem->text();
            settings.setValue(key, value);
        }
    }
}

