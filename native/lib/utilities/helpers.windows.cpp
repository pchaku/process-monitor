
#include "helpers.windows.hpp"

#include <dwmapi.h>

#include <array>

std::string Window::GetTitle(const HWND hwnd)
{
  std::array<char, 1024> title;
  ::GetWindowTextA(hwnd, title.data(), (int)title.size());
  return std::string(title.data());
}

std::string Window::GetClassName(const HWND hwnd)
{
  std::array<char, 1024> class_name;
  ::GetClassNameA(hwnd, class_name.data(), (int)class_name.size());
  return std::string(class_name.data());
}

std::pair<std::string, std::string> Window::GetProcInfo(const HWND hwnd)
{
  std::array<char, 1024> proc_name_arr;
  DWORD dwProcId = 0;
  GetWindowThreadProcessId(hwnd, &dwProcId);
  HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcId);
  DWORD size = 1024;
  QueryFullProcessImageNameA((HMODULE)hProc, 0, proc_name_arr.data(), &size);
  CloseHandle(hProc);
  auto proc_name_full = std::string(proc_name_arr.data());
  return {
    RemoveExtension(BaseName(proc_name_full)),
    proc_name_full
  };
}

ScannedApplication Window::IntoScannedApplication() const {
  const auto proc_info = Window::GetProcInfo(_hwnd);
  return ScannedApplication {
    Title(),
    proc_info.first,
    proc_info.second
  };
}

bool __stdcall IsSelfOwnedWindow(
  HWND hwnd,
  LONG idObject,
  LONG idChild
) {
  return IsWindow(hwnd) && idObject == OBJID_WINDOW && idChild == CHILDID_SELF;
}

DwmGetWindowAttributeFunc g_DwmGetWindowAttribute = DwmGetWindowAttribute;

bool IsAltTabWindow(Window const &window)
{
  // https://github.com/microsoft/Windows.UI.Composition-Win32-Samples/blob/a59e7586c0bd1a967e1e25f6ca0363e20151afe5/cpp/ScreenCaptureforHWND/ScreenCaptureforHWND/Win32WindowEnumeration.h#L45

  auto hwnd = window.Hwnd();
  HWND shellWindow = GetShellWindow();

  auto &title = window.Title();
  auto &className = window.ClassName();

  if (hwnd == shellWindow)
  {
    return false;
  }

  if (title.length() == 0)
  {
    return false;
  }

  if (!IsWindowVisible(hwnd))
  {
    return false;
  }

  if (GetAncestor(hwnd, GA_ROOT) != hwnd)
  {
    return false;
  }

  LONG style = GetWindowLong(hwnd, GWL_STYLE);
  if (!((style & WS_DISABLED) != WS_DISABLED))
  {
    return false;
  }

  DWORD cloaked = FALSE;
  HRESULT hrTemp = g_DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
  if (SUCCEEDED(hrTemp) &&
      cloaked == DWM_CLOAKED_SHELL)
  {
    return false;
  }

  return true;
}

