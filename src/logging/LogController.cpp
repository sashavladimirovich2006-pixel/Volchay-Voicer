#include "logging/LogController.h"

#include "logging/Logger.h"

namespace volchay {

LogController::LogController(QObject* parent)
    : QObject(parent)
{
    refresh();
}

QString LogController::logPath() const
{
    return Logger::logPath();
}

QStringList LogController::lines() const
{
    return m_lines;
}

void LogController::refresh()
{
    const auto updated = Logger::recentLines(160);
    if (updated == m_lines) {
        return;
    }
    m_lines = updated;
    emit linesChanged();
}

} // namespace volchay

