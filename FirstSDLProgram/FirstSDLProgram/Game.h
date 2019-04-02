#pragma once
#include <SDL.h>

class Game
{
	struct Vector2 {
		float x;
		float y;
	};

public:
	Game();
	~Game();

	bool Initialize();			// 初期化
	void RunLoop();				// メインループ
	void Shutdown();			// ゲームをシャットダウンする

private:
	void ProcessInput();		// 入力
	void UpdateGame();			// 更新
	void GenerateOutput();		// 出力

	SDL_Window *mWindow;		// ウィンドウ
	SDL_Renderer *mRenderer;	// レンダラ
	bool mIsRunning;			// フラグ

	Vector2 mPaddlePos;			// パドルの位置
	Vector2 mBallPos;			// ボールの位置
	Vector2 mBallVel;			// ボールの速度
	Uint32 mTicksCount;			// 経過時間の保持
	int mPaddleDir = 0;			// パドルの移動量
};