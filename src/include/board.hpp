#ifndef _BOARD_HPP_
#define _BOARD_HPP_

#include <SFML/Graphics.hpp>
#include "pieces.hpp"
#include <iostream>
#include <vector>
#include <memory>

class Board {
public:
  // Constructor initialization
  Board(sf::RenderWindow* window) : gameWindow(window), whitesMove(true), selectedIndex{-1, -1} {
    // Resize without copying
    Pieces.resize(ROW);
    for (auto& row : Pieces) {
      row.resize(COL);
    }

    squareSize = gameWindow->getSize().x / ROW;

    if (!InitializeTextures()) {
      std::cerr << "Failed to load textures." << std::endl;
    }
    placeInitialPieces();

    Rects.resize(ROW, std::vector<sf::RectangleShape>(COL));
    highlightRects.resize(ROW, std::vector<sf::RectangleShape>(COL));
    for (int i = 0; i < ROW; i++) {
      for (int j = 0; j < COL; j++) {
        Rects[i][j] = sf::RectangleShape(sf::Vector2f(squareSize, squareSize));
        Rects[i][j].setFillColor((i + j) % 2 == 0 ? sf::Color::White : sf::Color(80, 80, 80));
        Rects[i][j].setPosition(j * squareSize, i * squareSize);
        highlightRects[i][j] = sf::RectangleShape(sf::Vector2f(squareSize, squareSize));
        highlightRects[i][j].setFillColor(sf::Color(0, 0, 0, 0));
        highlightRects[i][j].setPosition(j * squareSize, i * squareSize);
      }
    }
  }

  void highlightPossibleMoves() {
    std::cout << "Highlighting: " << selectedIndex.x << ", " << selectedIndex.y << "\n";
    auto& piece = Pieces[selectedIndex.x][selectedIndex.y];
    if (piece) {
        highlightRects[selectedIndex.x][selectedIndex.y].setFillColor(sf::Color(0, 255, 0, 50)); // Highlight selected square
        if (piece->getType() == Pieces::Type::PAWN) {
            int direction = (piece->getColor() == Pieces::Color::WHITE) ? 1 : -1; // White moves up, Black moves down
            highlightPawnAttack(direction);
            if (isValidIndex({selectedIndex.x + direction, selectedIndex.y}) 
            && !(Pieces[selectedIndex.x][selectedIndex.y + 1] == nullptr) && Pieces[selectedIndex.x][selectedIndex.y + 1]->hasMoved == false){
                highlightRects[selectedIndex.x + direction][selectedIndex.y].setFillColor(sf::Color(0, 0, 255, 50)); // Highlight forward move
                highlightRects[selectedIndex.x + (direction * 2)][selectedIndex.y].setFillColor(sf::Color(0, 0, 255, 50)); // Highlight forward move
                Pieces[selectedIndex.x][selectedIndex.y + 1]->hasMoved = true;
            }
            else if (isValidIndex({selectedIndex.x + direction, selectedIndex.y}) 
            && (Pieces[selectedIndex.x][selectedIndex.y + 1] == nullptr)){
                highlightRects[selectedIndex.x + direction][selectedIndex.y].setFillColor(sf::Color(0, 0, 255, 50)); // Highlight forward move
            }
        }
    }
  }
// (Pieces[selectedIndex.x + direction][selectedIndex.y + 1]->getColor() == Pieces::Color::WHITE) == whitesMove)

  void highlightPawnAttack(int& direction){
    std::vector<int> yDir = {-1, 1};
    for(auto& d : yDir){
      if(highlightPawnHelper(direction, d)){
        highlightRects[selectedIndex.x + direction][selectedIndex.y + d].setFillColor(sf::Color(255, 0, 0, 50)); // Highlight forward move
      }
    }
  }
  bool highlightPawnHelper(int& direction, int& d){
    return (isValidIndex({selectedIndex.x + direction, selectedIndex.y + d}) 
        && (Pieces[selectedIndex.x + direction][selectedIndex.y + d] != nullptr) 
        // if pieces same color dont highlight
        && (Pieces[selectedIndex.x + direction][selectedIndex.y + d]->getColor()
        != Pieces[selectedIndex.x][selectedIndex.y]->getColor()));
  }

