#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QSharedPointer>
#include <QFile>
#include <QTextStream>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(logDebug)
Q_DECLARE_LOGGING_CATEGORY(logInfo)
Q_DECLARE_LOGGING_CATEGORY(logWarning)
Q_DECLARE_LOGGING_CATEGORY(logCritical)

static QSharedPointer<QFile> m_logFile;

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    // Smart pointer to log file



};

#endif // LOGGER_H
