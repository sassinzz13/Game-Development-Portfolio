
#include <SDL2/SDL.h>

class Game {




public:
static constexpr int WINDOW_WIDTH = 1024;
static constexpr int WINDOW_HEIGHT = 500;
static constexpr int WALL_THICKNESS = 15;
static constexpr int PADDLE_WIDTH = WALL_THICKNESS;
static constexpr int PADDLE_HEIGHT = 100;
int mPaddleDir = 0;
int rightPaddleDir = 0;

  // container of coordinates 
struct Vector2{
  float x;
  float y;
};

  Game();

  //Initialize the Game
  bool Initialize();
  //Run game until its over
  void RunLoop();
  // Shutdowns the game
  void Shutdown();

private:
  Uint32 mTicksCount;
  //Helper functions
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();
  //Create window
  SDL_Window* mWindow;
  //The renderer that draws graphics
  SDL_Renderer* mRenderer;
  //Game should continue to run
  bool mIsRunning;

  Vector2 mPaddlePos;
  Vector2 mBallPos;
  Vector2 mBallVel;
  Vector2 mPaddleVel;
  Vector2 rightPaddlePos;
  Vector2 rightPaddleVel;
};
