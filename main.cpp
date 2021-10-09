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

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

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
