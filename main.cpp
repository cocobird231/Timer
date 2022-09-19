#include "Timer.h"

class Obj
{
private:
    Timer* tm_;
    int cnt;

private:
    void _callback()
    {
        std::cout << "Obj count: " << cnt++ << std::endl;
    }

public:
    Obj(int interval_ms)
    {
        this->tm_ = new Timer(interval_ms, std::bind(&Obj::_callback, this));
        this->cnt = 0;
    }

    ~Obj()
    {
        this->tm_->destroy();
        delete this->tm_;
    }

    void start() { this->tm_->start(); }

    void stop() { this->tm_->stop(); }

    void destroy() { this->tm_->destroy(); }
};


void func()
{
    std::cout << "func called." << std::endl;
}

void funcLongProcess()
{
    std::this_thread::sleep_for(2s);
    std::cout << "funcLongProcess called." << std::endl;
}

int main(int argc, char** argv)
{
    Timer tm(1000, func);// Fixed calling rate
    Obj obj(500);// Passing variable in callback function with class method
    Timer tmLongProc(1000, funcLongProcess);// Fixed calling rate, ignore ticks calling while function still processing
    
    tm.start();
    obj.start();
    tmLongProc.start();
    
    std::this_thread::sleep_for(5s);
    tm.destroy();
    obj.destroy();
    tmLongProc.destroy();
    return 0;
}