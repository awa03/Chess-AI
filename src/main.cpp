#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

int main() {
    // Create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test");

    // Load a font
    sf::Font font;
    if (!font.loadFromFile("static/fonts/ArsenalSC-Bold.ttf")) { // Ensure the path is correct
        return EXIT_FAILURE; // Exit if the font couldn't be loaded
    }

    // Create a text object
    sf::Text title;
    title.setString("Simulator");
    title.setFont(font); // Set the font
    title.setCharacterSize(30); // Set the text size
    title.setFillColor(sf::Color::Red);
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, window.getSize().y / 2 - title.getGlobalBounds().height / 2); // Center the text

    // Create a circle shape
    sf::CircleShape circle(50); 
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(375, 275); 

    float speed = 100.0f; 
    sf::Clock clock; 

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();

        circle.move(speed * deltaTime, 0); 

        window.clear();

        window.draw(title); // Draw the text
        window.draw(circle); // Draw the circle

        window.display();
    }

    return 0;
}