  void clearHighlights() {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            highlightRects[i][j].setFillColor(sf::Color(0, 0, 0, 0)); // Reset to transparent
        }
    }
  }

  void selectPiece() {
    clearHighlights();
    selectedIndex = getHoveredSquare();
    if ((whitesMove && !isWhiteSelected()) || (!whitesMove && isWhiteSelected())) {
      isSelected = false;
      return;
    }

    // Check if correct player is moving -
    // TODO: SIMPLIFY EXPRESSION
    if(((Pieces[selectedIndex.x][selectedIndex.y]->getColor() == Pieces::Color::WHITE) && whitesMove) || 
    !((Pieces[selectedIndex.x][selectedIndex.y]->getColor() == Pieces::Color::WHITE) && whitesMove)) {
      isSelected = true;
    }

    else{ selectedIndex = {-1, -1}; return;}
    highlightPossibleMoves();
  }

  bool isWhiteSelected() {
    auto& piece = Pieces[selectedIndex.x][selectedIndex.y];
    std::cout << "Selected Type: " << piece->getTypeString() << "\n";
    if (piece) {
      return piece->getColor() == Pieces::Color::WHITE;
    }
    return false;
  }

  void placePiece() {
    sf::Vector2<int> placeIndex = getHoveredSquare();
    if ((whitesMove && !isWhiteSelected()) || (!whitesMove && isWhiteSelected())) {
      isSelected = false;
      return;
    }

    std::cout << "Selected Color Selected: " << Pieces[selectedIndex.x][selectedIndex.y]->getColorString() << "\n";
    std::cout << "Place Color Selected: " << placeIndex.x << ", " << placeIndex.y << "\n";


    if((isValidIndex(selectedIndex) && isValidIndex(selectedIndex) && Pieces[placeIndex.x][placeIndex.y] != nullptr) && 
      (Pieces[selectedIndex.x][selectedIndex.y]->getColor() == Pieces[placeIndex.x][placeIndex.y]->getColor())) {
      selectedIndex = placeIndex;
      clearHighlights();
      highlightPossibleMoves();
      return;
    }

    if (isValidIndex(selectedIndex) && isValidIndex(placeIndex)) {
      std::cout << "Place Color Selected 2: " << placeIndex.x << ", " << placeIndex.y << "\n";
      std::cout << "Place Color Selected 3: " << selectedIndex.x << ", " << selectedIndex.y << "\n";
      Pieces[placeIndex.x][placeIndex.y] = std::move(Pieces[selectedIndex.x][selectedIndex.y]);
      centerPiece(*Pieces[placeIndex.x][placeIndex.y]->getSprite(), placeIndex);

      isSelected = false;  
      selectedIndex = {-1, -1};  
      whitesMove = !whitesMove;
      clearHighlights();
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
          isSelected = true;
          std::cout << "Selected Position: " << i << ", " << j << "\n";
          return {i, j};
        }
      }
    }
    return {-1, -1};
  }

  void draw() {
    for (int i = 0; i < ROW; i++) {
      for (int j = 0; j < COL; j++) {
        gameWindow->draw(Rects[i][j]);
        if (Pieces[i][j] && Pieces[i][j]->getSprite()->getTexture()) {
          gameWindow->draw(*Pieces[i][j]->getSprite());
        }
        gameWindow->draw(highlightRects[i][j]);
      }
    }
  }

  bool isSelected = false;

