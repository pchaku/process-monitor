#ifndef WINDOWS_CLASSES_HPP
#define WINDOWS_CLASSES_HPP

#include <thread>
#include <memory>
#include <atomic>
#include <iostream>
#include <Windows.h>

/*
 * HookThread runs a WindowsOS mailbox pattern in a std::thread
 */

class HookThread
{
protected:
  
  void StartHookThread()
  {
    if (!_thread_stop)
    {
      // its up to the caller to check for errors here
      return;
    }
    _thread_stop = false;
    _hook_thread = std::make_unique<std::thread>(&HookThread::processHooks, this);
  }

  void StopHookThread()
  {
    if (!_thread_stop)
    {
      _thread_stop = true;
      if (_hook_thread)
      {
        if (_hook_thread->joinable())
        {
          // instead of making our own CV thread stop mechanism, just use native
          //  windows messaging
          auto thread_id = GetThreadId(_hook_thread->native_handle());
          PostThreadMessage(thread_id, WM_QUIT, 0, 0);
          _hook_thread->join();
        }
        _hook_thread.reset();
      }
    }
  }

  ~HookThread()
  {
    StopHookThread();
  }

  // allow the implementer to read _thread_stop state
  std::atomic<bool> _thread_stop = {true};

private:
  virtual void setupHooks() = 0;
  virtual void teardownHooks() = 0;

  void processHooks()
  {
    setupHooks();
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    teardownHooks();
  }

  std::unique_ptr<std::thread> _hook_thread;
};

#endif // WINDOWS_CLASSES_HPP