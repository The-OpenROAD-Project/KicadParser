#ifndef KICAD_PCB_POINT_H
#define KICAD_PCB_POINT_H

#include <math.h>
#include <iostream>
#include <tuple>
#include <vector>

//public math structures
using namespace std;

template <class T>
class Point_2D {
   public:
    Point_2D() {}
    Point_2D(const Point_2D &p) : m_x(p.m_x), m_y(p.m_y) {}
    Point_2D(T x, T y) : m_x(x), m_y(y) {}
    ~Point_2D() {}
    bool operator==(const Point_2D &p) const {
        return std::tie(m_x, m_y) == std::tie(p.m_x, p.m_y);
    }
    bool operator!=(const Point_2D &p) const {
        return std::tie(m_x, m_y) != std::tie(p.m_x, p.m_y);
    }
    bool operator<(const Point_2D &p) const {
        return std::tie(m_x, m_y) < std::tie(p.m_x, p.m_y);
    }
    void operator=(const Point_2D &p) {
        m_x = p.m_x;
        m_y = p.m_y;
    }
    const Point_2D &operator+=(const Point_2D &p) {
        m_x += p.m_x;
        m_y += p.m_y;
        return *this;
    }
    Point_2D operator-(const Point_2D &p) const {
        return Point_2D(m_x - p.m_x, m_y - p.m_y);
    }
    Point_2D operator+(const Point_2D &p) const {
        return Point_2D(m_x + p.m_x, m_y + p.m_y);
    }
    static T getDistance(const Point_2D &a, const Point_2D &b) {
        return sqrt(pow(a.m_x - b.m_x, 2.0) + pow(a.m_y - b.m_x, 2.0));
    }
    T x() const { return m_x; }
    T y() const { return m_y; }

    T m_x;
    T m_y;
};

using point_2d = Point_2D<double>;
using points_2d = std::vector<Point_2D<double>>;

template <class T>
inline ostream &operator<<(ostream &out, const Point_2D<T> &p) {
    out << "(" << p.x() << ", " << p.y() << ")";
    return out;
}

template <class T>
class Point_3D {
   public:
    Point_3D() {}
    Point_3D(T x, T y, T z) : m_x(x), m_y(y), m_z(z) {}
    bool operator==(const Point_3D &p) const {
        return std::tie(m_x, m_y, m_z) == std::tie(p.m_x, p.m_y, p.m_z);
    }
    bool operator!=(const Point_3D &p) const {
        return std::tie(m_x, m_y, m_z) != std::tie(p.m_x, p.m_y, p.m_z);
    }
    bool operator<(const Point_3D &p) const {
        return std::tie(m_x, m_y, m_z) < std::tie(p.m_x, p.m_y, p.m_z);
    }
    void operator=(const Point_3D &p) {
        m_x = p.m_x;
        m_y = p.m_y;
        m_z = p.m_z;
    }
    Point_3D operator-(const Point_3D &p) const {
        return Point_3D(m_x - p.m_x, m_y - p.m_y, m_z - p.m_z);
    }
    Point_3D operator+(const Point_3D &p) const {
        return Point_3D(m_x + p.m_x, m_y + p.m_y, m_z + p.m_z);
    }
    const Point_3D &operator+=(const Point_3D &p) {
        m_x += p.m_x;
        m_y += p.m_y;
        m_z += p.m_z;
        return *this;
    }
    static double getDistance2D(const Point_3D &a, const Point_3D &b) {
        return sqrt(pow(a.m_x - b.m_x, 2.0) + pow(a.m_y - b.m_x, 2.0));
    }
    T x() const { return m_x; }
    T y() const { return m_y; }
    T z() const { return m_z; }

    T m_x;
    T m_y;
    T m_z;
};

template <class T>
inline ostream &operator<<(ostream &out, const Point_3D<T> &p) {
    out << "(" << p.x() << ", " << p.y() << ", " << p.z() << ")";
    return out;
}

using point_3d = Point_3D<double>;
using points_3d = std::vector<Point_3D<double>>;
//using Location = Point_3D<int>;

#endif
