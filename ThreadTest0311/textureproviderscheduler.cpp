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
    //    qDebug()  <<"";
    static std::shared_ptr<TextureProviderScheduler> s_tp_sch = nullptr;

    if (s_tp_sch == nullptr) {
        qDebug() << "new instance";
        s_tp_sch = std::shared_ptr<TextureProviderScheduler>(new TextureProviderScheduler);
    }

    return s_tp_sch;
}

bool TextureProviderScheduler::Init()
{
    qDebug() << "[Init]";
    int res = -2;
    res = pthread_mutex_init(&m_thread_mutex, nullptr);
    res = pthread_cond_init(&m_thread_cond, nullptr);
}

bool TextureProviderScheduler::Run()
{
    qDebug() << "[Run]";
    int res = -2;
    res = pthread_create(&m_thread, nullptr, &MainThread, this);
}

void TextureProviderScheduler::Book(const std::vector<TextureProviderScheduler::preload_target>& targets)
{
    qDebug() << "[Book2] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    {
        std::lock_guard<std::mutex> guard(mtx_lock);
        for (auto i : targets) {
            m_list_target.push_back(i);
        }
    }
    int res = -2;

    qDebug() << "[Book2] wake!!";
    res = pthread_cond_signal(&m_thread_cond);
    qDebug() << "[Book2] wake!! end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
}

void TextureProviderScheduler::Book(const preload_target& targets)
{
    qDebug() << "[Book]";
    {
        std::lock_guard<std::mutex> guard(mtx_lock);
        m_list_target.push_back(targets);
    }

    int res = -2;

    qDebug() << "[Book] wake!!";
    res = pthread_cond_signal(&m_thread_cond);
    qDebug() << "[Book] wake!! end";
}

//void* TextureProviderScheduler::MainThread(void* _p_data)
//{
//    //    qDebug()  <<"thread enterance";
//    qDebug() << "[]";
//    TextureProviderScheduler* p_hndl = static_cast<TextureProviderScheduler*>(_p_data);

//    int res = -2;
//    //    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

//    pthread_t id;
//    // 현재 쓰레드의 id 를 얻어서 출력합니다
//    id = pthread_self();

//    while (true) {
//        if (p_hndl->m_list_target.size()) {
//            std::string path, extension;
//            int start, end;

//            {
//                std::lock_guard<std::mutex> guard(mtx_lock);
//                auto target = p_hndl->m_list_target.back();
//                p_hndl->m_list_target.pop_back();
//                std::tie(path, extension, start, end) = target;
//                qDebug() << "[MainThread] start path:" << path.c_str()
//                         << ", extenstion: " << extension.c_str()
//                         << ", start: " << start
//                         << ", end : " << end;
//            }

//            for (int i = start; i <= end; i++) {
//                TextureProvider::instance().requestTexture(
//                    QString("%1%2.%3").arg(QString::fromStdString(path)).arg(QString::number(i).rightJustified(3, '0')).arg(QString::fromStdString(extension)), {}, {});
//                //                qDebug()  <<"[TEXPRO_SCH] target: " << QString("%1%2.%3").arg(QString::fromStdString(path)).arg(QString::number(i).rightJustified(3, '0')).arg(QString::fromStdString(extension));
//            }
//            qDebug() << "[MainThread] finish";

//        } else {
//            qDebug() << "[MainThread] sleep";
//            res = pthread_cond_wait(&p_hndl->m_thread_cond, &p_hndl->m_thread_mutex);
//        }
//    }
//}
void* TextureProviderScheduler::MainThread(void* _p_data)
{
    //    qDebug()  <<"thread enterance";
    qDebug() << "[MainThread]";
    TextureProviderScheduler* p_hndl = static_cast<TextureProviderScheduler*>(_p_data);

    int res = -2;
    //    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

    pthread_t id;
    // 현재 쓰레드의 id 를 얻어서 출력합니다
    id = pthread_self();

    while (true) {
        if (p_hndl->m_list_target.size()) {
            std::string path, extension;
            int start, end;

            {
                std::lock_guard<std::mutex> guard(mtx_lock);
                auto target = p_hndl->m_list_target.back();
                p_hndl->m_list_target.pop_back();
                std::tie(path, extension, start, end) = target;
                qDebug() << "[MainThread] start path:" << path.c_str()
                         << ", extenstion: " << extension.c_str()
                         << ", start: " << start
                         << ", end : " << end;
            }

            for (int i = start; i <= end; i++) {
                bool bRes = false;
                bRes = TextureProvider::instance().requestTexture(
                    QString("%1%2.%3").arg(QString::fromStdString(path)).arg(QString::number(i).rightJustified(3, '0')).arg(QString::fromStdString(extension)), nullptr, {});
                //                qDebug() << "[TEXPRO_SCH] target: " << QString("%1%2.%3%4").arg(QString::fromStdString(path)).arg(QString::number(i).rightJustified(3, '0')).arg(QString::fromStdString(extension)).arg(bRes);
            }
            qDebug() << "[MainThread] finish";

        } else {
            qDebug() << "[MainThread] sleep";
            res = pthread_cond_wait(&p_hndl->m_thread_cond, &p_hndl->m_thread_mutex);
        }
    }
}
