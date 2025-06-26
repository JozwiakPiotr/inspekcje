#ifndef BACKENDCLIENT_H
#define BACKENDCLIENT_H

#include <QObject>
#include <QRestAccessManager>
#include <QNetworkRequestFactory>
#include <QNetworkAccessManager>
#include "monument.h"
#include <QJsonArray>
#include <QJsonDocument>

struct User{
    int id;
    QString username;

    static QList<User> fromJson(const QJsonObject& root);
};

struct Inspection{
    int MonumentId;
    QDate PlannedDate;
    QList<int> InspectorsIds;
    int DriverId = -1;

    static QJsonDocument toJsonDocument(const Inspection& inspection) {
        QJsonObject json;
        json["MonumentId"] = inspection.MonumentId;
        json["PlannedDate"] = inspection.PlannedDate.toString(Qt::ISODate);

        QJsonArray inspectorsArray;
        for (int id : inspection.InspectorsIds) {
            inspectorsArray.append(id);
        }
        json["InspectorsIds"] = inspectorsArray;

        if (inspection.DriverId != -1) {
            json["DriverId"] = inspection.DriverId;
        }

        return QJsonDocument(json);
    }
};

struct GetInspectionsResponse{
    int inspectionId;
    QString monumentName;
    QString orderedBy;
    QDate plannedDate;
    std::optional<QDate> completionDate;

    static QList<GetInspectionsResponse> fromJson(const QJsonObject& obj){
        QList<GetInspectionsResponse> inspections;

        auto arr = obj["$values"].toArray();

        for(auto val: arr){
            auto obj = val.toObject();
            auto id = obj["id"].toInt();
                auto name =               obj["monument"].toObject()["name"].toString();
                auto by =   obj["orderedBy"].toObject()["name"].toString();
                auto planned =   QDate::fromString(obj["plannedDate"].toString(), Qt::ISODate);
                std::optional<QDate> comp;
                if (obj.contains("dateOfCompletion") && !obj["dateOfCompletion"].toString().isEmpty()) {
                    QDate parsed = QDate::fromString(obj["dateOfCompletion"].toString(), Qt::ISODate);
                    if (parsed.isValid()) {
                        comp = parsed;
                    }
                }
            GetInspectionsResponse inspection {
                id,name,by,planned,comp
            };
            inspections.append(inspection);
        }
        return inspections;
    }
};

struct GetInspectionByIdResponse{
    int inspectionId;
    QDate PlannedDate;
    QList<User> Inspectors;
    std::optional<User> Driver;

    static GetInspectionByIdResponse fromJson(const QJsonObject& obj){

        User driver = {-1,""};
        if(obj.contains("driver") && obj["driver"].isObject()){
            driver.id = obj["driver"].toObject()["id"].toInt();
            driver.username = obj["driver"].toObject()["login"].toString();
        }

        return {
            obj["id"].toInt(),
            QDate::fromString(obj["plannedDate"].toString(), Qt::ISODate),
            User::fromJson(obj["inspectors"].toObject()),
            driver
        };
    }
};

struct EditInspectionRequest{
    int inspectionId;
    QDate PlannedDate;
    QList<int> InspectorsIds;
    int DriverId = -1;

    static QJsonDocument toJsonDocument(const EditInspectionRequest& inspection) {
        QJsonObject json;
        json["PlannedDate"] = inspection.PlannedDate.toString(Qt::ISODate);

        QJsonArray inspectorsArray;
        for (int id : inspection.InspectorsIds) {
            inspectorsArray.append(id);
        }
        json["InspectorsIds"] = inspectorsArray;

        if (inspection.DriverId != -1) {
            json["DriverId"] = inspection.DriverId;
        }

        return QJsonDocument(json);
    }
};

struct GetReportResponse {
    int Id;
    QDate Date;
    QString PerformedBy;
    bool RecommendationIssued;
    bool OrderIssued;
    std::optional<QDate> OrderDeadline;

    static GetReportResponse fromJson(const QJsonObject& obj) {
        GetReportResponse r;
        r.Id = obj["id"].toInt();
        r.Date = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        r.PerformedBy = obj["performedBy"].toString();
        r.RecommendationIssued = obj["recommendationIssued"].toBool();
        r.OrderIssued = obj["orderIssued"].toBool();

        if (obj.contains("orderDeadline") && !obj["orderDeadline"].isNull()) {
            r.OrderDeadline = QDate::fromString(obj["orderDeadline"].toString(), Qt::ISODate);
        }

        return r;
    }
};

struct InspectionForm
{
    int inspectionId;
    QDate reportDate;
    bool recommendationIssued;
    bool orderIssued;
    std::optional<QDate> orderDeadline;
    QString reportFilePath;
};

class BackendClient : public QObject
{
    Q_OBJECT
public:
    BackendClient(QObject *parrent);
    void login(const QString &login, const QString &password);
    void getMonuments();
    void getUsers(const QString& role);
    void createInspection(Inspection inspection);
    void getInspections();
    void editInspection(const EditInspectionRequest& inspection);
    void cancelInspection(int id);
    void getInspectionById(int id);
    void getReport(int id);
    void getProtocolFile(int id);
    void getOverallReport(const QDate& from, const QDate& to);
    void completeInspection(const InspectionForm& form);

signals:
    void loginResult(bool success, const QString &role, const QString &err);
    void getMonumentsResult(const QList<Monument>& monuments, const QString &err);
    void getUsersResult(const QList<User>& users, const QString &err);
    void createInspectionResult(const QString &err);
    void getInspectionsResult(const QList<GetInspectionsResponse>& inspections, const QString& err);
    void editInspectionResult(const QString& err);
    void cancelInspectionResult(const QString& err);
    void getInspectionByIdResult(const GetInspectionByIdResponse& inspection, const QString& err);
    void getReportResult(const GetReportResponse& report, const QString& err);
    void getProtocolFileResult(const QString& filePath, const QString& err);
    void completeInspectionResult(const QString& err);
    void getOverallReportResult(const QString& filePath, const QString& err);

private:
    QNetworkAccessManager* _networkManager;
    QRestAccessManager* _restManager;
    QNetworkRequestFactory _backendApi;
};

#endif // BACKENDCLIENT_H
