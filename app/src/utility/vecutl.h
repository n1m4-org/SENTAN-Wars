#pragma once
#include <type/Vector3.h>
#include <algorithm>
#include <limits>
#include <cmath>

namespace utl::vec
{
    inline Hagine::Vector3 ClampY(
        const Hagine::Vector3& vec,
        float minY = std::numeric_limits<float>::lowest(),
        float maxY = std::numeric_limits<float>::max())
    {
        Hagine::Vector3 result = vec;
        result.y = std::clamp(result.y, minY, maxY);
        return result;
    }

    inline float VectorToAngle(const Hagine::Vector3& vec)
    {
        float angle = std::atan2(vec.x, vec.z);
        return angle;
    }
}