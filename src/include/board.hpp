#ifndef _BOARD_HPP_
#define _BOARD_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "pieces.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

#define ENEMY_HIGHLIGHT sf::Color(255, 0, 0, 50)
#define CHECK_HIGHLIGHT sf::Color(255, 100, 255, 100)
#define CANMOVE_HIGHLIGHT sf::Color(0, 0, 255, 50)
#define SELECTED_HIGHLIGHT sf::Color(0, 255, 0, 50)

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
        Rects[i][j].setFillColor((i + j) % 2 == 0 ? sf::Color::White : sf::Color(100, 100, 100));
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
      highlightRects[selectedIndex.x][selectedIndex.y].setFillColor(SELECTED_HIGHLIGHT); // Highlight selected square
      std::vector<int> yDirVec = {-1, 1};
      int xDir = (piece->getColor() == Pieces::Color::WHITE) ? 1 : -1; // White moves up, Black moves down
      isInCheck(getKingPosition(getPlayerColor()), true);
      switch(piece->getType()){

        case Pieces::Type::PAWN:
          highlightPawnAttack(xDir, yDirVec);
          highlightPawnPossibleMoves(xDir);
          break;

        case Pieces::Type::ROOK:
          highlightRookPossibleMoves();
          break;

        case Pieces::Type::BISHOP:
          hightlightBishopPossibleMoves();
          break;

        case Pieces::Type::QUEEN:
          highlightQueenPossibleMoves();
          break;

        case Pieces::Type::KING:
          highlightKingPossibleMoves();
          break;

        case Pieces::Type::KNIGHT:
          highlightKnightPossibleMoves();
          break;

        default:
          break;
      }
    }
  }

  Pieces::Color getPlayerColor(){
    return (isWhiteSelected())? Pieces::Color::WHITE : Pieces::Color::BLACK;
  }

  // Check if the current players king is in check
  // Will find from the kings relative position if
  // any pieces are within scope for an attack
  //
  // So I was thinking in my sleep last night, 
  // I should refactor this to check for a king at a given position
  // not where the king currently is, and omg I already did it
  // LOL
  // Default will highlight
