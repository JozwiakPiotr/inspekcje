#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = nullptr);
    ~DialogSettings();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H
