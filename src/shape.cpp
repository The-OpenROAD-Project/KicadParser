#include "shape.h"

points_2d rotateShapeCoordsByAngles(const points_2d &shape, double instAngle, double padAngle)
{
    auto cords = points_2d{};
    auto rads = fmod((instAngle + padAngle) * -M_PI / 180, 2 * M_PI);
    auto s = sin(rads);
    auto c = cos(rads);
    for (auto &p : shape)
    {
        auto px = double(c * p.m_x - s * p.m_y);
        auto py = double(s * p.m_x + c * p.m_y);
        cords.push_back(point_2d{px, py});
    }
    return cords;
}

points_2d roundrect_to_shape_coords(const point_2d &size, const double &ratio)
{
    auto cords = points_2d{};
    auto width = size.m_x / 2;
    auto height = size.m_y / 2;
    auto radius = std::min(width, height) * ratio * 2;
    auto deltaWidth = width - radius;
    auto deltaHeight = height - radius;
    auto point = point_2d{};
    for (int i = 0; i < 10; ++i)
    { //10 points
        point.m_x = deltaWidth + radius * cos(-9 * i * M_PI / 180);
        point.m_y = -deltaHeight + radius * sin(-9 * i * M_PI / 180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
    }
    for (int i = 10; i < 20; ++i)
    { //10 points
        point.m_x = -deltaWidth + radius * cos(-9 * i * M_PI / 180);
        point.m_y = -deltaHeight + radius * sin(-9 * i * M_PI / 180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
    }
    for (int i = 20; i < 30; ++i)
    { //10 points
        point.m_x = -deltaWidth + radius * cos(-9 * i * M_PI / 180);
        point.m_y = deltaHeight + radius * sin(-9 * i * M_PI / 180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
    }
    for (int i = 30; i < 40; ++i)
    { //10 points
        point.m_x = deltaWidth + radius * cos(-9 * i * M_PI / 180);
        point.m_y = deltaHeight + radius * sin(-9 * i * M_PI / 180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
    }
    point.m_x = deltaWidth + radius * cos(0 * M_PI / 180);
    point.m_y = -deltaHeight + radius * sin(0 * M_PI / 180);
    //std::cout << point.m_x << " " << point.m_y << std::endl;
    cords.push_back(point);
    return cords;
}

// Ongoing work
// TODO: all coords in CW or CCW
// TODO: parameters for #points to Circle
points_2d shape_to_cords(const point_2d &size, point_2d &pos, padShape shape, double a1, double a2, const double &ratio)
{
    auto cords = points_2d{};
    switch (shape)
    {
    case padShape::CIRCLE:
    {
        auto radius = size.m_x / 2;
        auto point = point_2d{};
        for (int i = 0; i < 40; ++i)
        { //40 points
            point.m_x = pos.m_x + radius * cos(-9 * i * M_PI / 180);
            point.m_y = pos.m_y + radius * sin(-9 * i * M_PI / 180);
            cords.push_back(point);
        }
        point.m_x = pos.m_x + radius * cos(0 * M_PI / 180);
        point.m_y = pos.m_y + radius * sin(0 * M_PI / 180);
        cords.push_back(point);
        break;
    }
    case padShape::OVAL:
    {
        auto width = size.m_x / 2;
        auto height = size.m_y / 2;
        auto point = point_2d{};
        for (int i = 0; i < 40; ++i)
        { //40 points
            point.m_x = pos.m_x + width * cos(-9 * i * M_PI / 180);
            point.m_y = pos.m_y + height * sin(-9 * i * M_PI / 180);
            cords.push_back(point);
        }
        point.m_x = pos.m_x + width * cos(0 * M_PI / 180);
        point.m_y = pos.m_y + height * sin(0 * M_PI / 180);
        cords.push_back(point);
        break;
    }
    case padShape::RECT:
    {
        auto width = size.m_x / 2;
        auto height = size.m_y / 2;
        cords.push_back(point_2d{-1 * width, -1 * height});
        cords.push_back(point_2d{width, height});
        break;
    }
    case padShape::ROUNDRECT:
    {
        auto width = size.m_x / 2;
        auto height = size.m_y / 2;
        auto radius = std::min(width, height) * ratio * 2;
        auto deltaWidth = width - radius;
        auto deltaHeight = height - radius;
        auto point = point_2d{};
        for (int i = 0; i < 10; ++i)
        { //10 points
            point.m_x = pos.m_x + deltaWidth + radius * cos(-9 * i * M_PI / 180);
            point.m_y = pos.m_y - deltaHeight + radius * sin(-9 * i * M_PI / 180);
            cords.push_back(point);
        }
        for (int i = 10; i < 20; ++i)
        { //10 points
            point.m_x = pos.m_x - deltaWidth + radius * cos(-9 * i * M_PI / 180);
            point.m_y = pos.m_y - deltaHeight + radius * sin(-9 * i * M_PI / 180);
            cords.push_back(point);
        }
        for (int i = 20; i < 30; ++i)
        { //10 points
            point.m_x = pos.m_x - deltaWidth + radius * cos(-9 * i * M_PI / 180);
            point.m_y = pos.m_y + deltaHeight + radius * sin(-9 * i * M_PI / 180);
            cords.push_back(point);
        }
        for (int i = 30; i < 40; ++i)
        { //10 points
            point.m_x = pos.m_x + deltaWidth + radius * cos(-9 * i * M_PI / 180);
            point.m_y = pos.m_y + deltaHeight + radius * sin(-9 * i * M_PI / 180);
            cords.push_back(point);
        }
        point.m_x = pos.m_x + deltaWidth + radius * cos(0 * M_PI / 180);
        point.m_y = pos.m_y - deltaHeight + radius * sin(0 * M_PI / 180);
        cords.push_back(point);
        break;
    }
    case padShape::TRAPEZOID:
    {
        break;
    }
    default:
    {
        break;
    }
    }

    return rotateShapeCoordsByAngles(cords, a1, a2);
}
