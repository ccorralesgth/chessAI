#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>

const int LOG_VECTOR_SIZE = 20;
const int SIDE_PANEL_WIDTH = 400;
const int BOARD_WIDTH = 1000;
const int BOARD_HEIGHT = 1000;
const int SCREEN_WIDTH = BOARD_WIDTH; // + SIDE_PANEL_WIDTH;
const int SCREEN_HEIGHT = BOARD_HEIGHT;
const int BOARD_SIZE = 8;
const int TILE_SIZE = BOARD_WIDTH / BOARD_SIZE;

enum PieceType
{
	BLACK_PAWN = 1,
	BLACK_ROOK = 2,
	BLACK_KNIGHT = 3,
	BLACK_BISHOP = 4,
	BLACK_QUEEN = 5,
	BLACK_KING = 6,
	WHITE_PAWN = 7,
	WHITE_ROOK = 8,
	WHITE_KNIGHT = 9,
	WHITE_BISHOP = 10,
	WHITE_QUEEN = 11,
	WHITE_KING = 12,
};

// Function to initialize SDL
bool init(SDL_Window *&window, SDL_Renderer *&renderer, TTF_Font *&font)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	if (TTF_Init() == -1)
	{
		std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
		return false;
	}

	font = TTF_OpenFont("res/fonts/Roboto-Regular.ttf", 32);
	if (!font)
	{
		std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
		return -1;
	}

	return true;
}

// Function to render text using SDL_ttf
void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, int x, int y, bool centeredX = true, bool centeredY = true)
{
	if (!font)
	{
		std::cerr << "Error: Font is NULL. Cannot render text." << std::endl;
		return;
	}

	SDL_Color white = {255, 255, 255, 255}; // Text color
	SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), white);
	if (!surface)
	{
		std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
		return;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(surface);
		return;
	}

	int textWidth = surface->w;
	int textHeight = surface->h;
	SDL_FreeSurface(surface);

	SDL_Rect destRect = {x, y, textWidth, textHeight};
	if (centeredX)
		destRect.x -= textWidth / 2;
	if (centeredY)
		destRect.y -= textHeight / 2;

	SDL_RenderCopy(renderer, texture, nullptr, &destRect);
	SDL_DestroyTexture(texture);
}

// Function to render the game menu
void renderMenu(SDL_Renderer *renderer, TTF_Font *font)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Black with 50% opacity
	SDL_Rect menuBackground = {100, 100, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 200};
	SDL_RenderFillRect(renderer, &menuBackground);

	// Render menu options
	renderText(renderer, font, "Resume Game", SCREEN_WIDTH / 2, 200);
	renderText(renderer, font, "Restart Game", SCREEN_WIDTH / 2, 250);
	renderText(renderer, font, "Settings", SCREEN_WIDTH / 2, 300);
	renderText(renderer, font, "Help", SCREEN_WIDTH / 2, 350);
	renderText(renderer, font, "Exit", SCREEN_WIDTH / 2, 400);
}

// Function to draw the chessboard
void renderBoard(SDL_Renderer *renderer)
{
	bool isLightTile = true;
	for (int row = 0; row < 8; ++row)
	{
		for (int col = 0; col < 8; ++col)
		{
			SDL_Rect tile = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};
			if (isLightTile)
			{
				SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255);
			}
			SDL_RenderFillRect(renderer, &tile);
			isLightTile = !isLightTile;
		}
		isLightTile = !isLightTile;
	}
}

bool isPointInRect(int x, int y, SDL_Rect rect)
{
	return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

// Function to load a texture
SDL_Texture *loadTexture(const std::string &file, SDL_Renderer *ren)
{
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr)
	{
		std::cerr << "LoadTexture Error: " << IMG_GetError() << std::endl;
	}
	return texture;
}

void renderPiecesInBoard(SDL_Renderer *renderer, SDL_Texture *pieces[12], int board[8][8])
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] != 0)
			{
				SDL_Rect rect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
				SDL_RenderCopy(renderer, pieces[board[i][j] - 1], NULL, &rect);
			}
		}
	}
}

