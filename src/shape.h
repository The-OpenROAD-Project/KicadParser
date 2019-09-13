#ifndef KICAD_PCB_SHAPE_H
#define KICAD_PCB_SHAPE_H

#include <cmath>
#include <math.h>
#include <string>
#include <iostream>
#include "mymath.h"

enum class padShape
{
    RECT,
    ROUNDRECT,
    CIRCLE,
    OVAL,
    TRAPEZOID
};

struct line
{
    point_2d m_start;
    point_2d m_end;
    double m_width; //2*radius
    int m_layer;
};

struct circle
{
    point_2d m_center;
    point_2d m_end;
    double m_width; //2*radius
    int m_layer;
};

struct poly
{
    points_2d m_shape;
    double m_width;
    int m_layer;
};

struct arc
{
    point_2d m_start;
    point_2d m_end;
    double m_angle;
    double m_width;
    int m_layer;
};

points_2d shape_to_cords(const points_2d &shape, double a1, double a2);
points_2d roundrect_to_cords(const point_2d &size, const double &ratio);
points_2d shape_to_cords(const point_2d &size, point_2d &pos, padShape shape, double a1, double a2, const double &ratio);

#endif