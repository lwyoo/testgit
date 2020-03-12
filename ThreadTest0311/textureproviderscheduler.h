#ifndef TEXTUREPROVIDERSCHEDULER_H
#define TEXTUREPROVIDERSCHEDULER_H

#include <pthread.h>

#include <QLoggingCategory>
#include <memory>
#include <queue>
#include <tuple>
#include <vector>

//Q_DECLARE_LOGGING_CATEGORY( texproscheduler );
//#include <dlt/dlt.h>
#define TEST_STACK 0
#define TEST_QUEUE 1

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
#if TEST_STACK
    std::vector<preload_target> m_list_target;
#endif
    std::queue<preload_target> m2_list_target;
};

#endif // TEXTUREPROVIDERSCHEDULER_H
