# Timer
Timer calls function at fixed rate

## Description
1. Timer calls function (callback) at fixed rate (interval_ms).
2. Since Timer object constructed, the timer thread starts to loop until destroy function called.
3. Call start function to start counting and funciton callback.
4. If the process time of callback function is greater then timer interval, ignore the further funciton call until callback function complete.

## Usage
```bash=Ubuntu
g++ -Wall main.cpp -o main -lpthread
```