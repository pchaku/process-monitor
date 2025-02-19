#ifndef MAC_APPLICATION_SCANNER_HPP
#define MAC_APPLICATION_SCANNER_HPP

#include "application-scanner.hpp"

struct ScannedApp
{
  std::string procedureName;
  std::string windowTitle;
  std::string procedurePath;
};

class MacApplicationScanner : PlatformNativeApplicationScanner
{
public:
  MacApplicationScanner() {}
  static MacApplicationScanner &instance();
  static void temp();
  void ListenForApplications(Napi::Env &env, Napi::ThreadSafeFunction &&callback) final;
  void StopListener() final;
};

#endif // MAC_APPLICATION_SCANNER_HPP