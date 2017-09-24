//
//  GameSceneImpl.h
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#ifndef _BOARD_IMPL_H_
#define _BOARD_IMPL_H_

#include <random>
#include <functional>

#include "GameScene.h"
#include "GameBoard.h"
#include "Matcher.h"

const int TIME_LIMIT = 60;
const int POST_GAME_TIME = 1;

struct GameScene::impl
{
	Renderer				&renderer;

	BoardPtr				board;

	bool					gameStarted;
	unsigned int			gameStartTime;
	unsigned int			gameStopTime;
	int						timeLeftSeconds;
	int						score;
	bool					firstGame;

	impl(Renderer &r);
	~impl();

//game logic
	bool tryGameStart(unsigned int currentTime);
	void simulate(unsigned int currentTime);

//rendering
	void render(unsigned int currentTime);

//user input processing
	bool trySwap(unsigned int currentTime, PiecePtr src, PiecePtr dst);
	PiecePtr getSelectedBlock();

	void processMouseMotion(unsigned int currentTime, int x, int y);
	void processMouseDown(unsigned int currentTime, int x, int y);
	//simple block click (mouse down and up in the same block)
	void processBlockClick(unsigned int currentTime);
	//mouse up outside of mouse down block
	void processBlockDrag(unsigned int currentTime, int x, int y);
	void processMouseUp(unsigned int currentTime, int x, int y);
	bool pollEvents(unsigned int currentTime);

//main game loop
	void runEventLoop();
};



#endif