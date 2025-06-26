#include "monument.h"
#include <QJsonArray>

QList<Monument> Monument::fromJson(const QJsonObject& root)
{
    QList<Monument> list;

    if (!root.contains("$values") || !root["$values"].isArray())
        return list;

    QJsonArray monuments = root["$values"].toArray();
    const QDate today = QDate::currentDate();

    for (const QJsonValue& val : monuments) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();

        Monument m;
        m.id = obj["id"].toInt();
        m.name = obj["name"].toString();
        m.lastInspection = QDate();  // null date
        m.nextInspection = QDate();

        if (obj.contains("inspections")) {
            QJsonObject inspectionsObj = obj["inspections"].toObject();
            if (inspectionsObj.contains("$values") && inspectionsObj["$values"].isArray()) {
                QJsonArray inspections = inspectionsObj["$values"].toArray();

                for (const QJsonValue& insVal : inspections) {
                    if (!insVal.isObject()) continue;
                    QJsonObject inspection = insVal.toObject();

                    QString dateStr = inspection["plannedDate"].toString();
                    QDate date = QDate::fromString(dateStr, Qt::ISODate);
                    if (!date.isValid()) continue;

                    if (date <= today) {
                        if (!m.lastInspection.isValid() || date > m.lastInspection)
                            m.lastInspection = date;
                    } else {
                        if (!m.nextInspection.isValid() || date < m.nextInspection)
                            m.nextInspection = date;
                    }
                }
            }
        }

        list.append(m);
    }

    return list;
}
