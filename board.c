#include "board.h"
#include <stdio.h>

static inline int absint(int v)
{
    return (v < 0) ? -v : v;
}

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
    return "unknown";
}

bool straight_check(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col)
{
    if (from_row == to_row && from_col == to_col)
    {
        return false;
    }

    if (from_row == to_row)
    {
        int step = (to_col > from_col) ? 1 : -1;
        int column = from_col + step;

        while (column != to_col)
        {
            if (board->squares[from_row][column].has_piece)
                return false;
            column += step;
        }

        return true;
    }

    if (from_col == to_col)
    {
        int step = (to_row > from_row) ? 1 : -1;
        int row = from_row + step;

        while (row != to_row)
        {
            if (board->squares[row][from_col].has_piece)
                return false;
            row += step;
        }

        return true;
    }
    return false;
}

bool diag_check(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col)
{
    int delta_row = to_row - from_row;
    int delta_col = to_col - from_col;
    int abs_delta_row = absint(delta_row);
    int abs_delta_col = absint(delta_col);

    if (abs_delta_row == 0 && abs_delta_col == 0)
    {
        return false;
    }
    if (abs_delta_row == 0 || abs_delta_col == 0)
    {
        return false;
    }
    if (abs_delta_row != abs_delta_col)
    {
        return false;
    }

    int row_step = (delta_row > 0) ? 1 : -1;
    int column_step = (delta_col > 0) ? 1 : -1;
    int row = from_row + row_step;
    int column = from_col + column_step;

    while (row != to_row)
    {
        if (board->squares[row][column].has_piece)
        {
            return false;
        }
        row += row_step;
        column += column_step;
    }

    return true;
}

bool pawn_reach(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col, enum chess_player player)
{
    int forward_direction = (player == PLAYER_WHITE) ? -1 : +1;
    int start_row_index = (player == PLAYER_WHITE) ? 6 : 1;

    int delta_row = to_row - from_row;
    int delta_col = to_col - from_col;

    if (delta_row == forward_direction && (delta_col == 1 || delta_col == -1))
    {
        const struct square *destination = &board->squares[to_row][to_col];
        return destination->has_piece && destination->owner != player;
    }
    if (delta_col == 0 && delta_row == forward_direction)
    {
        return !board->squares[to_row][to_col].has_piece;
    }

    if (delta_col == 0 && delta_row == 2 * forward_direction && from_row == start_row_index)
    {
        int intermediate_row = from_row + forward_direction;
        return !board->squares[intermediate_row][from_col].has_piece && !board->squares[to_row][to_col].has_piece;
    }

    return false;
}

bool is_legal_move(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col)
{
    if (to_row < 0 || to_row >= BOARD_SIZE || to_col < 0 || to_col >= BOARD_SIZE)
    {
        return false;
    }

    const struct square *src = &board->squares[from_row][from_col];
    if (!src->has_piece)
    {
        return false;
    }

    enum chess_player player = src->owner;
    enum chess_piece piece = src->piece;

    const struct square *dst = &board->squares[to_row][to_col];
    if (dst->has_piece && dst->owner == player)
    {
        return false;
    }

    int delta_row = to_row - from_row;
    int delta_col = to_col - from_col;
    int absolute_delta_row = absint(delta_row);
    int absolute_delta_col = absint(delta_col);

    if (absolute_delta_row == 0 && absolute_delta_col == 0)
    {
        return false; // no “move”
    }

    bool is_legal = false;

    switch (piece)
    {
    case PIECE_PAWN:
        is_legal = pawn_reach(board, from_row, from_col, to_row, to_col, player);
        break;

    case PIECE_KNIGHT:
        is_legal = (absolute_delta_row == 2 && absolute_delta_col == 1) || (absolute_delta_row == 1 && absolute_delta_col == 2);
        break;

    case PIECE_BISHOP:
        if (absolute_delta_row == absolute_delta_col && absolute_delta_row != 0)
        {
            is_legal = diag_check(board, from_row, from_col, to_row, to_col);
        }
        break;

    case PIECE_ROOK:
        if ((delta_row == 0 && delta_col != 0) || (delta_col == 0 && delta_row != 0))
        {
            is_legal = straight_check(board, from_row, from_col, to_row, to_col);
        }
        break;

    case PIECE_QUEEN:
        is_legal = straight_check(board, from_row, from_col, to_row, to_col) || diag_check(board, from_row, from_col, to_row, to_col);
        break;

    case PIECE_KING:
        is_legal = (absolute_delta_row <= 1 && absolute_delta_col <= 1 && (absolute_delta_row + absolute_delta_col) > 0);
        break;
    }

    return is_legal;
}

