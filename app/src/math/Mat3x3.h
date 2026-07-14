#pragma once

/// 前方宣言
namespace Hagine
{
    struct Vector2;
}

namespace Ni
{
    /// <summary>
    /// 3x3行列
    /// </summary>
    class Matrix3x3 final
    {
    public:
        Matrix3x3() = default;
        ~Matrix3x3() = default;

        float m[3][3];

        /// ====================
        /// Minus Sign
        /// ====================

        Matrix3x3 operator-() const;

        /// =====================
        /// Make Matrix
        /// =====================

        static Matrix3x3 Identity();
        static Matrix3x3 RotateMatrix(float radian);
        static Matrix3x3 ScaleMatrix(const Hagine::Vector2& scale);
        static Matrix3x3 TranslateMatrix(const Hagine::Vector2& translate);

        /// ====================
        /// Operator
        /// ====================

        Matrix3x3& operator +=(const Matrix3x3& rm);
        Matrix3x3& operator -=(const Matrix3x3& rm);
        Matrix3x3& operator *=(const Matrix3x3& rm);

        Matrix3x3 operator +(const Matrix3x3& rm) const;
        Matrix3x3 operator -(const Matrix3x3& rm) const;
        Matrix3x3 operator *(const Matrix3x3& rm) const;

        Matrix3x3& operator *=(float f);
        Matrix3x3 operator *(float f) const;
    };

    Hagine::Vector2 Transform(const Hagine::Vector2& v, const Matrix3x3& m);
}