private:
  std::vector<std::vector<sf::RectangleShape>> Rects;
  std::vector<std::vector<sf::RectangleShape>> highlightRects;
  std::vector<std::vector<std::unique_ptr<Pieces::Piece>>> Pieces;
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

  bool InitializeTextures() {
    return w_pawn_texture.loadFromFile("static/pieces/w-pawn.png") &&
    b_pawn_texture.loadFromFile("static/pieces/b-pawn.png") &&
    w_knight_texture.loadFromFile("static/pieces/w-knight.png") &&
    b_knight_texture.loadFromFile("static/pieces/b-knight.png") &&
    w_rook_texture.loadFromFile("static/pieces/w-rook.png") &&
    b_rook_texture.loadFromFile("static/pieces/b-rook.png") &&
    w_king_texture.loadFromFile("static/pieces/w-king.png") &&
    b_king_texture.loadFromFile("static/pieces/b-king.png") &&
    w_queen_texture.loadFromFile("static/pieces/w-queen.png") &&
    b_queen_texture.loadFromFile("static/pieces/b-queen.png") &&
    w_bishop_texture.loadFromFile("static/pieces/w-bishop.png") &&
    b_bishop_texture.loadFromFile("static/pieces/b-bishop.png");
  }

  void placeInitialPieces() {
    // Place Pawns
    for (int j = 0; j < COL; j++) {
      Pieces[1][j] = createPiece(Pieces::PAWN, Pieces::WHITE);
      Pieces[6][j] = createPiece(Pieces::PAWN, Pieces::BLACK);
      Pieces[1][j]->setTexture(w_pawn_texture);
      Pieces[6][j]->setTexture(b_pawn_texture);
      centerPiece(*Pieces[1][j]->getSprite(), {1, j});
      centerPiece(*Pieces[6][j]->getSprite(), {6, j});
    }

    Pieces[0][0] = createPiece(Pieces::ROOK, Pieces::WHITE);
    Pieces[0][7] = createPiece(Pieces::ROOK, Pieces::WHITE);
    Pieces[7][0] = createPiece(Pieces::ROOK, Pieces::BLACK);
    Pieces[7][7] = createPiece(Pieces::ROOK, Pieces::BLACK);
    Pieces[0][0]->setTexture(w_rook_texture);
    Pieces[0][7]->setTexture(w_rook_texture);
    Pieces[7][0]->setTexture(b_rook_texture);
    Pieces[7][7]->setTexture(b_rook_texture);

    // Place Knights
    Pieces[0][1] = createPiece(Pieces::KNIGHT, Pieces::WHITE);
    Pieces[0][6] = createPiece(Pieces::KNIGHT, Pieces::WHITE);
    Pieces[7][1] = createPiece(Pieces::KNIGHT, Pieces::BLACK);
    Pieces[7][6] = createPiece(Pieces::KNIGHT, Pieces::BLACK);
    Pieces[0][1]->setTexture(w_knight_texture);
    Pieces[0][6]->setTexture(w_knight_texture);
    Pieces[7][1]->setTexture(b_knight_texture);
    Pieces[7][6]->setTexture(b_knight_texture);

    // Place Bishops
    Pieces[0][2] = createPiece(Pieces::BISHOP, Pieces::WHITE);
    Pieces[0][5] = createPiece(Pieces::BISHOP, Pieces::WHITE);
    Pieces[7][2] = createPiece(Pieces::BISHOP, Pieces::BLACK);
    Pieces[7][5] = createPiece(Pieces::BISHOP, Pieces::BLACK);
    Pieces[0][2]->setTexture(w_bishop_texture);
    Pieces[0][5]->setTexture(w_bishop_texture);
    Pieces[7][2]->setTexture(b_bishop_texture);
    Pieces[7][5]->setTexture(b_bishop_texture);

    // Place Queens
    Pieces[0][3] = createPiece(Pieces::QUEEN, Pieces::WHITE);
    Pieces[7][3] = createPiece(Pieces::QUEEN, Pieces::BLACK);
    Pieces[0][3]->setTexture(w_queen_texture);
    Pieces[7][3]->setTexture(b_queen_texture);

    // Place Kings
    Pieces[0][4] = createPiece(Pieces::KING, Pieces::WHITE);
    Pieces[7][4] = createPiece(Pieces::KING, Pieces::BLACK);
    Pieces[0][4]->setTexture(w_king_texture);
    Pieces[7][4]->setTexture(b_king_texture);

    for (int i = 0; i < ROW; i++) {
      for (int j = 0; j < COL; j++) {
        if (Pieces[i][j]) {
          centerPiece(*Pieces[i][j]->getSprite(), {i, j});
        }
      }
    }
  }

    std::unique_ptr<Pieces::Piece> createPiece(Pieces::Type type, Pieces::Color color) {
        switch (type) {
            case Pieces::PAWN: return std::make_unique<Pieces::Pawn>(color);
            case Pieces::QUEEN: return std::make_unique<Pieces::Queen>(color);
            case Pieces::KING: return std::make_unique<Pieces::King>(color);
            case Pieces::BISHOP: return std::make_unique<Pieces::Bishop>(color);
            case Pieces::KNIGHT: return std::make_unique<Pieces::Knight>(color);
            case Pieces::ROOK: return std::make_unique<Pieces::Rook>(color);
            default: return nullptr;
        }
    }

    void centerPiece(sf::Sprite& piece, const sf::Vector2<int>& position) {
        piece.setScale(static_cast<float>(squareSize) / piece.getTexture()->getSize().x,
                       static_cast<float>(squareSize) / piece.getTexture()->getSize().y);
        piece.setPosition(position.y * squareSize + (squareSize - piece.getGlobalBounds().width) / 2,
                          position.x * squareSize + (squareSize - piece.getGlobalBounds().height) / 2);
    }

    bool isValidIndex(const sf::Vector2<int>& index) const {
        return index.x >= 0 && index.x < ROW && index.y >= 0 && index.y < COL;
    }
};

#endif // _BOARD_HPP_

