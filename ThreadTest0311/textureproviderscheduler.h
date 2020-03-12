#ifndef TEXTUREPROVIDERSCHEDULER_H
#define TEXTUREPROVIDERSCHEDULER_H

#include <pthread.h>

#include <memory>
#include <tuple>
#include <vector>

#include <QLoggingCategory>

//Q_DECLARE_LOGGING_CATEGORY( texproscheduler );
//#include <dlt/dlt.h>

class TextureProviderScheduler {
public:
    using preload_target = std::tuple<std::string, std::string, int, int>;
    static std::shared_ptr<TextureProviderScheduler>& GetInstance(void);

public:
    TextureProviderScheduler();
    TextureProviderScheduler(const TextureProviderScheduler&) = delete;
    void operator=(const TextureProviderScheduler&) = delete;

public:
    bool Init();
    bool Run();
    void Book(const std::vector<preload_target>& targets);
    void Book(const preload_target& targets);

private:
    bool m_thread_running = false;
    pthread_t m_thread = 0;
    pthread_mutex_t m_thread_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t m_thread_cond = PTHREAD_COND_INITIALIZER;
    static void* MainThread(void* _p_data);

private:
    //    QVector<QStringList> m_list_target;
    std::vector<preload_target> m_list_target;
};

#endif // TEXTUREPROVIDERSCHEDULER_H
