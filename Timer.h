#include <iostream>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

using namespace std::chrono_literals;

class Timer
{
private:
    std::chrono::duration<float, std::milli> interval_;
    std::atomic<bool> activateF_;
    std::atomic<bool> exitF_;
    std::atomic<bool> funcCallableF_;
    std::function<void()> func_;

    std::thread timerTH_;
    std::thread callbackTH_;

private:
    void _timer()
    {
        while (!exitF_)
        {
            try
            {
                auto st = std::chrono::steady_clock::now();
                while (!exitF_ && this->activateF_ && (std::chrono::steady_clock::now() - st < this->interval_))
                    std::this_thread::yield();
                if (!exitF_ && this->activateF_ && this->funcCallableF_)
                {
                    if (this->callbackTH_.joinable())
                        this->callbackTH_.join();
                    this->callbackTH_ = std::thread(&Timer::_timer_tick, this);
                }
                std::this_thread::yield();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        if (this->callbackTH_.joinable())
            this->callbackTH_.join();
    }

    void _timer_tick()
    {
        this->funcCallableF_ = false;
        this->func_();
        this->funcCallableF_ = true;
    }

public:
    Timer(int interval_ms, const std::function<void()>& callback) : activateF_(false), exitF_(false), funcCallableF_(true)
    {
        this->interval_ = std::chrono::milliseconds(interval_ms);
        this->func_ = callback;
        this->timerTH_ = std::thread(&Timer::_timer, this);
    }

    ~Timer()
    {
        this->destroy();
    }

    void start() { this->activateF_ = true; }

    void stop() { this->activateF_ = false; }

    void destroy()
    {
        this->activateF_ = false;
        this->exitF_ = true;
        if (this->timerTH_.joinable())
            this->timerTH_.join();
    }
};