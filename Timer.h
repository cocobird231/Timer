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
    std::chrono::high_resolution_clock::duration interval_;
    std::chrono::high_resolution_clock::time_point st_;

    std::atomic<bool> activateF_;
    std::atomic<bool> exitF_;
    std::atomic<bool> funcCallableF_;
    std::function<void()> func_;

    std::thread timerTH_;
    std::thread callbackTH_;

private:
    void _timer()// Deprecated
    {
        while (!this->exitF_)
        {
            try
            {
                auto st = std::chrono::high_resolution_clock::now();
                while (!this->exitF_ && this->activateF_ && (std::chrono::high_resolution_clock::now() - st < this->interval_))
                    std::this_thread::yield();
                if (!this->exitF_ && this->activateF_ && this->funcCallableF_)
                {
                    if (this->callbackTH_.joinable())
                        this->callbackTH_.join();
                    this->callbackTH_ = std::thread(&Timer::_tick, this);
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

    void _timer_fixedRate()
    {
        while (!this->exitF_)
        {
            try
            {
                if (!this->exitF_ && this->activateF_)
                {
                    auto tickTimePoint = this->st_ + this->interval_;
                    this->st_ = tickTimePoint;

                    while (!this->exitF_ && this->activateF_ && (std::chrono::high_resolution_clock::now() < tickTimePoint))
                        std::this_thread::yield();
                    if (!this->exitF_ && this->activateF_ && this->funcCallableF_)
                    {
                        if (this->callbackTH_.joinable())
                            this->callbackTH_.join();
                        this->callbackTH_ = std::thread(&Timer::_tick, this);
                    }
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

    void _tick()
    {
        this->funcCallableF_ = false;
        this->func_();
        this->funcCallableF_ = true;
    }

public:
    Timer(double interval_ms, const std::function<void()>& callback) : activateF_(false), exitF_(false), funcCallableF_(true)
    {
        this->interval_ = std::chrono::high_resolution_clock::duration(std::chrono::nanoseconds(static_cast<uint64_t>(interval_ms * 1000000)));
        this->func_ = callback;
        this->st_ = std::chrono::high_resolution_clock::now();
        this->timerTH_ = std::thread(&Timer::_timer_fixedRate, this);
    }

    ~Timer()
    {
        this->destroy();
    }

    void start()
    {
        this->st_ = std::chrono::high_resolution_clock::now();
        this->activateF_ = true;
    }

    void stop() { this->activateF_ = false; }

    void setInterval(double interval_ms)// Experimental
    {
        this->stop();
        this->interval_ = std::chrono::high_resolution_clock::duration(std::chrono::nanoseconds(static_cast<uint64_t>(interval_ms * 1000000)));
        this->start();
    }

    void destroy()
    {
        this->activateF_ = false;
        this->exitF_ = true;
        if (this->timerTH_.joinable())
            this->timerTH_.join();
    }
};
