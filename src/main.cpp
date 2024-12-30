#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

const int LOG_VECTOR_SIZE = 20;
const int SIDE_PANEL_WIDTH = 400;
const int BOARD_WIDTH = 992;
const int BOARD_HEIGHT = 992;
const int SCREEN_WIDTH = BOARD_WIDTH + SIDE_PANEL_WIDTH;
const int SCREEN_HEIGHT = BOARD_HEIGHT;
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = BOARD_WIDTH / BOARD_SIZE;

enum PieceType
{
	PAWN = 1,
	ROOK = 2,
	KNIGHT = 3,
	BISHOP = 4,
	QUEEN = 5,
	KING = 6
};

enum BoardDirection
{
	VERTICAL = 1,
	HORIZONTAL = 2
};

void drawBoard(SDL_Renderer *renderer)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			SDL_Rect rect = {i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
			if ((i + j) % 2 == 0)
			{
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
			}
			SDL_RenderFillRect(renderer, &rect);
		}
	}
}

SDL_Texture *loadTexture(const std::string &file, SDL_Renderer *ren)
{
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr)
	{
		std::cerr << "LoadTexture Error: " << IMG_GetError() << std::endl;
	}
	return texture;
}

void renderPiecesInBoard(SDL_Renderer *renderer, SDL_Texture *pieces[12], int board[8][8], BoardDirection direction = VERTICAL)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] != 0)
			{
				if (direction == HORIZONTAL)
				{
					SDL_Rect rect = {i * SQUARE_SIZE, j * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
					SDL_RenderCopy(renderer, pieces[board[i][j] - 1], NULL, &rect);
				}
				else
				{
					SDL_Rect rect = {j * SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
					SDL_RenderCopy(renderer, pieces[board[i][j] - 1], NULL, &rect);
				}
			}
		}
	}
}

bool isValidMove2(int board[8][8], int startX, int startY, int endX, int endY)
{

	int piece = board[startX][startY];
	int target = board[endX][endY];

	// Ensure the move is within bounds
	if (startX < 0 || startX >= 8 || startY < 0 || startY >= 8 || endX < 0 || endX >= 8 || endY < 0 || endY >= 8)
	{
		return false;
	}

	// Ensure the piece is not moving to a square occupied by a piece of the same color
	if ((piece <= 6 && target <= 6 && target != 0) || (piece > 6 && target > 6))
	{
		return false;
	}

	// Ensure the piece is moving to a different square (not empty)
	if (piece == 0)
	{
		return false;
	}

	// Determine piece type and validate move accordingly
	switch (piece)
	{
	case 1: // Black Pawn
		if (startY == endY && startX - endX == 1 && target == 0)
			return true; // Move forward
		if (startY == endY && startX == 6 && startX - endX == 2 && target == 0)
			return true; // Double move from starting position
		if (abs(startY - endY) == 1 && startX - endX == 1 && target > 6)
			return true; // Capture
		break;
	// case 7: // White Pawn
	//     if (startY == endY && endX - startX == 1 && target == 0) return true; // Move forward
	//     if (startY == endY && startX == 1 && endX - startX == 2 && target == 0) return true; // Double move from starting position
	//     if (abs(startY - endY) == 1 && endX - startX == 1 && target <= 6 && target != 0) return true; // Capture
	//     break;
	// case 2: // Black Rook
	// case 8: // White Rook
	//     if (startX == endX || startY == endY) return true; // Move in straight lines
	//     break;
	// case 3: // Black Knight
	// case 9: // White Knight
	//     if ((abs(startX - endX) == 2 && abs(startY - endY) == 1) || (abs(startX - endX) == 1 && abs(startY - endY) == 2)) return true; // L-shaped move
	//     break;
	// case 4: // Black Bishop
	// case 10: // White Bishop
	//     if (abs(startX - endX) == abs(startY - endY)) return true; // Move diagonally
	//     break;
	// case 5: // Black Queen
	// case 11: // White Queen
	//     if (startX == endX || startY == endY || abs(startX - endX) == abs(startY - endY)) return true; // Move like rook or bishop
	//     break;
	// case 6: // Black King
	// case 12: // White King
	//     if (abs(startX - endX) <= 1 && abs(startY - endY) <= 1) return true; // Move one square in any direction
	//     break;
	default:
		return false;
	}

	return false;
}

bool isValidMove(int board[8][8], int startX, int startY, int endX, int endY)
{

	// check if a piece is being selected, set selected piece enum type
	// validate edge cases : if the move is within bounds
	// validate edge cases : if is unobstructed move
	// validate edge cases : if the move is not the same as the starting position
	// validate edge cases : if king is in check or mate before making the move
	// validate edge cases : if the move is not the same as the starting position

	return true;
}

