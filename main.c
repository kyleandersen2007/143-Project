#include "board.h"
#include <stdio.h>

void make_move(struct chess_board *board, enum chess_piece piece, int col, int row)
{
    struct chess_move mv = {
        .piece_type = piece,
        .player = board->next_move_player,
        .from_row = -1,
        .from_col = -1,
        .to_row = row,
        .to_col = col};
    board_complete_move(board, &mv);
    board_apply_move(board, &mv);
}

int main()
{
    struct chess_board board;
    board_initialize(&board);

    make_move(&board, PIECE_PAWN, 4, 4);
    make_move(&board, PIECE_PAWN, 4, 3);
    make_move(&board, PIECE_BISHOP, 2, 4);
    make_move(&board, PIECE_KNIGHT, 2, 2);
    make_move(&board, PIECE_QUEEN, 7, 3);
    make_move(&board, PIECE_KNIGHT, 5, 2);
    make_move(&board, PIECE_QUEEN, 5, 1);

    board_summarize(&board);

    return 0;
}
