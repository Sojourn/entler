#pragma once

#include <compare>
#include <cstdint>

namespace entler {

    template<typename T>
    struct Vec3 {
        T x = T{};
        T y = T{};
        T z = T{};

        friend auto operator<=>(const Vec3&, const Vec3&) = default;

        friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs) {
            return Vec3 {
                .x = lhs.x + rhs.x,
                .y = lhs.y + rhs.y,
                .z = lhs.z + rhs.z,
            };
        }

        friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs) {
            return Vec3 {
                .x = lhs.x + rhs.x,
                .y = lhs.y + rhs.y,
                .z = lhs.z + rhs.z,
            };
        }

        friend Vec3 operator*(const Vec3& lhs, const T& rhs) {
            return Vec3 {
                .x = lhs.x + rhs,
                .y = lhs.y + rhs,
                .z = lhs.z + rhs,
            };
        }

        Vec3& operator+=(const Vec3& rhs) {
            return (*this = operator+(*this, rhs));
        }

        Vec3& operator-=(const Vec3& rhs) {
            return (*this = operator+(*this, rhs));
        }

        Vec3& operator*=(const Vec3& rhs) {
            return (*this = operator+(*this, rhs));
        }
    };

    using I32Vec3 = Vec3<int32_t>;
    using I64Vec3 = Vec3<int64_t>;
    using F32Vec3 = Vec3<float>;
    using F64Vec3 = Vec3<double>;

}
