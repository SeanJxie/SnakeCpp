#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define WIN_WT 1000
#define WIN_HT 1000

#define CELL_SIZE 25
#define MOVE_INTERVAL 25


bool in_2d_vec(std::vector<std::vector<int>> v1, std::vector<int> v2)
{
	for (auto e : v1)
	{
		if (e == v2)
		{
			return true;
		}
	}

	return false;
}

std::vector<std::vector<int>> no_head(std::vector<std::vector<int>> v)
{
	return std::vector<std::vector<int>>(v.begin() + 1, v.end());
}

int main(int argc, char* argv[])
{
	int nInitSuccess = SDL_Init(SDL_INIT_VIDEO);

	if (nInitSuccess != 0)
	{
		printf("Init Error: %s", SDL_GetError());
		return EXIT_FAILURE;
	}


	SDL_Window* pWin = SDL_CreateWindow("C++ Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WT, WIN_HT, SDL_WINDOW_RESIZABLE);

	if (pWin == NULL)
	{
		printf("Window Error: %s", SDL_GetError());
		return EXIT_FAILURE;
	}



	SDL_Renderer* pRend = SDL_CreateRenderer(pWin, -1, SDL_RENDERER_ACCELERATED);

	if (pRend == NULL)
	{
		printf("Init Error: %s", SDL_GetError());

		if (pWin != NULL)
		{
			SDL_DestroyWindow(pWin);
		}

		SDL_Quit();

		return EXIT_FAILURE;
	}

	 
	SDL_RenderSetScale(pRend, CELL_SIZE, CELL_SIZE);

	SDL_Event event;
	std::vector<std::vector<int>> snake = { { rand() % 40, rand() % 40 } };
	std::vector<int> apple = { rand() % 40, rand() % 40 }, end;

	// Make sure apple is not in snake
	while (apple[0] == snake[0][0] && apple[1] == snake[0][1])
	{
		apple = { rand() % 40, rand() % 40 };
	}

	bool hasMoved = false, bRunning = true;
	int nFrameCount = 0, moveDir = 0;

	Uint64 now = SDL_GetPerformanceCounter(), last = 0, deltaTime;

	while (bRunning)
	{
		last = now;
		
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					bRunning = false;
					break;

				case SDL_KEYDOWN:
					hasMoved = true;
					switch (event.key.keysym.sym)
					{
						case SDLK_UP:    if (moveDir != 2) moveDir = 1; break;
						case SDLK_DOWN:  if (moveDir != 1) moveDir = 2; break;
						case SDLK_LEFT:  if (moveDir != 4) moveDir = 3; break;
						case SDLK_RIGHT: if (moveDir != 3) moveDir = 4; break;
					}
			}
		}

		// Clear
		SDL_SetRenderDrawColor(pRend, 255, 255, 255, 255);
		SDL_RenderClear(pRend);

		// Render snake
		for (auto b : snake) 
		{
			SDL_SetRenderDrawColor(pRend, rand() % 256, rand() % 256, rand() % 256, 255);
			SDL_RenderDrawPoint(pRend, b[0], b[1]);
		}

		// Render apple
		SDL_SetRenderDrawColor(pRend, 255, 0, 0, 255);
		SDL_RenderDrawPoint(pRend, apple[0], apple[1]);
		
		// Update
		SDL_RenderPresent(pRend);

		// Delta time calculation
		now = SDL_GetPerformanceCounter();

		deltaTime = ((now - last) * 1000 / (double)SDL_GetPerformanceFrequency()) * 0.001;

		// Count frame
		nFrameCount += 1 - deltaTime;

		// Every move interval, update game state
		if (nFrameCount >= MOVE_INTERVAL)
		{
			// Check wall and self collision
			if (snake[0][0] <= 0 || snake[0][0] >= WIN_WT / CELL_SIZE || snake[0][1] <= 0 || snake[0][1] >= WIN_HT / CELL_SIZE || in_2d_vec(no_head(snake), snake[0]))
			{
				snake = { { rand() % 40, rand() % 40 } };
				apple = { rand() % 40, rand() % 40 }, end;

				// Make sure apple is not in snake
				while (apple[0] == snake[0][0] && apple[1] == snake[0][1])
				{
					apple = { rand() % 40, rand() % 40 };
				}

				hasMoved = false;
				bRunning = true;
				nFrameCount = 0;
				moveDir = 0;
			}


			// Move body of snake
			if (hasMoved)
			{
				for (int i = snake.size() - 1; i > 0; i--)
				{
					snake[i] = snake[i - 1];
				}
			}

			nFrameCount = 0;

			// Move head of snake
			switch (moveDir)
			{
				case 1: snake[0][1]--; break;
				case 2: snake[0][1]++; break;
				case 3: snake[0][0]--; break;
				case 4: snake[0][0]++; break;
			}

			// Check snake-apple collision
			if (apple[0] == snake[0][0] && apple[1] == snake[0][1])
			{
				// Add body based on move dir
				end = snake.back();
				switch (moveDir)
				{
					case 1: snake.push_back({ end[0], end[1] + 1 }); break;
					case 2: snake.push_back({ end[0], end[1] - 1 }); break;
					case 3: snake.push_back({ end[0] + 1, end[1] }); break;
					case 4: snake.push_back({ end[0] - 1, end[1] }); break;
				}

				// Reset apple pos
				apple = { rand() % 40, rand() % 40 };

				// Make sure apple is not in snake
				while (in_2d_vec(snake, apple))
				{
					apple = { rand() % 40, rand() % 40 };
				}
			}
		}
	}

	SDL_DestroyRenderer(pRend);
	SDL_DestroyWindow(pWin);
	SDL_Quit();

	return EXIT_SUCCESS;
}