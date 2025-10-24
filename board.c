#include "board.h"

const char *player_string(enum chess_player player)
{
    switch (player)
    {
    case PLAYER_WHITE:
        return "white";
    case PLAYER_BLACK:
        return "black";
    }
}

const char *piece_string(enum chess_piece piece)
{
    switch (piece)
    {
    case PIECE_PAWN:
        return "pawn";
    case PIECE_KNIGHT:
        return "knight";
    case PIECE_BISHOP:
        return "bishop";
    case PIECE_ROOK:
        return "rook";
    case PIECE_QUEEN:
        return "queen";
    case PIECE_KING:
        return "king";
    }
}

void board_initialize(struct chess_board *board)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int f = 0; f < BOARD_SIZE; ++f)
        {
            board->squares[r][f].type = (enum chess_piece) - 1;
            board->squares[r][f].owner = PLAYER_WHITE;
        }
    }

    board->squares[0][0].type = PIECE_ROOK;
    board->squares[0][0].owner = PLAYER_WHITE;
    board->squares[0][1].type = PIECE_KNIGHT;
    board->squares[0][1].owner = PLAYER_WHITE;
    board->squares[0][2].type = PIECE_BISHOP;
    board->squares[0][2].owner = PLAYER_WHITE;
    board->squares[0][3].type = PIECE_QUEEN;
    board->squares[0][3].owner = PLAYER_WHITE;
    board->squares[0][4].type = PIECE_KING;
    board->squares[0][4].owner = PLAYER_WHITE;
    board->squares[0][5].type = PIECE_BISHOP;
    board->squares[0][5].owner = PLAYER_WHITE;
    board->squares[0][6].type = PIECE_KNIGHT;
    board->squares[0][6].owner = PLAYER_WHITE;
    board->squares[0][6].column = 6;
    board->squares[0][6].row = 0;
    board->squares[0][7].type = PIECE_ROOK;
    board->squares[0][7].owner = PLAYER_WHITE;

    for (int column = 0; column < BOARD_SIZE; ++column)
    {
        board->squares[1][column].type = PIECE_PAWN;
        board->squares[1][column].owner = PLAYER_WHITE;
    }

    for (int column = 0; column < BOARD_SIZE; ++column)
    {
        board->squares[6][column].type = PIECE_PAWN;
        board->squares[6][column].owner = PLAYER_BLACK;
    }

    board->squares[7][0].type = PIECE_ROOK;
    board->squares[7][0].owner = PLAYER_BLACK;
    board->squares[7][1].type = PIECE_KNIGHT;
    board->squares[7][1].owner = PLAYER_BLACK;
    board->squares[7][2].type = PIECE_BISHOP;
    board->squares[7][2].owner = PLAYER_BLACK;
    board->squares[7][3].type = PIECE_QUEEN;
    board->squares[7][3].owner = PLAYER_BLACK;
    board->squares[7][4].type = PIECE_KING;
    board->squares[7][4].owner = PLAYER_BLACK;
    board->squares[7][5].type = PIECE_BISHOP;
    board->squares[7][5].owner = PLAYER_BLACK;
    board->squares[7][6].type = PIECE_KNIGHT;
    board->squares[7][6].owner = PLAYER_BLACK;
    board->squares[7][7].type = PIECE_ROOK;
    board->squares[7][7].owner = PLAYER_BLACK;

    board->next_move_player = PLAYER_WHITE;
}

static inline int is_within_board(int row, int column) {
    return row >= 0 && row < BOARD_SIZE && column >= 0 && column < BOARD_SIZE;
}

// Checks that all squares along a straight line between (startRow,startCol) and (endRow,endCol) are empty
static int is_path_clear(const struct chess_board *board,
                         int startRow, int startCol,
                         int stepRow, int stepCol,
                         int endRow, int endCol)
{
    int previousRow = startRow + stepRow;
    int currentCol = startCol + stepCol;
    while (previousRow != endRow || currentCol != endCol) {
        if (!is_within_board(previousRow, currentCol))
            return 0;
        if (board->squares[previousRow][currentCol].type != PIECE_NONE)
            return 0;
        previousRow += stepRow;
        currentCol += stepCol;
    }
    return 1;
}

