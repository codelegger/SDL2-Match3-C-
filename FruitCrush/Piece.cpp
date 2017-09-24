//
//  Piece.cpp
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#include "Piece.h"
#include <cmath>

const int BLOCK_MARK_TIME = 150;
const int BLOCK_MOVE_TIME = 300;
const int BLOCK_KILL_TIME = 300;
const double FALL_ACCELERATION = 25.0;		//board units per second squared

const double MAX_MARKER_OPACITY = 0.5;

enum class BlockState
{
	Normal,
	Moving,
	Falling,
	Disappearing,
	Dead,
};

enum class BlockMarkerState
{
	None,
	Marking,
	Unmarking,
	Marked
};

struct Piece::impl
{
	Renderer			&renderer;

	int					boardX;
	int					boardY;
	TextureID			texture;
	BlockState			state;

	BlockMarkerState	markerState;
	double				currentMarkerOpacity;
	unsigned int		markerChangeStartTime;
	bool				selected;

	unsigned int		moveStartTime;
	int					moveFromScreenX;
	int					moveFromScreenY;
	bool				moveOnTop;

	unsigned int		killStartTime;

	unsigned int		fallStartTime;
	int					fallFromScreenX;
	int					fallFromScreenY;

	void moveTo(unsigned int currentTime, int newBoardX, int newBoardY, bool topLayer = false);

	int getScreenXPos() const;
	int getScreenYPos() const;
	void renderMarker() const;
	void renderNormal() const;
	void renderMoving(const unsigned int currentTime) const;
	void renderDisappearing(const unsigned int currentTime) const;
	void renderFalling(const unsigned int currentTime) const;

	impl(Renderer &r);
	~impl();

//exposed methods
	void init(const int boardX, const int boardY, const TextureID texture);

	bool isInside(const int x, const int y) const;
	bool isSelected() const;
	bool isDead() const;
	bool canMove() const;
	bool isNeighbor(std::unique_ptr<impl> &block) const;

	int getBoardX() const;
	int getBoardY() const;
	int getType() const;
	void getSwapDirection(const int x, const int y, int &dx, int &dy) const;

	void mark(const unsigned int currentTime);
	void unmark(const unsigned int currentTime);
	void select(const unsigned int currentTime);
	void unselect(const unsigned int currentTime);
	void swapWith(const unsigned int currentTime, std::unique_ptr<impl> &block);
	void kill(const unsigned int currentTime);
	void fallTo(const unsigned int currentTime, const int targetX, const int targetY);
	void simulate(const unsigned int currentTime);

	void render(const unsigned int currentTime) const;
	void renderOverlay(const unsigned int currentTime) const;

};

Piece::impl::impl(Renderer &r) :
renderer(r),
markerState(BlockMarkerState::None),
currentMarkerOpacity(0.0),
markerChangeStartTime(0),
selected(false),
boardX(0),
boardY(0),
state(BlockState::Normal),
texture(TID_LAST),
moveStartTime(0),
moveFromScreenX(0),
moveFromScreenY(0),
killStartTime(0),
fallStartTime(0),
fallFromScreenX(0),
fallFromScreenY(0)
{
}

Piece::impl::~impl()
{
}

void Piece::impl::init(const int bX, const int bY, const TextureID tex)
{
	boardX = bX;
	boardY = bY;
	state = BlockState::Normal;
	texture = tex;
}

int Piece::impl::getScreenXPos() const
{
	return BOARD_POS_X + boardX * BLOCK_SIZE_X;
}

int Piece::impl::getScreenYPos() const
{
	return BOARD_POS_Y + boardY * BLOCK_SIZE_Y;
}

bool Piece::impl::isInside(const int x, const int y) const
{
	if(BlockState::Normal != state)
	{
		throw new NotImplementedException();
	}
	int posX = getScreenXPos();
	if(x < posX)
	{
		return false;
	}
	if(x >= posX + BLOCK_SIZE_X)
	{
		return false;
	}
	int posY = getScreenYPos();
	if(y < posY)
	{
		return false;
	}
	if(y >= posY + BLOCK_SIZE_Y)
	{
		return false;
	}
	return true;
}

bool Piece::impl::isSelected() const
{
	return selected;
}

bool Piece::impl::isDead() const
{
	return state == BlockState::Dead;
}

bool Piece::impl::canMove() const
{
	return BlockState::Normal == state;
}

bool Piece::impl::isNeighbor(std::unique_ptr<impl> &block) const
{
	int diffX = fabs(boardX - block->boardX);
	int diffY = fabs(boardY - block->boardY);
	return ((0 == diffX && 1 == diffY) || (1 == diffX && 0 == diffY));
}

int Piece::impl::getBoardX() const
{
	return boardX;
}

int Piece::impl::getBoardY() const
{
	return boardY;
}

int Piece::impl::getType() const
{
	if(state != BlockState::Normal)
	{
		return -1;
	}
	return texture;
}

