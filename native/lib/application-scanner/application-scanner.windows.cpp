
#include "application-scanner.windows.hpp"

#include "../utilities/helpers.hpp"

#include <vector>

void WindowsApplicationScanner::ListenForApplications(Napi::Env &env, Napi::ThreadSafeFunction &&callback) {
  PlatformNativeApplicationScanner::ListenForApplications(env, std::move(callback));
  // call run scan on startup to avoid race condition with hook thread usage
  runScan();
  StartHookThread();
}

void WindowsApplicationScanner::StopListener() {
	StopHookThread();
  _windows = {};
	PlatformNativeApplicationScanner::StopListener();
}

/*
 * Hook thread startup / teardown
 */

void WindowsApplicationScanner::setupHooks()
{
  _windows_callback_hook = SetWinEventHook(
    EVENT_OBJECT_CREATE, EVENT_OBJECT_DESTROY, NULL, &EnqueueWindowEvent,
    0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
  );
};

void WindowsApplicationScanner::teardownHooks()
{
  if (_windows_callback_hook)
  {
    UnhookWinEvent(_windows_callback_hook);
    _windows_callback_hook = NULL;
  }
}

void WindowsApplicationScanner::EnqueueWindowEvent(
  HWINEVENTHOOK hWinEventHook,
  DWORD event,
  HWND hwnd,
  LONG idObject,
  LONG idChild,
  DWORD idEventThread,
  DWORD dwmsEventTime
) {
  auto &scanner = WindowsApplicationScanner::instance();
  if (IsSelfOwnedWindow(hwnd, idObject, idChild))
  {
    scanner.runScan();
  }
}

static BOOL enumWindowCallback(HWND hwnd, LPARAM lparam)
{
  auto window = Window(hwnd);
  if (!IsAltTabWindow(window))
  {
    return true;
  }
  auto &windows = *reinterpret_cast<WindowMap *>(lparam);
  windows.insert({hwnd, std::move(window)});
  return true;
}

void WindowsApplicationScanner::runScan()
{
  WindowMap windows = {};
  EnumDesktopWindows(NULL, enumWindowCallback, (LPARAM)&windows);
  if (windows == _windows) {
    return;
  }
  _windows = windows;
  std::vector<ScannedApplication> context;
  context.reserve(_windows.size());
  for (const auto &[_, window] : _windows)
  {
    // a little more expensive call; doing it here because we know we're about to send it out
    context.push_back(window.IntoScannedApplication());
  }
  // todo: maybe raise this up to platform level
  _callback.BlockingCall(
    [ctx = std::move(context)](Napi::Env env, Napi::Function jsCallback)
    {
      auto windows_out = Napi::Array::New(env, ctx.size());
      auto title_prop = Napi::String::New(env, "windowTitle");
      auto proc_path_prop = Napi::String::New(env, "procedurePath");
      auto proc_prop = Napi::String::New(env, "procedureName");
      int i = 0;
      for (const auto &info : ctx)
      {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set(title_prop, info.window_title);
        obj.Set(proc_path_prop, info.procedure_path);
        obj.Set(proc_prop, info.procedure_name);
        windows_out.Set(i++, obj);
      }
      jsCallback.Call({windows_out});
    }
  );
}