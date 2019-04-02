#include "Game.h"

const int thickness = 15;		// ����
const float paddleH = 100.0f;	// �p�h���̍���

Game::Game()
{
	mWindow = nullptr;
	mIsRunning = true;
}

Game::~Game()
{
}

/// <summary>
/// ������
/// </summary>
/// <returns>����:true, ���s:false</returns>
bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("SDL���������ł��܂���F%s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Part1)",		// �E�B���h�E�̃^�C�g��
		100,	// �E�B���h�E�������x���W
		100,	// �E�B���h�E�������y���W
		1024,	// �E�B���h�E�̕��iwidth�j
		768,	// �E�B���h�E�̍����iheight�j
		0		// �t���O�i�ݒ肵�Ȃ��Ƃ���0�j
	);
	if (!mWindow)
	{
		SDL_Log("�E�B���h�E�̍쐬�Ɏ��s���܂����F%s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,		// �쐬���郌���_���[�̕`��ΏۂƂȂ�E�B���h�E
		-1,				// �f�t�H���g��[-1]
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer)
	{
		SDL_Log("�����_���[�̍쐬�Ɏ��s���܂����F%s", SDL_GetError());
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
/// ���C�����[�v
/// </summary>
void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();		// ������
		UpdateGame();		// �X�V
		GenerateOutput();	// �o��
	}
}

/// <summary>
/// �Q�[�����V���b�g�_�E������
/// </summary>
void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

/// <summary>
/// ����
/// </summary>
void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// [X]�{�^��������
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	// �L�[�{�[�h����̓��͂��擾����
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// [ESC]�L�[��������Ă��Ă��A���[�v���I����
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
/// �X�V
/// </summary>
void Game::UpdateGame()
{
	// �O�̃t���[������A16ms���߂���܂őҋ@����
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	// deltatime�͑O�̃t���[���Ƃ̎����̍���b�ɕϊ������l
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	mTicksCount = SDL_GetTicks();

	// �f���^�^�C�����ő�l�Ő�������
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// �p�h���̈ʒu���ړ�
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

		// ��ʏ㕔�̕ǂ𒴂��Ȃ��悤�ɒ���
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		// ��ʉ����̕ǂ𒴂��Ȃ��悤�ɒ���
		if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = (768.0f - paddleH / 2.0f - thickness);
		}
	}

	// �{�[���̈ړ�
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	// �Փ˔���
	float diff = mBallPos.y - mPaddlePos.y;
	// ��Βl
	diff = (diff > 0.0f) ? diff : -diff;
	// �p�h���Ƃ̏Փ˔���
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
		// �Q�[���I�[�o�[
		mIsRunning = false;
	}
	// ��ʏ㕔�̕ǂɏՓ�
	else if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		// Y�������ɔ��]
		mBallVel.y *= -1;
	}
	// ��ʉ����̕ǂɏՓ�
	else if (mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f)
	{
		// Y�������ɔ��]
		mBallVel.y *= -1;
	}
	//�@��ʉE���̕ǂɏՓ�
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		// X�������ɔ��]
		mBallVel.x *= -1;
	}

	return;
}

/// <summary>
/// �o��
/// </summary>
void Game::GenerateOutput()
{
	// �`��F��[��]�ɐݒ�
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G
		255,	// B
		255		// A
	);
	// �`��F�ŃN���A����
	SDL_RenderClear(mRenderer);

	// �ǂ̏��
	SDL_Rect wall{
		0,
		0,
		1024,
		thickness
	};

	// �`��F��[��]�ɕύX
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	// ��ʏ㕔�̕ǂ��o��
	SDL_RenderFillRect(mRenderer, &wall);
	// ��ʉ����̕ǂ��o��
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	// ��ʉE���̕ǂ��o��
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.h = 768;
	wall.w = thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// �{�[���̏��
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness / 2),
		static_cast<int>(mBallPos.y - thickness / 2),
		thickness,
		thickness,
	};
	SDL_RenderFillRect(mRenderer, &ball);

	// �p�h���̏��
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH),
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	// �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@�̓���ւ�
	SDL_RenderPresent(mRenderer);

	return;
}