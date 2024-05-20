/* front.c - a lexical analyzer system for simple
arithmetic expressions */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

string stack[100];
int stacklocation = 0;

string Action[12][6] =  {
    //                     ID    +       *       (       )      EOF
                /*0*/    {"S5", "0",    "0",    "S4",   "0",    "0"},
                /*1*/    {"0",  "S6",   "0",    "0",    "0",    "A"},
                /*2*/    {"0",  "R2",   "S7",   "0",    "R2",   "R2"},
                /*3*/    {"0",  "R4",   "R4",   "0",    "R4",   "R4"},
                /*4*/    {"S5", "0",    "0",    "S4",   "0",    "0"},
                /*5*/    {"0",  "R6",   "R6",   "0",    "R6",   "R6"},
                /*6*/    {"S5", "0",    "0",    "S4",   "0",    "0"},
                /*7*/    {"S5", "0",    "0",    "S4",   "0",    "0"},
                /*8*/    {"0",  "S6",   "0",    "0",    "S11",  "0"},
                /*9*/    {"0",  "R1",   "S7",   "0",    "R1",   "R1"},
                /*10*/   {"0",  "R3",   "R3",   "0",    "R3",   "R3"},
                /*11*/   {"0",  "R5",   "R5",   "0",    "R5",   "R5"}};

string Goto[12][3] =  {
    //                     E    T    F
                /*0*/    {"1", "2", "3"},
                /*1*/    {"0", "0", "0"},
                /*2*/    {"0", "0", "0"},
                /*3*/    {"0", "0", "0"},
                /*4*/    {"8", "2", "3"},
                /*5*/    {"0", "0", "0"},
                /*6*/    {"0", "9", "3"},
                /*7*/    {"0", "0", "10"},
                /*8*/    {"0", "0", "0"},
                /*9*/    {"0", "0", "0"},
                /*10*/   {"0", "0", "0"},
                /*11*/   {"0", "0", "0"}};


/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
void expr();
void factor();
void term();

//FunctionsFor LR
void ActionLookup(int type, int state);
void Reduce(char rule);
void Shift(char rule);
int TypeSelection();
void PrintStack();

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

/******************************************************/
/* main driver */
int main()
{
    stack[0] = "0";
    PrintStack();
    /* Open the input data file and process its contents */
    if ((in_fp = fopen("LR.in", "r")) == NULL)
        printf("ERROR - cannot open front.in \n");
    else
    {
        getChar();
        do
        {
            lex();
            
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
        addChar();
        nextToken = LEFT_PAREN;
        break;
    case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        break;
    case '+':
        addChar();
        nextToken = ADD_OP;
        break;
    case '-':
        addChar();
        nextToken = SUB_OP;
        break;
    case '*':
        addChar();
        nextToken = MULT_OP;
        break;
    case '/':
        addChar();
        nextToken = DIV_OP;
        break;
    default:
        addChar();
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
        printf("Error - lexeme is too long \n");
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
    ActionLookup(TypeSelection(), stoi(stack[stacklocation]));

    return nextToken;
}






void PrintStack()
{
    cout << "-------------The Stack is ";
    for (int i = 0; i <= stacklocation; i++)
    {
        cout << stack[i];
    }
    cout << endl;
}


int TypeSelection()
{
    int type;

    switch (nextToken)
    {
    case 11:
        type = 0;
        break;
    case 21:
        type = 1;
        break;
    case 23:
        type = 2;
        break;
    case 25:
        type = 3;
        break;
    case 26:
        type = 4;
        break;
    case -1:
        type = 5;
        break;
    
    default:
        type = 5;
        break;
    }

    return type;
}

void ActionLookup(int type, int state)
{
    PrintStack();
    string stateaction = Action[state][type];

    // cout << "type is: " << type << endl;
    // cout << "state is: " << state << endl;
    // cout << "stateaction is: " << stateaction << endl;

    if (stateaction == "A")
    {
        cout << "The Grammer is Correct!\n";
        return;
    }
    else if (stateaction[0] == 'S')
    {
        /* Shift */
        Shift(stateaction[1]);
    }
    else if (stateaction[0] == 'R')
    {
        /* Reduce */
        Reduce(stateaction[1]);
    }
    else
    {
        cout << "ActionLookup Action not found!\nGiven Statement not possible!";
        exit(0);
    }
}

void Shift(char rule)
{
    
    cout << "In Shift. The rule is: " << rule << "\n";
    stacklocation+=2;
    switch (rule)
    {
    case '4':
        stack[stacklocation] = "4";
        stack[stacklocation-1] = lexeme;
        break;
    case '5':
        stack[stacklocation] = "5";
        stack[stacklocation-1] = lexeme;
        break;
    case '6':
        stack[stacklocation] = "6";
        stack[stacklocation-1] = lexeme;
        break;
    case '7':
        stack[stacklocation] = "7";
        stack[stacklocation-1] = lexeme;
        break;
    case '1':
        stack[stacklocation] = "11";
        stack[stacklocation-1] = lexeme;
        break;
    
    default:
        cout << "Shift Rule Not Found";
        exit(0);
        break;
    }
    
}

void Reduce(char rule)
{
    
    cout << "In Reduce. The rule is: " << rule << "\n";
    switch (rule)
    {
    case '1':
    //special
        stack[stacklocation] = "";//Goto[stoi(stack[stacklocation-2])][2];
        stack[stacklocation-1] = "";
        stack[stacklocation-2] = "";
        stack[stacklocation-3] = "";
        stacklocation -= 4;
        break;
    case '2':
        stack[stacklocation] = Goto[stoi(stack[stacklocation-2])][0];
        stack[stacklocation-1] = "E";
        break;
    case '3':
    //special
        stack[stacklocation] = ""; //Goto[stoi(stack[stacklocation-2])][2];
        stack[stacklocation-1] = "";
        stack[stacklocation-2] = "";
        stack[stacklocation-3] = "";
        stacklocation -= 4;
        break;
    case '4':
        stack[stacklocation] = Goto[stoi(stack[stacklocation-2])][1];
        stack[stacklocation-1] = "T";
        break;
    case '5':
        stack[stacklocation] = ""; //Goto[stoi(stack[stacklocation-2])][2];
        stack[stacklocation-1] = "";
        stack[stacklocation-2] = "";
        stack[stacklocation-3] = "";
        stack[stacklocation-4] = Goto[stoi(stack[stacklocation-6])][2];
        stack[stacklocation-5] = "F";
        stacklocation -= 4;
        break;
    case '6':
        stack[stacklocation] = Goto[stoi(stack[stacklocation-2])][2];
        stack[stacklocation-1] = "F";
        break;
    default:
        cout << "Reduce Rule Not Found";
        exit(0);
        break;
    }

    ActionLookup(TypeSelection(), stoi(stack[stacklocation]));
}