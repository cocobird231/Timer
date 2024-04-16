#include "timer.h"
#include <iomanip>

class TestClass
{
private:
    std::deque<std::shared_ptr<LiteTimer> > tms_;
    std::deque<std::deque<std::chrono::high_resolution_clock::time_point> > tmTps_;

private:
    void _callback(int i)
    {
        this->tmTps_[i].emplace_back(std::chrono::high_resolution_clock::now());
    }

public:
    TestClass(int n, double period_ms)
    {
        for (int i = 0; i < n; i++)
        {
            std::shared_ptr<LiteTimer> tm = std::make_shared<LiteTimer>(period_ms, std::bind(&TestClass::_callback, this, i));
            this->tms_.push_back(tm);
        }
        this->tmTps_.resize(n);
    }

    void start()
    {
        for (int i = 0; i < this->tms_.size(); i++)
        {
            this->tms_[i]->start();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::deque<std::deque<std::chrono::high_resolution_clock::time_point> > stop()
    {
        for (int i = 0; i < this->tms_.size(); i++)
        {
            this->tms_[i]->stop();
        }
        return this->tmTps_;
    }
};

int main()
{
    int n = 10;// timers.
    double period_ms = 50;
    double testTime_s = 30;
    TestClass tc(n, period_ms);
    std::this_thread::sleep_for(std::chrono::duration<double>(1));// Test for 30 seconds.
    std::cout << "Start testing for " << testTime_s << " s." << std::endl;
    tc.start();
    std::this_thread::sleep_for(std::chrono::duration<double>(testTime_s));// Test for 30 seconds.
    auto tmTps = tc.stop();
    // Get cumulative errors.
    std::vector<double> errors;
    for (int i = 0; i < tmTps.size(); i++)
    {
        double error = 0;
        for (int j = 1; j < tmTps[i].size(); j++)
        {
            error += std::abs((tmTps[i][j] - tmTps[i][j - 1]).count() / 1000000.0 - period_ms);// Ideal error is 0.
        }
        errors.push_back(error);
    }
    // Print errors.
    for (int i = 0; i < errors.size(); i++)
    {
        std::cout << "Timer " << i << " error: " << errors[i] << " ms in " << testTime_s << " s." << std::endl;
    }
    return 0;
}