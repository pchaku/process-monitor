
#include "application-scanner.hpp"

#if defined(_WIN32)
#include "application-scanner.windows.hpp"
#elif __APPLE__
#include <TargetConditionals.h>
#ifdef TARGET_OS_MAC
#include "application-scanner.mac.hpp"
#endif
#endif

#include <iostream>

NativeApplicationScanner::NativeApplicationScanner(Napi::Env env, Napi::Object exports)
{
  /* Advertise to node the functions this module supports */
  DefineAddon(
      exports,
      {
          InstanceMethod("ListenForApplications", &NativeApplicationScanner::ListenForApplications, napi_enumerable),
          InstanceMethod("StopListener", &NativeApplicationScanner::StopListener, napi_enumerable),
      });
}

void NativeApplicationScanner::ListenForApplications(const Napi::CallbackInfo &info)
{
  std::cout << "C++ version: " << __cplusplus << std::endl;

  /*
   * Parse the inputs; we don't really ensure concrete types, just the type shape
   *  throw if anything looks wrong
   */
  auto env = info.Env();
  auto length = info.Length();
  if (length != 1 || !info[0].IsFunction())
  {
    Napi::TypeError::New(env, "Expected 1 argument of callbackFn").ThrowAsJavaScriptException();
    return;
  }
  auto callback_fn = info[0].As<Napi::Function>();
  if (!callback_fn)
  {
    Napi::TypeError::New(env, "Failed to retrieve pointer of callbackFn")
        .ThrowAsJavaScriptException();
    return;
  }
  auto threadsafe_callback_fn = Napi::ThreadSafeFunction::New(
      env, callback_fn, "ContextWatcherCallback", 0, 1);
  if (!threadsafe_callback_fn)
  {
    Napi::TypeError::New(env, "Failed to cast callbackFn to threadsafe")
        .ThrowAsJavaScriptException();
    return;
  }

  /*
   * Invoke platform specific application scanner; throw if one is not available
   */

#if defined(_WIN32)
  WindowsApplicationScanner::instance().ListenForApplications(env, std::move(threadsafe_callback_fn));
#elif defined(TARGET_OS_MAC)
  // MacApplicationScanner::temp();
  MacApplicationScanner::instance().ListenForApplications(env, std::move(threadsafe_callback_fn));
#else
  Napi::TypeError::New(env, "No PlatformNativeApplicationScanner available for this platform")
      .ThrowAsJavaScriptException();
#endif

  Napi::Function consoleLog = env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>();
  consoleLog.Call({Napi::String::New(env, "Successfully started NativeApplicationScanner")});
}

void NativeApplicationScanner::StopListener(const Napi::CallbackInfo &info)
{

  auto env = info.Env();

  /*
   * Invoke platform specific application scanner; don't throw even if ones not available as the function
   *  would be idempotent
   */

#if defined(_WIN32)
  WindowsApplicationScanner::instance().StopListener();
#elif defined(TARGET_OS_MAC)
  // MacApplicationScanner::temp();
  MacApplicationScanner::instance().StopListener();
#endif

  Napi::Function consoleLog = env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>();
  consoleLog.Call({Napi::String::New(env, "Successfully stopped NativeApplicationScanner")});
}

NODE_API_ADDON(NativeApplicationScanner)
