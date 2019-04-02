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

	bool Initialize();			// ������
	void RunLoop();				// ���C�����[�v
	void Shutdown();			// �Q�[�����V���b�g�_�E������

private:
	void ProcessInput();		// ����
	void UpdateGame();			// �X�V
	void GenerateOutput();		// �o��

	SDL_Window *mWindow;		// �E�B���h�E
	SDL_Renderer *mRenderer;	// �����_��
	bool mIsRunning;			// �t���O

	Vector2 mPaddlePos;			// �p�h���̈ʒu
	Vector2 mBallPos;			// �{�[���̈ʒu
	Vector2 mBallVel;			// �{�[���̑��x
	Uint32 mTicksCount;			// �o�ߎ��Ԃ̕ێ�
	int mPaddleDir = 0;			// �p�h���̈ړ���
};