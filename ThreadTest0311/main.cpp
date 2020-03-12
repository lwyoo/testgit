#include "TextureProvider.h"
#include "textureproviderscheduler.h"
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    qDebug() << "thread id : " << QThread::currentThreadId();

    QString rscPath
        = QCoreApplication::applicationDirPath() + "/image.rsc";

    // size of inf for eco-gauge is 273019873 bytes.
    TextureProvider::CreateInstance("/home/ldg/Project/TEST_TEX_PROVIDER/image.rsc", 1186139614, 0);

    TextureProviderScheduler::GetInstance()->Init();
    TextureProviderScheduler::GetInstance()->Run();

    {
        std::vector<TextureProviderScheduler::preload_target> pre_firsts = {
            std::make_tuple("ECO_NUM_KPH/ECO_NUM_KMH_", "pkm", 0, 70),
            std::make_tuple("ECO_NUM_KPH_BG/eCO_NUM_KPH_BG_ECO_NUM_KPH_BG_", "pkm", 0, 40),

            std::make_tuple("ECO_NUM_MPH_BG/ECO_NUM_MPH_", "pkm", 0, 60),
            std::make_tuple("ECO_NUM_MPH_NUM/ECO_NUM_MPH_", "pkm", 0, 40),

            std::make_tuple("ECO_NUM_RPM_BG/eCO_NUM_RPM_BG_ECO_NUM_RPM_BG_", "pkm", 0, 59),
            std::make_tuple("ECO_NUM_RPM/eCO_NUM_RPM_BG_ECO_NUM_RPM_", "pkm", 0, 80),

            std::make_tuple("ECO_NUM_DSL_BG/ECO_NUM_DSL_", "pkm", 0, 120),
            std::make_tuple("ECO_NUM_DSL_NUM/ECO_NUM_DSL_", "pkm", 0, 120),
        };

        //        TextureProviderScheduler::preload_target pre_firsts = std::make_tuple("ECO_NUM_KPH/ECO_NUM_KMH_", "pkm", 0, 70);
        TextureProviderScheduler::GetInstance()->Book(pre_firsts);
    }

    //stack test
    {
        std::vector<TextureProviderScheduler::preload_target> pre_firsts = {
            std::make_tuple("ECO_NUM_KPH/ECO_NUM_KMH_", "pkm", 71, 520),

            std::make_tuple("ECO_NUM_MPH_NUM/ECO_NUM_MPH_", "pkm", 41, 480),

            std::make_tuple("ECO_NUM_RPM/eCO_NUM_RPM_BG_ECO_NUM_RPM_", "pkm", 81, 480),

            std::make_tuple("ECO_NUM_DSL_BG/ECO_NUM_DSL_", "pkm", 121, 450),
            std::make_tuple("ECO_NUM_DSL_NUM/ECO_NUM_DSL_", "pkm", 121, 450),
        };

        TextureProviderScheduler::GetInstance()->Book(pre_firsts);
    }

    return app.exec();
}
