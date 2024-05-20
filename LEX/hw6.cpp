/* front.c - a lexical analyzer system for simple
arithmetic expressions */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
int inComment = 0;
FILE *in_fp, *fopen();

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
void Comment();
void Other();
void Error(char* errortext);

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define COMMENT_START 27
#define COMMENT_END 28

/******************************************************/
/* main driver */
int main()
{
    /* Open the input data file and process its contents */
    if ((in_fp = fopen("hw6.in", "r")) == NULL)
        Error("ERROR - cannot open hw6.in \n");
    else
    {
        getChar();
        do
        {
            lex();
            if (nextToken == COMMENT_START)
            {
                Comment();
            }
            else if (nextToken != EOF)
            {
                Other();
            }
            
            
        } while (nextToken != EOF);
    }
}

/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */
int lookup(char ch)
{
    switch (ch)
    {
    case '(':
        nextToken = LEFT_PAREN;
        break;
    case ')':
        nextToken = RIGHT_PAREN;
        break;
    case '+':
        nextToken = ADD_OP;
        break;
    case '-':
        nextToken = SUB_OP;
        break;
    case '*':
        addChar();
        getChar();
        if (strcmp("*/", lexeme))
        {
            nextToken = COMMENT_END;
            inComment = 0;
        }
        else
        {
            nextToken = MULT_OP;
        }
        
        break;
    case '/':
        addChar();
        getChar();
        if (strcmp("/*", lexeme))
        {
            nextToken = COMMENT_START;
            inComment = 1;
        }
        else
        {
            nextToken = DIV_OP;
        }
        break;
    default:
        nextToken = EOF;
        break;
    }

    return nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar()
{
    if (lexLen <= 98)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
        Error("Error - lexeme is too long \n");
}

/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */
void getChar()
{
    if ((nextChar = getc(in_fp)) != EOF)
    {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */
void getNonBlank()
{
    while (isspace(nextChar))
        getChar();
}

/***************************************************** /
    /* lex - a simple lexical analyzer for arithmetic
    expressions */
int lex()
{
    lexLen = 0;
    getNonBlank();
    switch (charClass)
    {
    /* Parse identifiers */
    case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        nextToken = IDENT;
        break;
    /* Parse integer literals */
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        nextToken = INT_LIT;
        break;
    /* Parentheses and operators */
    case UNKNOWN:
        lookup(nextChar);
        addChar();
        getChar();
        break;
    /* EOF */
    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    } /* End of switch */

    printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
    return nextToken;
} /* End of function lex */

void Other()
{
    printf("Enter <Other>\n");
    while (inComment == 0 && nextToken != EOF)
    {
        lex();
    }
    printf("Exit <Other>\n");
    Comment();
}

void Comment()
{
    if (inComment == 1)
    {
        printf("Enter <Comment>\n");
    }

    while (inComment == 1 && nextToken != EOF)
    {
        lex();
    }
    if (nextToken == EOF && inComment == 1)
    {
        Error("Error - Reached end of file without closing comment\n");
        // printf("Error - Reached end of file without closing comment");
    }
    
    if (inComment == 0 && nextToken != EOF)
    {
        printf("Exit <Comment>\n");
    }
}

void Error(char* errortext)
{
    printf(errortext);
}
