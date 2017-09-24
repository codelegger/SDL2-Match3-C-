//
//  Matcher.cpp
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#include "GameSceneImpl.h"

Matcher::Matcher(const GameBoard &board)
{
	initBlockTypes(board);
}

void Matcher::initBlockTypes(const GameBoard &board)
{
	board.mapBlocks<int>(blockTypes, [](PiecePtr b) -> int {
		if(!b)
		{
			return -1;
		}
		return b->getType();
	});
}

int Matcher::getBlockType(int x, int y) const
{
	if(y < 0 || y >= NUM_BLOCK_ROWS ||
		x < 0 || x >= NUM_BLOCK_COLUMNS)
	{
		return -1;
	}
	return blockTypes[y][x];
}

void Matcher::swapTypes(int srcX, int srcY, int dstX, int dstY)
{
	std::swap(blockTypes[dstY][dstX], blockTypes[srcY][srcX]);
}

bool Matcher::findVerticalSequence(int x, int y) const
{
	for(int baseY = 0; baseY <= 2; ++baseY)
	{
		if((getBlockType(x, y + baseY - 2) == getBlockType(x, y + baseY - 1)) &&
			(getBlockType(x, y + baseY - 1) == getBlockType(x, y + baseY)))
		{
			return true;
		}
	}
	return false;
}

bool Matcher::findHorizontalSequence(int x, int y) const
{
	for(int baseX = 0; baseX <= 2; ++baseX)
	{
		if((getBlockType(x + baseX - 2, y) == getBlockType(x + baseX - 1, y)) &&
			(getBlockType(x + baseX - 1, y) == getBlockType(x + baseX, y)))
		{
			return true;
		}
	}
	return false;
}

void Matcher::calculateKills()
{
	for(int i = 0; i < NUM_BLOCK_ROWS; ++i)
	{
		for(int j = 0; j < NUM_BLOCK_COLUMNS; ++j)
		{
			blockKills[i][j] = false;
			if(getBlockType(j, i) == -1)
			{
				continue;
			}
			if(findHorizontalSequence(j, i) || findVerticalSequence(j, i))
			{
				blockKills[i][j] = true;
			}
		}
	}
}

bool Matcher::hasMatches() const
{
	for(int i = 0; i < NUM_BLOCK_ROWS; ++i)
	{
		for(int j = 0; j < NUM_BLOCK_COLUMNS; ++j)
		{
			if(blockKills[i][j])
			{
				return true;
			}
		}
	}
	return false;
}

bool Matcher::findMatchAtPosition(int x, int y) const
{
	return blockKills[y][x];
}
