
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

            std::vector<std::string> applications;

            for (CFIndex i = 0; i < windowCount; ++i)
            {
                CFDictionaryRef windowInfo = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(windowList, i));
                //CFShow(windowInfo);
                CFStringRef ownerName = static_cast<CFStringRef>(CFDictionaryGetValue(windowInfo, kCGWindowOwnerName));

                //debugging
                // std::cout << "Owner Name: " << std::endl;
                // CFShow(ownerName);

                if (ownerName)
                {
                    char ownerNameBuffer[256];
                    if (CFStringGetCString(ownerName, ownerNameBuffer, sizeof(ownerNameBuffer), kCFStringEncodingUTF8))
                    {
                        std::string appName(ownerNameBuffer);
              
                        if (std::find(applications.begin(), applications.end(), appName) == applications.end())
                        {
                            applications.push_back(appName);
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
                  appObject.Set("procedureName", applications[i]);
                  appObject.Set("windowTitle", applications[i]); // Use app name as placeholder title
                  appObject.Set("procedurePath", ""); // Empty for now
          
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