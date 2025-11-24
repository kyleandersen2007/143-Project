#include "board.h"
#include <stdio.h>

// Helper function to make a move on the board since we don't have a parser yet :C
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
    // d4 Nf6 Bf4 g6 e3 Bg7 Bd3 d5 Nd2 c6 c3 Qb6 Qb3 Nbd7 Ngf3 Nh5 Qxb6 axb6 h3 Nxf4 exf4 Nf6 a3
    // O-O O-O Nh5 g3 Bxh3 Rfe1 e6 c4 Bg4 cxd5 exd5 Ne5 Bxe5 dxe5 c5 f3 Bd7 g4 Nxf4 Bc2 Bb5
    make_move(&board, PIECE_PAWN, 4, 4);  // e4
    make_move(&board, PIECE_PAWN, 3, 3);  // d5
    make_move(&board, PIECE_PAWN, 3, 3);  // exd5
    make_move(&board, PIECE_QUEEN, 3, 3); // Qxd5

    make_move(&board, PIECE_PAWN, 0, 4); // a4
    // make_move(&board, PIECE_BISHOP, 3, 5); // Bd3
    // make_move(&board, PIECE_KNIGHT, 2, 2); // Nc6
    // make_move(&board, PIECE_KNIGHT, 5, 5); // Nf3
    // make_move(&board, PIECE_BISHOP, 6, 4); // Bg4
    // make_move(&board, PIECE_PAWN, 3, 3);  // exd5
    // make_move(&board, PIECE_QUEEN, 3, 3); // Qxd5
    // struct chess_move castle_move = {
    //     .is_castle = true,
    //     .castle_kingside = true};
    // board_complete_move(&board, &castle_move);
    // board_apply_move(&board, &castle_move); // O-O
    board_summarize(&board);

    return 0;
}