bool isValidPawnMove(int board[8][8], int startX, int startY, int endX, int endY)
{
	// if init state validate 2 move
	// validate 2 move
	if (startY == endY && startX - endX == 2 && board[endX][endY] == 0)
		return true; // Double move from starting position

	// Validate
	// can move forward
	// diagonal capture
	// initial two-square advance.

	return false;
}

bool isValidMove3(int board[8][8], int startX, int startY, int endX, int endY)
{
	int piece = board[startX][startY];
	// int target = board[endX][endY];

	// Ensure the move is within bounds
	// Ensure the move is within bounds
	// if (startX < 0 || startX >= 8 || startY < 0 || startY >= 8 || endX < 0 || endX >= 8 || endY < 0 || endY >= 8) {
	//     return false;
	// }

	// // Ensure the start and end positions are not the same
	// if (startX == endX && startY == endY) {
	//     return false;
	// }

	// // Get the piece at the starting position
	// int piece = board[startY][startX];

	// // Ensure there is a piece to move
	// if (piece == 0) {
	//     return false;
	// }

	// Example simple move validation (can be extended for each piece type)
	// For this example, assume:
	// - Positive values represent one player's pieces
	// - Negative values represent the opponent's pieces
	// - 1 represents a pawn, 2 a knight, 3 a bishop, etc.

	switch (abs(piece))
	{
	case 1: // Pawn (example: simple forward movement)
		if (piece > 0)
		{ // White pawn
			return (endX == startX && endY == startY - 1 && board[endY][endX] == 0) ||
				   (endX == startX && startY == 6 && endY == startY - 2 && board[startY - 1][startX] == 0 && board[endY][endX] == 0);
		}
		else
		{ // Black pawn
			return (endX == startX && endY == startY + 1 && board[endY][endX] == 0) ||
				   (endX == startX && startY == 1 && endY == startY + 2 && board[startY + 1][startX] == 0 && board[endY][endX] == 0);
		}
	// case 2: // Knight
	//     return (abs(endX - startX) == 2 && abs(endY - startY) == 1) || (abs(endX - startX) == 1 && abs(endY - startY) == 2);
	// case 3: // Bishop
	//     return abs(endX - startX) == abs(endY - startY);
	// case 4: // Rook
	//     return (endX == startX || endY == startY);
	// case 5: // Queen
	//     return (abs(endX - startX) == abs(endY - startY)) || (endX == startX || endY == startY);
	// case 6: // King
	//     return abs(endX - startX) <= 1 && abs(endY - startY) <= 1;
	default:
		return false;
	}

	return true;
}

void movePiece(int board[8][8], int startX, int startY, int endX, int endY)
{
	if (isValidMove(board, startX, startY, endX, endY))
	{
		board[endX][endY] = board[startX][startY];
		board[startX][startY] = 0;
	}
}

