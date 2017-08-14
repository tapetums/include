#pragma once

//---------------------------------------------------------------------------//
// 
// Task.hpp
// タスク処理用スレッドプールクラス
//   Copyright (C) 2013-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <functional>
#include <deque>
#include <thread>
#include <vector>

#include "Lock.hpp"

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    class ThreadPool;
    class TaskWorker;

    using Task = std::function<void (TaskWorker&)>;
    static constexpr Task task_null;
}

//---------------------------------------------------------------------------//
// ThreadPool Class
//---------------------------------------------------------------------------//

class tapetums::ThreadPool final
{
public:
    static constexpr uint32_t MAX_THREADS_PER_PROCESSOR { 500 };

private:
    CS::Lock m_lock;
    std::vector<TaskWorker> m_workers;
    size_t index { 0 };

public:
    explicit ThreadPool(size_t worker_count);

public:
    ThreadPool() = delete;

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&& rhs) noexcept = default;
    ThreadPool& operator=(ThreadPool&&) noexcept = default;

    ~ThreadPool() { Stop(); }

public:
    size_t worker_count() const noexcept { return m_workers.size(); }

public:
    void AddTask  (const Task& task);
    void AddTask  (Task&& task);
    Task QueryTask();
    void Start    ();
    void Stop     ();
    void Pause    () noexcept;
    void Resume   () noexcept;

private:
    void next_worker()
    {
        ++index;
        if ( index >= worker_count() )
        {
            index = 0;
        }
    }
};

//---------------------------------------------------------------------------//
// ThreadPool ctor
//---------------------------------------------------------------------------//

inline tapetums::ThreadPool::ThreadPool(size_t worker_count)
{
    SYSTEM_INFO si { };
    ::GetNativeSystemInfo(&si);
    const auto num_procs = si.dwNumberOfProcessors;
    const auto max_count = num_procs * MAX_THREADS_PER_PROCESSOR;

    size_t count;
    if ( worker_count == 0 )
    {
        count = num_procs;
    }
    else if ( worker_count > max_count )
    {
        count = max_count;
    }
    else
    {
        count = worker_count;
    }

    while ( count-- )
    {
        m_workers.push_back(TaskWorker(this));
    }
}

//---------------------------------------------------------------------------//
// ThreadPool Methods
//---------------------------------------------------------------------------//

inline void tapetums::ThreadPool::AddTask(const Task& task)
{
    CS::LockGuard guard(m_lock);

    m_workers[index].AddTask(task);

    next_worker();
}

//---------------------------------------------------------------------------//

inline void tapetums::ThreadPool::AddTask(Task&& task)
{
    CS::LockGuard guard(m_lock);

    m_workers[index].AddTask(std::move(task));

    next_worker();
}

//---------------------------------------------------------------------------//

inline tapetums::Task tapetums::ThreadPool::QueryTask()
{
    CS::LockGuard guard(m_lock);

    const auto count = m_workers.size();
    for ( size_t i = 0; i < count; ++i )
    {
        auto task = m_workers[index].StealTask();
        if ( static_cast<bool>(task) )
        {
            return task;
        }

        next_worker();
    }

    return task_null;
}

//---------------------------------------------------------------------------//

