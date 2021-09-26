#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Shlwapi.h"

#include <QLocale>
#include <QTranslator>
#include "dev/core/core.h"
#include <QDebug>


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

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

    ShellExecute(NULL, NULL, "Updater.exe", "/F", NULL, SW_SHOWNORMAL);

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
