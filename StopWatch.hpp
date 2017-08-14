#pragma once

//---------------------------------------------------------------------------//
//
// StopWatch.hpp
//  経過時間の計測
//   Copyright (C) 2011-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//

namespace tapetums
{
    class StopWatch;
}

//---------------------------------------------------------------------------//

class tapetums::StopWatch final
{
private:
    INT64 frequency  { 0 };
    INT64 start_time { 0 };
    INT64 stop_time  { 0 };

public:
    StopWatch() noexcept { reset(); }
    ~StopWatch() = default;

    StopWatch(const StopWatch&) = delete;
    StopWatch& operator =(const StopWatch&) = delete;

    StopWatch(StopWatch&&) noexcept = delete;
    StopWatch& operator =(StopWatch&&) noexcept = delete;

public:
    void start() noexcept
    {
        ::QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
    }

    void stop() noexcept
    {
        ::QueryPerformanceCounter((LARGE_INTEGER*)&stop_time);
    }

    void reset() noexcept
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