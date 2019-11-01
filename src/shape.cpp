#include "shape.h"

points_2d rotateShapeCoordsByAngles(const points_2d &shape, double instAngle, double padAngle) {
    auto cords = points_2d{};
    auto rads = fmod((instAngle + padAngle) * -M_PI / 180, 2 * M_PI);
    auto s = sin(rads);
    auto c = cos(rads);
    for (auto &p : shape) {
        auto px = double(c * p.m_x - s * p.m_y);
        auto py = double(s * p.m_x + c * p.m_y);
        cords.push_back(point_2d{px, py});
    }
    return cords;
}

points_2d roundrect_to_shape_coords(const point_2d &size, const double &ratio) {
    auto cords = points_2d{};
    auto width = size.m_x / 2;
    auto height = size.m_y / 2;
    auto radius = std::min(width, height) * ratio * 2;
    auto deltaWidth = width - radius;
    auto deltaHeight = height - radius;
    auto point = point_2d{};
    for (int i = 0; i < 10; ++i) {  //10 points
        point.m_x = deltaWidth + radius * cos(-9 * i * M_PI / 180);
        point.m_y = -deltaHeight + radius * sin(-9 * i * M_PI / 180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
    }
    for (int i = 10; i < 20; ++i) {  //10 points
        point.m_x = -deltaWidth + radius * cos(-9 * i * M_PI / 180);
        point.m_y = -deltaHeight + radius * sin(-9 * i * M_PI / 180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
    }
    for (int i = 20; i < 30; ++i) {  //10 points
        point.m_x = -deltaWidth + radius * cos(-9 * i * M_PI / 180);
        point.m_y = deltaHeight + radius * sin(-9 * i * M_PI / 180);
        //std::cout << point.m_x << " " << point.m_y << std::endl;
        cords.push_back(point);
    }
    for (int i = 30; i < 40; ++i) {  //10 points
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
// TODO: all coords must be in CW (follow the rules of Boost Polygon of Rings)
// TODO: parameters for #points to Circle
points_2d shape_to_coords(const point_2d &size, const point_2d &pos, padShape shape, const double a1, const double a2, const double &ratio) {
    auto cords = points_2d{};
    switch (shape) {
        case padShape::CIRCLE: {
            auto radius = size.m_x / 2;
            auto point = point_2d{};
            for (int i = 0; i < 40; ++i) {  //40 points
                point.m_x = pos.m_x + radius * cos(-9 * i * M_PI / 180);
                point.m_y = pos.m_y + radius * sin(-9 * i * M_PI / 180);
                cords.push_back(point);
            }
            point.m_x = pos.m_x + radius * cos(0 * M_PI / 180);
            point.m_y = pos.m_y + radius * sin(0 * M_PI / 180);
            cords.push_back(point);
            break;
        }
        case padShape::OVAL: {
            auto width = size.m_x / 2;
            auto height = size.m_y / 2;
            auto point = point_2d{};
            for (int i = 0; i < 40; ++i) {  //40 points
                point.m_x = pos.m_x + width * cos(-9 * i * M_PI / 180);
                point.m_y = pos.m_y + height * sin(-9 * i * M_PI / 180);
                cords.push_back(point);
            }
            point.m_x = pos.m_x + width * cos(0 * M_PI / 180);
            point.m_y = pos.m_y + height * sin(0 * M_PI / 180);
            cords.push_back(point);
            break;
        }
        case padShape::RECT: {
            auto width = size.m_x / 2;
            auto height = size.m_y / 2;
            auto point = point_2d{};
            point.m_x = pos.m_x + width;
            point.m_y = pos.m_y + height;
            cords.push_back(point);

            point.m_y = pos.m_y - height;
            cords.push_back(point);

            point.m_x = pos.m_x - width;
            cords.push_back(point);

            point.m_y = pos.m_y + height;
            cords.push_back(point);

            //cords.push_back(point_2d{-1 * width, -1 * height});
            //cords.push_back(point_2d{width, height});
            break;
        }
        case padShape::ROUNDRECT: {
            auto width = size.m_x / 2;
            auto height = size.m_y / 2;
            auto radius = std::min(width, height) * ratio * 2;
            auto deltaWidth = width - radius;
            auto deltaHeight = height - radius;
            auto point = point_2d{};
            for (int i = 0; i < 10; ++i) {  //10 points
                point.m_x = pos.m_x + deltaWidth + radius * cos(-9 * i * M_PI / 180);
                point.m_y = pos.m_y - deltaHeight + radius * sin(-9 * i * M_PI / 180);
                cords.push_back(point);
            }
            for (int i = 10; i < 20; ++i) {  //10 points
                point.m_x = pos.m_x - deltaWidth + radius * cos(-9 * i * M_PI / 180);
                point.m_y = pos.m_y - deltaHeight + radius * sin(-9 * i * M_PI / 180);
                cords.push_back(point);
            }
            for (int i = 20; i < 30; ++i) {  //10 points
                point.m_x = pos.m_x - deltaWidth + radius * cos(-9 * i * M_PI / 180);
                point.m_y = pos.m_y + deltaHeight + radius * sin(-9 * i * M_PI / 180);
                cords.push_back(point);
            }
            for (int i = 30; i < 40; ++i) {  //10 points
                point.m_x = pos.m_x + deltaWidth + radius * cos(-9 * i * M_PI / 180);
                point.m_y = pos.m_y + deltaHeight + radius * sin(-9 * i * M_PI / 180);
                cords.push_back(point);
            }
            point.m_x = pos.m_x + deltaWidth + radius * cos(0 * M_PI / 180);
            point.m_y = pos.m_y - deltaHeight + radius * sin(0 * M_PI / 180);
            cords.push_back(point);
            break;
        }
        case padShape::TRAPEZOID: {
            break;
        }
        default: {
            break;
        }
    }

    return rotateShapeCoordsByAngles(cords, a1, a2);
}

/////////////////////////////////
//      [1]    [0]
//      |--------|
//      |    s   |
//      |        |
//      |________|
//      [2]    [3]
/////////////////////////////////

points_2d segment_to_rect(const points_2d &point, const double &w) {
    auto cords = points_2d{};
    auto p = point_2d{};
    auto width = w / 2;

    //vertical
    if (point[0].m_x == point[1].m_x) {
        if (point[0].m_y > point[1].m_y) {
            p.m_x = point[0].m_x + width;
            p.m_y = point[0].m_y + width;
            cords.push_back(p);
            p.m_x = point[0].m_x - width;
            cords.push_back(p);

            p.m_x = point[0].m_x - width;
            p.m_y = point[0].m_y - width;
            cords.push_back(p);
            p.m_x = point[0].m_x - width;
            cords.push_back(p);
        } else {
            p.m_x = point[0].m_x + width;
            p.m_y = point[1].m_y + width;
            cords.push_back(p);
            p.m_x = point[0].m_x - width;
            cords.push_back(p);

            p.m_x = point[0].m_x - width;
            p.m_y = point[0].m_y - width;
            cords.push_back(p);
            p.m_x = point[0].m_x + width;
            cords.push_back(p);
        }
    }
    //horizontal
    else if (point[0].m_y == point[1].m_y) {
        if (point[0].m_x > point[1].m_x) {
            p.m_x = point[0].m_x + width;
            p.m_y = point[0].m_y + width;
            cords.push_back(p);
            p.m_x = point[1].m_x - width;
            cords.push_back(p);

            p.m_y = point[0].m_y - width;
            cords.push_back(p);
            p.m_x = point[0].m_x + width;
            cords.push_back(p);
        } else {
            p.m_x = point[1].m_x + width;
            p.m_y = point[0].m_y + width;
            cords.push_back(p);
            p.m_x = point[0].m_x - width;
            cords.push_back(p);

            p.m_y = point[0].m_y - width;
            cords.push_back(p);
            p.m_x = point[1].m_x + width;
            cords.push_back(p);
        }
    }
    //45degree
    else if ((point[0].m_x > point[1].m_x) && (point[0].m_y > point[1].m_y)) {
        p.m_x = point[0].m_x + sqrt(2) * width;
        p.m_y = point[0].m_y;
        cords.push_back(p);

        p.m_x = point[0].m_x;
        p.m_y = point[0].m_y + sqrt(2) * width;
        cords.push_back(p);

        p.m_x = point[1].m_x - sqrt(2) * width;
        p.m_y = point[1].m_y;
        cords.push_back(p);

        p.m_x = point[1].m_x;
        p.m_y = point[1].m_y + sqrt(2) * width;
        cords.push_back(p);
    } else if ((point[0].m_x < point[1].m_x) && (point[0].m_y < point[1].m_y)) {
        p.m_x = point[1].m_x + sqrt(2) * width;
        p.m_y = point[1].m_y;
        cords.push_back(p);

        p.m_x = point[1].m_x;
        p.m_y = point[1].m_y + sqrt(2) * width;
        cords.push_back(p);

        p.m_x = point[0].m_x - sqrt(2) * width;
        p.m_y = point[0].m_y;
        cords.push_back(p);

        p.m_x = point[0].m_x;
        p.m_y = point[0].m_y + sqrt(2) * width;
        cords.push_back(p);

    }
    //135degree
    else if ((point[1].m_x < point[0].m_x) && (point[1].m_y > point[0].m_y)) {
        p.m_x = point[1].m_x;
        p.m_y = point[1].m_y + sqrt(2) * width;
        cords.push_back(p);

        p.m_x = point[1].m_x - sqrt(2) * width;
        p.m_y = point[1].m_y;
        cords.push_back(p);

        p.m_x = point[0].m_x;
        p.m_y = point[0].m_y - sqrt(2) * width;
        cords.push_back(p);

        p.m_x = point[0].m_x + sqrt(2) * width;
        p.m_y = point[0].m_y;
        cords.push_back(p);
    } else if ((point[1].m_x > point[0].m_x) && (point[1].m_y < point[0].m_y)) {
        p.m_x = point[0].m_x;
        p.m_y = point[0].m_y + sqrt(2) * width;
        cords.push_back(p);

        p.m_x = point[0].m_x - sqrt(2) * width;
        p.m_y = point[0].m_y;
        cords.push_back(p);

        p.m_x = point[1].m_x;
        p.m_y = point[1].m_y - sqrt(2) * width;
        cords.push_back(p);

        p.m_x = point[1].m_x + sqrt(2) * width;
        p.m_y = point[1].m_y;
        cords.push_back(p);
    }
    return cords;
}

points_2d via_to_circle(const point_2d &pos, const double &size) {
    auto radius = size / 2;
    auto coords = points_2d{};
    auto point = point_2d{};
    for (int i = 0; i < 40; ++i) {  //40 points
        point.m_x = pos.m_x + radius * cos(-9 * i * M_PI / 180);
        point.m_y = pos.m_y + radius * sin(-9 * i * M_PI / 180);
        coords.push_back(point);
    }
    point.m_x = pos.m_x + radius * cos(0 * M_PI / 180);
    point.m_y = pos.m_y + radius * sin(0 * M_PI / 180);
    coords.push_back(point);
    return coords;
}

points_2d viaToOctagon(const double &size, const point_2d &pos, const double &clearance) {
    auto _size = point_2d{size / 2, size / 2};

    auto coords = points_2d{};
    auto point = point_2d{};

    //[0]
    point.m_x = pos.m_x + _size.m_x + (clearance * tan(22.5 * M_PI / 180));
    point.m_y = pos.m_y + _size.m_y + clearance;
    coords.push_back(point);

    //[1]
    point.m_x = pos.m_x + _size.m_x + clearance;
    point.m_y = pos.m_y + _size.m_y + (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[2]
    point.m_x = pos.m_x + _size.m_x + clearance;
    point.m_y = pos.m_y - _size.m_y - (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[3]
    point.m_x = pos.m_x + _size.m_x + (clearance * tan(22.5 * M_PI / 180));
    point.m_y = pos.m_y - _size.m_y - clearance;
    coords.push_back(point);

    //[4]
    point.m_x = pos.m_x - _size.m_x - (clearance * tan(22.5 * M_PI / 180));
    point.m_y = pos.m_y - _size.m_y - clearance;
    coords.push_back(point);

    //[5]
    point.m_x = pos.m_x - _size.m_x - clearance;
    point.m_y = pos.m_y - _size.m_y - (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[6]
    point.m_x = pos.m_x - _size.m_x - clearance;
    point.m_y = pos.m_y + _size.m_y + (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[7]
    point.m_x = pos.m_x - _size.m_x - (clearance * tan(22.5 * M_PI / 180));
    point.m_y = pos.m_y + _size.m_y + clearance;
    coords.push_back(point);

    return coords;
}

/////////////////////////////////
//      [7]    [0]
//    [6] /------\  [1]
//       |  pin   |
//    [5] \______/ [2]
//      [4]    [3]
/////////////////////////////////

points_2d pinShapeToOctagon(const point_2d &size, const point_2d &pos, const double &clearance, const double &instAngle, const double &pinAngle) {
    auto coords = points_2d{};
    auto point = point_2d{};

    //[0]
    point.m_x = size.m_x / 2 + (clearance * tan(22.5 * M_PI / 180));
    point.m_y = size.m_y / 2 + clearance;
    coords.push_back(point);

    //[1]
    point.m_x = size.m_x / 2 + clearance;
    point.m_y = size.m_y / 2 + (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[2]
    point.m_x = size.m_x / 2 + clearance;
    point.m_y = -size.m_y / 2 - (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[3]
    point.m_x = size.m_x / 2 + (clearance * tan(22.5 * M_PI / 180));
    point.m_y = -size.m_y / 2 - clearance;
    coords.push_back(point);

    //[4]
    point.m_x = -size.m_x / 2 - (clearance * tan(22.5 * M_PI / 180));
    point.m_y = -size.m_y / 2 - clearance;
    coords.push_back(point);

    //[5]
    point.m_x = -size.m_x / 2 - clearance;
    point.m_y = -size.m_y / 2 - (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[6]
    point.m_x = -size.m_x / 2 - clearance;
    point.m_y = size.m_y / 2 + (clearance * tan(22.5 * M_PI / 180));
    coords.push_back(point);

    //[7]
    point.m_x = -size.m_x / 2 - (clearance * tan(22.5 * M_PI / 180));
    point.m_y = size.m_y / 2 + clearance;
    coords.push_back(point);

    return rotateShapeCoordsByAngles(coords, instAngle, pinAngle);
}

points_2d segmentToOctagon(const points_2d &point, const double &w, const double &clearance) {
    auto cords = points_2d{};
    auto p = point_2d{};
    auto width = w / 2;

    //vertical
    if (point[0].m_x == point[1].m_x) {
        if (point[0].m_y > point[1].m_y) {
            p.m_x = point[0].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y + width + clearance;
            cords.push_back(p);

            p.m_x = point[0].m_x + width + clearance;
            p.m_y = point[0].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x + width + clearance;
            p.m_y = point[1].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[1].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[1].m_x - width - clearance;
            p.m_y = point[1].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x - width - clearance;
            p.m_y = point[0].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y + width + clearance;
            cords.push_back(p);

        } else {
            p.m_x = point[1].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y + width + clearance;
            cords.push_back(p);

            p.m_x = point[1].m_x + width + clearance;
            p.m_y = point[1].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x + width + clearance;
            p.m_y = point[0].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[0].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[0].m_x - width - clearance;
            p.m_y = point[0].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x - width - clearance;
            p.m_y = point[1].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y + width + clearance;
            cords.push_back(p);
        }
    }

    //horizontal
    else if (point[0].m_y == point[1].m_y) {
        if (point[0].m_x > point[1].m_x) {
            p.m_x = point[0].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y + width + clearance;
            cords.push_back(p);

            p.m_x = point[0].m_x + width + clearance;
            p.m_y = point[0].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x + width + clearance;
            p.m_y = point[0].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[1].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[1].m_x - width - clearance;
            p.m_y = point[1].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x - width - clearance;
            p.m_y = point[1].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y + width + clearance;
            cords.push_back(p);
        } else {
            p.m_x = point[1].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y + width + clearance;
            cords.push_back(p);

            p.m_x = point[1].m_x + width + clearance;
            p.m_y = point[1].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x + width + clearance;
            p.m_y = point[1].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[1].m_x + width + (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[1].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[0].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y - width - clearance;
            cords.push_back(p);

            p.m_x = point[0].m_x - width - clearance;
            p.m_y = point[0].m_y - width - (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x - width - clearance;
            p.m_y = point[0].m_y + width + (clearance * tan(22.5 * M_PI / 180));
            cords.push_back(p);

            p.m_x = point[0].m_x - width - (clearance * tan(22.5 * M_PI / 180));
            p.m_y = point[0].m_y + width + clearance;
            cords.push_back(p);
        }
    }

    //45degree

    else if ((point[0].m_x > point[1].m_x) && (point[0].m_y > point[1].m_y)) {
        p.m_x = point[0].m_x + sqrt(2) * width + clearance;
        p.m_y = point[0].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[0].m_x + sqrt(2) * width + clearance;
        p.m_y = point[0].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);
        //2
        p.m_x = point[1].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        //3
        p.m_x = point[1].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        p.m_x = point[1].m_x - sqrt(2) * width - clearance;
        p.m_y = point[1].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[1].m_x - sqrt(2) * width - clearance;
        p.m_y = point[1].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[0].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);

        p.m_x = point[0].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);
    }

    else if ((point[0].m_x < point[1].m_x) && (point[0].m_y < point[1].m_y)) {
        p.m_x = point[1].m_x + sqrt(2) * width + clearance;
        p.m_y = point[1].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[1].m_x + sqrt(2) * width + clearance;
        p.m_y = point[1].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);
        //2
        p.m_x = point[0].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        //3
        p.m_x = point[0].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        p.m_x = point[0].m_x - sqrt(2) * width - clearance;
        p.m_y = point[0].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[0].m_x - sqrt(2) * width - clearance;
        p.m_y = point[0].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[1].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);

        p.m_x = point[1].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);

    }

    //135degree
    else if ((point[1].m_x < point[0].m_x) && (point[1].m_y > point[0].m_y)) {
        p.m_x = point[1].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);

        p.m_x = point[1].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);

        p.m_x = point[0].m_x + sqrt(2) * width + clearance;
        p.m_y = point[0].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[0].m_x + sqrt(2) * width + clearance;
        p.m_y = point[0].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[0].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        p.m_x = point[0].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        p.m_x = point[1].m_x - sqrt(2) * width - clearance;
        p.m_y = point[1].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[1].m_x - sqrt(2) * width - clearance;
        p.m_y = point[1].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

    }

    else if ((point[1].m_x > point[0].m_x) && (point[1].m_y < point[0].m_y)) {
        p.m_x = point[0].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);

        p.m_x = point[0].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[0].m_y + sqrt(2) * width + clearance;
        cords.push_back(p);

        p.m_x = point[1].m_x + sqrt(2) * width + clearance;
        p.m_y = point[1].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[1].m_x + sqrt(2) * width + clearance;
        p.m_y = point[1].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[1].m_x + (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        p.m_x = point[1].m_x - (clearance * tan(22.5 * M_PI / 180));
        p.m_y = point[1].m_y - sqrt(2) * width - clearance;
        cords.push_back(p);

        p.m_x = point[0].m_x - sqrt(2) * width - clearance;
        p.m_y = point[0].m_y - (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);

        p.m_x = point[0].m_x - sqrt(2) * width - clearance;
        p.m_y = point[0].m_y + (clearance * tan(22.5 * M_PI / 180));
        cords.push_back(p);
    }
    return cords;
}