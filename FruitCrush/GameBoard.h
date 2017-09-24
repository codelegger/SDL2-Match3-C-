//
//  GameBoard.h
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#ifndef _GAME_BOARD_H_
#define _GAME_BOARD_H_

template<typename SrcT, typename DstT>
void mapTable(SrcT mapFrom[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS],
			  DstT mapTo[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS],
			  const std::function<DstT (const SrcT&)> &f)
{
	for(int i = 0; i < NUM_BLOCK_ROWS; ++i)
	{
		for(int j = 0; j < NUM_BLOCK_COLUMNS; ++j)
		{
			mapTo[i][j] = f(mapFrom[i][j]);
		}
	}
}

struct GameBoard
{
	Renderer				&renderer;
	std::mt19937			rng;

	PiecePtr				blocks[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];
	PiecePtr				mouseDownBlock;

	GameBoard(Renderer &renderer);

	void applyToAllBlocks(const std::function<void (PiecePtr&)> &f);
	PiecePtr findBlock(const std::function<bool (PiecePtr)> &predicate);

	template<typename T>
	void mapBlocks(T mapTo[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS],
		const std::function<T (const PiecePtr&)> &f) const
	{
		mapTable<const PiecePtr, T>(blocks, mapTo, f);
	}

//board logic
	void generate();
	//return number of blocks killed
	int simulateKills(unsigned int currentTime);
	void removeDeadBlocks();
	//check for falling blocks/new blocks
	void simulateFalling(unsigned int currentTime);
};

typedef std::shared_ptr<GameBoard> BoardPtr;

#endif