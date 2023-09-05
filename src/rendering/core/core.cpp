#include "rendering/core.h"

#include <stdexcept>
#include <codecvt>
#include <locale>

using namespace rendering;

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1

CharString tools::CStringHelper::unicode_to_utf8(const String &str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
}

#undef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
