#ifndef WINDOWS_APPLICATION_SCANNER_HPP
#define WINDOWS_APPLICATION_SCANNER_HPP

#include "application-scanner.hpp"

#include "../utilities/classes.hpp"
#include "../utilities/classes.windows.hpp"
#include "../utilities/helpers.windows.hpp"

#include <map>


typedef std::map<HWND, Window> WindowMap;

/*
 * WindowApplicationScanner is a singleton that starts up a windows mailbox thread;
 *  we use request messages on window open / close, and use that to trigger a refresh
 *  of running applications
 */

class WindowsApplicationScanner:
  public PlatformNativeApplicationScanner,
  public Singleton<WindowsApplicationScanner>,
  public HookThread
{
public:
  WindowsApplicationScanner() {}
  void ListenForApplications(Napi::Env &env, Napi::ThreadSafeFunction &&callback) final;
  void StopListener() final;

  // windows mailbox handler
  static void EnqueueWindowEvent(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime
  );

private:
    void setupHooks() final;
    void teardownHooks() final;
    // application logic scan
    void runScan();
    WindowMap _windows = {};
    HWINEVENTHOOK _windows_callback_hook = NULL;
};


#endif // WINDOWS_APPLICATION_SCANNER_HPP