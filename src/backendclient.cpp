#include "backendclient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QJsonArray>
#include <QAnyStringView>
#include <QRestReply>
#include <QFile>
#include <QFileInfo>
#include "jwtmanager.h"
#include <QHttpMultiPart>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

BackendClient::BackendClient(QObject *parrent)
    : QObject{parrent}
{
    _networkManager = new QNetworkAccessManager(this);
    _restManager = new QRestAccessManager(_networkManager);
    QSettings settings("inspekcje");
    QString baseUrl = settings.value("baseUrl").toString();
    _backendApi = QNetworkRequestFactory {{ baseUrl }};
    QHttpHeaders headers;
    headers.append("Content-Type", "application/json");
    headers.append("Content-Type", "application/json");
    _backendApi.setCommonHeaders(headers);
    if(!JwtManager::instance().token().isEmpty()){
        _backendApi.setBearerToken(JwtManager::instance().token().toUtf8());
    }
}

void BackendClient::login(const QString &username, const QString &password)
{
    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    _restManager->post(
        _backendApi.createRequest("login"),
        QJsonDocument(json),
        this,
        [this](QRestReply &reply){
            if(reply.httpStatus() == 401){
                emit loginResult(false, "", "Niepoprawny login lub hasło");
                return;
            }
            if (!reply.isSuccess()) {
                emit loginResult(false, "", reply.readText());
                return;
            }
            auto obj = reply.readJson()->object();
            if(!obj.contains("token")){
                emit loginResult(false, "", "Brak tokena w odpowiedzi serwera.");
                return;
            }
            JwtManager::instance().setToken(obj["token"].toString());
            _backendApi.setBearerToken(obj["token"].toString().toUtf8());
            _restManager->get(
                _backendApi.createRequest("me"),
                this,
                [this](QRestReply &meReply){
                    if(!meReply.isSuccess()){
                        emit loginResult(false, "", meReply.readText());
                        return;
                    }
                    QString role;
                    auto arr = meReply.readJson()->object()["$values"].toArray();
                    for (const auto &val : arr) {
                        auto claim = val.toObject();
                        if (claim["type"].toString().endsWith("/claims/role")) {
                            role = claim["value"].toString();
                            break;
                        }
                    }

                    if (role.isEmpty()) {
                        emit loginResult(false, "", "Nie znaleziono roli w claimach.");
                    } else {
                        emit loginResult(true, role, "");
                    }
                });
        });
}

