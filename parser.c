#include "parser.h"
#include <stdio.h>
#include <ctype.h>

bool parse_move(struct chess_move *move)
{
    char pieceChar = '\0';
    char columnChar = '\0';
    char rowChar = '\0';

    do
    {
        pieceChar = getc(stdin);
    } while (pieceChar == ' ');

    if (pieceChar == '\n' || pieceChar == '\r')
        return false;

    if (isupper(pieceChar))
    {
        switch (pieceChar)
        {
        case 'N':
            move->piece_type = PIECE_KNIGHT;
            break;
        case 'B':
            move->piece_type = PIECE_BISHOP;
            break;
        case 'R':
            move->piece_type = PIECE_ROOK;
            break;
        case 'Q':
            move->piece_type = PIECE_QUEEN;
            break;
        case 'K':
            move->piece_type = PIECE_KING;
            break;
        default:
            printf("Invalid piece letter in move.");
        }
        columnChar = getc(stdin);
        rowChar = getc(stdin);
    }
    else
    {
        move->piece_type = PIECE_PAWN;
        columnChar = pieceChar;
        rowChar = getc(stdin);
    }

    switch (tolower(columnChar))
    {
    case 'a':
        move->targetColumn = 0;
        break;
    case 'b':
        move->targetColumn = 1;
        break;
    case 'c':
        move->targetColumn = 2;
        break;
    case 'd':
        move->targetColumn = 3;
        break;
    case 'e':
        move->targetColumn = 4;
        break;
    case 'f':
        move->targetColumn = 5;
        break;
    case 'g':
        move->targetColumn = 6;
        break;
    case 'h':
        move->targetColumn = 7;
        break;
    default:
        printf("Invalid column letter in move.");
    }

    if (rowChar >= '1' && rowChar <= '8')
    {
        move->targetRow = (rowChar - '1');
    }
    else
        printf("Invalid row number in move.");


    int nextChar;
    while ((nextChar = getc(stdin)) == ' ')

    return true;
}