void Piece::impl::getSwapDirection(const int x, const int y, int &dx, int &dy) const
{
	int posX = (int)(BOARD_POS_X + (boardX + 0.5) * BLOCK_SIZE_X);
	int posY = (int)(BOARD_POS_Y + (boardY + 0.5) * BLOCK_SIZE_Y);
	int pdx = x - posX;
	int pdy = y - posY;
	if(fabs(pdx) > fabs(pdy))
	{
		dy = 0;
		if(pdx > 0)
		{
			dx = 1;
		}
		else
		{
			dx = -1;
		}
	}
	else
	{
		dx = 0;
		if(pdy > 0)
		{
			dy = 1;
		}
		else
		{
			dy = -1;
		}
	}
}

void Piece::impl::mark(const unsigned int currentTime)
{
	if(BlockMarkerState::Marking == markerState  ||
		BlockMarkerState::Marked == markerState)
	{
		return;
	}
	if(BlockMarkerState::None == markerState)
	{
		markerChangeStartTime = currentTime;
	}
	if(BlockMarkerState::Unmarking == markerState)
	{
		markerChangeStartTime = currentTime - (unsigned int)(currentMarkerOpacity * BLOCK_MARK_TIME);
	}
	markerState = BlockMarkerState::Marking;
}

void Piece::impl::unmark(const unsigned int currentTime)
{
	if(BlockMarkerState::None == markerState  ||
		BlockMarkerState::Unmarking == markerState)
	{
		return;
	}
	if(selected)
	{
		return;
	}
	if(BlockMarkerState::Marked == markerState)
	{
		markerChangeStartTime = currentTime;
	}
	if(BlockMarkerState::Marking == markerState)
	{
		markerChangeStartTime = currentTime - (unsigned int)((1.0 - currentMarkerOpacity) * BLOCK_MARK_TIME);
	}
	markerState = BlockMarkerState::Unmarking;
}

void Piece::impl::select(const unsigned int currentTime)
{
	selected = true;
	mark(currentTime);
}

void Piece::impl::unselect(const unsigned int currentTime)
{
	selected = false;
	unmark(currentTime);
}

void Piece::impl::moveTo(const unsigned int currentTime, const int newBoardX,
						 const int newBoardY, const bool topLayer)
{
	unselect(currentTime);
	if(BlockState::Normal != state)
	{
		return;
	}
	moveStartTime = currentTime;
	state = BlockState::Moving;
	moveOnTop = topLayer;
	moveFromScreenX = getScreenXPos();
	moveFromScreenY = getScreenYPos();
	boardX = newBoardX;
	boardY = newBoardY;
}

void Piece::impl::swapWith(const unsigned int currentTime, std::unique_ptr<impl> &block)
{
	int oldBoardX = boardX;
	int oldBoardY = boardY;
	moveTo(currentTime, block->boardX, block->boardY, true);
	block->moveTo(currentTime, oldBoardX, oldBoardY, false);
}

void Piece::impl::kill(const unsigned int currentTime)
{
	unselect(currentTime);
	if(BlockState::Normal != state)
	{
		return;
	}
	killStartTime = currentTime;
	state = BlockState::Disappearing;
}

void Piece::impl::fallTo(const unsigned int currentTime, const int targetX, const int targetY)
{
	unselect(currentTime);
	if(BlockState::Normal != state)
	{
		return;
	}
	fallStartTime = currentTime;
	state = BlockState::Falling;
	fallFromScreenX = getScreenXPos();
	fallFromScreenY = getScreenYPos();
	boardX = targetX;
	boardY = targetY;
}

void Piece::impl::simulate(const unsigned int currentTime)
{
	if(BlockMarkerState::Marking == markerState)
	{
		if(currentTime > markerChangeStartTime + BLOCK_MARK_TIME)
		{
			markerState = BlockMarkerState::Marked;
			currentMarkerOpacity = 1.0;
		}
		else
		{
			currentMarkerOpacity = (currentTime - markerChangeStartTime)/(double)BLOCK_MARK_TIME;
		}
	}
	if(BlockMarkerState::Unmarking == markerState)
	{
		if(currentTime > markerChangeStartTime + BLOCK_MARK_TIME)
		{
			markerState = BlockMarkerState::None;
		}
		else
		{
			currentMarkerOpacity = 1.0 - (currentTime - markerChangeStartTime)/(double)BLOCK_MARK_TIME;
		}
	}
	if(BlockState::Moving == state)
	{
		if(currentTime > moveStartTime + BLOCK_MOVE_TIME)
		{
			state = BlockState::Normal;
		}
	}
	if(BlockState::Disappearing == state)
	{
		if(currentTime > killStartTime + BLOCK_KILL_TIME)
		{
			state = BlockState::Dead;
		}
	}
	if(BlockState::Falling == state)
	{
		double t = (currentTime - fallStartTime) * 0.001;
		double s = FALL_ACCELERATION * t * t * 0.5;
		int newY = (int)(fallFromScreenY + s * BLOCK_SIZE_Y);
		int posY = BOARD_POS_Y + boardY * BLOCK_SIZE_Y;
		if(newY >= posY)
		{
			state = BlockState::Normal;
		}
	}
}