void BackendClient::getMonuments(){
    _restManager->get(
        _backendApi.createRequest("monuments"),
        this,
        [this](QRestReply &reply){
            if(!reply.isSuccess()){
                emit getMonumentsResult(
                    {},
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                auto monuments = Monument::fromJson(reply.readJson()->object());
                emit getMonumentsResult(monuments,"");
            }
        });
}

void BackendClient::getUsers(const QString& role){
    auto path = QString("users");
    if(!role.isEmpty()){
        QUrlQuery query;
        query.addQueryItem("group", role);
        path += "?" + query.toString(QUrl::FullyEncoded);
    }
    _restManager->get(
        _backendApi.createRequest(path),
        this,
        [this](QRestReply &reply){
            if(!reply.isSuccess()){
                emit getUsersResult(
                    {},
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                auto users = User::fromJson(reply.readJson()->object());
                emit getUsersResult(users,"");
            }
        });
}

QList<User> User::fromJson(const QJsonObject& root)
{
    QList<User> list;
    if (!root.contains("$values") || !root["$values"].isArray())
        return list;

    QJsonArray values = root["$values"].toArray();
    for (const QJsonValue& val : values) {
        if (!val.isObject()) continue;

        QJsonObject obj = val.toObject();
        User m;
        m.id = obj["id"].toInt();
        m.username = obj["login"].toString();
        list.append(m);
    }

    return list;
}

void BackendClient::createInspection(Inspection inspection){
    auto doc =Inspection::toJsonDocument(inspection);
    qDebug().noquote() << doc.toJson(QJsonDocument::Indented);
    _restManager->post(
        _backendApi.createRequest("inspections"),
        doc,
        this,
        [this](QRestReply& reply){
            if(!reply.isSuccess()){
                emit createInspectionResult(
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                emit createInspectionResult("");
            }
        });
}

void BackendClient::getInspections()
{
    _restManager->get(
        _backendApi.createRequest("inspections"),
        this,
        [this](QRestReply& reply){
            if(!reply.isSuccess()){
                emit getInspectionsResult(
                    {},
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                emit getInspectionsResult(GetInspectionsResponse::fromJson(reply.readJson()->object()), "");
            }
        });
}

void BackendClient::editInspection(const EditInspectionRequest &inspection)
{
    _restManager->put(
        _backendApi.createRequest("inspections/"+QString::number(inspection.inspectionId)),
        EditInspectionRequest::toJsonDocument(inspection),
        this,
        [this](QRestReply& reply){
            if(!reply.isSuccess()){
                emit editInspectionResult(
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                emit editInspectionResult("");
            }
        });
}

void BackendClient::cancelInspection(int id)
{
    _restManager->deleteResource(
        _backendApi.createRequest("inspections/"+QString::number(id)),
        this,
        [this](QRestReply& reply){
            if(!reply.isSuccess()){
                emit cancelInspectionResult(
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                emit cancelInspectionResult("");
            }
        });
}

void BackendClient::getInspectionById(int id)
{
    _restManager->get(
        _backendApi.createRequest("inspections/"+QString::number(id)),
        this,
        [this](QRestReply& reply){
            if(!reply.isSuccess()){
                emit getInspectionByIdResult(
                    {},
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                emit getInspectionByIdResult(GetInspectionByIdResponse::fromJson(reply.readJson()->object()), "");
            }
        });
}

void BackendClient::getReport(int id)
{
    _restManager->get(
        _backendApi.createRequest(QString("inspections/%1/report").arg(id)),
        this,
        [this](QRestReply& reply){
            if(!reply.isSuccess()){
                emit getReportResult(
                    {},
                    QString("HTTP status: %1, error message: %2")
                        .arg(reply.httpStatus())
                        .arg(reply.readText()));
            } else {
                emit getReportResult(GetReportResponse::fromJson(reply.readJson()->object()), "");
            }
        });
}

void BackendClient::getProtocolFile(int id)
{
    _restManager->get(
        _backendApi.createRequest(QString("reports/%1/protocol").arg(id)),
        this,
        [this, id](QRestReply &reply) {
            if (!reply.isSuccess()) {
                QString err = QString("Błąd pobierania: HTTP %1, %2")
                                  .arg(reply.httpStatus())
                                  .arg(reply.readText());
                emit getProtocolFileResult(QString(), err);
                return;
            }

            QByteArray pdfData = reply.readBody();

            QString downloadsPath = QStandardPaths::writableLocation(
                QStandardPaths::DownloadLocation);
            QString fileName = QString("report_%1_protocol.pdf").arg(id);
            QString fullPath = downloadsPath + QDir::separator() + fileName;

            QFile file(fullPath);
            if (!file.open(QIODevice::WriteOnly)) {
                QString err = QString("Nie można zapisać pliku: %1").arg(fullPath);
                emit getProtocolFileResult(QString(), err);
            } else {
                file.write(pdfData);
                file.close();
                emit getProtocolFileResult(fullPath, QString());
            }
        });
}

void BackendClient::getOverallReport(const QDate &from, const QDate &to)
{
    _restManager->get(
        _backendApi.createRequest(QString("reports?from=%1&to=%2")
                                      .arg(from.toString(Qt::ISODate))
                                      .arg(to.toString(Qt::ISODate))),
        this,
        [this, from, to](QRestReply &reply) {
            if (!reply.isSuccess()) {
                QString err = QString("Błąd pobierania raportu: HTTP %1, %2")
                                  .arg(reply.httpStatus())
                                  .arg(reply.readText());
                emit getOverallReportResult(QString(), err);
                return;
            }
            QByteArray xlsxData = reply.readBody();

            QString downloadsPath = QStandardPaths::writableLocation(
                QStandardPaths::DownloadLocation);
            QString fileName = QString("report_%1_%2.xlsx")
                                   .arg(from.toString(Qt::ISODate))
                                   .arg(to.toString(Qt::ISODate));
            QString fullPath = downloadsPath + QDir::separator() + fileName;

            QFile file(fullPath);
            if (!file.open(QIODevice::WriteOnly)) {
                QString err = QString("Nie można zapisać pliku: %1").arg(fullPath);
                emit getOverallReportResult(QString(), err);
            } else {
                file.write(xlsxData);
                file.close();
                emit getOverallReportResult(fullPath, QString());
            }
        }
        );
}

void BackendClient::completeInspection(const InspectionForm& form)
{
    QString boundary = "WebAppBoundary";
    auto request = _backendApi.createRequest("inspections/" + QString::number(form.inspectionId) + "/tasks/complete");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary);

    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary(boundary.toUtf8());

    auto addTextPart = [&](const QString& name, const QString& value) {
        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(name));
        part.setBody(value.toUtf8());
        return part;
    };

    multiPart->append(addTextPart("ReportDate", form.reportDate.toString(Qt::ISODate)));
    multiPart->append(addTextPart("RecommendationIssued", form.recommendationIssued ? "true" : "false"));
    multiPart->append(addTextPart("OrderIssued", form.orderIssued ? "true" : "false"));
    if(form.orderDeadline.has_value()){
        multiPart->append(addTextPart("OrderDeadline", form.orderDeadline.value().toString(Qt::ISODate)));
    }

    // Plik
    QFile* file = new QFile(form.reportFilePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit completeInspectionResult("Nie udało się otworzyć pliku: " + form.reportFilePath);
        delete file;
        return;
    }

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       "form-data; name=\"ReportFile\"; filename=\"" + QFileInfo(*file).fileName() + "\"");
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/pdf");
    filePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(filePart);

    QNetworkReply* reply = _networkManager->post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit completeInspectionResult("Błąd: " + reply->errorString());
            return;
        }

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode != 200 && statusCode != 204) {
            emit completeInspectionResult(QString("Błąd HTTP: %1\n%2")
                                              .arg(statusCode)
                                              .arg(QString::fromUtf8(reply->readAll())));
            return;
        }

        emit completeInspectionResult("");
    });
}