bool in_check(const struct chess_board *board, enum chess_player player)
{
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if (board->squares[row][col].has_piece && board->squares[row][col].owner == player && board->squares[row][col].piece == PIECE_KING)
            {
                int king_row = row;
                int king_col = col;

                for (int from_row = 0; from_row < BOARD_SIZE; from_row++)
                {
                    for (int from_col = 0; from_col < BOARD_SIZE; from_col++)
                    {
                        const struct square *source_square = &board->squares[from_row][from_col];
                        if (!source_square->has_piece)
                        {
                            continue;
                        }
                        if (source_square->owner == player)
                        {
                            continue;
                        }
                        if (is_legal_move(board, from_row, from_col, king_row, king_col))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool in_checkmate(const struct chess_board *board, enum chess_player player)
{
    if (!in_check(board, player))
        return false;

    for (int from_row = 0; from_row < BOARD_SIZE; ++from_row)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; ++from_col)
        {
            const struct square *src = &board->squares[from_row][from_col];
            if (!src->has_piece || src->owner != player)
                continue;

            for (int to_row = 0; to_row < BOARD_SIZE; ++to_row)
            {
                for (int to_col = 0; to_col < BOARD_SIZE; ++to_col)
                {
                    const struct square *dst = &board->squares[to_row][to_col];
                    if (dst->has_piece && dst->owner == player)
                        continue;

                    if (!is_legal_move(board, from_row, from_col, to_row, to_col))
                    {
                        continue;
                    }

                    struct chess_move test_move;

                    test_move.player = player,
                    test_move.piece_type = src->piece,
                    test_move.from_row = from_row;
                    test_move.from_col = from_col;
                    test_move.to_row = to_row;
                    test_move.to_col = to_col;
                    test_move.is_capture = (dst->has_piece && dst->owner != player);

                    if (test_move.piece_type == PIECE_PAWN)
                    {
                        if ((player == PLAYER_WHITE && to_row == 0) || (player == PLAYER_BLACK && to_row == 7))
                        {
                            test_move.is_promotion = true;
                        }
                    }

                    struct chess_board test = *board;
                    board_apply_move(&test, &test_move);
                    if (!in_check(&test, player))
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool can_castle(const struct chess_board *board, enum chess_player player, bool kingside)
{
    int row = (player == PLAYER_WHITE) ? 7 : 0;

    if (player == PLAYER_WHITE)
    {
        if (kingside && !board->rights.white_kingside)
        {
            return false;
        }
        if (!kingside && !board->rights.white_queenside)
        {
            return false;
        }
    }
    else
    {
        if (kingside && !board->rights.black_kingside)
        {
            return false;
        }
        if (!kingside && !board->rights.black_queenside)
        {
            return false;
        }
    }

    int king_from_col = 4;
    int king_to_col = kingside ? 6 : 2;

    int step = (king_to_col > king_from_col) ? 1 : -1;
    for (int c = king_from_col + step; c != king_to_col; c += step)
    {
        if (board->squares[row][c].has_piece)
            return false;
    }

    if (in_check(board, player))
        return false;

    struct chess_board test_board = *board;
    struct chess_move test_move;
    test_move.player = player;
    test_move.piece_type = PIECE_KING;
    test_move.is_castle = false;
    test_move.is_capture = false;
    test_move.is_promotion = false;

    int king_col = king_from_col;
    while (king_col != king_to_col)
    {
        int next_col = king_col + step;

        test_move.from_row = row;
        test_move.from_col = king_col;
        test_move.to_row = row;
        test_move.to_col = next_col;

        board_apply_move(&test_board, &test_move); // keep checking moves until we finally castle

        king_col = next_col;

        if (in_check(&test_board, player))
            return false;
    }

    return true;
}

bool in_stalemate(const struct chess_board *board, enum chess_player player)
{
    if (in_check(board, player))
    {
        return false;
    }
    for (int from_row = 0; from_row < BOARD_SIZE; ++from_row)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; ++from_col)
        {
            const struct square *src = &board->squares[from_row][from_col];
            if (!src->has_piece || src->owner != player)
                continue;

            for (int to_row = 0; to_row < BOARD_SIZE; ++to_row)
            {
                for (int to_col = 0; to_col < BOARD_SIZE; ++to_col)
                {
                    const struct square *dst = &board->squares[to_row][to_col];
                    if (dst->has_piece && dst->owner == player)
                        continue;

                    if (!is_legal_move(board, from_row, from_col, to_row, to_col))
                    {
                        continue;
                    }

                    struct chess_move test_move;

                    test_move.player = player,
                    test_move.piece_type = src->piece,
                    test_move.from_row = from_row;
                    test_move.from_col = from_col;
                    test_move.to_row = to_row;
                    test_move.to_col = to_col;
                    test_move.is_capture = (dst->has_piece && dst->owner != player);

                    if (test_move.piece_type == PIECE_PAWN)
                    {
                        if ((player == PLAYER_WHITE && to_row == 0) || (player == PLAYER_BLACK && to_row == 7))
                        {
                            test_move.is_promotion = true;
                        }
                    }

                    struct chess_board test = *board;
                    board_apply_move(&test, &test_move);
                    if (in_check(&test, player))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void board_initialize(struct chess_board *board)
{
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            board->squares[row][col].has_piece = false;
            board->squares[row][col].owner = PLAYER_WHITE;
        }
    }

    board->squares[0][0] = (struct square){true, PIECE_ROOK, PLAYER_BLACK};
    board->squares[0][1] = (struct square){true, PIECE_KNIGHT, PLAYER_BLACK};
    board->squares[0][2] = (struct square){true, PIECE_BISHOP, PLAYER_BLACK};
    board->squares[0][3] = (struct square){true, PIECE_QUEEN, PLAYER_BLACK};
    board->squares[0][4] = (struct square){true, PIECE_KING, PLAYER_BLACK};
    board->squares[0][5] = (struct square){true, PIECE_BISHOP, PLAYER_BLACK};
    board->squares[0][6] = (struct square){true, PIECE_KNIGHT, PLAYER_BLACK};
    board->squares[0][7] = (struct square){true, PIECE_ROOK, PLAYER_BLACK};

    for (int col = 0; col < 8; col++)
    {
        board->squares[1][col] = (struct square){true, PIECE_PAWN, PLAYER_BLACK};
        board->squares[6][col] = (struct square){true, PIECE_PAWN, PLAYER_WHITE};
    }

    board->squares[7][0] = (struct square){true, PIECE_ROOK, PLAYER_WHITE};
    board->squares[7][1] = (struct square){true, PIECE_KNIGHT, PLAYER_WHITE};
    board->squares[7][2] = (struct square){true, PIECE_BISHOP, PLAYER_WHITE};
    board->squares[7][3] = (struct square){true, PIECE_QUEEN, PLAYER_WHITE};
    board->squares[7][4] = (struct square){true, PIECE_KING, PLAYER_WHITE};
    board->squares[7][5] = (struct square){true, PIECE_BISHOP, PLAYER_WHITE};
    board->squares[7][6] = (struct square){true, PIECE_KNIGHT, PLAYER_WHITE};
    board->squares[7][7] = (struct square){true, PIECE_ROOK, PLAYER_WHITE};

    board->next_move_player = PLAYER_WHITE;

    board->rights.white_kingside = true;
    board->rights.white_queenside = true;
    board->rights.black_kingside = true;
    board->rights.black_queenside = true;
}

void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    move->player = board->next_move_player;

    if (move->is_castle)
    {
        int row = (move->player == PLAYER_WHITE) ? 7 : 0;

        move->from_row = row;
        move->from_col = 4;
        move->to_row = row;
        move->to_col = move->castle_kingside ? 6 : 2;

        move->piece_type = PIECE_KING;
        move->is_capture = false;
        move->is_promotion = false;
        return;
    }

    if (move->to_row < 0 || move->to_row >= BOARD_SIZE || move->to_col < 0 || move->to_col >= BOARD_SIZE)
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }
    if (board->squares[move->to_row][move->to_col].has_piece && board->squares[move->to_row][move->to_col].owner == move->player)
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }

    int possible_rows[16];
    int possible_cols[16];
    int possible_moves = 0;

    for (int from_row = 0; from_row < BOARD_SIZE; ++from_row)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; ++from_col)
        {
            const struct square *source_square = &board->squares[from_row][from_col];
            if (!source_square->has_piece)
            {
                continue;
            }
            if (source_square->owner != move->player)
            {
                continue;
            }
            if (source_square->piece != move->piece_type)
            {
                continue;
            }

            if (!is_legal_move(board, from_row, from_col, move->to_row, move->to_col))
            {
                continue;
            }

            if (possible_moves < (int)(sizeof(possible_rows) / sizeof(possible_rows[0])))
            {
                possible_rows[possible_moves] = from_row;
                possible_cols[possible_moves] = from_col;
                possible_moves++;
            }
        }
    }

    if (possible_moves == 0)
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }
    else if (possible_moves > 1)
    {
        panicf("parse error: ambiguous move\n");
    }

    move->from_row = possible_rows[0];
    move->from_col = possible_cols[0];

    move->is_capture = board->squares[move->to_row][move->to_col].has_piece && board->squares[move->to_row][move->to_col].owner != move->player;

    if (move->piece_type == PIECE_PAWN)
    {
        if ((move->player == PLAYER_WHITE && move->to_row == 0) || (move->player == PLAYER_BLACK && move->to_row == 7))
        {
            move->is_promotion = true;
        }
        else
        {
            move->is_promotion = false;
        }
    }
}

void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    if (move->from_row < 0 || move->from_row >= BOARD_SIZE || move->from_col < 0 || move->from_col >= BOARD_SIZE || move->to_row < 0 || move->to_row >= BOARD_SIZE || move->to_col < 0 || move->to_col >= BOARD_SIZE)
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }

    struct square *src = &board->squares[move->from_row][move->from_col];
    struct square *dst = &board->squares[move->to_row][move->to_col];

    if (!src->has_piece || src->owner != move->player || src->piece != move->piece_type) // if our source square does not have a piece or the owner if the source square is not the current player or the source piece does not equal the move piece type
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }

    if (move->is_castle)
    {
        int row = (move->player == PLAYER_WHITE) ? 7 : 0;

        struct square *rook_src;
        struct square *rook_dst;

        if (move->castle_kingside)
        {
            rook_src = &board->squares[row][7];
            rook_dst = &board->squares[row][5];
        }
        else
        {
            rook_src = &board->squares[row][0];
            rook_dst = &board->squares[row][3];
        }

        if (!rook_src->has_piece || rook_src->owner != move->player || rook_src->piece != PIECE_ROOK)
        {
            panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
        }

        *rook_dst = *rook_src;
        rook_src->has_piece = false;
    }

    *dst = *src;

    if (move->is_promotion)
    {
        dst->piece = move->promo_piece;
    }

    src->has_piece = false;

    board->next_move_player = (board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
}

void board_summarize(const struct chess_board *board)
{
    if (in_checkmate(board, board->next_move_player))
    {
        printf("%s wins by checkmate\n", player_string((board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE));
    }
    else if (in_stalemate(board, board->next_move_player))
    {
        printf("stalemate\n");
    }
    else if (in_check(board, (board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE))
    {
        printf("%s is in check\n", player_string((board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE));
    }
    else
    {
        printf("game incomplete\n");
    }
    // TODO add stalemate
}