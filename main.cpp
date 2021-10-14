#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include "Shlwapi.h"

#include <QLocale>
#include <QTranslator>
#include "dev/core/core.h"
#include <QDebug>

#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>


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
    setlocale(LC_ALL,"Russian");


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
    if(is_running){
        QApplication app2(argc, argv);

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("SSStats2 is already running");
        msgBox.exec();
        return 1;
    }

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

    QObject::connect(core, &Core::sendExit, &app, &QGuiApplication::quit);

    core->grubStatsWindow();

    return app.exec();
}
