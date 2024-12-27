#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 992; //  
const int SCREEN_HEIGHT = 992; // 

void drawBoard(SDL_Renderer* renderer) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			SDL_Rect rect = {i * 124, j * 124, 124, 124};
			if ((i + j) % 2 == 0) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
			}
			SDL_RenderFillRect(renderer, &rect);
		}
	}
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {
    SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
    if (texture == nullptr) {
        std::cerr << "LoadTexture Error: " << IMG_GetError() << std::endl;
    }
    return texture;
}

void renderPiecesInBoard(SDL_Renderer* renderer, SDL_Texture* pieces[12], int board[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != 0) {
                SDL_Rect rect = {i * 124, j * 124, 124, 124};
                SDL_RenderCopy(renderer, pieces[board[i][j] - 1], NULL, &rect);
            }
        }
    }
}


int main(int argc, char* argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		SDL_Log("SDL_Init Error: %s", SDL_GetError());
		return 1;
	}

	if (TTF_Init() != 0) {
		std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
		SDL_Log("TTF_Init Error: %s", TTF_GetError());
		SDL_Quit();
		return 1;
	}
	
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
		SDL_Log("IMG_Init Error: %s", IMG_GetError());
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Chess AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	// TODO: main screen 
	// init game pieces
	SDL_Texture* pieces[12];
	pieces[0] = loadTexture("res/pieces-svg/pawn-b.svg", renderer);
	pieces[1] = loadTexture("res/pieces-svg/rook-b.svg", renderer);
	pieces[2] = loadTexture("res/pieces-svg/knight-b.svg", renderer);
	pieces[3] = loadTexture("res/pieces-svg/bishop-b.svg", renderer);
	pieces[4] = loadTexture("res/pieces-svg/queen-b.svg", renderer);
	pieces[5] = loadTexture("res/pieces-svg/king-b.svg", renderer);	
	pieces[6] = loadTexture("res/pieces-svg/pawn-w.svg", renderer);
	pieces[7] = loadTexture("res/pieces-svg/rook-w.svg", renderer);
	pieces[8] = loadTexture("res/pieces-svg/knight-w.svg", renderer);
	pieces[9] = loadTexture("res/pieces-svg/bishop-w.svg", renderer);
	pieces[10] = loadTexture("res/pieces-svg/queen-w.svg", renderer);
	pieces[11] = loadTexture("res/pieces-svg/king-w.svg", renderer);
	



	// pieces[6] = loadTexture("res/pieces-svg/pawn-w.svg", renderer);
	// pieces[7] = loadTexture("res/pieces-svg/pawn-w.svg", renderer);
	// pieces[8] = loadTexture("res/pieces-svg/pawn-w.svg", renderer);
	// pieces[9] = loadTexture("res/pieces-svg/pawn-w.svg", renderer);
	// pieces[10] = loadTexture("res/pieces-svg/pawn-w.svg", renderer);
	// pieces[11] = loadTexture("res/pieces-svg/pawn-w.svg", renderer);
	

    //pieces[0] = loadTexture("res/pieces-svg/bpawn-b.svg", renderer);	
    //pieces[1] = loadTexture("res/pieces-png/black-amazon.png", renderer);	
    // pieces[2] = loadTexture("src/pieces/black-knight.png", renderer);
    // pieces[3] = loadTexture("src/pieces/black-bishop.png", renderer);
    // pieces[4] = loadTexture("src/pieces/black-queen.png", renderer);
    // pieces[5] = loadTexture("src/pieces/black-king.png", renderer);
    // pieces[6] = loadTexture("src/pieces/white-pawn.png", renderer);
    // pieces[7] = loadTexture("src/pieces/white-rook.png", renderer);
    // pieces[8] = loadTexture("src/pieces/white-knight.png", renderer);
    // pieces[9] = loadTexture("src/pieces/white-bishop.png", renderer);
    // pieces[10] = loadTexture("src/pieces/white-queen.png", renderer);
    // pieces[11] = loadTexture("src/pieces/white-king.png", renderer);

    int board[8][8] = {
        {2, 3, 4, 5, 6, 4, 3, 2},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {7, 7, 7, 7, 7, 7, 7, 7},
        {8, 9, 10, 11, 12, 10, 9, 8}
    };

	//game loop
	bool running = true;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
				running = false;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		drawBoard(renderer);
		renderPiecesInBoard(renderer, pieces, board);
		// Create the pieces

		// Create the players
		// Create the game loop
		// Create the game logic
		// Create the game rendering

		SDL_RenderPresent(renderer);
		SDL_Delay(16); // 60FPS
	}

	for (int i = 0; i < 12; i++) {
        SDL_DestroyTexture(pieces[i]);
    }
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return 0;
}