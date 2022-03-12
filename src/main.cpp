#include <SFML/Graphics.hpp>

#include "ray/ray.h"
#include "camera/camera.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Doom COPY");
    window.setVerticalSyncEnabled(true);

    auto image = sf::Image();
    image.create(100, 100);
    for(int i = 0; i < 50; i++) {
        image.setPixel(50, 25+i, sf::Color::White);
    }
    sf::RectangleShape shape{};

    sf::Texture tex{};
    tex.loadFromFile("/home/simi/Pictures/brick.png");
    shape.setTexture(&tex, true);

    sf::Texture floorTex{};
    floorTex.loadFromFile("/home/simi/Pictures/floor.png");
    const auto floorImg = floorTex.copyToImage();

    while(window.isOpen()) {
        sf::Event event{};
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            else if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Left) {
                    camera::rotate(-0.1f);
                }
                else if(event.key.code == sf::Keyboard::Right) {
                    camera::rotate(0.1f);
                }
                else if(event.key.code == sf::Keyboard::Up) {
                    camera::move_forward(1);
                }
                else if(event.key.code == sf::Keyboard::Down) {
                    camera::move_forward(-1);
                }
                else if(event.key.code == sf::Keyboard::O) {
                    camera::settings.dir.y -= 0.1f;
                }
                else if(event.key.code == sf::Keyboard::P) {
                    camera::settings.dir.y += 0.1f;
                }
                else if(event.key.code == sf::Keyboard::U) {
                    camera::settings.pixel_blocks--;
                    if(camera::settings.pixel_blocks < 1) {
                        camera::settings.pixel_blocks = 1;
                    }
                }
                else if(event.key.code == sf::Keyboard::I) {
                    camera::settings.pixel_blocks++;
                }
            }
        }
        window.clear(sf::Color::Black);
        camera::floor(window, image, floorImg);
        camera::walls(window, image, shape);
        window.display();
    }

    return 0;
}
