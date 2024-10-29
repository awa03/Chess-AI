#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include "include/board.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Chess", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    sf::Font font;
    if (!font.loadFromFile("static/fonts/ArsenalSC-Bold.ttf")) {
        return EXIT_FAILURE;
    }

    sf::Text title;
    title.setString("Chess");
    title.setFont(font);
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::Red);
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, window.getSize().y / 2 - title.getGlobalBounds().height / 2);

    sf::Clock clock; 
    Board b(&window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                if (b.isSelected) {
                  b.placePiece();
                } else {
                  b.selectPiece();
                }
              }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        // b.DebugInfo();
        window.clear();
        // window.draw(title);  // Uncomment this line to display the title
        b.draw();
        window.display();
    }

    return 0;
}

