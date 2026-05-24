#pragma once

#include <QString>
#include <QStringList>

namespace volchay {

class Logger {
public:
    static void initialize();
    static QString logPath();
    static QString logsDirectory();
    static QStringList recentLines(int maxLines);
    static void flush();
};

} // namespace volchay