inline void tapetums::ThreadPool::Start()
{
    for ( auto& worker : m_workers )
    {
        worker.Start();
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::ThreadPool::Stop()
{
    for ( auto& worker : m_workers )
    {
        worker.Stop();
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::ThreadPool::Pause() noexcept
{
    for ( auto& worker : m_workers )
    {
        worker.Pause();
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::ThreadPool::Resume() noexcept
{
    for ( auto& worker : m_workers )
    {
        worker.Resume();
    }
}

//---------------------------------------------------------------------------//
// TaskWorker Class
//---------------------------------------------------------------------------//

class tapetums::TaskWorker final
{
    friend class ThreadPool; // インスタンスを生成できるのは ThreadPool だけ

private:
    ThreadPool* m_pool { nullptr };

    bool  m_working   { false };
    DWORD m_thread_id { 0 };

    CS::Lock m_lock;
    std::thread m_thread;
    std::deque<Task> m_tasks;

private:
    explicit TaskWorker(ThreadPool* pool) : m_pool(pool) { }

public:
    TaskWorker() = delete;

    TaskWorker(const TaskWorker&) = delete;
    TaskWorker& operator=(const TaskWorker&) = delete;

    TaskWorker(TaskWorker&& rhs) noexcept { swap(std::move(rhs)); }
    TaskWorker& operator=(TaskWorker&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    ~TaskWorker() { Stop(); }

private:
    void swap(TaskWorker&& rhs) noexcept;

public:
    bool   empty     () const noexcept { return m_tasks.empty(); }
    bool   is_paused () const noexcept { return ! m_working; }
    bool   is_running() const noexcept { return m_thread_id != 0; }
    size_t task_count() const noexcept { return m_tasks.size(); }
    DWORD  thread_id () const noexcept { return m_thread_id; }

public:
    void AddTask  (const Task& task);
    void AddTask  (Task&& task);
    Task QueryTask();
    Task StealTask();
    void Start    ();
    void Stop     ();
    void Pause    () noexcept;
    void Resume   () noexcept;

private:
    void MainLoop();
};

//---------------------------------------------------------------------------//
// TaskWorker move ctor
//  std::deque が noexcept な move ctor を持っていないため 定義が必要
//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::swap(TaskWorker&& rhs) noexcept
{
    if ( this == &rhs ) { return; }

    std::swap(m_pool,      rhs.m_pool);
    std::swap(m_working,   rhs.m_working);
    std::swap(m_thread_id, rhs.m_thread_id);
    std::swap(m_lock,      rhs.m_lock);
    std::swap(m_thread,    rhs.m_thread);
    std::swap(m_tasks,     rhs.m_tasks);
}

//---------------------------------------------------------------------------//
// TaskWorker Methods
//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::AddTask(const Task& task)
{
    CS::LockGuard guard(m_lock);

    m_tasks.push_back(task);
}

//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::AddTask(Task&& task)
{
    CS::LockGuard guard(m_lock);

    m_tasks.push_back(std::move(task));
}

//---------------------------------------------------------------------------//

inline tapetums::Task tapetums::TaskWorker::QueryTask()
{
    CS::LockGuard guard(m_lock);

    if ( m_tasks.empty() )
    {
        return task_null;
    }

    // キューの先頭から取得
    auto task = m_tasks.front();
    m_tasks.pop_front();

    return task;
}

//---------------------------------------------------------------------------//

inline tapetums::Task tapetums::TaskWorker::StealTask()
{
    CS::LockGuard guard(m_lock);

    if ( m_tasks.empty() )
    {
        return task_null;
    }

    // キューの末尾から取得
    auto task = m_tasks.back();
    m_tasks.pop_back();

    return task;
}

//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::Start()
{
    if ( is_running() )
    {
        if ( is_paused() )
        {
            Resume();
        }

        return;
    }

    m_working = true;

    m_thread = std::thread([this]()
    {
        m_thread_id = ::GetCurrentThreadId();

        MainLoop();

        m_thread_id = 0;
    });

    while ( ! is_running() ) { ::Sleep(0); }
}

//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::Stop()
{
    if ( ! is_running() ) { return; }

    ::PostThreadMessage(m_thread_id, WM_QUIT, 0, 0);

    if ( m_thread.joinable() )
    {
        m_thread.join();
    }

    m_tasks.clear();
}

//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::Pause() noexcept
{
    m_working = false;
}

//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::Resume() noexcept
{
    m_working = true;

    ::PostThreadMessage(m_thread_id, WM_NULL, 0, 0);
}

//---------------------------------------------------------------------------//
// TaskWorker Inner Methods
//---------------------------------------------------------------------------//

inline void tapetums::TaskWorker::MainLoop()
{
    MSG msg;
    for ( ; ; )
    {
        if ( ::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE) )
        {
            if ( ::GetMessage(&msg, nullptr, 0, 0) <= 0 )
            {
                break;
            }
            ::TranslateMessage(&msg);
            ::DispatchMessage (&msg);
        }
        else if ( m_working )
        {
            // 自分のキューからタスクを取得
            auto task = QueryTask();
            if ( static_cast<bool>(task) )
            {
                task(*this);
                continue;
            }

            // 他人のキューからタスクを取得
            auto others_task = m_pool->QueryTask();
            if ( static_cast<bool>(others_task) )
            {
                others_task(*this);
                continue;
            }
        }
        else
        {
            ::MsgWaitForMultipleObjects
            (
                0, nullptr, FALSE, INFINITE, QS_ALLINPUT
            );
        }
    }
}

//---------------------------------------------------------------------------//

// Task.hpp