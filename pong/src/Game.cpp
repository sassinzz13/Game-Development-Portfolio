#include "../include/Game.hpp"
#include <SDL2/SDL.h> 
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <iostream>
#include <stdio.h>

Game::Game()
    : mWindow(nullptr),
      mIsRunning(true),
      mRenderer(nullptr),
      mBallPos{WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f},
      mPaddlePos{WALL_THICKNESS, WINDOW_HEIGHT / 2.0f},
      mTicksCount(0),
      mPaddleDir(0),
      mBallVel{-200.0f, 235.0f}
{
}

// Initialize SDL and create the window
bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    //initialize the window 
    mWindow = SDL_CreateWindow(
        "Pongers", // title
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    //initialize the graphics window
    mRenderer = SDL_CreateRenderer(
    mWindow, // Window to create the renderer,  
    -1,  // Usually -1 = use first available rendering driver
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // Flags
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
   
    return true; 
}

// Render objects to the screen
void Game::GenerateOutput(){
  SDL_SetRenderDrawColor(mRenderer,0,0,255,255);
  SDL_RenderClear(mRenderer);
  SDL_SetRenderDrawColor(mRenderer,255,255,255,255);
  SDL_Rect wall {
    0,
    0,
    1024,
    WALL_THICKNESS
  };
  	SDL_RenderFillRect(mRenderer, &wall);
  	// Draw bottom wall
	wall.y = 500 - WALL_THICKNESS;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	wall.x = 1024 - WALL_THICKNESS;
	wall.y = 0;
	wall.w = WALL_THICKNESS;
	wall.h = 1024;
  SDL_RenderFillRect(mRenderer, &wall);

  //wall.x = 0;         // left edge
 // wall.y = 0;         // top
 // wall.w = WALL_THICKNESS; // width
 // wall.h = 768;       // height	

  // ball
  SDL_Rect ball {
    static_cast<int>(mBallPos.x - WALL_THICKNESS/2),
    static_cast<int>(mBallPos.y - WALL_THICKNESS/2),
    WALL_THICKNESS,
    WALL_THICKNESS
  };
  SDL_RenderFillRect(mRenderer, &ball);

  // paddle 
  SDL_Rect paddle {
    static_cast<int>(mPaddlePos.x - PADDLE_WIDTH/2),
    static_cast<int>(mPaddlePos.y - PADDLE_HEIGHT/2),
    PADDLE_WIDTH,
    PADDLE_HEIGHT
  };
  SDL_RenderFillRect(mRenderer, &paddle);
  SDL_RenderPresent(mRenderer); 
}

//this section looks confusing af
void Game::UpdateGame(){
  float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.f;
  mTicksCount = SDL_GetTicks();

  // frame limiting
  while(!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 10));

  //to stop delta time from jumping on stop
  if(deltaTime > 0.05f){
    deltaTime = 0.05f;
  }

  //Updates the paddle position
  if(mPaddleDir != 0){
    mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

    //bounderies so that the paddle doesnt go off bounds, basically collision 
    if(mPaddlePos.y < (PADDLE_HEIGHT/2.0f + WALL_THICKNESS)){
      mPaddlePos.y = PADDLE_HEIGHT/2.0f + WALL_THICKNESS;
    }
    else if(mPaddlePos.y > (WINDOW_HEIGHT - PADDLE_HEIGHT/2.0f - WALL_THICKNESS)){
      mPaddlePos.y = WINDOW_HEIGHT - PADDLE_HEIGHT/2.0f - WALL_THICKNESS;  
    }
  }
  mBallPos.x += mBallVel.x * deltaTime;
  mBallPos.y += mBallVel.y * deltaTime;

  //Calculate top wall 
  if(mBallPos.y <= WALL_THICKNESS && mBallVel.y < 0.0f){
    mBallVel.y *= -1.0f;
  }
  else if(mBallPos.y >= WINDOW_HEIGHT - WALL_THICKNESS && mBallVel.y > 0.0f){
    mBallVel.y *= -1.0f;
  }
  float diff = mBallPos.y - mPaddlePos.y;
  if(mBallPos.x - WALL_THICKNESS/2 <= mPaddlePos.x + PADDLE_WIDTH/2 &&
     mBallPos.x + WALL_THICKNESS/2 >= mPaddlePos.x - PADDLE_WIDTH/2 &&
     diff <= PADDLE_HEIGHT / 2.0f && diff >= -PADDLE_HEIGHT / 2.0f &&
     mBallVel.x < 0.0f){
    mBallVel.x *= -1.0f; // bounce
  }

  //right wall collission
  if(mBallPos.x >= WINDOW_WIDTH - WALL_THICKNESS && mBallVel.x > 0.0f){
    mBallVel.x *= -1.0f;
  }
  if(mBallPos.x + WALL_THICKNESS/2<0){
    std::cout << "Out of bounds" << std::endl;
    mIsRunning = false; // terminates
  }
}

void Game::ProcessInput(){
  //mPaddleDir = 0;
  const Uint8* state = SDL_GetKeyboardState(NULL);
  SDL_Event event;
  while(SDL_PollEvent(&event)){
    switch(event.type){
      case SDL_QUIT: mIsRunning = false;
      break;
    }
    if(state[SDL_SCANCODE_ESCAPE]){
      mIsRunning = false;
    }
    // adds controls to the paddle 
    if(state[SDL_SCANCODE_W]){
      mPaddleDir = -1;
    }
    if(state[SDL_SCANCODE_S]){
      mPaddleDir = 1;
    }
    /*
    restart
    if(state[SDL_SCANCODE_R]){
    mBallPos = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
    mBallVel = {-200.0f, 235.0f}; // reset to initial velocity
    mPaddlePos = {WALL_THICKNESS, WINDOW_HEIGHT / 2.0f};
    mPaddleDir = 0;
    }
    */
  }
}

// Shutdown SDL and destroy the window
void Game::Shutdown()
{
    //Destroy the window if it exist 
    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
    // Destroy the renderer if it exists
    if(mRenderer){
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }
    SDL_Quit();
}

// Main game loop
void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}


