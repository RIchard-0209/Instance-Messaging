#pragma once

#include <QString>

namespace AppResources {
constexpr const char* ImageRoot = ":/src/img/";
constexpr const char* SearchIcon = "search.png";
constexpr const char* CloseSearchIcon = "close_search.png";
constexpr const char* CloseTransparentIcon = "close_transparent.png";

inline QString getSearchIconPath() { return QString(ImageRoot) + SearchIcon; }
inline QString getCloseSearchIconPath() {
  return QString(ImageRoot) + CloseSearchIcon;
}
inline QString getCloseTransparentIconPath() {
  return QString(ImageRoot) + CloseTransparentIcon;
}
}  // namespace AppResources
