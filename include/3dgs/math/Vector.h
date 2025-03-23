#pragma once

#include "pch.h"

#include "3dgs/math/Matrix.hpp"

namespace iiixrlab::math
{
    template<uint8_t ColumnSize, Arithmetic ElementType>
    using Vector = Matrix<1, ColumnSize, ElementType>;

    template<Arithmetic ElementType>
    using Vector2 = Vector<2, ElementType>;
    
    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;
    using Vector2i = Vector2<int32_t>;
    using Vector2u = Vector2<uint32_t>;

    template<Arithmetic ElementType>
    using Vector3 = Vector<3, ElementType>;

    using Vector3f = Vector3<float>;
    using Vector3d = Vector3<double>;
    using Vector3i = Vector3<int32_t>;
    using Vector3u = Vector3<uint32_t>;

    template<Arithmetic ElementType>
    using Vector4 = Vector<4, ElementType>;

    using Vector4f = Vector4<float>;
    using Vector4d = Vector4<double>;
    using Vector4i = Vector4<int32_t>;
    using Vector4u = Vector4<uint32_t>;
}   // namespace iiixrlab::math