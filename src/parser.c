#include "parser.h"
#include "panic.h"
#include <stdio.h>
#include <stdbool.h>

bool parse_move(struct chess_move *move)
{
    char current_char; // current character being read from input
    do
    {
        current_char = getc(stdin); // read the next character from stdin
        if (current_char == EOF) // check if we hit a end of file
        {
            return false; // lets just return false to prevent infinite loops
        }
    } while (current_char == ' '); // do this while we are reading spaces

    if (current_char == '\n' || current_char == '\r') //if we get a new line or a carriage
    {
        return false; // return false
    }

    char input_buffer[32]; // define an input buffer to hold the entire move string
    int input_length = 0; // we need to keep track of the length of the input

    input_buffer[input_length++] = current_char; // store the character in the input buffer and increment the length

    // read the rest of the input line
    while (true) //changed from while (1) because we can use stdbool.h
    {
        current_char = getc(stdin); // read the next character from stdin
        if (current_char == EOF || current_char == '\n' || current_char == '\r') // if we hit end of file or a new line or a carriage return
        {
            break;
        }
        if (current_char == ' ') // we need to ignore spaces
        {
            continue;
        }
        if (input_length < (int)(sizeof(input_buffer) - 1)) // check to see if our input buffer is full
        {
            input_buffer[input_length++] = current_char; // store the character in the input buffer and increment the length
        }
    }

    input_buffer[input_length] = '\0'; // IMPORTANT******** we need to null terminate the string

    // we declare the move struct values here because we don't want to have garbage values
    move->is_castle = false;
    move->castle_kingside = false;
    move->is_capture = false;
    move->is_promotion = false;
    move->promo_piece = PIECE_QUEEN;
    move->from_row = -1;
    move->from_col = -1;
    move->to_row = -1;
    move->to_col = -1;

    //check for castling
    if (input_buffer[0] == 'O')
    {
        if (input_buffer[1] == '-' && input_buffer[2] == 'O' && input_buffer[3] == '\0')
        {
            move->is_castle = true;
            move->castle_kingside = true;
        }
        else if (input_buffer[1] == '-' && input_buffer[2] == 'O' && input_buffer[3] == '-' && input_buffer[4] == 'O' && input_buffer[5] == '\0')
        {
            move->is_castle = true;
            move->castle_kingside = false;
        }
        else
        {
            return false;
        }
        move->piece_type = PIECE_KING;
        return true;
    }

    // normal move parsing
    enum chess_piece piece = PIECE_PAWN;
    int i = 0;

    switch (input_buffer[i])
    {
    case 'K':
        piece = PIECE_KING;
        i++;
        break;
    case 'Q':
        piece = PIECE_QUEEN;
        i++;
        break;
    case 'R':
        piece = PIECE_ROOK;
        i++;
        break;
    case 'B':
        piece = PIECE_BISHOP;
        i++;
        break;
    case 'N':
        piece = PIECE_KNIGHT;
        i++;
        break;
    default:
        if (input_buffer[i] < 'a' || input_buffer[i] > 'h') // if the character is not a file character, it must be a pawn move
        {
            return false;
        }
        piece = PIECE_PAWN;
        break;
    }

    // disambiguation and destination parsing
    char disamb_file = 0;
    char disamb_rank = 0;
    char dest_file = 0;
    char dest_rank = 0;

    while (input_buffer[i] != '\0') // while not finished input buffer
    {
        char current_character = input_buffer[i]; //current character

        if (current_character == 'x') // if we read an x, its technically a capture
        {
            // under the hood we handle captures by just overwriting the dest square struct and checking if a piece exists there
            move->is_capture = true;
            i++;
            continue;
        }

        if (current_character >= 'a' && current_character <= 'h') // if the character is a file character
        {
            if (input_buffer[i + 1] != '\0' && input_buffer[i + 1] >= '1' && input_buffer[i + 1] <= '8') // check if the next character is a rank character and within bounds
            {
                dest_file = current_character; // set the destination file and rank
                dest_rank = input_buffer[i + 1];
                i += 2; // we increment i by 2 since we read 2 characters
                break;
            }
            if (disamb_file != 0) // if we already have a disambiguation file, this is an error
            {
                return false;
            }
            disamb_file = current_character; // set the disambiguation file
            i++;
            continue;
        }

        if (current_character >= '1' && current_character <= '8') // if the character is a rank character
        {
            if ((i + 2 < input_length) && input_buffer[i + 1] >= 'a' && input_buffer[i + 1] <= 'h' && input_buffer[i + 2] >= '1' && input_buffer[i + 2] <= '8' && dest_file == 0 && dest_rank == 0) // check if the next two characters are a file and rank character and we haven't already set destination
            {
                if (disamb_rank != 0) // again: we alr have a disambiguation rank, error
                {
                    return false;
                }
                disamb_rank = current_character;
                dest_file = input_buffer[i + 1];
                dest_rank = input_buffer[i + 2];
                i += 3; // we increment i by 3 since we read 3 characters
                break;
            }
            if (disamb_rank != 0) // if we already have a disambiguation rank, this is an error
            {
                return false;
            }
            disamb_rank = current_character; // set the disambiguation rank
            i++;
            continue;
        }

        break;
    }

    if (dest_file == 0 || dest_rank == 0) // if we didn't get a destination square, error
    {
        if (input_buffer[i] >= 'a' && input_buffer[i] <= 'h' && input_buffer[i + 1] >= '1' && input_buffer[i + 1] <= '8') // check if the next two characters are a file and rank character
        {
            dest_file = input_buffer[i];
            dest_rank = input_buffer[i + 1];
            i += 2;
        }
        else
        {
            return false;
        }
    }

    move->piece_type = piece;
    move->to_col = dest_file - 'a';
    move->to_row = 8 - (dest_rank - '0');

    if (disamb_file != 0)
    {
        move->from_col = disamb_file - 'a';
    }
    if (disamb_rank != 0)
    {
        move->from_row = 8 - (disamb_rank - '0');
    }

    if (input_buffer[i] != '\0')
    {
        char promo_ch = input_buffer[i];
        if (promo_ch == '=')
        {
            ++i;
            if (input_buffer[i] == '\0') {
                return false;
            }
            promo_ch = input_buffer[i];
        }
        if (promo_ch == 'Q' || promo_ch == 'R' || promo_ch == 'B' || promo_ch == 'N')
        {
            move->is_promotion = true;
            switch (promo_ch)
            {
            case 'Q':
                move->promo_piece = PIECE_QUEEN;
                break;
            case 'R':
                move->promo_piece = PIECE_ROOK;
                break;
            case 'B':
                move->promo_piece = PIECE_BISHOP;
                break;
            case 'N':
                move->promo_piece = PIECE_KNIGHT;
                break;
            }
            i++;
        }
        if (input_buffer[i] != '\0')
        {
            return false;
        }
    }

    return true;
}