// Function to draw chess notation on the board borders
void renderBoardNotation(SDL_Renderer *renderer, TTF_Font *font)
{
	// TODO: for better performance, we can add a notation flag on the renderBoard function and iterate only once
	// while rendering the board and the notation
	SDL_Color color;

	// Draw rank numbers (1-8) along the left side
	for (int row = 0; row < 8; ++row)
	{
		std::string rank = std::to_string(8 - row);
		if (row % 2 == 0)
		{
			color = {118, 150, 86, 255};
		}
		else
		{
			color = {238, 238, 210, 255};
		}
		// SDL_Surface* textSurface = TTF_RenderText_Solid(font, rank.c_str(), white);
		SDL_Surface *textSurface = TTF_RenderText_Blended(font, rank.c_str(), color);
		SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

		// Get text dimensions
		int textWidth, textHeight;
		SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);

		// Position text
		// SDL_Rect destRect = {5, row * TILE_SIZE + TILE_SIZE / 2 - textHeight / 2, textWidth, textHeight};
		SDL_Rect destRect = {5, row * TILE_SIZE + 5, textWidth, textHeight};
		SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);

		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(textTexture);
	}

	// Draw file letters (a-h) along the bottom
	for (int col = 0; col < 8; ++col)
	{
		char file = 'a' + col;
		if (col % 2 == 0)
		{
			color = {238, 238, 210, 255};
		}
		else
		{
			color = {118, 150, 86, 255};
		}
		std::string fileStr(1, file);
		SDL_Surface *textSurface = TTF_RenderText_Solid(font, fileStr.c_str(), color);
		SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

		// Get text dimensions
		int textWidth, textHeight;
		SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);

		// Position text
		SDL_Rect destRect = {col * TILE_SIZE + 5, SCREEN_HEIGHT - textHeight - 5, textWidth, textHeight};
		SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);

		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(textTexture);
	}
}

// Function to log selected pieces on the board [when dragging]
void logSelectedPiece(int pieceSelected)
{
	// PieceType pieceType = static_cast<PieceType>(board[row][col]);
	switch (pieceSelected)
	{
	case 1:
		std::cout << "Selected piece: Black Pawn" << std::endl;
		break;
	case 2:
		std::cout << "Selected piece: Black Rook" << std::endl;
		break;
	case 3:
		std::cout << "Selected piece: Black Knight" << std::endl;
		break;
	case 4:
		std::cout << "Selected piece: Black Bishop" << std::endl;
		break;
	case 5:
		std::cout << "Selected piece: Black Queen" << std::endl;
		break;
	case 6:
		std::cout << "Selected piece: Black King" << std::endl;
		break;
	case 7:
		std::cout << "Selected piece: White Pawn" << std::endl;
		break;
	case 8:
		std::cout << "Selected piece: White Rook" << std::endl;
		break;
	case 9:
		std::cout << "Selected piece: White Knight" << std::endl;
		break;
	case 10:
		std::cout << "Selected piece: White Bishop" << std::endl;
		break;
	case 11:
		std::cout << "Selected piece: White Queen" << std::endl;
		break;
	case 12:
		std::cout << "Selected piece: White King" << std::endl;
		break;
	default:
		std::cout << "Empty Tile Selected" << std::endl;
		break;
	}
}