void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    if (move->previousRow < 0 || move->previousColumn < 0) {
        panicf("Cannot apply move: missing source square.\n");
        return;
    }
    if (!is_within_board(move->previousRow, move->previousColumn) ||
        !is_within_board(move->targetRow, move->targetColumn)) {
        panicf("Invalid move: target square is off the board.\n");
        return;
    }
    const square *sourceSquare = &board->squares[move->previousRow][move->previousColumn];
    const square *destinationSquare = &board->squares[move->targetRow][move->targetColumn];

    if (sourceSquare->type == PIECE_NONE) {
        panicf("Invalid move: no piece on source square.\n");
        return;
    }

    if (sourceSquare->owner != board->next_move_player) {
        panicf("Invalid move: not %s’s turn.\n", player_string(sourceSquare->owner));
        return;
    }

    if (destinationSquare->type != PIECE_NONE &&
        destinationSquare->owner == sourceSquare->owner) {
        panicf("Invalid move: cannot capture your own piece.\n");
        return;
    }

    move->piece_type = sourceSquare->type;
    int rowDifference = move->targetRow - move->previousRow;
    int columnDifference = move->targetColumn - move->previousColumn;
    int absRowDiff = (rowDifference < 0) ? -rowDifference : rowDifference;
    int absColDiff = (columnDifference < 0) ? -columnDifference : columnDifference;

    switch (sourceSquare->type)
    {
    case PIECE_PAWN: {
        int forwardDirection = (sourceSquare->owner == PLAYER_WHITE) ? +1 : -1;
        int startingRow = (sourceSquare->owner == PLAYER_WHITE) ? 1 : 6;

        if (columnDifference == 0 && rowDifference == forwardDirection &&
            destinationSquare->type == PIECE_NONE)
            return;

        // pawns can move twice at beginning
        if (columnDifference == 0 && rowDifference == 2 * forwardDirection &&
            move->previousRow == startingRow) {
            int intermediateRow = move->previousRow + forwardDirection;
            if (board->squares[intermediateRow][move->previousColumn].type == PIECE_NONE &&
                destinationSquare->type == PIECE_NONE)
                return;
        }

        // Diagonal capture
        if (absRowDiff == 1 && absColDiff == 1 && rowDifference == forwardDirection &&
            destinationSquare->type != PIECE_NONE &&
            destinationSquare->owner != sourceSquare->owner)
            return;

        panicf("Invalid move: pawn move not allowed.\n");
        return;
    }

    case PIECE_KNIGHT:
        if ((absRowDiff == 2 && absColDiff == 1) || (absRowDiff == 1 && absColDiff == 2))
            return;
        panicf("Invalid move: knight move not allowed.\n");
        return;

    case PIECE_BISHOP:
        if (absRowDiff == absColDiff && absRowDiff > 0) {
            int stepRow = (rowDifference > 0) ? +1 : -1;
            int stepCol = (columnDifference > 0) ? +1 : -1;
            if (is_path_clear(board, move->previousRow, move->previousColumn,
                              stepRow, stepCol, move->targetRow, move->targetColumn))
                return;
        }
        panicf("Invalid move: bishop path blocked or not diagonal.\n");
        return;

    case PIECE_ROOK:
        if ((absRowDiff == 0 && absColDiff > 0) || (absColDiff == 0 && absRowDiff > 0)) {
            int stepRow = (absRowDiff ? ((rowDifference > 0) ? +1 : -1) : 0);
            int stepCol = (absColDiff ? ((columnDifference > 0) ? +1 : -1) : 0);
            if (is_path_clear(board, move->previousRow, move->previousColumn,
                              stepRow, stepCol, move->targetRow, move->targetColumn))
                return;
        }
        panicf("Invalid move: rook path blocked or not straight.\n");
        return;

    case PIECE_QUEEN: {
        int validStraight = (absRowDiff == 0 && absColDiff > 0) || (absColDiff == 0 && absRowDiff > 0); //type cast bool
        int validDiagonal = (absRowDiff == absColDiff && absRowDiff > 0); //type cast bool
        if (validStraight || validDiagonal) 
        {
            int stepRow = (rowDifference == 0) ? 0 : ((rowDifference > 0) ? +1 : -1);
            int stepCol = (columnDifference == 0) ? 0 : ((columnDifference > 0) ? +1 : -1);
            if (is_path_clear(board, move->previousRow, move->previousColumn, stepRow, stepCol, move->targetRow, move->targetColumn)){
                return;
            }
        }
        panicf("Invalid move: queen path blocked or not aligned.\n");
        return;
    }

    case PIECE_KING:
        if (absRowDiff <= 1 && absColDiff <= 1 && (absRowDiff + absColDiff) > 0)
            return;
        panicf("Invalid move: king move not allowed.\n");
        return;

    case PIECE_NONE:
        panicf("Invalid move: no piece selected.\n");
        return;
    }
}

void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    struct chess_move validatedMove = *move;
    board_complete_move(board, &validatedMove);

    square *sourceSquare = &board->squares[move->previousRow][move->previousColumn];
    if (sourceSquare->type == PIECE_NONE || sourceSquare->owner != board->next_move_player)
        return;

    square *destinationSquare = &board->squares[move->targetRow][move->targetColumn];
    if (destinationSquare->type != PIECE_NONE && destinationSquare->owner == sourceSquare->owner)
        return;

    *destinationSquare = *sourceSquare;
    sourceSquare->type = PIECE_NONE;

    board->next_move_player = (board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
}


void board_summarize(const struct chess_board *board)
{
    printf("Side to move: %s\n", player_string(board->next_move_player));
    for (int r = BOARD_SIZE - 1; r >= 0; --r)
    {
        printf("%d ", r + 1);
        for (int f = 0; f < BOARD_SIZE; ++f)
        {
            const square *sq = &board->squares[r][f];
            char c = '.';
            switch (sq->type)
            {
            case PIECE_PAWN:
                c = 'P';
                break;
            case PIECE_KNIGHT:
                c = 'N';
                break;
            case PIECE_BISHOP:
                c = 'B';
                break;
            case PIECE_ROOK:
                c = 'R';
                break;
            case PIECE_QUEEN:
                c = 'Q';
                break;
            case PIECE_KING:
                c = 'K';
                break;
            case PIECE_NONE:
                c = '.';
                break;
            }
            if (sq->owner == PLAYER_BLACK && c != '.')
                c += ('a' - 'A'); // lowercase for black
            printf("%c  ", c);
        }
        printf("%d\n", r + 1);
    }
}