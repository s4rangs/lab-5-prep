#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;

const int ANIMATION_FRAMES = 120;
const float CIRCLE_RADIUS = 30.0f;

int currentFrame = 0;

// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Functions can be from lecture or from https://easings.net/#
        // ====== ====== ======
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->scancode) {
                case sf::Keyboard::Scan::Num1: // linear
                    tween = [](float a, float b, float t) { // a = start, b = end, t = progress from 0 to 1
                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num2: // ease in quadratic
                    tween = [](float a, float b, float t) {
                        t = t * t; // t^2 = t*t
                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num3: // ease out quadratic
                    tween = [](float a, float b, float t) {
                        t = 1.0f - (1.0f - t) * (1.0f - t); // 1 - (1-t)^2
                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num4: //. ease in out quad
                    tween = [](float a, float b, float t) {
                        if (t < 0.5f) { // first half: 2t^2 = 2 * t * t
                            t = 2.0f * t * t;
                        } else { // 1 = [(-2t+2)^2]/2
                            t = 1.0f -((-2.0f * t + 2.0f) * (-2.0f * t + 2.0f)) / 2.0f;
                        }

                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num5: // ease in cubic
                    tween = [](float a, float b, float t) {
                        t = t * t * t; //t^3
                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num6: // ease out cubic
                    tween = [](float a, float b, float t) {
                        t = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
                        // 1 - (1-t)^3
                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num7: // ease in quartic
                    tween = [](float a, float b, float t) {
                        t = t * t * t * t; // t^4
                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num8: // ease out quartic
                    tween = [](float a, float b, float t) {
                        t = 1.0f -(1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
                        // 1 - (1-t)^4
                        return (1.0f - t) * a + t * b;
                    };
                    break;

                case sf::Keyboard::Scan::Num9: // ease in out quartic
                    tween = [](float a, float b, float t) {
                        if (t < 0.5f) { // first half: 8t^4
                            t = 8.0f * t * t * t * t;
                        } else { //second half: 1 - (x^4/2)
                            float x = -2.0f * t + 2.0f;
                            t = 1.0f - (x * x * x * x) / 2.0f;
                        }

                        return (1.0f - t) * a + t * b;
                    };
                    break;

                default:
                    break;
            }
        }

    }
}

void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======
    float t = static_cast<float>(currentFrame) / static_cast<float>(ANIMATION_FRAMES); // frame number into val between 0 and 1
    float x = tween(100.0f, 700.0f, t);
    float y = static_cast<float>(WINDOW_HEIGHT) / 3.0f; // circle is at about = 1/3 from top

    sf::CircleShape circle(CIRCLE_RADIUS); // creates sfml circle 
    circle.setPosition({x - CIRCLE_RADIUS, y - CIRCLE_RADIUS}); // setting coords by getting circles centre instead of top left corner of circle

    window.draw(circle); // puts it on screen

    currentFrame = (currentFrame + 1) % ANIMATION_FRAMES;


    // ====== ====== ======
    // TODO: (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======

    const float GRAPH_LEFT = 100.0f;
    const float GRAPH_TOP = 550.0f;
    const float GRAPH_WIDTH = 600.0f;
    const float GRAPH_HEIGHT = 200.0f;

    // x-axis
    sf::VertexArray xAxis(sf::PrimitiveType::Lines, 2);
    xAxis[0].position = {GRAPH_LEFT, GRAPH_TOP + GRAPH_HEIGHT};
    xAxis[1].position = {
        GRAPH_LEFT + GRAPH_WIDTH,
        GRAPH_TOP + GRAPH_HEIGHT
    };
    window.draw(xAxis);

    // y-axis
    sf::VertexArray yAxis(sf::PrimitiveType::Lines, 2);
    yAxis[0].position = {GRAPH_LEFT, GRAPH_TOP + GRAPH_HEIGHT};
    yAxis[1].position = {GRAPH_LEFT, GRAPH_TOP};
    window.draw(yAxis);

    // Draw tween curve
    sf::VertexArray curve(sf::PrimitiveType::LineStrip);

    for (int i = 0; i <= 100; ++i) {
        float t = static_cast<float>(i) / 100.0f;
        float value = tween(0.0f, 1.0f, t);

        float graphX = GRAPH_LEFT + t * GRAPH_WIDTH;
        float graphY = GRAPH_TOP + GRAPH_HEIGHT - value * GRAPH_HEIGHT;

        curve.append(sf::Vertex({graphX, graphY}));
    }

    window.draw(curve);

    // Current animation position
    float currentT =
        static_cast<float>(currentFrame) /
        static_cast<float>(ANIMATION_FRAMES);

    float currentValue = tween(0.0f, 1.0f, currentT);

    float dotX = GRAPH_LEFT + currentT * GRAPH_WIDTH;
    float dotY = GRAPH_TOP + GRAPH_HEIGHT - currentValue * GRAPH_HEIGHT;
    sf::CircleShape dot(5.0f);
    dot.setPosition({dotX - 5.0f, dotY - 5.0f});

    window.draw(dot);

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
