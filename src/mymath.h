#ifndef KICAD_PCB_MYMATH_H
#define KICAD_PCB_MYMATH_H

#include <vector>
#include <tuple>
#include <math.h>

//public math structures

struct point_2d
{
	point_2d() : m_x(0.0), m_y(0.0) {}
	point_2d(double x, double y) : m_x(x), m_y(y) {}
	bool operator==(const point_2d &p) const
	{
		return std::tie(m_x, m_y) == std::tie(p.m_x, p.m_y);
	}
	bool operator!=(const point_2d &p) const
	{
		return std::tie(m_x, m_y) != std::tie(p.m_x, p.m_y);
	}
	bool operator<(const point_2d &p) const
	{
		return std::tie(m_x, m_y) < std::tie(p.m_x, p.m_y);
	}
	static double getDistance(const point_2d &a, const point_2d &b)
	{
		return sqrt(pow(a.m_x - b.m_x, 2.0) + pow(a.m_y - b.m_x, 2.0));
	}
	double m_x;
	double m_y;
};

typedef std::vector<point_2d> points_2d;

struct point_3d
{
	point_3d() : m_x(0.0), m_y(0.0), m_z(0.0) {}
	point_3d(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}
	bool operator==(const point_3d &p) const
	{
		return std::tie(m_x, m_y, m_z) == std::tie(p.m_x, p.m_y, p.m_z);
	}
	bool operator!=(const point_3d &p) const
	{
		return std::tie(m_x, m_y, m_z) != std::tie(p.m_x, p.m_y, p.m_z);
	}
	bool operator<(const point_3d &p) const
	{
		return std::tie(m_x, m_y, m_z) < std::tie(p.m_x, p.m_y, p.m_z);
	}
	double m_x;
	double m_y;
	double m_z;
};

typedef std::vector<point_3d> points_3d;

#endif
