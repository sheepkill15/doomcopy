//
// Created by simi on 19.12.2021.
//

#ifndef DOOM_COPY_RAY_H
#define DOOM_COPY_RAY_H


#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Image.hpp>

struct ray_hit {
    const float dist;
    const int side;
};

class ray {
public:
    ray() = delete;

    static ray_hit cast(const sf::Vector2f& dir, const sf::Vector2f& origin, const sf::Image& map, uint max_dist = 10000);
};


#endif //DOOM_COPY_RAY_H
