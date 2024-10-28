#ifndef _BOARD_HPP_
#define _BOARD_HPP_

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

class Board {
public:
    Board(sf::RenderWindow* window) : gameWindow(window), whitesMove(true) {
        Rects.resize(ROW, std::vector<sf::RectangleShape>(COL));
        Pieces.resize(ROW, std::vector<sf::Sprite>(COL));
        selectedIndex = {-1, -1};

        std::filesystem::path currentPath = std::filesystem::current_path();
        std::cout << "Current working directory: " << currentPath << std::endl;

        squareSize = gameWindow->getSize().x / ROW;

        InitializePieces();

        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                Rects[i][j] = sf::RectangleShape(sf::Vector2f(squareSize, squareSize));
                Rects[i][j].setFillColor((i + j) % 2 == 0 ? sf::Color::White : sf::Color(80, 80, 80));
                Rects[i][j].setPosition(j * squareSize, i * squareSize);
            }
        }

        placeInitialPieces();
    }

    void selectPiece() {
        selectedIndex = getHoveredSquare();
    }

    bool isWhiteSelected(){
      sf::Sprite piece = Pieces[selectedIndex.x][selectedIndex.y];
      if(piece.getTexture() == &w_pawn_texture){
        return true; 
      }
      else if(piece.getTexture() == &w_knight_texture){
        return true; 
      }
      else if(piece.getTexture() == &w_queen_texture){
        return true; 
      }
      else if(piece.getTexture() == &w_pawn_texture){
        return true; 
      }
      else if(piece.getTexture() == &w_rook_texture){
        return true; 
      }
      else if(piece.getTexture() == &w_bishop_texture){
        return true; 
      }
      else if(piece.getTexture() == &w_king_texture){
        return true; 
      }
      return false;
    }

    void placePiece() {
        sf::Vector2<int> placeIndex = getHoveredSquare();
        if((!whitesMove && isWhiteSelected()) || (whitesMove && !isWhiteSelected())) {
          isSelected = false;
          return;
        }

        if (isValidIndex(selectedIndex) && isValidIndex(placeIndex)) {
            Pieces[placeIndex.x][placeIndex.y] = Pieces[selectedIndex.x][selectedIndex.y];
            Pieces[selectedIndex.x][selectedIndex.y] = sf::Sprite(); // Clear the old position

            centerPiece(Pieces[placeIndex.x][placeIndex.y], placeIndex);
            isSelected = false;  // Reset selection status
            selectedIndex = {-1, -1};  // Clear selected index
            whitesMove = !whitesMove;
        } else {
            std::cerr << "Invalid selection or placement index.\n";
        }
    }

    sf::Vector2<int> getHoveredSquare() {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(*gameWindow);
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (Rects[i][j].getGlobalBounds().contains(
                    static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                    std::cout << "Mouse is in square: (" << i << ", " << j << ")\n";
                    isSelected = true;
                    return {i, j};  // Return row and column index instead of mouse position
                }
            }
        }
        return {-1, -1};  // Return invalid index if no square contains the mouse
    }

    void draw() {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                gameWindow->draw(Rects[i][j]);
                if (Pieces[i][j].getTexture()) {
                    gameWindow->draw(Pieces[i][j]);
                }
            }
        }
    }

    void DebugInfo(){
      std::cout << "\n\nDEBUG INFO ___________________________________________\n";
      std::cout << "isSelected: " << isSelected << "\n";
      std::cout << "whitesMove: " << whitesMove << "\n";
      std::cout << "squareSize: " << squareSize << "\n";
      // std::cout << "isWhiteSelected: " << isWhiteSelected() << "\n"; 
    }

    bool isSelected = false;

