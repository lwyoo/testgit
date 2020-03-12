#include "textureproviderscheduler.h"
#include "TextureProvider.h"

#include <QDebug>
#include <mutex>
#include <tuple>
std::mutex mtx_lock;

TextureProviderScheduler::TextureProviderScheduler()
{
}

std::shared_ptr<TextureProviderScheduler>& TextureProviderScheduler::GetInstance()
{
    //    qDebug() << Q_FUNC_INFO <<"";
    static std::shared_ptr<TextureProviderScheduler> s_tp_sch = nullptr;

    if (s_tp_sch == nullptr) {
        qDebug() << Q_FUNC_INFO << "new instance";
        s_tp_sch = std::shared_ptr<TextureProviderScheduler>(new TextureProviderScheduler);
    }

    return s_tp_sch;
}

bool TextureProviderScheduler::Init()
{
    qDebug() << Q_FUNC_INFO << "init";
    int res = -2;
    res = pthread_mutex_init(&m_thread_mutex, nullptr);
    qDebug() << Q_FUNC_INFO << "pthread_mutex_init res " << res;
    res = pthread_cond_init(&m_thread_cond, nullptr);
    qDebug() << Q_FUNC_INFO << "pthread_cond_init res " << res;
}

bool TextureProviderScheduler::Run()
{
    qDebug() << Q_FUNC_INFO << "run start ";
    int res = -2;
    res = pthread_create(&m_thread, nullptr, &MainThread, this);
    qDebug() << Q_FUNC_INFO << "pthread_create res " << res << "asdasda ?? " << m_thread;
    qDebug() << Q_FUNC_INFO << "run end : ";
}

void TextureProviderScheduler::Book(const std::vector<TextureProviderScheduler::preload_target>& targets)
{
    qDebug() << Q_FUNC_INFO << "book";
    //    {
    //        std::lock_guard<std::mutex> guard(mtx_lock);
    //        qDebug() << Q_FUNC_INFO << "book1111111111111111";
    //        for (auto i : targets) {
    //            qDebug() << Q_FUNC_INFO << "book2222222222222";
    //            m_list_target.push_back(i);
    //        }
    //    }
    int res = -2;

    qDebug() << Q_FUNC_INFO << "book 3333333333333";
    res = pthread_cond_signal(&m_thread_cond);
    qDebug() << Q_FUNC_INFO << "pthread_cond_signal res " << res;
    qDebug() << Q_FUNC_INFO << "book 444444444444" << m_thread;
}

void TextureProviderScheduler::Book(const preload_target& targets)
{
    qDebug() << Q_FUNC_INFO << "book";
    {
        std::lock_guard<std::mutex> guard(mtx_lock);
        m_list_target.push_back(targets);
    }

    int res = -2;

    res = pthread_cond_signal(&m_thread_cond);
    qDebug() << Q_FUNC_INFO << "pthread_cond_signal res " << res;
}

void* TextureProviderScheduler::MainThread(void* _p_data)
{
    //    qDebug() << Q_FUNC_INFO <<"thread enterance";
    qDebug() << Q_FUNC_INFO << "MainThread 555555555555";
    TextureProviderScheduler* p_hndl = static_cast<TextureProviderScheduler*>(_p_data);

    qDebug() << Q_FUNC_INFO << "MainThread 6666666666666666";
    int res = -2;
    //    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    qDebug() << Q_FUNC_INFO << "pthread_setcancelstate res " << res;
    qDebug() << Q_FUNC_INFO << "MainThread 777777777777777";

    pthread_t id;
    // 현재 쓰레드의 id 를 얻어서 출력합니다
    id = pthread_self();
    qDebug() << Q_FUNC_INFO << "MainThread 777777777777777 pthread_self() : " << id;

    while (true) {
        qDebug() << Q_FUNC_INFO << "thread loop p_hndl->m_list_target.size() : " << p_hndl->m_list_target.size();
        if (p_hndl->m_list_target.size()) {
            qDebug() << Q_FUNC_INFO << "remain: " << p_hndl->m_list_target.size();
            std::string path, extension;
            int start, end;

            {
                qDebug() << Q_FUNC_INFO << "MainThread 888888888888";
                std::lock_guard<std::mutex> guard(mtx_lock);
                qDebug() << Q_FUNC_INFO << "MainThread 99999999999999999";
                auto target = p_hndl->m_list_target.back();
                p_hndl->m_list_target.pop_back();
                std::tie(path, extension, start, end) = target;
                qDebug() << Q_FUNC_INFO << "[TEXPRO_SCH] start path:" << path.c_str()
                         << ", extenstion: " << extension.c_str()
                         << ", start: " << start
                         << ", end : " << end;
            }

            for (int i = start; i <= end; i++) {
                TextureProvider::instance().requestTexture(
                    QString("%1%2.%3").arg(QString::fromStdString(path)).arg(QString::number(i).rightJustified(3, '0')).arg(QString::fromStdString(extension)), {}, {});
                //                qDebug() << Q_FUNC_INFO <<"[TEXPRO_SCH] target: " << QString("%1%2.%3").arg(QString::fromStdString(path)).arg(QString::number(i).rightJustified(3, '0')).arg(QString::fromStdString(extension));
            }
            qDebug() << Q_FUNC_INFO << "[TEXPRO_SCH] finish";

        } else {
            qDebug() << Q_FUNC_INFO << "[TEXPRO_SCH] sleep";
            res = pthread_cond_wait(&p_hndl->m_thread_cond, &p_hndl->m_thread_mutex);
            qDebug() << Q_FUNC_INFO << "pthread_cond_wait res " << res;
            qDebug() << Q_FUNC_INFO << "[TEXPRO_SCH] wake";
        }
    }
}
