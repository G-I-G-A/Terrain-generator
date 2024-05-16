#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <cmath>
#include <array>
#include <type_traits>

template<typename T>
struct Point2d
{
    static constexpr int ndim = 2;

    Point2d(const T& x_ = 0, const T& y_ = 0)
        : x(x_)
        , y(y_)
    {}

    Point2d(const Point2d& pt)
        : x(pt.x)
        , y(pt.y)
    {}

    T x;
    T y;
};

template<typename T>
struct Point3d
{
    static constexpr int ndim = 3;

    Point3d(const T& x_ = 0, const T& y_ = 0, const T& z_ = 0)
        : x(x_)
        , y(y_)
        , z(z_)
    {}

    Point3d(const Point3d& pt)
        : x(pt.x)
        , y(pt.y)
        , z(pt.z)
    {}

    T x;
    T y;
    T z;
};

template<typename T>
struct Point4d
{
    static constexpr int ndim = 4;

    Point4d(const T& x_ = 0, const T& y_ = 0, const T& z_ = 0, const T& w_ = 0)
        : x(x_)
        , y(y_)
        , z(z_)
        , w(w_)
    {}

    Point4d(const Point4d& pt)
        : x(pt.x)
        , y(pt.y)
        , z(pt.z)
        , w(pt.w)
    {}

    T x;
    T y;
    T z;
    T w;
};

struct AxisX
{
    static constexpr int r1 = 1;
    static constexpr int r2 = 2;
    static constexpr int i1 = 0;
    static constexpr int i2 = 3;
};

struct AxisY
{
    static constexpr int r1 = 0;
    static constexpr int r2 = 2;
    static constexpr int i1 = 1;
    static constexpr int i2 = 3;
};

struct AxisZ
{
    static constexpr int r1 = 0;
    static constexpr int r2 = 1;
    static constexpr int i1 = 2;
    static constexpr int i2 = 3;
};

template<typename... Ts>
struct typelist{};

template<typename T, typename TypelistType>
struct contains
{};

template<typename T, typename... Ts>
struct contains<T, typelist<Ts...> >
{
    static constexpr bool value = ( std::is_same_v<T, Ts> || ...);
};

template<typename T, typename... Ts>
constexpr bool contains_v = contains<T, Ts...>::value;

using AllAxis = typelist<AxisX, AxisY, AxisZ>;

template<typename T>
class Mat4
{
public:
    Mat4()
    {
        m_coefs.fill(0);
    }

    T& operator()(int row, int col) { return m_coefs[row + col * 4]; }
    const T& operator()(int row, int col) const { return m_coefs[row + col * 4]; }

    const T* data() const { return m_coefs.data(); }

    static Mat4<T> identity()
    {
        Mat4<T> m;
        m(0, 0) = m(1, 1) = m(2, 2) = m(3, 3) = 1.;
        return m;
    }

    static Mat4<T> translation(const Point3d<T>& t)
    {
        Mat4<T> m = identity();
        m(0, 3) = t.x;
        m(1, 3) = t.y;
        m(2, 3) = t.z;
        return m;
    }

    template<typename Axis>
    static Mat4<T> rotationAxisAligned(const T& angle)
    {
        static_assert(contains_v<Axis, AllAxis>, "Attention coquinou tu n'as pas utilise un AxisMachin");
        Mat4<T> m = identity();
        m(Axis::r1, Axis::r1) = std::cos(angle);
        m(Axis::r1, Axis::r2) = -std::sin(angle);
        m(Axis::r2, Axis::r1) = std::sin(angle);
        m(Axis::r2, Axis::r2) = std::cos(angle);
        return m;
    }

    static Mat4<T> rotationX(const T& angle)
    {
        return rotationAxisAligned<AxisX>(angle);
    }

    static Mat4<T> rotationY(const T& angle)
    {
        return rotationAxisAligned<AxisY>(angle);
    }

    static Mat4<T> rotationZ(const T& angle)
    {
        return rotationAxisAligned<AxisZ>(angle);
    }

    static Mat4<T> projection(const T& aspect, const T& fov, const T& nearPlane, const T& farPlane)
    {
        Mat4<T> P = identity();
        P(0, 0) = 1.f / (aspect * std::tan(fov / 2.f));
        P(1, 1) = 1.f / (std::tan(fov / 2.f));
        P(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
        P(2, 3) = -(2.f * farPlane * nearPlane) / (farPlane - nearPlane);
        P(3, 2) = -1.f;
        return P;
    }

private:
    std::array<T, 16> m_coefs;
};

template<typename T>
Mat4<T> operator*(const Mat4<T>& op1, const Mat4<T>& op2)
{
    Mat4<T> result;
    for(int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result(row, col) = 0;
            for (int k = 0; k < 4; ++k)
                result(row, col) += op1(row, k) * op2(k, col);
        }
    }

    return result;
}

#endif MATHHELPER_H