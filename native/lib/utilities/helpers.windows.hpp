#ifndef WINDOWS_HELPERS_HPP
#define WINDOWS_HELPERS_HPP

#include "helpers.hpp"

#include <Windows.h>

#include <string>

class Window
{
public:
  Window() = delete;
  explicit Window(HWND hwnd) : 
    _hwnd(hwnd), _title(Window::GetTitle(hwnd)), _class_name(Window::GetClassName(hwnd))
  {}
  // getters
  const HWND Hwnd() const noexcept { return _hwnd; }
  const std::string &Title() const noexcept { return _title; }
  const std::string &ClassName() const noexcept { return _class_name; }
  ScannedApplication IntoScannedApplication() const;
  // static accessory methods
  static std::string GetTitle(HWND hwnd);
  static std::string GetClassName(HWND hwnd);
  static std::pair<std::string, std::string> GetProcInfo(const HWND hwnd);
  // operators
  bool operator==(const Window &other) const noexcept
  {
    return _hwnd == other._hwnd;
  }
private:
  HWND _hwnd;
  std::string _title;
  std::string _class_name;
};

bool __stdcall IsSelfOwnedWindow(
  HWND hwnd,
  LONG idObject,
  LONG idChild
);

typedef HRESULT(WINAPI *DwmGetWindowAttributeFunc)(HWND, DWORD, PVOID, DWORD);
extern DwmGetWindowAttributeFunc g_DwmGetWindowAttribute;

bool IsAltTabWindow(Window const &window);

#endif // WINDOWS_HELPERS_HPP