void Piece::impl::render(const unsigned int currentTime) const
{
	renderMarker();
	if(BlockState::Normal == state)
	{
		renderNormal();
	}
	if(BlockState::Moving == state)
	{
		if(!moveOnTop)
		{
			renderMoving(currentTime);
		}
	}
	if(BlockState::Disappearing == state)
	{
		renderDisappearing(currentTime);
	}
	if(BlockState::Falling == state)
	{
		renderFalling(currentTime);
	}
}

void Piece::impl::renderOverlay(const unsigned int currentTime) const
{
	if(BlockState::Moving == state)
	{
		if(moveOnTop)
		{
			renderMoving(currentTime);
		}
	}
}

void Piece::impl::renderMarker() const
{
	if(BlockMarkerState::None == markerState)
	{
		return;
	}
	int posX = getScreenXPos();
	int posY = getScreenYPos();
	renderer.setColor(255, 255, 255, (unsigned char)(255 * currentMarkerOpacity * MAX_MARKER_OPACITY));
	renderer.drawFilledRectangle(posX, posY, BLOCK_SIZE_X, BLOCK_SIZE_Y);
}

void Piece::impl::renderNormal() const
{
	int posX = getScreenXPos();
	int posY = getScreenYPos();

	renderer.drawTextureCentered(texture, posX, posY, BLOCK_SIZE_X, BLOCK_SIZE_Y);
}

void Piece::impl::renderMoving(const unsigned int currentTime) const
{
	int destPosX = getScreenXPos();
	int destPosY = getScreenYPos();

	double lerpFactor = (currentTime - moveStartTime) / (double)BLOCK_MOVE_TIME;
	int dx = (int)((destPosX - moveFromScreenX) * lerpFactor);
	int dy = (int)((destPosY - moveFromScreenY) * lerpFactor);
	int posX = moveFromScreenX + dx;
	int posY = moveFromScreenY + dy;

	renderer.drawTextureCentered(texture, posX, posY, BLOCK_SIZE_X, BLOCK_SIZE_Y);
}

void Piece::impl::renderDisappearing(const unsigned int currentTime) const
{
	double scalingFactor = 1.0 - (currentTime - killStartTime) / (double)BLOCK_KILL_TIME;
	int posX = getScreenXPos();
	int posY = getScreenYPos();

	renderer.drawTextureCentered(texture, posX, posY, BLOCK_SIZE_X, BLOCK_SIZE_Y, scalingFactor);
}

void Piece::impl::renderFalling(const unsigned int currentTime) const
{
	double t = (currentTime - fallStartTime) * 0.001;
	double s = FALL_ACCELERATION * t * t * 0.5;
	int posX = BOARD_POS_X + boardX * BLOCK_SIZE_X;
	int posY = (int)(fallFromScreenY + s * BLOCK_SIZE_Y);

	renderer.drawTextureCentered(texture, posX, posY, BLOCK_SIZE_X, BLOCK_SIZE_Y);
}

Piece::Piece(Renderer &r)
{
	pimpl = std::unique_ptr<impl>(new impl(r));
}

Piece::~Piece()
{
}

void Piece::init(int boardX, int boardY, TextureID texture)
{
	pimpl->init(boardX, boardY, texture);
}

bool Piece::isInside(int x, int y) const
{
	return pimpl->isInside(x, y);
}

bool Piece::isSelected() const
{
	return pimpl->isSelected();
}

bool Piece::isDead() const
{
	return pimpl->isDead();
}

bool Piece::canMove() const
{
	return pimpl->canMove();
}

bool Piece::isNeighbor(PiecePtr block) const
{
	return pimpl->isNeighbor(block->pimpl);
}

int Piece::getBoardX() const
{
	return pimpl->getBoardX();
}

int Piece::getBoardY() const
{
	return pimpl->getBoardY();
}

int Piece::getType() const
{
	return pimpl->getType();
}

void Piece::getSwapDirection(const int x, const int y, int &dx, int &dy) const
{
	pimpl->getSwapDirection(x, y, dx, dy);
}

void Piece::mark(const unsigned int currentTime)
{
	pimpl->mark(currentTime);
}

void Piece::unmark(const unsigned int currentTime)
{
	pimpl->unmark(currentTime);
}

void Piece::select(const unsigned int currentTime)
{
	pimpl->select(currentTime);
}

void Piece::unselect(const unsigned int currentTime)
{
	pimpl->unselect(currentTime);
}

void Piece::swapWith(const unsigned int currentTime, PiecePtr block)
{
	pimpl->swapWith(currentTime, block->pimpl);
}

void Piece::kill(const unsigned int currentTime)
{
	pimpl->kill(currentTime);
}

void Piece::fallTo(const unsigned int currentTime, const int targetX, const int targetY)
{
	pimpl->fallTo(currentTime, targetX, targetY);
}

void Piece::simulate(const unsigned int currentTime)
{
	pimpl->simulate(currentTime);
}

void Piece::render(const unsigned int currentTime) const
{
	pimpl->render(currentTime);
}

void Piece::renderOverlay(const unsigned int currentTime) const
{
	pimpl->renderOverlay(currentTime);
}
