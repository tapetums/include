#pragma once

//---------------------------------------------------------------------------//
//
// StopWatch.hpp
//  Measurement of elapsed time
//   Copyright (C) 2011-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//
// Forward Declaration
//---------------------------------------------------------------------------//

namespace tapetums
{
    class StopWatch;
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

class tapetums::StopWatch final
{
private:
    INT64 frequency  { 0 };
    INT64 start_time { 0 };
    INT64 stop_time  { 0 };

public:
    StopWatch() { reset(); }
    ~StopWatch() = default;

    StopWatch(const StopWatch&)             = delete;
    StopWatch& operator =(const StopWatch&) = delete;

    StopWatch(StopWatch&&)             noexcept = delete;
    StopWatch& operator =(StopWatch&&) noexcept = delete;

public:
    void start()
    {
        ::QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
    }

    void stop()
    {
        ::QueryPerformanceCounter((LARGE_INTEGER*)&stop_time);
    }

    void reset()
    {
        ::QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
        start_time = stop_time = 0;
    }

public:
    double sec() const noexcept
    {
        return 1.0 * (stop_time - start_time) / frequency;
    }

    double msec() const noexcept
    {
        return 1000.0 * (stop_time - start_time) / frequency;
    }

    double usec() const noexcept
    {
        return 1000.0 * 1000.0 * (stop_time - start_time) / frequency;
    }

    double nsec() const noexcept
    {
        return 1000.0 * 1000.0 * 1000.0 * (stop_time - start_time) / frequency;
    }
};

//---------------------------------------------------------------------------//

// StopWatch.hpp