void renderHighlightTiles(
	SDL_Renderer *renderer, bool isPieceSelected, int pieceRowSelected, int pieceColSelected,
	int pieceRowDragged, int pieceColDragged, std::vector<std::pair<int,int>> selectedRedTiles, int selectedRedCount)
{

	SDL_Rect rect;
	// Highlight selected piece tile
	if (isPieceSelected)
	{
		rect = {pieceColSelected * TILE_SIZE, pieceRowSelected * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 185, 202, 66, 128); 
		SDL_RenderFillRect(renderer, &rect);

		// Highlight dragged piece tile
		rect = {pieceColDragged * TILE_SIZE, pieceRowDragged * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 246, 246, 130, 128);
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	// Render the selected red tiles
	for (const auto &tile : selectedRedTiles)
	{
		//std::cout << "Total tiles: " << selectedRedTiles.size() << std::endl;
		SDL_Rect rect = {tile.first * TILE_SIZE, tile.second * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Red with 50% opacity
		SDL_RenderFillRect(renderer, &rect);
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

}

// Main function
int main(int argc, char *argv[])
{
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	TTF_Font *font = nullptr;

	bool isMenuVisible = false;

	if (!init(window, renderer, font))
	{
		return -1;
	}

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

	bool isRunning = true;
	SDL_Event event;
	bool dragging = false;
	int draggedPiece = 0;
	bool pieceSelected = false;
	int pieceRowSelected, pieceColSelected = -1;
	int pieceRowDragged, pieceColDragged = -1;

	// vector of selected red tiles (row, col)
	// std::vector<std::vector<int>> selectedRedTiles(8, std::vector<int>(8, 0));
	std::vector<std::pair<int,int>> selectedRedTiles;
	int selectedRedTilesCount = selectedRedTiles.size();
	// an array of selected red tiles (row, col)
	// int selectedRedTilesArray[8][8] = {0};

	int draggingX, draggingY = -1;

	while (isRunning) //TODO: maybe for chess game do not need to check if isRunning
	{

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				isRunning = false;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				selectedRedTiles.clear();

				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				if (event.button.button && isMenuVisible)
				{
					// Define the bounds of the "Exit" menu entry
					SDL_Rect exitRect = {SCREEN_WIDTH / 2 - 50, 400, 100, 30};

					// Check if the "Exit" menu entry is clicked
					if (isPointInRect(mouseX, mouseY, exitRect))
					{
						isRunning = false;
					}
				}

				// Check if a piece is selected
				if (board[mouseY / TILE_SIZE][mouseX / TILE_SIZE] != 0)
				{
					SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
					pieceColSelected = mouseX / TILE_SIZE;
					pieceRowSelected = mouseY / TILE_SIZE;
					pieceSelected = true;
					draggedPiece = board[pieceRowSelected][pieceColSelected];
					board[pieceRowSelected][pieceColSelected] = 0;

					logSelectedPiece(draggedPiece);
					std::cout << "Piece selected at: (Row: " << pieceRowSelected << ", Col: " << pieceColSelected << ")" << std::endl;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
			{
				std::cout << "Right button clicked" << std::endl;
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				pieceSelected = false;
				dragging = false;
				draggedPiece = 0;

				bool tileFound = false;
				for (int i = 0; i < selectedRedTiles.size(); i++)
				{
					// check if tile exists in vector
					if (selectedRedTiles[i].first == mouseX / TILE_SIZE && selectedRedTiles[i].second == mouseY / TILE_SIZE)
					{
						// Remove the tile from the vector
						selectedRedTiles.erase(selectedRedTiles.begin() + i);
						std::cout << "Tile removed at: (Row: " << mouseY / TILE_SIZE << ", Col: " << mouseX / TILE_SIZE << ")" << std::endl;
						tileFound = true;
						break;
					}
				}
				if(!tileFound)
				{
					selectedRedTiles.push_back({mouseX / TILE_SIZE, mouseY / TILE_SIZE});
					std::cout << "Tile added at: (Row: " << mouseY / TILE_SIZE << ", Col: " << mouseX / TILE_SIZE << ")" << std::endl;
				}
				std::cout << "Total tiles: " << selectedRedTiles.size() << std::endl;				
				
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				if (pieceSelected)
				{
					if (dragging)
					{
						pieceColDragged = mouseX / TILE_SIZE;
						pieceRowDragged = mouseY / TILE_SIZE;
						dragging = false;
						board[pieceRowDragged][pieceColDragged] = draggedPiece;
						draggedPiece = 0;

						SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
					}
					else
					{
						board[pieceRowSelected][pieceColSelected] = draggedPiece;
						draggedPiece = 0;
						pieceColSelected = -1;
						pieceRowSelected = -1;
						pieceRowDragged = -1;
						pieceColDragged = -1;
						pieceSelected = false;
						SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
					}
				}

				// if (dragging)
				// {
				// 	pieceRowDragged = mouseX / TILE_SIZE;
				// 	pieceColDragged = mouseY / TILE_SIZE;
				// 	dragging = false;
				// 	board[pieceRowDragged][pieceColDragged] = draggedPiece;
				// 	draggedPiece = 0;

				// 	SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
				// }
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				// int mouseX, mouseY;
				SDL_GetMouseState(&draggingX, &draggingY);

				if (pieceSelected)
					dragging = true;

				// move piece while dragging
				//  SDL_Rect rect = {mouseX - TILE_SIZE / 2, mouseY - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
				//  SDL_RenderCopy(renderer, pieces[draggedPiece - 1], NULL, &rect);
			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m)
			{
				isMenuVisible = !isMenuVisible;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		if (isMenuVisible)
		{
			renderMenu(renderer, font);
		}
		else
		{
			renderBoard(renderer);
			renderBoardNotation(renderer, font);
			renderHighlightTiles(renderer, pieceSelected, pieceRowSelected, pieceColSelected, pieceRowDragged, pieceColDragged, selectedRedTiles,selectedRedTilesCount);
			renderPiecesInBoard(renderer, pieces, board);
			if (dragging && draggedPiece != 0)
			{
				SDL_Rect rect = {draggingX - TILE_SIZE / 2, draggingY - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
				SDL_RenderCopy(renderer, pieces[draggedPiece - 1], NULL, &rect);
			}
		}

		SDL_RenderPresent(renderer);
	}

	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
