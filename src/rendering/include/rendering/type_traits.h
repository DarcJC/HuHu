#pragma once

#include <string>
#include <memory>

#define FORCEINLINE inline
#define NODISCARD [[nodiscard]]
#define LIKELY_BRANCH [[likely]]
#define UNLIKELY_BRANCH [[unlikely]]

#ifdef RENDERING_EXPORT
#define RENDERING_API __declspec(dllexport)
#else
#define RENDERING_API __declspec(dllimport)
#endif

namespace rendering {

    using CharString = std::string;
    using String = std::wstring;

    template<class T> using TSharedPtr = std::shared_ptr<T>;

}// namespace rendering

namespace rendering::tools {

    struct CStringHelper {
        static CharString unicode_to_utf8(const String& str);
    };

}// namespace rendering::tools
