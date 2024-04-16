#pragma once

#include <thread>
#include <memory>

struct thread_deleter
{
    void operator()(std::thread* t)
    {
        if (t->joinable())
            t->join();
        delete t;
    }
};

using unique_thread = std::unique_ptr<std::thread, thread_deleter>;

template<typename F, typename... args>
unique_thread make_unique_thread(F&& f, args&&... a)
{
    return unique_thread(new std::thread(std::forward<F>(f), std::forward<args>(a)...));
}

using shared_thread = std::shared_ptr<std::thread>;

template<typename F, typename... args>
shared_thread make_shared_thread(F&& f, args&&... a)
{
    return shared_thread(new std::thread(std::forward<F>(f), std::forward<args>(a)...), thread_deleter());
}