//
// Created by simi on 19.12.2021.
//
#include <cmath>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include "camera.h"
#include "ray/ray.h"

camera::camera_settings camera::settings{};

void camera::walls(sf::RenderWindow &window, const sf::Image &map, sf::RectangleShape &shape) {
    const auto[width, height] = window.getSize();

    const auto[texWidth, texHeight] = shape.getTexture()->getSize();

    for (int x = 0; x < width / settings.pixel_blocks; x++) {
        float cameraX = 2.f * static_cast<float>(x * settings.pixel_blocks) / static_cast<float>(width) - 1;
        sf::Vector2f rayDir{settings.dir.x + settings.plane.x * cameraX, settings.dir.y + settings.plane.y * cameraX};
        const auto hit = ray::cast(rayDir, settings.pos, map);
        const float lineHeight = static_cast<float>(height) / hit.dist;
        shape.setSize({static_cast<float>(settings.pixel_blocks), lineHeight});
        shape.setPosition({static_cast<float>(x * settings.pixel_blocks) / static_cast<float>(width) * width,
                           -lineHeight / 2.f + height / 2});

        if (hit.side == 1) {
            shape.setFillColor(sf::Color(128, 128, 128, 255));
        } else {
            shape.setFillColor(sf::Color::White);
        }

        float wallX;
        if (hit.side == 0) wallX = settings.pos.y + hit.dist * rayDir.y;
        else wallX = settings.pos.x + hit.dist * rayDir.x;

        wallX -= std::floor(wallX);

        int texX = int(wallX * texWidth);
        if (hit.side == 0 && rayDir.x > 0) texX = texWidth - texX - 1;
        if (hit.side == 1 && rayDir.y < 0) texX = texWidth - texX - 1;

        shape.setTextureRect(sf::IntRect(texX, 0, 1, texHeight));

        window.draw(shape);
    }
}

void camera::move_forward(float amount) {
    settings.pos += settings.dir * amount;
}

void camera::rotate(float amount) {
    const auto oldDir = settings.dir;
    settings.dir.x = settings.dir.x * std::cos(amount) - settings.dir.y * std::sin(amount);
    settings.dir.y = oldDir.x * std::sin(amount) + settings.dir.y * std::cos(amount);
    const auto oldPlane = settings.plane;
    settings.plane.x = settings.plane.x * std::cos(amount) - settings.plane.y * std::sin(amount);
    settings.plane.y = oldPlane.x * std::sin(amount) + settings.plane.y * std::cos(amount);
}

namespace {

    const std::string fragmentShaderSource = R"(
    uniform vec2 rayDir1;
    uniform vec2 rayDir2;
    uniform vec2 size;

    uniform vec2 camera_pos;

    uniform sampler2D src_texture;
    uniform vec2 src_tex_size;

    void main()
    {
        vec2 coord = gl_FragCoord.xy * size;
        float screenY = coord.y;
        float posZ = 0.5 * (size.y);
        float rowDistance = posZ / coord.y;
        vec2 floorStep = vec2((rayDir2.x - rayDir1.x) * (rowDistance / size.x), (rayDir2.y - rayDir1.y) * (rowDistance / size.x));
        vec2 floor = vec2(camera_pos.x + rowDistance * rayDir1.x + floorStep.x * (coord.x),
camera_pos.y + rowDistance * rayDir1.y + floorStep.y * (coord.x));

        float tx = fract(floor.x);
        float ty = fract(floor.y);

        vec4 pixel = texture2D(src_texture, vec2(tx, ty));
        gl_FragColor = pixel;
    }
)";

    bool loaded = false;
    sf::Shader fragment;
    sf::Texture tex;

    bool render_floor_with_shaders(sf::RenderWindow &window, const sf::Image &src, sf::RectangleShape& shape)
    {
        if(!loaded && !fragment.loadFromMemory(fragmentShaderSource, sf::Shader::Fragment)) {
            return false;
        }
        if(!loaded) {
            tex.loadFromImage(src);
        }
        loaded = true;
        sf::Vector2f rayDir1{camera::settings.dir.x - camera::settings.plane.x, camera::settings.dir.y - camera::settings.plane.y};
        sf::Vector2f rayDir2{camera::settings.dir.x + camera::settings.plane.x, camera::settings.dir.y + camera::settings.plane.y};

        fragment.setUniform("rayDir1", rayDir1);
        fragment.setUniform("rayDir2", rayDir2);
        fragment.setUniform("size", (sf::Vector2f)window.getSize());
        fragment.setUniform("camera_pos", camera::settings.pos);
        fragment.setUniform("src_texture", tex);
        fragment.setUniform("src_tex_size", (sf::Vector2f)tex.getSize());
        sf::Texture placeholder{};
        placeholder.create(window.getSize().x, window.getSize().y / 2);
        shape.setTexture(&placeholder, false);

        window.draw(shape, &fragment);

        return true;
    }
}

void camera::floor(sf::RenderWindow &window, const sf::Image &map, const sf::Image &src) {
    const auto[width, height] = window.getSize();
    const auto[texWidth, texHeight] = src.getSize();

    sf::RectangleShape shape{};
    shape.setSize({static_cast<float>(width), static_cast<float>(height / 2)});

    shape.setPosition(0, height / 2);

    if(sf::Shader::isAvailable()) {
        if(render_floor_with_shaders(window, src, shape)) {
            return;
        }
    }

    sf::Vector2f rayDir1{settings.dir.x - settings.plane.x, settings.dir.y - settings.plane.y};
    sf::Vector2f rayDir2{settings.dir.x + settings.plane.x, settings.dir.y + settings.plane.y};
    sf::Image newImage;

    sf::Color pixels[width * height / settings.pixel_blocks / 2];

    for (int y = 0; y < height / settings.pixel_blocks / 2; y++) {
        const int screenY = height / 2 + y * settings.pixel_blocks;

        const float p = screenY - height / 2.f;
        const float posZ = 0.5f * height;
        const float rowDistance = posZ / p;

        const auto floorStep = rowDistance / static_cast<float>(width) * (rayDir2 - rayDir1);

        sf::Vector2f floor = settings.pos + rowDistance * rayDir1;

            const auto yIndex = y * width;
            for (int x = 0; x < width; x++) {
                int cellX = static_cast<int>(floor.x);
                int cellY = static_cast<int>(floor.y);

                unsigned int tx = static_cast<unsigned int>(texWidth * (floor.x - cellX)) % (texWidth);
                unsigned int ty = static_cast<unsigned int>(texHeight * (floor.y - cellY)) % (texHeight);

                    floor += floorStep;
                const auto colorToSet = src.getPixel(tx, ty);
//                newImage.setPixel(x, screenY - height / 2 + imgY, colorToSet);
                pixels[yIndex + x] = colorToSet;
        }
    }
    newImage.create(width, height / settings.pixel_blocks / 2, reinterpret_cast<const sf::Uint8 *>(pixels));


    sf::Texture newTex{};
    newTex.loadFromImage(newImage);

    shape.setTexture(&newTex, false);

    window.draw(shape);
}

void camera::ceiling(sf::RenderWindow &window, const sf::Image &map, const sf::Image &src) {

}