private:
    std::vector<std::vector<sf::RectangleShape>> Rects;
    std::vector<std::vector<sf::Sprite>> Pieces;
    sf::Vector2<int> selectedIndex;
    sf::RenderWindow* gameWindow;
    bool whitesMove;

    static const int ROW = 8;
    static const int COL = 8;
    int squareSize;

    sf::Texture w_pawn_texture, b_pawn_texture;
    sf::Texture w_knight_texture, b_knight_texture;
    sf::Texture w_rook_texture, b_rook_texture;
    sf::Texture w_king_texture, b_king_texture;
    sf::Texture w_queen_texture, b_queen_texture;
    sf::Texture w_bishop_texture, b_bishop_texture;

    void InitializePieces() {
        if (!w_pawn_texture.loadFromFile("static/pieces/w-pawn.png")) std::cerr << "Failed to load white pawn texture.\n";
        if (!b_pawn_texture.loadFromFile("static/pieces/b-pawn.png")) std::cerr << "Failed to load black pawn texture.\n";
        if (!w_knight_texture.loadFromFile("static/pieces/w-knight.png")) std::cerr << "Failed to load white knight texture.\n";
        if (!b_knight_texture.loadFromFile("static/pieces/b-knight.png")) std::cerr << "Failed to load black knight texture.\n";
        if (!w_rook_texture.loadFromFile("static/pieces/w-rook.png")) std::cerr << "Failed to load white rook texture.\n";
        if (!b_rook_texture.loadFromFile("static/pieces/b-rook.png")) std::cerr << "Failed to load black rook texture.\n";
        if (!w_king_texture.loadFromFile("static/pieces/w-king.png")) std::cerr << "Failed to load white king texture.\n";
        if (!b_king_texture.loadFromFile("static/pieces/b-king.png")) std::cerr << "Failed to load black king texture.\n";
        if (!w_queen_texture.loadFromFile("static/pieces/w-queen.png")) std::cerr << "Failed to load white queen texture.\n";
        if (!b_queen_texture.loadFromFile("static/pieces/b-queen.png")) std::cerr << "Failed to load black queen texture.\n";
        if (!w_bishop_texture.loadFromFile("static/pieces/w-bishop.png")) std::cerr << "Failed to load white bishop texture.\n";
        if (!b_bishop_texture.loadFromFile("static/pieces/b-bishop.png")) std::cerr << "Failed to load black bishop texture.\n";
    }

    void placeInitialPieces() {
        for (int j = 0; j < COL; j++) {
            // White pieces (Row 0)
            if (j == 0 || j == 7) Pieces[0][j].setTexture(w_rook_texture);
            else if (j == 1 || j == 6) Pieces[0][j].setTexture(w_knight_texture);
            else if (j == 2 || j == 5) Pieces[0][j].setTexture(w_bishop_texture);
            else if (j == 3) Pieces[0][j].setTexture(w_queen_texture);
            else if (j == 4) Pieces[0][j].setTexture(w_king_texture);

            // Black pieces (Row 7)
            if (j == 0 || j == 7) Pieces[7][j].setTexture(b_rook_texture);
            else if (j == 1 || j == 6) Pieces[7][j].setTexture(b_knight_texture);
            else if (j == 2 || j == 5) Pieces[7][j].setTexture(b_bishop_texture);
            else if (j == 3) Pieces[7][j].setTexture(b_queen_texture);
            else if (j == 4) Pieces[7][j].setTexture(b_king_texture);

            // Set scale and position for pieces to fit in the square
            centerPiece(Pieces[0][j], {0, j});
            centerPiece(Pieces[7][j], {7, j});
        }

        // Place pawns for both white and black
        for (int j = 0; j < COL; j++) {
            centerPawn(Pieces[1][j], w_pawn_texture, {1, j}); // White pawns
            centerPawn(Pieces[6][j], b_pawn_texture, {6, j}); // Black pawns
        }
    }

    void centerPiece(sf::Sprite& piece, const sf::Vector2<int>& position) {
        piece.setScale(static_cast<float>(squareSize) / piece.getTexture()->getSize().x,
                       static_cast<float>(squareSize) / piece.getTexture()->getSize().y);
        piece.setPosition(position.y * squareSize + (squareSize - piece.getGlobalBounds().width) / 2,
                          position.x * squareSize + (squareSize - piece.getGlobalBounds().height) / 2);
    }

    void centerPawn(sf::Sprite& pawn, const sf::Texture& texture, const sf::Vector2<int>& position) {
        pawn.setTexture(texture);
        centerPiece(pawn, position);
    }

    bool isValidIndex(const sf::Vector2<int>& index) const {
        return index.x >= 0 && index.x < ROW && index.y >= 0 && index.y < COL;
    }
};

#endif // _BOARD_HPP_

