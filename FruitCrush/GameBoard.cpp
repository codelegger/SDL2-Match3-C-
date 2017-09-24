//
//  GameBoard.cpp
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#include "GameSceneImpl.h"
#include <ctime>
#include <random>
#include <memory>

GameBoard::GameBoard(Renderer &r) :
renderer(r)
{
	rng.seed((unsigned int)std::time(0));
}

void GameBoard::applyToAllBlocks(const std::function<void (PiecePtr&)> &f)
{
	for(int i = 0; i < NUM_BLOCK_ROWS; ++i)
	{
		for(int j = 0; j < NUM_BLOCK_COLUMNS; ++j)
		{
			if(!blocks[i][j])
			{
				continue;
			}
			f(blocks[i][j]);
		}
	}
}

PiecePtr GameBoard::findBlock(const std::function<bool (PiecePtr)> &predicate)
{
	for(int i = 0; i < NUM_BLOCK_ROWS; ++i)
	{
		for(int j = 0; j < NUM_BLOCK_COLUMNS; ++j)
		{
			if(!blocks[i][j])
			{
				continue;
			}
			if(predicate(blocks[i][j]))
			{
				return blocks[i][j];
			}
		}
	}
	return nullptr;
}

void GameBoard::generate()
{
	//std::uniform_int<>block_dist(TID_BLOCK_1, TID_BLOCK_1 + NUM_BLOCK_TYPES - 1);
    	for(int i = 0; i < NUM_BLOCK_ROWS; ++i)
	{
		for(int j = 0; j < NUM_BLOCK_COLUMNS; ++j)
		{
			PiecePtr block = std::make_shared<Piece>(renderer);
            int block_dist = rand()%NUM_BLOCK_TYPES + 1;

            block->init(j, i, (TextureID)block_dist);
			blocks[i][j] = block;
		}
	}
}

int GameBoard::simulateKills(const unsigned int currentTime)
{
	Matcher killCalculator(*this);
	killCalculator.calculateKills();

	int killCount = 0;
	applyToAllBlocks([&] (PiecePtr b) {
		if(killCalculator.findMatchAtPosition(b->getBoardX(), b->getBoardY()))
		{
			b->kill(currentTime);
			killCount++;
		}
	});
	return killCount;
}

void GameBoard::removeDeadBlocks()
{
	applyToAllBlocks([] (PiecePtr &b) {
		if(b->isDead())
		{
			b = nullptr;
		}
	});
}

void GameBoard::simulateFalling(const unsigned int currentTime)
{
	//std::uniform_int<>		block_dist(TID_BLOCK_1, TID_BLOCK_1 + NUM_BLOCK_TYPES - 1);

	int numBlocksGenerated[NUM_BLOCK_COLUMNS];
	for(int i = 0; i < NUM_BLOCK_COLUMNS; ++i)
	{
		numBlocksGenerated[i] = 0;
	}
	for(int i = 0; i < NUM_BLOCK_ROWS; ++i)
	{
		for(int j = 0; j < NUM_BLOCK_COLUMNS; ++j)
		{

			int reverseRowIndex = NUM_BLOCK_ROWS - i - 1;
			if(blocks[reverseRowIndex][j])
			{
				continue;
			}
			//search for block that can fall to this position
			bool blockFound = false;
			int testRow;
			for(testRow = reverseRowIndex - 1; testRow >= 0; --testRow)
			{
				if(!blocks[testRow][j])
				{
					continue;
				}
				if(blocks[testRow][j]->canMove())
				{
					blockFound = true;
					break;
				}
			}
			if(blockFound)
			{
				blocks[testRow][j]->fallTo(currentTime, j, reverseRowIndex);
				blocks[reverseRowIndex][j] = blocks[testRow][j];
				blocks[testRow][j] = nullptr;
			}
			else if(testRow == -1)
			{
				//generate new
				PiecePtr block(new Piece(renderer));
                int block_dist = rand()%NUM_BLOCK_TYPES + 1;
				block->init(j, -(numBlocksGenerated[j] + 1), (TextureID)block_dist);
				numBlocksGenerated[j]++;
				block->fallTo(currentTime, j, reverseRowIndex);
				blocks[reverseRowIndex][j] = block;
			}
		}
	}
}
