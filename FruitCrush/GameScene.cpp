//
//  GameScene.cpp
//  FruitCrush
//
//  Created by ubaid on 24/09/2017.
//  Copyright (c) 2017 ubaid. All rights reserved.
//

#include "GameSceneImpl.h"
#include "SDL2/SDL.h"
#include <sstream>

GameScene::impl::impl(Renderer &r) :
renderer(r),
board(new GameBoard(r)),
gameStarted(false),
gameStartTime(0),
gameStopTime(0),
timeLeftSeconds(TIME_LIMIT),
score(0),
firstGame(true)
{
}

GameScene::impl::~impl()
{
}

void GameScene::impl::render(const unsigned int currentTime)
{
	renderer.clear();
	renderer.drawBackground(TID_BACKGROUND);

	renderer.setClipRect(BOARD_POS_X, BOARD_POS_Y, NUM_BLOCK_COLUMNS * BLOCK_SIZE_X, NUM_BLOCK_ROWS * BLOCK_SIZE_Y);

	board->applyToAllBlocks([&] (PiecePtr b) {
		b->simulate(currentTime);
		b->render(currentTime);
	});


	board->applyToAllBlocks([&] (PiecePtr b) {
		b->renderOverlay(currentTime);
	});

	renderer.resetClipRect();

	std::ostringstream timeStream;
	timeStream << "Time: "  << timeLeftSeconds;
	renderer.drawText(timeStream.str().c_str(), 25, 250);

	std::ostringstream scoreStream;
	scoreStream << "Score: "  << score;
	renderer.drawTextWithColor(scoreStream.str().c_str(), 400, 45, 255, 255, 0);

	renderer.present();
}

void GameScene::impl::runEventLoop()
{
	board->generate();

	bool quit = false;

	while(!quit)
	{
		unsigned int currentTime = SDL_GetTicks();

		if(pollEvents(currentTime))
		{
			quit = true;
		}

		simulate(currentTime);

		render(currentTime);
	}
}

GameScene::GameScene(Renderer &r)
{
	pimpl = std::unique_ptr<impl>(new impl(r));
}

GameScene::~GameScene()
{
}

void GameScene::runEventLoop()
{
	pimpl->runEventLoop();
}
