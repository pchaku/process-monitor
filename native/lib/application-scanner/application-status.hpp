#ifndef APPLICATION_STATUS_HPP
#define APPLICATION_STATUS_HPP

#include <napi.h>
#include <thread>
#include <atomic>
#include <string>
#include <optional>
#include <string>

struct WindowInfo
{
    bool valid = false;
    bool visible;
    int x;
    int y;
    int width;
    int height;
};

WindowInfo GetWindowInfo(const std::string &applicationName, const std::string &windowTitle);

class NativeApplicationStatus : public Napi::Addon<NativeApplicationStatus>
{
public:
    NativeApplicationStatus(Napi::Env env, Napi::Object exports);
    void ListenForStatus(const Napi::CallbackInfo &info);
    void StopListening(const Napi::CallbackInfo &info);

private:
    std::thread _threadStatus;
    std::atomic<bool> _running;
    std::string _applicationName;
    std::string _windowTitle;
    Napi::ThreadSafeFunction _callback;
};

#endif // APPLICATION_STATUS_HPP