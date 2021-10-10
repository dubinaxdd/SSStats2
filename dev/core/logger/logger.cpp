#include "logger.h"

#include <QDateTime>
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
    // Open stream file writes
    QTextStream out(m_logFile.data());
    // Write the date of recording
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    // By type determine to what level belongs message
    switch (type)
    {
        case QtInfoMsg:      break;
        case QtDebugMsg:     break;
        case QtWarningMsg:   break;
        case QtCriticalMsg:  break;
        case QtFatalMsg:     break;
    }
    // Write to the output category of the message and the message itself
    out << context.category << ": "
        << msg << Qt::endl;

    out.flush();    // Clear the buffered data
}

