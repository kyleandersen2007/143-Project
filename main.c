#include "board.h"
#include <stdio.h>

void make_move(struct chess_board *board,
               enum chess_piece piece,
               const char square[2],
               int hint_from_row,
               int hint_from_col)
{
    if (!square)
        panicf("invalid square: null pointer");

    char file = square[0];
    char rank = square[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
        panicf("invalid square: expected like \"e4\"");

    int to_col = file - 'a';       // a..h -> 0..7
    int to_row = 8 - (rank - '0'); // '8'->0, '1'->7

    struct chess_move mv = {
        .piece_type = piece,
        .to_row = to_row,
        .to_col = to_col,
        .hint_from_row = hint_from_row,
        .hint_from_col = hint_from_col};

    board_complete_move(board, &mv);
    board_apply_move(board, &mv);
    board_summarize(board);
}

int main()
{
    struct chess_board board;
    board_initialize(&board);
    board_summarize(&board);

    make_move(&board, PIECE_PAWN, "e4", -1, -1);
    make_move(&board, PIECE_PAWN, "e5", -1, -1);
    make_move(&board, PIECE_KNIGHT, "f3", -1, -1);
    make_move(&board, PIECE_KNIGHT, "c6", -1, -1);
    make_move(&board, PIECE_BISHOP, "c4", -1, -1);

    return 0;
}
