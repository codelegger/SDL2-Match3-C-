//
//  GameScene.h
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "Piece.h"

const int NUM_BLOCK_TYPES = 5;
const int NUM_BLOCK_COLUMNS = 8;
const int NUM_BLOCK_ROWS = 8;

class GameScene
{
	struct					impl;
	std::unique_ptr<impl>	pimpl;
public:
	GameScene(Renderer &r);
	~GameScene();

	void runEventLoop();
};

#endif