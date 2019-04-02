#include "Game.h"

const int thickness = 15;		// 厚さ
const float paddleH = 100.0f;	// パドルの高さ

Game::Game()
{
	mWindow = nullptr;
	mIsRunning = true;
}

Game::~Game()
{
}

/// <summary>
/// 初期化
/// </summary>
/// <returns>成功:true, 失敗:false</returns>
bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("SDLを初期化できません：%s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Part1)",		// ウィンドウのタイトル
		100,	// ウィンドウ左上隅のx座標
		100,	// ウィンドウ左上隅のy座標
		1024,	// ウィンドウの幅（width）
		768,	// ウィンドウの高さ（height）
		0		// フラグ（設定しないときは0）
	);
	if (!mWindow)
	{
		SDL_Log("ウィンドウの作成に失敗しました：%s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,		// 作成するレンダラーの描画対象となるウィンドウ
		-1,				// デフォルトが[-1]
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer)
	{
		SDL_Log("レンダラーの作成に失敗しました：%s", SDL_GetError());
		return false;
	}

	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	mBallPos.x = 1024.0f / 2.0f;
	mBallPos.y = 768.0f / 2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;

	return true;
}

/// <summary>
/// メインループ
/// </summary>
void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();		// 初期化
		UpdateGame();		// 更新
		GenerateOutput();	// 出力
	}
}

/// <summary>
/// ゲームをシャットダウンする
/// </summary>
void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

/// <summary>
/// 入力
/// </summary>
void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// [X]ボタンを押下
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	// キーボードからの入力を取得する
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// [ESC]キーが押されていても、ループを終える
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}

	return;
}

/// <summary>
/// 更新
/// </summary>
void Game::UpdateGame()
{
	// 前のフレームから、16ms超過するまで待機する
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	// deltatimeは前のフレームとの時刻の差を秒に変換した値
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	mTicksCount = SDL_GetTicks();

	// デルタタイムを最大値で制限する
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// パドルの位置を移動
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

		// 画面上部の壁を超えないように調整
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		// 画面下部の壁を超えないように調整
		if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = (768.0f - paddleH / 2.0f - thickness);
		}
	}

	// ボールの移動
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	// 衝突判定
	float diff = mBallPos.y - mPaddlePos.y;
	// 絶対値
	diff = (diff > 0.0f) ? diff : -diff;
	// パドルとの衝突判定
	if (
		diff <= (paddleH / 2.0f)
		&& mBallPos.x <= 25.0f 
		&& mBallPos.x >= 20.0f
		&& mBallVel.x < 0.0f
		)
	{
		mBallVel.x *= -1.0f;
	}
	else if (mBallPos.x <= 0)
	{
		// ゲームオーバー
		mIsRunning = false;
	}
	// 画面上部の壁に衝突
	else if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		// Y軸方向に反転
		mBallVel.y *= -1;
	}
	// 画面下部の壁に衝突
	else if (mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f)
	{
		// Y軸方向に反転
		mBallVel.y *= -1;
	}
	//　画面右部の壁に衝突
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		// X軸方向に反転
		mBallVel.x *= -1;
	}

	return;
}

/// <summary>
/// 出力
/// </summary>
void Game::GenerateOutput()
{
	// 描画色を[青]に設定
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G
		255,	// B
		255		// A
	);
	// 描画色でクリアする
	SDL_RenderClear(mRenderer);

	// 壁の情報
	SDL_Rect wall{
		0,
		0,
		1024,
		thickness
	};

	// 描画色を[白]に変更
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	// 画面上部の壁を出力
	SDL_RenderFillRect(mRenderer, &wall);
	// 画面下部の壁を出力
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	// 画面右部の壁を出力
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.h = 768;
	wall.w = thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// ボールの情報
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness / 2),
		static_cast<int>(mBallPos.y - thickness / 2),
		thickness,
		thickness,
	};
	SDL_RenderFillRect(mRenderer, &ball);

	// パドルの情報
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH),
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	// フロントバッファとバックバッファの入れ替え
	SDL_RenderPresent(mRenderer);

	return;
}