//
// Created by simi on 19.12.2021.
//

#ifndef DOOM_COPY_CAMERA_H
#define DOOM_COPY_CAMERA_H

#include <SFML/System/Vector2.hpp>

namespace sf {
    class RenderWindow;
    class Image;
}

class camera {
public:
    static struct camera_settings {
        sf::Vector2f pos{};
        sf::Vector2f dir{1, 0};
        sf::Vector2f plane{0, 0.66f};
        int pixel_blocks = 10;
    } settings;

    static void walls(sf::RenderWindow& window, const sf::Image& map, sf::RectangleShape& shape);
    static void floor(sf::RenderWindow& window, const sf::Image& map, const sf::Image& src);
    static void ceiling(sf::RenderWindow& window, const sf::Image& map, const sf::Image& src);

    static void move_forward(float amount);
    static void rotate(float amount);
};


#endif //DOOM_COPY_CAMERA_H
