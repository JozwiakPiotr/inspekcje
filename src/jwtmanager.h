#ifndef JWTMANAGER_H
#define JWTMANAGER_H

#include <QObject>
#include <QString>

class JwtManager {
public:
    static JwtManager& instance() {
        static JwtManager instance;
        return instance;
    }

    void setToken(const QString& token) {
        m_token = token;
    }

    QString token() const {
        return m_token;
    }

    void clearToken() {
        m_token.clear();
    }

private:
    JwtManager() = default;
    ~JwtManager() = default;

    JwtManager(const JwtManager&) = delete;
    JwtManager& operator=(const JwtManager&) = delete;

    QString m_token;
};

#endif // JWTMANAGER_H
