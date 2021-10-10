#include "logger.h"

#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <iostream>

Q_LOGGING_CATEGORY(logDebug,    "DEBUG")
Q_LOGGING_CATEGORY(logInfo,     "INFO")
Q_LOGGING_CATEGORY(logWarning,  "WARNING")
Q_LOGGING_CATEGORY(logCritical, "CRITICAL")

Logger::Logger(QObject *parent) : QObject(parent)
{
    m_logFile.reset(new QFile(QCoreApplication::applicationDirPath() + QDir::separator() + "statsLog.txt"));
    m_logFile.data()->open(QFile::WriteOnly | QFile::Truncate);
    qInstallMessageHandler(messageHandler);
}

Logger::~Logger()
{
    m_logFile.data()->close();
}

// The implementation of the handler
void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    QString logMessage = date + context.category + ": " + msg;

    // Open stream file writes
    QTextStream fileOutput(m_logFile.data());
    fileOutput << logMessage <<  Qt::endl;
    fileOutput.flush();

    QTextStream(stdout) << logMessage <<  Qt::endl;
}
