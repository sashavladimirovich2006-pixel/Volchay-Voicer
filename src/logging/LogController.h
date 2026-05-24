#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

namespace volchay {

class LogController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString logPath READ logPath CONSTANT)
    Q_PROPERTY(QStringList lines READ lines NOTIFY linesChanged)

public:
    explicit LogController(QObject* parent = nullptr);

    QString logPath() const;
    QStringList lines() const;

    Q_INVOKABLE void refresh();

signals:
    void linesChanged();

private:
    QStringList m_lines;
};

} // namespace volchay

