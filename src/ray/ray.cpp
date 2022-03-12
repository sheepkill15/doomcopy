//
// Created by simi on 19.12.2021.
//

#include <cmath>
#include "ray.h"

ray_hit ray::cast(const sf::Vector2f &dir, const sf::Vector2f &origin, const sf::Image &map, uint max_dist) {
    auto [x, y] = origin;
    sf::Vector2u currPoint{ origin };
    float distance;
    const auto [width, height] = (map.getSize());
    float deltaDistX = (dir.x == 0) ? 1e10f : std::abs(1.f / dir.x);
    float deltaDistY = (dir.y == 0) ? 1e10f : std::abs(1.f / dir.y);

    sf::Vector2i step{};
    sf::Vector2f sideDist{};
    int side;

    if(dir.x < 0) {
        step.x = -1;
        sideDist.x = (x - currPoint.x) * deltaDistX;
    }
    else {
        step.x = 1;
        sideDist.x = (currPoint.x + 1.0f - x) * deltaDistX;
    }
    if(dir.y < 0) {
        step.y = -1;
        sideDist.y = (y - currPoint.y) * deltaDistY;
    }
    else {
        step.y = 1;
        sideDist.y = (currPoint.y + 1.0f - y) * deltaDistY;
    }

    while(currPoint.x >= 0 && currPoint.x < width && currPoint.y >= 0 && currPoint.y < height &&
    map.getPixel(currPoint.x, currPoint.y) == sf::Color::Black) {
        if(sideDist.x < sideDist.y) {
            sideDist.x += deltaDistX;
            currPoint.x += step.x;
            side = 0;
        } else {
            sideDist.y += deltaDistY;
            currPoint.y += step.y;
            side = 1;
        }
    }
    if(side == 0) distance = (sideDist.x - deltaDistX);
    else distance = (sideDist.y - deltaDistY);


    return {distance, side};
}
