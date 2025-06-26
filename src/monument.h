#ifndef MONUMENT_H
#define MONUMENT_H
#include <QString>
#include <QJsonObject>

struct Monument {
    int id;
    QString name;
    QDate lastInspection;
    QDate nextInspection;

    static QList<Monument> fromJson(const QJsonObject& json);
};

#endif // MONUMENT_H
