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

void renderHoveredTileBorder(SDL_Renderer *renderer, int hoveredRow, int hoveredCol, bool isDragging)
{
	// std::cout << "is dragging: (" << static_cast<bool>(isDragging) << ")" << std::endl;
	if (hoveredRow >= 0 && hoveredCol >= 0 /*&& hoveredRow < 8 && hoveredCol < 8*/ && isDragging)
	{
		SDL_Rect rect = {hoveredCol * TILE_SIZE, hoveredRow * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
		for (int i = 0; i < 5; ++i) // Increase the number of iterations for a wider border
		{
			SDL_RenderDrawRect(renderer, &rect);
			rect.x += 1;
			rect.y += 1;
			rect.w -= 2;
			rect.h -= 2;
		}
	}
}

void renderHighlightSelectedPieceTile(
	SDL_Renderer *renderer, bool isPieceSelected, int pieceRowSelected, int pieceColSelected)
{
	SDL_Rect rect;
	if (pieceColSelected >= 0 && pieceRowSelected >= 0)
	{
		// Highlight selected piece tile
		rect = {pieceColSelected * TILE_SIZE, pieceRowSelected * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 185, 202, 66, 128); // Green with 50% opacity
		SDL_RenderFillRect(renderer, &rect);
	}
	// Highlight selected piece tile
	// if (isPieceSelected)
	// {
	// 	rect = {pieceColSelected * TILE_SIZE, pieceRowSelected * TILE_SIZE, TILE_SIZE, TILE_SIZE};
	// 	SDL_SetRenderDrawColor(renderer, 185, 202, 66, 128); // Green with 50% opacity
	// 	SDL_RenderFillRect(renderer, &rect);
	// }
}

void renderHighlightDraggedPieceTile(
	SDL_Renderer *renderer, bool isPieceSelected, int pieceRowDragged, int pieceColDragged)
{
	SDL_Rect rect;
	if (pieceColDragged >= 0 && pieceRowDragged >= 0 && isPieceSelected)
	{
		// Highlight dragged piece tile
		rect = {pieceColDragged * TILE_SIZE, pieceRowDragged * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 246, 246, 130, 128); // Yellow with 50% opacity
		SDL_RenderFillRect(renderer, &rect);
		//std::cout << "Piece dragged at: (Row: " << pieceRowDragged << ", Col: " << pieceColDragged << ")" << std::endl;
	}
}

void renderHighlightRedTile(
	SDL_Renderer *renderer, bool isPieceSelected, std::vector<std::pair<int, int>> selectedRedTiles)
{
	SDL_Rect rect;
	// Highlight selected piece tile	
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	for (const auto &tile : selectedRedTiles)
	{
		// std::cout << "Total tiles: " << selectedRedTiles.size() << std::endl;
		rect = {tile.first * TILE_SIZE, tile.second * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Red with 50% opacity
		SDL_RenderFillRect(renderer, &rect);
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// Function to validate if a pawn move is valid
bool IsValidToMovePawn(int board[8][8], int piece, int selectedRow, int selectedCol, int draggedRow, int draggedCol)
{
	int target = board[draggedRow][draggedCol];

	if (piece == 1) // Black Pawn
	{
		std::cout << "Selected black pawn"  << std::endl;
		// Move forward
		if (selectedCol == draggedCol && selectedRow + 1 == draggedRow && target == 0)
			return true;

		// Double move from starting position
		if (selectedCol == draggedCol && selectedRow == 1 && selectedRow + 2 == draggedRow && target == 0 && board[selectedRow + 1][selectedCol] == 0)
			return true;

		// Capture
		if (abs(selectedCol - draggedCol) == 1 && selectedRow + 1 == draggedRow && target > 6)
			return true;
	}
	else if (piece == 7) // White Pawn
	{
		std::cout << "Selected white pawn"  << std::endl;
		// Move forward
		if (selectedCol == draggedCol && selectedRow - 1 == draggedRow && target == 0)
			return true;

		// Double move from starting position
		if (selectedCol == draggedCol && selectedRow == 6 && selectedRow - 2 == draggedRow && target == 0 && board[selectedRow - 1][selectedCol] == 0)
			return true;

		// Capture
		if (abs(selectedCol - draggedCol) == 1 && selectedRow - 1 == draggedRow && target > 0 && target <= 6)
			return true;
	}

	return false;
}

// Function to validate if a move is valid
bool isValidMove(int board[8][8], int piece, int selectedRow, int selectedCol, int draggedRow, int draggedCol)
{
	
	// Ensure the move is within bounds
	if (selectedRow < 0 || selectedRow >= 8 || selectedCol < 0 || selectedCol >= 8 || draggedRow < 0 || draggedRow >= 8 || draggedCol < 0 || draggedCol >= 8)
	{
		return false;
	}

	// Ensure the move is not to the same position
	if (selectedRow == draggedRow && selectedCol == draggedCol)
	{
		return false;
	}

	
	// Ensure the move is not to the same color piece
	// if (board[selectedRow][selectedCol] > 6 && board[draggedRow][draggedCol] > 6)
	// {
	// 	return false;
	// }
	// if (board[selectedRow][selectedCol] < 7 && board[draggedRow][draggedCol] < 7)
	// {
	// 	return false;
	// }

	//TODO: move below validation to the piece validation function, there are instances 
	//where piece can move if the target contains a piece of the same color 
	// // Ensure the move is not to an invalid tile (tile with piece in it) 
	// if (board[draggedRow][draggedCol] != 0)
	// {
	// 	return false;
	// }
	std::cout << "passed all validation before switch" << piece << std::endl;
	// Determine piece type and validate move accordingly
	switch (piece)
	{
		case 1: // Black Pawn
		case 7: // White Pawn
			return IsValidToMovePawn(board, piece, selectedRow, selectedCol, draggedRow, draggedCol);
			//break;
	// case 7: // White Pawn
	//     if (startY == endY && endX - startX == 1 && target == 0)
	//         return true; // Move forward
	//     if (startY == endY && startX == 1 && endX - startX == 2 && target == 0)
	//         return true; // Double move from starting position
	//     if (abs(startY - endY) == 1 && endX - startX == 1 && target <= 6 && target != 0)
	//         return true; // Capture
	//     break;
	// case 2: // Black Rook
	// case 8: // White Rook
	//     if (startX == endX || startY == endY)
	//         return true; // Move in straight lines
	//     break;
	// case 3: // Black Knight
	// case 9: // White Knight
	//     if ((abs(startX - endX) == 2 && abs(startY - endY) == 1) || (abs(startX - endX) == 1 && abs(startY - endY) == 2))
	//         return true; // L-shaped move
	//     break;
	// case 4: // Black Bishop
	// case 10: // White Bishop
	//     if (abs(startX - endX) == abs(startY - endY))
	//         return true; // Move diagonally
	//     break;
	// case 5: // Black Queen
	// case 11: // White Queen
	//     if (startX == endX || startY == endY || abs(startX - endX) == abs(startY - endY))
	//         return true; // Move like rook or bishop
	//     break;
	// case 6: // Black King
	// case 12: // White King
	//     if (abs(startX - endX) <= 1 && abs(startY - endY) <= 1)
	//         return true; // Move one square in any direction
	//     break;
	default:
		return false;
	}

	return false;
}

// Function to render a inside centered circle in a tile
void renderCircleInsideTile(SDL_Renderer *renderer, std::vector<std::pair<int, int>> tiles = std::vector<std::pair<int, int>>())
{
	try
	{
		// Render the circle inside the tile

		// tiles = std::vector<std::pair<int,int>>();
		//  tiles.push_back(std::make_pair(2, 3));
		//  tiles.push_back(std::make_pair(2, 4));
		//  tiles.push_back(std::make_pair(2, 5));

		SDL_Rect rect = {2 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		// SDL_Rect rect = {tile.first * TILE_SIZE, tile.second * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Red with 50% opacity
		SDL_RenderFillRect(renderer, &rect);

		// // Render the circle inside the tile
		// for (const auto &tile : tiles)
		// {
		// 	SDL_Rect rect = {2 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		// 	// SDL_Rect rect = {tile.first * TILE_SIZE, tile.second * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		// 	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // Red with 50% opacity
		// 	SDL_RenderFillRect(renderer, &rect);
		// }
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}	
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
	int pieceRowSelected = -1, pieceColSelected = -1;
	int pieceRowDragged = -1, pieceColDragged = -1;

	// vector of selected red tiles (row, col)
	// std::vector<std::vector<int>> selectedRedTiles(8, std::vector<int>(8, 0));
	std::vector<std::pair<int, int>> selectedRedTiles;
	//int selectedRedTilesCount = selectedRedTiles.size();
	// an array of selected red tiles (row, col)
	// int selectedRedTilesArray[8][8] = {0};

	int draggingX = -1, draggingY = -1;
	int hoveredRow = -1, hoveredCol = -1;

	while (isRunning) // TODO: maybe for chess game do not need to check if isRunning
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

					dragging = true;
					draggingX = mouseX;
					draggingY = mouseY;

					logSelectedPiece(draggedPiece);
					std::cout << "Piece selected at: (Row: " << pieceRowSelected << ", Col: " << pieceColSelected << ")" << std::endl;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
			{
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				pieceSelected = false;
				dragging = false;
				draggedPiece = 0;
				bool tileFound = false;

				for (int i = 0; i < static_cast<int>(selectedRedTiles.size()); i++)
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
				if (!tileFound)
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
						if (isValidMove(board, draggedPiece, pieceRowSelected, pieceColSelected, pieceRowDragged, pieceColDragged))
						{
							std::cout << "Valid move from: (Row: " << pieceRowSelected << ", Col: " << pieceColSelected << ") to (Row: " << pieceRowDragged << ", Col: " << pieceColDragged << ")" << std::endl;
							board[pieceRowDragged][pieceColDragged] = draggedPiece;
							SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
						}
						else
						{
							board[pieceRowSelected][pieceColSelected] = draggedPiece;
							// draggedPiece = 0;
							pieceColSelected = -1;
							pieceRowSelected = -1;
							pieceRowDragged = -1;
							pieceColDragged = -1;
							// pieceSelected = false;
							// dragging = false;
							SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
						}
					}
					else
					{
						board[pieceRowSelected][pieceColSelected] = draggedPiece;
						// draggedPiece = 0;
						pieceColSelected = -1;
						pieceRowSelected = -1;
						pieceRowDragged = -1;
						pieceColDragged = -1;
						// pieceSelected = false;
						// dragging = false;
						SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
					}
				}
				pieceSelected = false;
				draggedPiece = 0;
				dragging = false;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				SDL_GetMouseState(&draggingX, &draggingY);

				if (pieceSelected)
				{
					dragging = true;
					hoveredRow = draggingY / TILE_SIZE;
					hoveredCol = draggingX / TILE_SIZE;
				}
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

			// renderHighlightTiles(renderer, pieceSelected, pieceRowSelected, pieceColSelected, pieceRowDragged, pieceColDragged, selectedRedTiles,selectedRedTilesCount);

			renderHighlightSelectedPieceTile(renderer, pieceSelected, pieceRowSelected, pieceColSelected);
			renderHoveredTileBorder(renderer, hoveredRow, hoveredCol, dragging);
			renderHighlightDraggedPieceTile(renderer, pieceSelected, pieceRowDragged, pieceColDragged);
			renderHighlightRedTile(renderer, pieceSelected, selectedRedTiles);
			//renderCircleInsideTile(renderer);
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
