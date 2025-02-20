
#include "application-scanner.mac.hpp"
#include <iostream>

#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <thread>
#include <chrono>

void MacApplicationScanner::ListenForApplications(Napi::Env &env, Napi::ThreadSafeFunction &&callback)
{
  // TODO: platform specific initialization
  PlatformNativeApplicationScanner::ListenForApplications(env, std::move(callback));

  std::thread([callback = std::move(callback)]() mutable
              {
    while (true)
    {
        CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
        CFIndex windowCount = CFArrayGetCount(windowList);

        std::vector<ScannedApp> applications;

        for (CFIndex i = 0; i < windowCount; ++i)
        {
            CFDictionaryRef windowInfo = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowList, i));
            //CFShow(windowInfo);
            CFStringRef ownerName = static_cast<CFStringRef>(CFDictionaryGetValue(windowInfo, kCGWindowOwnerName));
            CFStringRef windowName = static_cast<CFStringRef>(CFDictionaryGetValue(windowInfo, kCGWindowName));

            //debugging
            // std::cout << "Owner Name: " << std::endl;
            // CFShow(ownerName);

            if (ownerName)
            {
                char ownerNameBuffer[256];
                if (CFStringGetCString(ownerName, ownerNameBuffer, sizeof(ownerNameBuffer), kCFStringEncodingUTF8))
                {
                  //convert ownerName into a string
                    std::string ownerNameStr(ownerNameBuffer);

                    // assign windowName if valid, or empty string if not
                    std::string windowNameStr;
                    if (windowName) {
                      char windowNameBuffer[256];
                      if (CFStringGetCString(windowName, windowNameBuffer, sizeof(windowNameBuffer), kCFStringEncodingUTF8)) {
                        windowNameStr = windowNameBuffer;
                      }
                    } else {
                      windowNameStr = "";
                    }
          
                    auto it = std::find_if(applications.begin(), applications.end(),
                                          [&ownerNameStr, &windowNameStr](const ScannedApp &app)
                                          { return app.procedureName == ownerNameStr && app.windowTitle == windowNameStr; });

                        if (it == applications.end())
                        {
                            applications.push_back({ownerNameStr, windowNameStr, ""});
                        }
                }
            }
        }

        CFRelease(windowList); 

        callback.NonBlockingCall([applications](Napi::Env env, Napi::Function jsCallback) {
          Napi::Array result = Napi::Array::New(env, applications.size());
          for (size_t i = 0; i < applications.size(); ++i)
          {
              Napi::Object appObject = Napi::Object::New(env);
              appObject.Set("procedureName", applications[i].procedureName);
              appObject.Set("windowTitle", applications[i].windowTitle); // Use app name as placeholder title
              appObject.Set("procedurePath", applications[i].procedurePath); // Empty for now
      
              result.Set(i, appObject);
          }
          jsCallback.Call({result});
      });
          

        std::this_thread::sleep_for(std::chrono::seconds(3));
    } })
      .detach();
}

MacApplicationScanner &MacApplicationScanner::instance()
{
  static MacApplicationScanner instance;
  return instance;
}

void MacApplicationScanner::StopListener()
{
  // TODO: platform specific teardown
  PlatformNativeApplicationScanner::StopListener();
}

void MacApplicationScanner::temp()
{
  std::cout << "hellooo" << std::endl;
}