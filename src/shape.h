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

points_2d rotateShapeCoordsByAngles(const points_2d &shape, double instAngle, double padAngle);

// Initialize ROUNDRECT shape_coords
points_2d roundrect_to_shape_coords(const point_2d &size, const double &ratio);

// Ongoing work
points_2d shape_to_cords(const point_2d &size, const point_2d &pos, padShape shape, const double a1, const double a2, const double &ratio);

points_2d segment_to_rect(const points_2d &point, const double &width);

points_2d via_to_circle(const point_2d &pos, const double &size);

#endif
