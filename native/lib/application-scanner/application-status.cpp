#include "application-status.hpp"

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#include "application-scanner.mac.hpp"
#endif
#endif

#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <napi.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <Carbon/Carbon.h>
#include <iostream>

WindowInfo GetWindowInfo(const std::string &applicationName, const std::string &windowTitle)
{
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly | kCGWindowListOptionIncludingWindow, kCGNullWindowID);
    CFIndex windowCount = CFArrayGetCount(windowList);

    std::cout << "Looking for Application: " << applicationName << ", Window Title: " << windowTitle << std::endl;

    for (CFIndex i = 0; i < windowCount; ++i)
    {
        CFDictionaryRef windowInfo = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowList, i));
        CFStringRef ownerName = static_cast<CFStringRef>(CFDictionaryGetValue(windowInfo, kCGWindowOwnerName));
        CFStringRef windowName = static_cast<CFStringRef>(CFDictionaryGetValue(windowInfo, kCGWindowName));
        int pid = 0;
        CFNumberRef pidRef = static_cast<CFNumberRef>(CFDictionaryGetValue(windowInfo, kCGWindowOwnerPID));
        if (pidRef)
        {
            CFNumberGetValue(pidRef, kCFNumberIntType, &pid);
        }

        char ownerNameBuffer[256];
        char windowNameBuffer[256];

        std::string windowNameStr;

        if (windowName)
        {
            if (CFStringGetCString(windowName, windowNameBuffer, sizeof(windowNameBuffer), kCFStringEncodingUTF8))
            {
                windowNameStr = windowNameBuffer;
            }
            std::cout << "Window Title: " << windowNameStr << std::endl;
        }
        // else
        // {
        //     windowNameStr = "";
        //     std::cout << "Window Title: NULL" << std::endl;
        // }

        if (ownerName && CFStringGetCString(ownerName, ownerNameBuffer, sizeof(ownerNameBuffer), kCFStringEncodingUTF8))
        {
            if (applicationName == ownerNameBuffer && windowTitle == windowNameStr)
            {
                std::cout << "MATCH FOUND!" << std::endl;

                WindowInfo info;
                info.valid = true;
                CGRect bounds;
                CFDictionaryRef boundsDict = static_cast<CFDictionaryRef>(CFDictionaryGetValue(windowInfo, kCGWindowBounds));
                if (boundsDict)
                {
                    CGRectMakeWithDictionaryRepresentation(boundsDict, &bounds);
                    info.x = bounds.origin.x;
                    info.y = bounds.origin.y;
                    info.width = bounds.size.width;
                    info.height = bounds.size.height;

                    std::cout << "Bounds: x=" << info.x << ", y=" << info.y
                              << ", width=" << info.width << ", height=" << info.height << std::endl;
                }

                AXUIElementRef appElement = AXUIElementCreateApplication(pid);
                CFArrayRef windowArray;
                if (AXUIElementCopyAttributeValue(appElement, kAXWindowsAttribute, (CFTypeRef *)&windowArray) == kAXErrorSuccess)
                {
                    for (CFIndex j = 0; j < CFArrayGetCount(windowArray); ++j)
                    {
                        AXUIElementRef windowElement = (AXUIElementRef)CFArrayGetValueAtIndex(windowArray, j);
                        CFStringRef windowTitleValue;
                        if (AXUIElementCopyAttributeValue(windowElement, kAXTitleAttribute, (CFTypeRef *)&windowTitleValue) == kAXErrorSuccess)
                        {
                            char axTitleBuffer[256];
                            if (windowTitleValue && CFStringGetCString(windowTitleValue, axTitleBuffer, sizeof(axTitleBuffer), kCFStringEncodingUTF8))
                            {
                                if (windowTitle == axTitleBuffer)
                                {
                                    CFTypeRef minimizedValue;
                                    if (AXUIElementCopyAttributeValue(windowElement, kAXMinimizedAttribute, &minimizedValue) == kAXErrorSuccess)
                                    {
                                        info.visible = !CFBooleanGetValue((CFBooleanRef)minimizedValue);
                                        CFRelease(minimizedValue);
                                    }
                                    CFRelease(windowTitleValue);
                                    CFRelease(windowArray);
                                    CFRelease(appElement);
                                    CFRelease(windowList);
                                    return info;
                                }
                            }
                            CFRelease(windowTitleValue);
                        }
                    }
                    CFRelease(windowArray);
                }
                CFRelease(appElement);
            }
        }
    }

    CFRelease(windowList);
    return WindowInfo{};
}

NativeApplicationStatus::NativeApplicationStatus(Napi::Env env, Napi::Object exports)
{
    DefineAddon(
        exports,
        {InstanceMethod("ListenForStatus", &NativeApplicationStatus::ListenForStatus, napi_enumerable),
         InstanceMethod("StopListening", &NativeApplicationStatus::StopListening, napi_enumerable)});
}

void NativeApplicationStatus::ListenForStatus(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    if (info.Length() != 2 || !info[0].IsObject() || !info[1].IsFunction())
    {
        Napi::TypeError::New(env, "Expected 2 arguments: { applicationName: string, windowTitle: string } and callback function").ThrowAsJavaScriptException();
        return;
    }

    auto options = info[0].As<Napi::Object>();
    if (!options.Has("applicationName") || !options.Has("windowTitle") ||
        !options.Get("applicationName").IsString() || !options.Get("windowTitle").IsString())
    {
        Napi::TypeError::New(env, "Expected object with 'applicationName' and 'windowTitle' as strings").ThrowAsJavaScriptException();
        return;
    }

    _applicationName = options.Get("applicationName").As<Napi::String>().Utf8Value();
    _windowTitle = options.Get("windowTitle").As<Napi::String>().Utf8Value();
    auto callback_fn = info[1].As<Napi::Function>();
    _callback = Napi::ThreadSafeFunction::New(env, callback_fn, "ApplicationStatusCallback", 0, 1);

    _running = true;
    _threadStatus = std::thread([this]()
                                {
        while (_running) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            WindowInfo windowInfo = GetWindowInfo(_applicationName, _windowTitle);

            _callback.BlockingCall([windowInfo](Napi::Env env, Napi::Function jsCallback) {
                Napi::Object result = Napi::Object::New(env);

                if (!windowInfo.valid) {
                    result.Set("status", "closed");
                } else if (!windowInfo.visible) {
                    result.Set("status", "hidden");
                } else {
                    result.Set("status", "open");
                    Napi::Object dimensions = Napi::Object::New(env);
                    dimensions.Set("x", windowInfo.x);
                    dimensions.Set("y", windowInfo.y);
                    dimensions.Set("width", windowInfo.width);
                    dimensions.Set("height", windowInfo.height);
                    result.Set("dimensions", dimensions);
                }
                jsCallback.Call({result});
            });
        } });
}

void NativeApplicationStatus::StopListening(const Napi::CallbackInfo &info)
{
    _running = false;
    if (_threadStatus.joinable())
    {
        _threadStatus.join();
    }
    _callback.Release();
}

NODE_API_ADDON(NativeApplicationStatus)
