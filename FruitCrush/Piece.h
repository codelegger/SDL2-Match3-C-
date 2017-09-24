//
//  Piece.h
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#ifndef _PIECE_H_
#define _PIECE_H_

#include "Renderer.h"

class Piece;
typedef std::shared_ptr<Piece> PiecePtr;
typedef std::shared_ptr<const Piece> ConstPiecePtr;

const int BLOCK_SIZE_X = 42;
const int BLOCK_SIZE_Y = 42;
const int BOARD_POS_X = 330;
const int BOARD_POS_Y = 105;

struct NotImplementedException : public std::exception
{
};

class Piece
{
	struct impl;
	std::unique_ptr<impl> pimpl;
public:
	Piece(Renderer &r);
	~Piece();

	void init(int boardX, int boardY, TextureID texture);

	bool isInside(int x, int y) const;
	bool isSelected() const;
	bool isDead() const;
	bool canMove() const;
	bool isNeighbor(PiecePtr block) const;

	int getBoardX() const;
	int getBoardY() const;
	//return some numeric block type or -1 for empty/inactive block
	int getType() const;
	//return best swap direction given mouse coordinates
	//sets (dx, dy) to one of (-1, 0), (1, 0), (0, -1), (0, 1)
	void getSwapDirection(const int x, const int y, int &dx, int &dy) const;

	void mark(unsigned int currentTime);
	void unmark(unsigned int currentTime);
	void select(unsigned int currentTime);
	void unselect(const unsigned int currentTime);

	void swapWith(unsigned int currentTime, PiecePtr block);
	void kill(unsigned int currentTime);
	void fallTo(unsigned int currentTime, const int targetX, const int targetY);

	void simulate(unsigned int currentTime);

	void render(unsigned int currentTime) const;
	void renderOverlay(unsigned int currentTime) const;
};


#endif