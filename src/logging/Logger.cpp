#include "logging/Logger.h"

#include "app/AppInfo.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

namespace volchay {

QString Logger::logsDirectory()
{
    const auto base = QCoreApplication::applicationDirPath().isEmpty()
        ? QDir::currentPath()
        : QCoreApplication::applicationDirPath();
    return QDir(base).filePath("logs");
}

QString Logger::logPath()
{
    return QDir(logsDirectory()).filePath(LogFileName);
}

void Logger::initialize()
{
    QDir().mkpath(logsDirectory());
    const auto path = logPath().toStdString();
    auto logger = spdlog::rotating_logger_mt("volchay", path, 1024 * 1024, 5);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    logger->flush_on(spdlog::level::warn);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    spdlog::info("{} {} starting", AppName, AppVersion);
    spdlog::info("Log file: {}", path);
}

QStringList Logger::recentLines(int maxLines)
{
    QStringList lines;
    QFile file(logPath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return lines;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        lines.append(stream.readLine());
        while (lines.size() > maxLines) {
            lines.removeFirst();
        }
    }
    return lines;
}

void Logger::flush()
{
    if (auto logger = spdlog::default_logger()) {
        logger->flush();
    }
}

} // namespace volchay
