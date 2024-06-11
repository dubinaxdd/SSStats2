#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include "Shlwapi.h"
#include <QDir>

#include <QLocale>
#include <QTranslator>
#include <dev/core.h>
#include <QDebug>
#include <QIcon>

#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>

#include "dev/hookManager/hookManager.h"

#include <cstring>
#include <mbstring.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>


// Запускает программу автообновления, которая возвращает exitCode в зависимости от наличия обновлений
int runAutoUpdate() {
    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    //ShExecInfo.lpFile = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "\\Updater.exe").toStdString().c_str();
    ShExecInfo.lpFile = QString(QCoreApplication::applicationDirPath() + "\\Updater.exe").toStdString().c_str();
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
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app3(argc, argv);

    if(runAutoUpdate() == 5){
        return 0; // Не запускаем ssstats - т.к. есть обновления и сейчас пойдет загрузка обновы
    }

    QSystemSemaphore semaphore("<uniq id>", 1);     // создаём семафор
    semaphore.acquire();                            // Поднимаем семафор, запрещая другим экземплярам работать с разделяемой памятью

    QSharedMemory sharedMemory("<uniq id 2>");      // Создаём экземпляр разделяемой памяти
    bool is_running;                                // переменную для проверки ууже запущенного приложения
    if (sharedMemory.attach())
    {                                               // пытаемся присоединить экземпляр разделяемой памяти
                                                    // к уже существующему сегменту
        is_running = true;                          // Если успешно, то определяем, что уже есть запущенный экземпляр
    }
    else
    {
        sharedMemory.create(1);                     // В противном случае выделяем 1 байт памяти
        is_running = false;                         // И определяем, что других экземпляров не запущено
    }
    semaphore.release();                            // Опускаем семафор

    // Если уже запущен один экземпляр приложения, то сообщаем об этом пользователю
    // и завершаем работу текущего экземпляра приложения
    if(is_running)
    {
        QApplication app2(argc, argv);

        QMessageBox msgBox;

        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("DowStatsClient is already running");
        msgBox.exec();

        return 0;
    }

    HookManager::instance();

//#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif

    QGuiApplication app(argc, argv);
    app.setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    QTranslator translator;

    std::function<void()> loadSystemTranslation([&] {
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "DowStatsClient_" + QLocale(locale).name();
            if (translator.load(baseName, ":/translations/resources/translations"))
            {
                app.installTranslator(&translator);
                break;
            }
        }
    });

    loadSystemTranslation();

    qmlRegisterSingletonType(QUrl("qrc:/resources/qml/GlobalMouseProvider.qml"), "GlobalMouseProvider", 1, 0, "GlobalMouseProvider");
    qmlRegisterSingletonType(QUrl("qrc:/resources/qml/DowStatsStyle.qml"), "DowStatsStyle", 1, 0, "DowStatsStyle");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/resources/qml/main.qml"));

    QQmlContext *context = engine.rootContext();

    Core core(context, &app);

    engine.addImageProvider("ImageProvider",core.uiBackend()->imageProvider());

    app.setWindowIcon(QIcon(":/icons/resources/icons/DowStatsClient.ico"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QObject::connect(&core, &Core::sendExit, [&] {
        engine.removeImageProvider("imageprovider");
        app.exit(0);});


    QObject::connect(core.uiBackend()->settingsPageModel(), &SettingsPageModel::languageChanged, [&](Language::LanguageEnum language) {
        if (!translator.isEmpty())
            QCoreApplication::removeTranslator(&translator);

        if (language == Language::LanguageEnum::System)
            loadSystemTranslation();
        else
        {
            QString baseName;

            if (language == Language::LanguageEnum::Ru)
                baseName = "DowStatsClient_" + QLocale("ru-RU").name();

            if (language == Language::LanguageEnum::Eng)
                baseName = "DowStatsClient_" + QLocale("en-US").name();

            translator.load(baseName, ":/translations/resources/translations");
            app.installTranslator(&translator);
        }
        engine.retranslate();
    });

    core.overlayWindowController()->grubStatsWindow();

    return app.exec();
}
