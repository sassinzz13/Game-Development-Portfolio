#include "../include/Game.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <iostream>
#include <stdio.h>

// Constructor
Game::Game()
    : mWindow(nullptr),
      mIsRunning(true),
      mRenderer(nullptr),
      mBallPos{WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f},
      mPaddlePos{WALL_THICKNESS, WINDOW_HEIGHT / 2.0f},
      rightPaddlePos{WINDOW_WIDTH / 1.02f, WINDOW_HEIGHT / 2.0f},
      mTicksCount(0),
      mPaddleDir(0),
      rightPaddleDir(0),
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

    // Initialize the window
    mWindow = SDL_CreateWindow(
        "Pongers C++ Edition",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    // Initialize the renderer
    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    return true;
}

// Render objects to the screen
void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
    SDL_RenderClear(mRenderer);

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

    // Top wall
    SDL_Rect wall{
        0,
        0,
        1024,
        WALL_THICKNESS
    };
    SDL_RenderFillRect(mRenderer, &wall);

    // Bottom wall
    wall.y = 500 - WALL_THICKNESS;
    SDL_RenderFillRect(mRenderer, &wall);

    // Ball
    SDL_Rect ball{
        static_cast<int>(mBallPos.x - WALL_THICKNESS / 2),
        static_cast<int>(mBallPos.y - WALL_THICKNESS / 2),
        WALL_THICKNESS,
        WALL_THICKNESS
    };
    SDL_RenderFillRect(mRenderer, &ball);

    // Left paddle
    SDL_Rect paddle{
        static_cast<int>(mPaddlePos.x - PADDLE_WIDTH / 2),
        static_cast<int>(mPaddlePos.y - PADDLE_HEIGHT / 2),
        PADDLE_WIDTH,
        PADDLE_HEIGHT
    };
    SDL_RenderFillRect(mRenderer, &paddle);

    // Right paddle
    SDL_Rect paddleTwo{
        static_cast<int>(rightPaddlePos.x - PADDLE_WIDTH / 2),
        static_cast<int>(rightPaddlePos.y - PADDLE_HEIGHT / 2),
        PADDLE_WIDTH,
        PADDLE_HEIGHT
    };
    SDL_RenderFillRect(mRenderer, &paddleTwo);

    SDL_RenderPresent(mRenderer);
}

void Game::UpdateGame()
{
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.f;
    mTicksCount = SDL_GetTicks();

    // Frame limiting
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 10));

    // Prevent delta time from jumping on stop
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    // Right paddle movement
    if (rightPaddleDir != 0)
    {
        rightPaddlePos.y += rightPaddleDir * 300.0f * deltaTime;

        if (rightPaddlePos.y < (PADDLE_HEIGHT / 2.0f + WALL_THICKNESS))
        {
            rightPaddlePos.y = PADDLE_HEIGHT / 2.0f + WALL_THICKNESS;
        }
        else if (rightPaddlePos.y > (WINDOW_HEIGHT - PADDLE_HEIGHT / 2.0f - WALL_THICKNESS))
        {
            rightPaddlePos.y = WINDOW_HEIGHT - PADDLE_HEIGHT / 2.0f - WALL_THICKNESS;
        }
    }

    // Left paddle movement
    if (mPaddleDir != 0)
    {
        mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

        if (mPaddlePos.y < (PADDLE_HEIGHT / 2.0f + WALL_THICKNESS))
        {
            mPaddlePos.y = PADDLE_HEIGHT / 2.0f + WALL_THICKNESS;
        }
        else if (mPaddlePos.y > (WINDOW_HEIGHT - PADDLE_HEIGHT / 2.0f - WALL_THICKNESS))
        {
            mPaddlePos.y = WINDOW_HEIGHT - PADDLE_HEIGHT / 2.0f - WALL_THICKNESS;
        }
    }

    // Ball movement
    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;

    // Wall collision
    if (mBallPos.y <= WALL_THICKNESS && mBallVel.y < 0.0f)
    {
        mBallVel.y *= -1.0f;
    }
    else if (mBallPos.y >= WINDOW_HEIGHT - WALL_THICKNESS && mBallVel.y > 0.0f)
    {
        mBallVel.y *= -1.0f;
    }

    // Left paddle collision
    float diff = mBallPos.y - mPaddlePos.y;
    if (mBallPos.x - WALL_THICKNESS / 2 <= mPaddlePos.x + PADDLE_WIDTH / 2 &&
        mBallPos.x + WALL_THICKNESS / 2 >= mPaddlePos.x - PADDLE_WIDTH / 2 &&
        diff <= PADDLE_HEIGHT / 2.0f && diff >= -PADDLE_HEIGHT / 2.0f &&
        mBallVel.x < 0.0f)
    {
        mBallVel.x *= -1.0f; // bounce
    }

    // Right paddle collision
    float rightDiff = mBallPos.y - rightPaddlePos.y;
    if (mBallPos.x - WALL_THICKNESS / 2 <= rightPaddlePos.x + PADDLE_WIDTH / 2 &&
        mBallPos.x + WALL_THICKNESS / 2 >= rightPaddlePos.x - PADDLE_WIDTH / 2 &&
        rightDiff <= PADDLE_HEIGHT / 2.0f && rightDiff >= -PADDLE_HEIGHT / 2.0f &&
        mBallVel.x > 0.0f)
    {
        mBallVel.x *= -1.0f;
    }

    // Out of bounds (left)
    if (mBallPos.x + WALL_THICKNESS / 2 < 0)
    {
        std::cout << "Out of bounds (left)" << std::endl;
        mIsRunning = false;
    }

    // Out of bounds (right)
    if (mBallPos.x - WALL_THICKNESS / 2 > WINDOW_WIDTH)
    {
        std::cout << "Out of bounds (right)" << std::endl;
        mIsRunning = false;
    }
}

void Game::ProcessInput()
{
    const Uint8* state = SDL_GetKeyboardState(NULL);
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                mIsRunning = false;
                break;
        }

        if (state[SDL_SCANCODE_ESCAPE])
        {
            mIsRunning = false;
        }

        // Player 1 controls (W/S)
        if (state[SDL_SCANCODE_W])
        {
            mPaddleDir = -1;
        }
        if (state[SDL_SCANCODE_S])
        {
            mPaddleDir = 1;
        }

        // Player 2 controls (I/J)
        if (state[SDL_SCANCODE_I])
        {
            rightPaddleDir = -1;
        }
        if (state[SDL_SCANCODE_J])
        {
            rightPaddleDir = 1;
        }

        /*
        // Restart (optional)
        if (state[SDL_SCANCODE_R])
        {
            mBallPos = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
            mBallVel = {-200.0f, 235.0f};
            mPaddlePos = {WALL_THICKNESS, WINDOW_HEIGHT / 2.0f};
            mPaddleDir = 0;
        }
        */
    }
}

// Shutdown SDL and destroy the window
void Game::Shutdown()
{
    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    if (mRenderer)
    {
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