void highlightMoves(SDL_Renderer *renderer, int board[8][8], int startX, int startY)
{
	for (int endX = 0; endX < 8; endX++)
	{
		for (int endY = 0; endY < 8; endY++)
		{
			SDL_Rect rect = {endX * SQUARE_SIZE, endY * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
			if (isValidMove(board, startX, startY, endX, endY))
			{
				SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128); // Blue for valid moves
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Red for invalid moves
			}
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
}

void drawLogSidePanel(SDL_Renderer *renderer, TTF_Font *font, const std::vector<std::string> &log)
{
	// Draw the side panel
	SDL_Rect sidePanel = {BOARD_WIDTH, 0, SIDE_PANEL_WIDTH, SCREEN_HEIGHT};
	// SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
	// SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark grey background
	SDL_RenderFillRect(renderer, &sidePanel);

	// Dummy text rendering placeholder (replace with SDL_ttf for real text rendering)
	SDL_Color color = {255, 255, 255}; // black
	// SDL_Color color = {0, 0, 0}; // white
	int yOffset = 10;
	for (const auto &message : log)
	{
		// Text rendering would go here using SDL_ttf library
		std::cout << "Log: " << message << std::endl; 
		
		int y = 0;
		for (const std::string &message : log)
		{
			SDL_Surface *surface = TTF_RenderText_Blended(font, message.c_str(), color);
			if (surface == nullptr)
			{
				std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
				SDL_Log("TTF_RenderText_Solid Error: %s", TTF_GetError());				
				continue;
			}

			SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
			if (texture == nullptr)
			{
				std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
				SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
				SDL_FreeSurface(surface);				
				continue;
			}

			SDL_Rect rect = {SCREEN_WIDTH + 10, yOffset, surface->w, surface->h};
			SDL_RenderCopy(renderer, texture, NULL, &rect);

			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);

			// y += 20;
			yOffset += 20;
		}
	}
}

void logEvent(std::vector<std::string> &log, const std::string &message)
{
	if (log.size() > LOG_VECTOR_SIZE)
	{
		log.erase(log.begin());
	}
	log.push_back(message);
}

bool InitLibraries(SDL_Renderer *&renderer, SDL_Window *&window, TTF_Font *&font, std::vector<std::string> &log)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		SDL_Log("SDL_Init Error: %s", SDL_GetError());
		return false;
	}

	if (TTF_Init() != 0)
	{
		std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
		SDL_Log("TTF_Init Error: %s", TTF_GetError());
		SDL_Quit();
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
		SDL_Log("IMG_Init Error: %s", IMG_GetError());
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	window = SDL_CreateWindow("Chess AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	font = TTF_OpenFont("res/fonts/Roboto-Regular.ttf", 16);
	if (font == nullptr)
	{
		std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		SDL_Log("TTF_OpenFont Error: %s", TTF_GetError());
		return false;
	}

	logEvent(log, "Initialized libraries successfully");

	return true;
}

int main(int argc, char *argv[])
{
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	std::vector<std::string> log = {};
	TTF_Font *font = nullptr;

	if (!InitLibraries(renderer, window, font, log))
	{
		std::cerr << "Failed to initialize libraries" << std::endl;
		return -1;
	}

	// TODO: main screen

	SDL_Texture *pieces[12];
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

	int board[8][8] = {
		{2, 3, 4, 5, 6, 4, 3, 2},
		{1, 1, 1, 1, 1, 1, 1, 1},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{7, 7, 7, 7, 7, 7, 7, 7},
		{8, 9, 10, 11, 12, 10, 9, 8}};

	// game loop
	bool running = true;
	SDL_Event event;
	bool pieceSelected = false;
	int selectedX = -1, selectedY = -1;

	while (running)
	{
		int mouseX, mouseY;
		int hoverX, hoverY = -1;
		//logEvent(log, "Mouse  at: (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")");		
		SDL_GetMouseState(&mouseX, &mouseY);

		// if (mouseX >= BOARD_WIDTH -1 || mouseX <= 0)
		// 	hoverX = - 1;
		// else
		// 	hoverX = mouseX / SQUARE_SIZE;

		// if (mouseY >= BOARD_HEIGHT -1 || mouseY <= 0)
		// 	mouseY = - 1;
		// else
		// 	hoverY = mouseY / SQUARE_SIZE;
		
		// if ((hoverX >= 0 && hoverX < 8) && (hoverY >= 0 && hoverY < 8))
		// 	logEvent(log, "Mouse hover at: (" + std::to_string(hoverX) + ", " + std::to_string(hoverY) + ")");
			

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				running = false;
			}
			// Handle mouse click events for moving pieces
			// if (!pieceSelected && board[startX][startY] != 0)
				// {
				// 	pieceSelected = true;
				// 	selectedX = startX;
				// 	selectedY = startY;
				// }
				// else if (pieceSelected)
				// {
				// 	movePiece(board, selectedX, selectedY, startX, startY);
				// 	pieceSelected = false;
				// 	selectedX = -1;
				// 	selectedY = -1;
				// }

				// if (pieceSelected)
				// {
				// 	highlightMoves(renderer, board, selectedX, selectedY);
				// 	SDL_RenderPresent(renderer);
				// }

				// Wait for the second click to get the destination
				//while (SDL_WaitEvent(&event))
				// while (SDL_WaitEventTimeout(&event, 1000*10)) // 10 seconds
				// {
				// 	if (event.type == SDL_MOUSEBUTTONDOWN)
				// 	{
				// 		SDL_GetMouseState(&x, &y);
				// 		int endX = x / SQUARE_SIZE;
				// 		int endY = y / SQUARE_SIZE;
				// 		//movePiece(board, startX, startY, endX, endY);
				// 		logEvent(log, "Mouse click end at: (" + std::to_string(startX) + ", " + std::to_string(startY) + ")");
				// 		break;
				// 	}
				// }
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		drawBoard(renderer);
		renderPiecesInBoard(renderer, pieces, board);
		drawLogSidePanel(renderer, font, log);

		// Create the players
		// Create the game loop
		// Create the game logic
		// Create the game rendering

		// Highlight the hovered block
		// SDL_Rect hoverRect = {hoverX * SQUARE_SIZE, hoverY * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
		// SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow with some transparency
		// SDL_RenderFillRect(renderer, &hoverRect);
		// // SDL_RenderDrawRect(renderer, &hoverRect);

		// renderPiecesInBoard(renderer, pieces, board);

		// clear log
		log.clear();

		SDL_RenderPresent(renderer);
		SDL_Delay(16); // 60FPS
	}

	for (int i = 0; i < 12; i++)
	{
		SDL_DestroyTexture(pieces[i]);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	return 0;
}