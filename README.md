## Description
- The `LiteTimer` class is a timer which can call a function periodically with given period.
- The `TimerPool` class is a pool for LiteTimer objects. It can manage multiple timers to save cpu resources.
- This repo is using pthread library and it is only tested on linux platform.
- The `LiteTimer` and `TimerPool` classes are using self-defined threads (`unique_thread`) under utils.h for safe thread management (RAII).

### LiteTimer
```cpp
// It is good to use std::shared_ptr to manage the LiteTimer object.
std::shared_ptr<LiteTimer> timer = std::make_shared<LiteTimer>(period, callback);


//======== Timer functions ========

timer->start();// Start the timer.
timer->stop();// Stop the timer.
timer->setPeriod();// Set the period of the timer. It can be called when the timer is running.
timer->destroy();// Destroy the timer if it is not needed anymore.


//======== Callback function ========

// The callback function should be a void function with no arguments.
void callback();

// The callback function can be binded with a class member function, for instance:
class MakeLiteTimer
{
private:
    std::shared_ptr<LiteTimer> timer_;

private:
    void _callback()
    {
        // Do something periodically.
    }

public:
    MakeLiteTimer(double period_ms)
    {
        this->timer_ = std::make_shared<LiteTimer>(period_ms, std::bind(&MakeLiteTimer::_callback, this));
        this->timer_->start();
    }
};
```

### TimerPool
```cpp
std::shared_ptr<LiteTimer> tm1 = std::make_shared<LiteTimer>(period, callback);// Create a LiteTimer object.
std::shared_ptr<LiteTimer> tm2 = std::make_shared<LiteTimer>(period, callback);
TimerPool tp;// Create a TimerPool object.

tp.addTimer(tm1);// OK, tm1 is added to the TimerPool.

tm2->start();// tm2 start timing.
tp.addTimer(tm2);// Warning, tm2 is not added to the TimerPool because it is already running.

tm2->stop();// tm2 stop timing.
tp.addTimer(tm2);// OK, tm2 is added to the TimerPool.

tm1->start();// Warning, tm1 ignores the call because it is controlled by the TimerPool.
tm1->stop();// Same as above.
tm1->setPeriod();// Same as above.
tm1->destroy();// Same as above.

tp.start();// Start all the timers in the TimerPool.
tp.stop();// Stop all the timers in the TimerPool.
tp.release();// Release all the timers in the TimerPool. Warning, each timer will not be destroyed.
```

## Examples
- The `test_nopool.cpp` is an example of using LiteTimer and create timer stability test.

    Build:
    ```bash
    g++ test_nopool.cpp -o test_nopool -lpthread
    ```
    Run test:
    ```bash
    ./test_nopool
    ```

- The `test_pool.cpp` is an example of using TimerPool to manage multiple timers and create timer stability test.

    Build:
    ```bash
    g++ test_pool.cpp -o test_pool -lpthread
    ```
    Run test:
    ```bash
    ./test_pool
    ```