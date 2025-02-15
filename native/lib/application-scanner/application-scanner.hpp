#ifndef APPLICATION_SCANNER_HPP
#define APPLICATION_SCANNER_HPP

#include <napi.h>

class PlatformNativeApplicationScanner {
  public:
    virtual ~PlatformNativeApplicationScanner() {
      StopListener();
    }
    virtual void ListenForApplications(Napi::Env &env, Napi::ThreadSafeFunction &&callback) {
      _callback = std::move(callback);
    }
    virtual void StopListener() {
      _callback = NULL;
    }
  protected:
    PlatformNativeApplicationScanner() = default;
    Napi::ThreadSafeFunction _callback = NULL;
};

class NativeApplicationScanner: public Napi::Addon<NativeApplicationScanner> {
  public:
    NativeApplicationScanner(Napi::Env env, Napi::Object exports);
  private:
    void ListenForApplications(const Napi::CallbackInfo& info);
    void StopListener(const Napi::CallbackInfo& info);
};

#endif // APPLICATION_SCANNER_HPP