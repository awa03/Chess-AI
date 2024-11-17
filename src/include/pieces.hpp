#ifndef _PIECES_HPP_
#define _PIECES_HPP_

#include <SFML/Graphics.hpp>
#include <memory>

namespace Pieces {
  enum Type {
    PAWN, 
    ROOK,
    KNIGHT,
    KING,
    QUEEN,
    BISHOP
  };

  enum Color {
    WHITE,
    BLACK,
    NAC
  };

  class Piece {
  public:
    virtual ~Piece() = default;
    virtual void setTexture(const sf::Texture& t){
      sprite->setTexture(t);
    };
    virtual Color getColor() {
      return pieceColor;
    }

    virtual std::string getColorString() {
      return (pieceColor == Color::WHITE)? "WHITE" : "BLACK";
    }
    virtual Type getType(){
      return pieceType;
    }


    virtual std::string getTypeString() = 0;

    virtual void setSprite(sf::Sprite& sS) {
      sprite = std::make_unique<sf::Sprite>(sS);
    }

    sf::Sprite* getSprite() {
      return sprite.get();
    }

  bool hasMoved = false;
  protected: 
    std::unique_ptr<sf::Sprite> sprite;
    Type pieceType;
    Color pieceColor;
  };

  class Pawn : public Piece {
  public:
    explicit Pawn(Color sC) {
      pieceType = Type::PAWN; 
      pieceColor = sC; 
      sprite = std::make_unique<sf::Sprite>(); 
    }
    std::string getTypeString() override {
      return "PAWN";
    }
  };

  class King : public Piece {
  public:
    explicit King(Color sC) {
      pieceType = Type::KING;
      pieceColor = sC;
      sprite = std::make_unique<sf::Sprite>(); 
    }
    std::string getTypeString() override {
      return "KING";
    }
  };

  class Bishop : public Piece {
  public:
    explicit Bishop(Color sC) {
      pieceType = Type::BISHOP;
      pieceColor = sC;
      sprite = std::make_unique<sf::Sprite>(); 
    }
    std::string getTypeString() override {
      return "BISHOP";
    }
  };

  class Queen : public Piece {
  public:
    explicit Queen(Color sC) {
      pieceType = Type::QUEEN;
      pieceColor = sC;
      sprite = std::make_unique<sf::Sprite>(); 
    }
    std::string getTypeString() override {
      return "QUEEN";
    }
  };

  class Rook : public Piece {
  public:
    explicit Rook(Color sC) {
      pieceType = Type::ROOK;
      pieceColor = sC;
      sprite = std::make_unique<sf::Sprite>(); 
    }
    std::string getTypeString() override {
      return "ROOK";
    }
  };

  class Knight : public Piece {
  public:
    explicit Knight(Color sC) {
      pieceType = Type::KNIGHT;
      pieceColor = sC;
      sprite = std::make_unique<sf::Sprite>(); 
    }
    std::string getTypeString() override {
      return "KNIGHT";
    }
  };
}

#endif // _PIECES_HPP_

