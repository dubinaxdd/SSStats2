#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Shlwapi.h"

#include <QLocale>
#include <QTranslator>
#include "dev/core/core.h"
#include <QDebug>

// Запускает программу автообновления, которая возвращает exitCode в зависимости от наличия обновлений
int runAutoUpdate() {
    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = "Updater.exe";
    ShExecInfo.lpParameters = "/F";
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOWNORMAL;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteEx(&ShExecInfo);
    DWORD  lp;
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    GetExitCodeProcess(ShExecInfo.hProcess, &lp);
    qDebug() << "Updater return exit code" << lp;
    return lp;
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    if(runAutoUpdate() == 5){
        return 0; // Не запускаем ssstats - т.к. есть обновления и сейчас пойдет загрузка обновы
    }

    QGuiApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SSStats2_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/resources/qml/main.qml"));

    QQmlContext *context = engine.rootContext();

    Core *core = new Core(context, &app);

    engine.addImageProvider("ImageProvider",core->uiBackend()->imageProvider());

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    core->grubStatsWindow();

    return app.exec();
}
