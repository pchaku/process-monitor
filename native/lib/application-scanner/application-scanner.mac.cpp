
#include "application-scanner.mac.hpp"
#include <iostream>

void MacApplicationScanner::ListenForApplications(Napi::Env &env, Napi::ThreadSafeFunction &&callback) {
  PlatformNativeApplicationScanner::ListenForApplications(env, std::move(callback));
	// TODO: platform specific initialization
}

void MacApplicationScanner::StopListener() {
  // TODO: platform specific teardown
  PlatformNativeApplicationScanner::StopListener();
}

void MacApplicationScanner::temp() {
  std::cout << "todo" << std::endl;
}