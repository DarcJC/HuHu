#pragma once

#include <string>
#include <memory>
#include "Eigen/Dense"

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
    template<class T> using TUniquePtr = std::unique_ptr<T>;

    using Vector = Eigen::Vector3d;
    using Vector4d = Eigen::Vector4d;
    using Vector4f = Eigen::Vector4f;
    using Vector3d = Eigen::Vector3d;
    using Vector3f = Eigen::Vector3f;
    using Vector2f = Eigen::Vector2f;
    using Vector2d = Eigen::Vector2f;

}// namespace rendering

namespace rendering::tools {

    struct CStringHelper {
        static CharString unicode_to_utf8(const String& str);
    };

}// namespace rendering::tools