bool isInCheck(sf::Vector2<int> kingPos, bool highlight = false){

        
    Pieces::Color playerColor = getPlayerColor();

    // error if index invalid for king pos
    // should be impossible for this condition to run...
    if(kingPos.x == -1 || kingPos.y == -1){
      std::cerr << "error: king position not found" << std::endl;
      throw 404;
    }

    bool checkFlag = false;
    if(checkDiagonal(kingPos, playerColor, highlight)){
      std::cerr << "in check diagonal" << std::endl;
      checkFlag = true;
    }
    if(checkStraight(kingPos, playerColor, highlight)){
      std::cerr << "in check straight" << std::endl;
      checkFlag = true;
    }
    
    if(checkKnight(kingPos, playerColor, highlight)) {
      std::cerr << "in check by knight" << std::endl;
    }

    return checkFlag;
  }

  bool checkKnight(sf::Vector2<int> kingPos, Pieces::Color c, bool highlight = true){
    bool inCheckFlag = checkKnightHelper({}, kingPos, c, highlight);
    if(checkKnightHelper({2, 1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkKnightHelper({2, -1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkKnightHelper({-2, -1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkKnightHelper({-2, 1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkKnightHelper({1, 2}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkKnightHelper({-1, 2}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkKnightHelper({1, -2}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkKnightHelper({-1, -2}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    return inCheckFlag;
  }

  bool checkKnightHelper(sf::Vector2<int> off, sf::Vector2<int> kingPos, Pieces::Color c, bool highlight){
    auto checkPos = kingPos + off;
    if(!isValidIndex(checkPos)) return false;
    if(isSameColor(checkPos, c)) return false;
    if(!checkPosition(checkPos)) return false;
    if(Pieces[checkPos.x][checkPos.y]->getType() == Pieces::Type::KNIGHT){
      if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
      return true;
    }
    return false;
  }

  bool checkStraight(sf::Vector2<int> kingPos, Pieces::Color c, bool highlight = true){
    bool inCheckFlag = checkStraightHelper({1, 0}, kingPos, c, highlight);
    if(checkStraightHelper({0, 1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkStraightHelper({-1, 0}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkStraightHelper({0, -1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    return inCheckFlag;
  }

  bool checkStraightHelper(sf::Vector2<int> off, sf::Vector2<int> kingPos, Pieces::Color c, bool highlight){
    int depth = 0;
    auto checkPos = kingPos + off;
    bool flag = false;
    while(isValidIndex(checkPos)){
      if(isSameColor(checkPos, c)) return false;
      if(checkPosition(checkPos)){
        auto& piece = Pieces[checkPos.x][checkPos.y];
        if(piece->getType() == Pieces::Type::KING && depth == 0){
          if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
          flag = true;
        }
        else if(piece->getType() == Pieces::Type::QUEEN){
          if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
          flag = true;
        }
        else if(piece->getType() == Pieces::Type::ROOK){
          if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
          flag = true;
        }
        return flag;
      }
      checkPos += off;
      depth++;
    }
    return flag;
  }

  bool checkDiagonal(sf::Vector2<int> kingPos, Pieces::Color c, bool highlight = true){
    // TODO: Make one return -- my brain doesnt want to deal with this rn
    bool inCheckFlag = checkDiagonalHelper({1, 1}, kingPos, c, highlight);
    if(checkDiagonalHelper({1, -1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkDiagonalHelper({-1, 1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    if(checkDiagonalHelper({-1, -1}, kingPos, c, highlight) && !inCheckFlag) inCheckFlag = true;
    return inCheckFlag;
  }

  // make understandable to the model
  std::vector<std::vector<std::pair<char, Pieces::Color> > > getBoardChar() {
    std::vector<std::vector<std::pair<char, Pieces::Color> > > board;

    std::unordered_map<Pieces::Type, char> colorMap = {
      {Pieces::Type::KING, 'K'},
      {Pieces::Type::QUEEN, 'Q'},
      {Pieces::Type::ROOK, 'R'},
      {Pieces::Type::KNIGHT, 'N'},
      {Pieces::Type::BISHOP, 'B'},
      {Pieces::Type::PAWN, 'P'},
    };
    board.resize(8);
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        auto& piece = Pieces[i][j];
        if(piece == nullptr) {
          board[i].emplace_back('-', Pieces::Color::NAC);
        } else {
          auto characterRep = colorMap.find(piece->getType())->second;
          board[i].emplace_back(characterRep, piece->getColor());
        }
      }
    }
    return board;
  }

  bool checkDiagonalHelper(sf::Vector2<int> off, sf::Vector2<int> kingPos, Pieces::Color c, bool highlight){
    int depth = 0;
    bool flag = false;
    auto checkPos = kingPos + off;
    while(isValidIndex(checkPos)){
      if(isSameColor(checkPos, c)) return false;
      if(checkPosition(checkPos)){
        auto& piece = Pieces[checkPos.x][checkPos.y];
        if(piece->getType() == Pieces::Type::PAWN && depth == 0){
          if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
          flag = true;
        }
        if(piece->getType() == Pieces::Type::KING && depth == 0){
          if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
          flag = true;
        }
        else if(piece->getType() == Pieces::Type::QUEEN){
          if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
          flag = true;
        }
        else if(piece->getType() == Pieces::Type::BISHOP){
          if(highlight) highlightRects[checkPos.x][checkPos.y].setFillColor(CHECK_HIGHLIGHT);
          flag = true;
        }
        return flag;
      }
      checkPos += off;
      depth++;
    }
    return flag;
  }

  bool isSameColor(sf::Vector2<int> pieceIndex, Pieces::Color c){
    if(Pieces[pieceIndex.x][pieceIndex.y] == nullptr) return false;
    else return Pieces[pieceIndex.x][pieceIndex.y]->getColor() == c;
  }

  

  // Check to see if the kings in check helper
  // Used to see if an offset from the kings position
  // has an enemy on the tile 
  bool checkPosition(sf::Vector2<int> newPos){
    if(!isValidIndex(newPos)) return false;
    if(Pieces[newPos.x][newPos.y] == nullptr) return false;
    return true;
  }

  // Maybe useful -- check if correct piece is moving
  // (Pieces[selectedIndex.x + direction][selectedIndex.y + 1]->getColor() == Pieces::Color::WHITE) == whitesMove)
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
    // std::cout << "Whites Move: " << Pieces[selectedIndex.x][selectedIndex.y]->getColorString() << std::endl;
    if((
      (!isValidIndex(selectedIndex))
      || (Pieces[selectedIndex.x][selectedIndex.y] == nullptr)
      || ((Pieces[selectedIndex.x][selectedIndex.y]->getColor() == Pieces::Color::WHITE) != whitesMove))
    ){
      isSelected = false;
      return;
    }
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


    // TODO: REFACTOR FOR EARLY RETURNS
    // 
    // Checks if the place index is in check -- ensures cant move to check pos
    // if (isValidIndex(selectedIndex) && 
    //   isValidIndex(placeIndex) &&
    //   Pieces[placeIndex.x][placeIndex.y] != nullptr && 
    //   Pieces[selectedIndex.x][selectedIndex.y]->getColor() == Pieces[placeIndex.x][placeIndex.y]->getColor() && 
    //   Pieces[selectedIndex.x][selectedIndex.y]->getType() == Pieces::KING && 
    //   isInCheck(placeIndex, false)) 
    // {
    //   std::cout << "player is now in check! Cant move to this position" << std::endl;
    //   highlightRects[placeIndex.x][placeIndex.y].setFillColor(CHECK_HIGHLIGHT);  
    //   return;
    // }

    
    if((isValidIndex(selectedIndex) && isValidIndex(selectedIndex) && Pieces[placeIndex.x][placeIndex.y] != nullptr) && 
      (Pieces[selectedIndex.x][selectedIndex.y]->getColor() == Pieces[placeIndex.x][placeIndex.y]->getColor())) {
      selectedIndex = placeIndex;
      clearHighlights();
      highlightPossibleMoves();
      return;
    }

    else if(highlightRects[placeIndex.x][placeIndex.y].getFillColor() == ENEMY_HIGHLIGHT){
      std::cout << "player captured" << std::endl;   
    }
    else if(highlightRects[placeIndex.x][placeIndex.y].getFillColor() != CANMOVE_HIGHLIGHT){
      std::cout << "space invalid" << std::endl;
      return;
    }

    if (isValidIndex(selectedIndex) && isValidIndex(placeIndex)) {
      Pieces[selectedIndex.x][selectedIndex.y]->hasMoved = true;
      Pieces[placeIndex.x][placeIndex.y] = std::move(Pieces[selectedIndex.x][selectedIndex.y]);
      centerPiece(*Pieces[placeIndex.x][placeIndex.y]->getSprite(), placeIndex);
      // placeSound.play();
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
  sf::Vector2<int> wKingPos;
  sf::Vector2<int> bKingPos;
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

    // Place Rooks
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

    // Center all pieces on their squares
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

  // PAWN MOVE HIGHLIGHT -------------------------------------------------------------------------------------------------- // 
  void highlightPawnPossibleMoves(int xDir){
    if (
      isValidIndex({selectedIndex.x + xDir, selectedIndex.y}) 
      && (Pieces[selectedIndex.x][selectedIndex.y]->hasMoved == false)
      && (Pieces[selectedIndex.x + (xDir * 2)][selectedIndex.y] == nullptr)
      && (Pieces[selectedIndex.x + xDir][selectedIndex.y] == nullptr)
    ){
      highlightRects[selectedIndex.x + (xDir * 2)][selectedIndex.y].setFillColor(CANMOVE_HIGHLIGHT); // Highlight forward move
    }
    if (isValidIndex({selectedIndex.x + xDir, selectedIndex.y}) 
      && (Pieces[selectedIndex.x + xDir][selectedIndex.y] == nullptr)){
      highlightRects[selectedIndex.x + xDir][selectedIndex.y].setFillColor(CANMOVE_HIGHLIGHT); // Highlight forward move
    }
  }
  void highlightPawnAttack(int xDir, std::vector<int> yDirVec){
    int highX = selectedIndex.x + xDir;
    for(auto yDir : yDirVec){
      int highY = selectedIndex.y + yDir;
      if(isValidIndex({highX, highY}) && highlightPawnAttackHelper(highX, highY)){
        highlightRects[highX][highY].setFillColor(ENEMY_HIGHLIGHT); 
      }
    }
  }

  bool highlightPawnAttackHelper(int highX, int highY){
    if(!isValidIndex({highX, highY})){return false;}
    if(Pieces[highX][highY] == nullptr){return false;}
    return (Pieces[highX][highY]->getColor() != Pieces[selectedIndex.x][selectedIndex.y]->getColor());
  }


  void highlightKnightPossibleMoves(){
    // up
    highlightDepthOne(2, 1);
    highlightDepthOne(2, -1);

    // down
    highlightDepthOne(-2, 1);
    highlightDepthOne(-2, -1);

    // right
    highlightDepthOne(1, 2);
    highlightDepthOne(-1, 2);

    // left
    highlightDepthOne(1, -2);
    highlightDepthOne(-1, -2);


  }
  
  void highlightKingPossibleMoves(){
    highlightDepthOne(1, 0);
    highlightDepthOne(-1, 0);
    highlightDepthOne(0, 1);
    highlightDepthOne(0, -1);
    highlightDepthOne(-1, 1);
    highlightDepthOne(-1, -1);
    highlightDepthOne(1, -1);
    highlightDepthOne(1, 1);
  }

  void hightlightBishopPossibleMoves(){
    hightlightAllAvailableDiagonal();
  }

  void highlightQueenPossibleMoves(){
    hightlightAllAvailableDiagonal();
    highlightAllAvailableStraight();
  }

  void highlightRookPossibleMoves(){ 
    highlightAllAvailableStraight();
  }

  void hightlightAllAvailableDiagonal(){
    highlightAvailableStraightHelper(1, 1); 
    highlightAvailableStraightHelper(-1, -1); 
    highlightAvailableStraightHelper(-1, 1); 
    highlightAvailableStraightHelper(1, -1); 
  }

  void highlightAllAvailableStraight() {
    highlightAvailableStraightHelper(1, 0); 
    highlightAvailableStraightHelper(-1, 0); 
    highlightAvailableStraightHelper(0, 1); 
    highlightAvailableStraightHelper(0, -1); 
  }

  void highlightAvailableStraightHelper(int x, int y){
    int xOff = x;
    int yOff = y;
    while(isValidIndex({selectedIndex.x + xOff, selectedIndex.y +yOff})){
      auto& piece = Pieces[selectedIndex.x + xOff][selectedIndex.y + yOff];
      auto& highRect = highlightRects[selectedIndex.x + xOff][selectedIndex.y + yOff];
      if(piece == nullptr){
        highRect.setFillColor(CANMOVE_HIGHLIGHT); 
      }
      else if((piece->getColor() == Pieces::Color::WHITE) != isWhiteSelected()){
        highRect.setFillColor(ENEMY_HIGHLIGHT);
        break;
      }
      else {
        break;
      }
      xOff+=x;
      yOff+= y;
    }
  }

  void highlightDepthOne(int xOff, int yOff){
    if(!isValidIndex({selectedIndex.x + xOff, selectedIndex.y +yOff})) return; 
    auto& piece = Pieces[selectedIndex.x + xOff][selectedIndex.y + yOff];
    auto& highRect = highlightRects[selectedIndex.x + xOff][selectedIndex.y + yOff];
    if(piece == nullptr){
      highRect.setFillColor(CANMOVE_HIGHLIGHT); 
    }
    else if((piece->getColor() == Pieces::Color::WHITE) != isWhiteSelected()){
      highRect.setFillColor(ENEMY_HIGHLIGHT);
    }
  }

  // Working as expected
  // find the kings position within a 2d array
  // This will be deprecated soon for performance
  // Reduce O(m * n) to O(1)
  sf::Vector2<int> getKingPosition(Pieces::Color playerColor){
    // Implement Tommorow
    // auto& wPos = Pieces[wKingPos.x][wKingPos.y];
    // auto& bPos = Pieces[bKingPos.x][bKingPos.y];
    // if((wPos->getType() == Pieces::Type::KING) && (isSameColor(wKingPos, Pieces::Color::BLACK))){
    //   return wKingPos;
    // }
    // else if((bPos->getType() == Pieces::Type::KING) && (isSameColor(bKingPos, Pieces::Color::BLACK))){
    //   return bKingPos;
    // }

    // Should never run but just in case -- ? 
    for(int rowIndex = 0; rowIndex < ROW; rowIndex++){
      for(int colIndex =0; colIndex < COL; colIndex++){
        if(Pieces[rowIndex][colIndex] != nullptr && 
          Pieces[rowIndex][colIndex]->getColor() == playerColor &&
          Pieces[rowIndex][colIndex]->getType() == Pieces::Type::KING){
          return {rowIndex, colIndex};
        }
      }
    }
    return {-1, -1};
  }
};

#endif // _BOARD_HPP_

