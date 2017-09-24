//
//  Matcher.h
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#ifndef _MATCHER_H
#define _MATCHER_H

class Matcher
{
	int						blockTypes[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];
	bool					blockKills[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];

	void initBlockTypes(const GameBoard &board);
	int getBlockType(int x, int y) const;
	bool findVerticalSequence(int x, int y) const;
	bool findHorizontalSequence(int x, int y) const;
public:
	Matcher(const GameBoard &board);

	void swapTypes(int srcX, int srcY, int dstX, int dstY);
	void calculateKills();
	bool hasMatches() const;
	bool findMatchAtPosition(int x, int y) const;
};

#endif
