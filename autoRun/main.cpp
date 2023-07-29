#include <QCoreApplication>
#include <QProcess>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QProcess* clientProcess = new QProcess();
    clientProcess->startDetached(QDir::toNativeSeparators('\"' + QCoreApplication::applicationDirPath() + "\\DowStatsClient.exe" + '\"'));

    delete clientProcess;
    return 0;
}
