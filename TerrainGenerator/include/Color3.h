#ifndef COLOR3_H
#define COLOR3_H

template<typename T>
struct Color3
{
    static constexpr int ndim = 3;

    Color3(const T& r_ = 0, const T& g_ = 0, const T& b_ = 0)
        : r(r_)
        , g(g_)
        , b(b_)
    {}

    Color3(const Color3& pt)
        : r(pt.r)
        , g(pt.g)
        , b(pt.b)
    {}

    T r, g, b;
};

#endif COLOR3_H