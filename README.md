# Details
This is a chess game built with C++ and SDL 

# Scope

- A  chess game that handles 2 playing modes. player vs player and player vs AI
- Game UI should be similar to chess.com board and game interactions.
- Game Board should contain a left/right panel to  show historical moves.
- Game UI should start allowing the gamer to select Board type, Game Mode and Pieces color.


## Done (90m)
- [x] Create basic project structure, repo | 12/27/24 | 10m
- [x] Create Main windows (test SDL) | 12/27/24 | 10m
- [x] Create board | 12/27/24 | 30m
- [x] Add Game pieces images | 12/27/24 | 40m
- [x] Implement board pieces direction (horizontal or vertical) 12/28/24 | 5m
- [x] Selected board squares blocks logs and validations 12/28/24 | 30m
- [ ] Create main menu (with options)
- [x] Create function to show notation 1/09/25 | 1h
- [x] Add new color board palette 1/11/25 | 10min
- [x] Remove horizontal mode (is not necessary) 1/11/25 | 10min
- [x] Create new file chess.cpp with cleaner code  1/11/25 | 10min
- [x] Finish board notation 1/11/25 | 30min
- [x] Drag and drop pieces functionality 1/12/25 | 1h45min
- [x] Build main menu 1/11/25 | 30 min
- [x] Work on highlight/remove red tiles when right clicking  tile | 1/13/25 | 30min
- [x] Work on highlight tiles when right clicking tile | 1/13/25 | 10min
- [ ] Work on highlight tiles when dragging piece  | 1/13/25 | 2h
- [ ] Bug when clicking before dragging, piece disappear | 1/13/25 | 2h
- [ ] Add Pieces movement Logic. (isValidMove)  | 1/-/25 | 30m



## Backlog
- Understand difference on loading images btw svg and png.
- Let player change direction of the board game (default is horizontal) .
- Fix the lib dependencies when debugging.
- Add AI vs Player logic.
- Add highlighted path if piece is selected (red for invalid path, blue for valid path)
- add history of moves in a side board
- use OOP to organize code, (don't get fancy) and don't affect performance
- add main menu to select board color (with animation and color palette) , activate notation, select ai mode, and others board and game configurations
- add network players
- add ai to show next best move on side panel
- create a left/right board to log movement and positioning in the same game board 1/09/25 | 1h


# General Improvement & Optimizations

1. **Piece Movement and Validations**
   
Implement **isValidMove()** for all piece types:

Add complete movement logic for each chess piece (e.g., pawn, knight, rook, etc.).
Use direction vectors for sliding pieces (rooks, bishops, queens).
Add logic for special cases like castling, pawn promotion, and  passant.

Precompute valid moves for knights and kings:

Store possible moves for these pieces on every square since their movements are constant.
Use a lookup table to speed up isValidMove() and reduce redundant calculations.

2. **Drag-and-Drop for Piece Interaction**

Implement drag-and-drop functionality for moving pieces:
Use SDL mouse events (SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP, SDL_MOUSEMOTION) to detect and move pieces.
Add logic to "snap" the dragged piece back to its original position if the move is invalid.


3. **Highlight Possible Moves**

Show valid moves on the board:

Highlight squares where the selected piece can legally move.
Use precomputed moves (e.g., for knights) or dynamic pathfinding (e.g., for rooks).
Render the highlighted squares using a semi-transparent overlay.

4. **Log Panel Improvements**

Replace the current console-based logging with SDL_ttf for rendering text directly in the log panel.

Limit log size to avoid performance degradation:
Remove older entries when the log exceeds a certain size (e.g., 20 messages).

Add more meaningful logs:
Log piece movement (e.g., "Knight moved to (3,4)").
Log special actions like "Check" or "Checkmate."

5. **Performance Optimizations**
**
Memoization:

Cache results for sliding piece path finding (rooks, bishops, queens) to avoid recalculating paths repeatedly for the same board state.

Lazy Calculation for Highlights:
Only calculate valid moves when a piece is selected instead of precomputing them for all pieces on every turn.

6. **Add a State Manager**

Implement a game state manager to track:
The current player's turn.
Selected piece.
Game state (e.g., in-progress, checkmate, stalemate).
Undo functionality for moves.

7. **Board Rendering Optimization**
Avoid redrawing the entire board every frame:
Only update and render the affected squares after a move or event.
Use dirty rectangles for optimized rendering.

8. **AI Integration (Optional for Single-Player Mode)**
Implement a basic AI using:
Minimax algorithm with alpha-beta pruning for move decision-making.
Add depth limitation or iterative deepening for performance in complex positions.

9. **Special Game Rules**
Add support for chess-specific rules:
Castling.
En passant.
Pawn promotion.
Check, checkmate, and stalemate detection.

10. **UI and Visual Enhancements**
Add visual feedback for player actions:
Highlight the selected piece.
Animate piece movement.
Theme the board:
Add textures or color schemes for the board tiles and pieces.
Add visual indicators for:
Current turn.
Check or checkmate status.

11. **General Cleanup**
Modularize the code:
Separate rendering, input handling, and game logic into distinct functions or classes.
Use constants and enums for piece types, colors, and game states for better readability and maintainability.

12. Testing
Add debugging tools:
Visualize internal state (e.g., highlighted squares, pathfinding calculations).
