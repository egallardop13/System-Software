// Emilio Morales and Ernesto Gallardo
// COP3402-22Summer C001

/*
	skeleton for Dr. Montagne's Systems Software project Summer 2022
	you may alter all of this code if desired, but you must provide clear
	instructions if you alter the compilation process or wish us to
	use different files during grading than the ones provided 
	(driver.c, compiler.h, vm.o/.c, or lex.o/.c)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

lexeme *tokens;
int lIndex = 0;// idx of curr token.
instruction *code;// keeps track of instructions we are generating.
int cIndex = 0;// keeps track of the next idx we available to put instruction in.
symbol *table;
int tIndex = 0;// curr idx in table

int level;
int varCount;
int errorFlag = 0;// keeps track if we have an error.

// keeps track of we encounter arithmetic assigner
int arithmeticFlag = 0;


void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);

void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

void programStart();
void block();
void const_declaration();
int var_declaration();
void procedure_declaration();
void statement();
void condition();
void expression();
void term();
void factor();


instruction *parser_code_generator(lexeme *list)
{
	int i;
	tokens = list;
	code = malloc(sizeof(instruction) * MAX_ARRAY_SIZE);
	table = malloc(sizeof(symbol) * MAX_ARRAY_SIZE);
	
	// Your code here. 
	// Make sure to remember to free table before returning always
	// and to free code and return NULL if there was an error
	lIndex = 0;
	cIndex = 0;
	tIndex = 0;

	programStart();

	if (errorFlag == 1)
	{
		free(table);
		free(code);
		return NULL;
	}

	code[cIndex].op = -1;

	printsymboltable();
	printassemblycode();

	free(table);
	return code;
}

void programStart()
{
	if (errorFlag == 1) return;

	emit(7, 0, 0);
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;
	block();

	if (errorFlag == 1) return;

	// period not found
	if (tokens[lIndex].type != periodsym)
	{
		printparseerror(1);
		return;
	}

	emit(9, 0, 3);

	for (int i = 0; i < cIndex; i++)
	{
		if (code[i].op == 5)
		{
			code[i].m = table[code[i].m].addr * 3;
		}
	}

	code[0].m = table[0].addr * 3;
}// END of program

void block()
{
	if (errorFlag == 1) return;

	level++;
	int current_procedure = tIndex - 1;
	const_declaration();
	
	if (errorFlag == 1) return;

	int x = var_declaration();
	procedure_declaration();

	if (errorFlag == 1) return;

	table[current_procedure].addr = cIndex;
	
	emit(6, 0, x + 3);
	statement();

	if (errorFlag == 1) return;

	mark();
	level--;
}// END of block

void const_declaration()
{
	if (errorFlag == 1) return;

	if (tokens[lIndex].type == constsym)
	{
		do
		{
			lIndex++;
			// checks for valid constant declaration pattern	
			if (tokens[lIndex].type != identsym)
			{
				printparseerror(2);
				return;
			}

			int symbolTableIdx = multipledeclarationcheck(tokens[lIndex].name);
			// checks for conflicting symbol declarations
			if (symbolTableIdx != -1)
			{
				printparseerror(18);
				return;
			}

			char *tempName = tokens[lIndex].name;
			
			lIndex++;
			// checks for valid constant declaration pattern	
			if (tokens[lIndex].type != becomessym)	
			{
				printparseerror(2);
				return;
			}

			lIndex++;
			// checks for valid constant declaration pattern	
			if (tokens[lIndex].type != numbersym)	
			{
				printparseerror(2);
				return;
			}

			addToSymbolTable(1, tempName, tokens[lIndex].value, level, 0, 0);
			lIndex++;
		}while (tokens[lIndex].type == commasym);

		if (tokens[lIndex].type != semicolonsym)
		{
			// checks if token is an identifier or missing ending semicolon
			// outputs appropriate error
			if (tokens[lIndex].type == identsym)
			{
				printparseerror(13);
				return;
			}
			else
			{
				printparseerror(14);
				return;
			}
		}

		lIndex++;
	}
	return;
}// END of const_declaration


int var_declaration()
{
	if (errorFlag == 1) return 0;

	varCount = 0;
	if (tokens[lIndex].type == varsym)
	{
		do
		{
			varCount++;
			lIndex++;
			// checks for incorrect var declaration pattern
			if (tokens[lIndex].type != identsym)
			{
				printparseerror(3);
				return -1;
			}

			int symbolTableIdx = multipledeclarationcheck(tokens[lIndex].name);
			// checks for conflicting symbol declarations
			if (symbolTableIdx != -1)
			{
				printparseerror(18);
				return -1;
			}

			addToSymbolTable(2, tokens[lIndex].name, 0, level, varCount + 2, 0);	

			lIndex++;
		}while (tokens[lIndex].type == commasym);

		if (tokens[lIndex].type != semicolonsym)
		{
			// checks if token is an identifier or missing ending semicolon
			// outputs appropriate error
			if (tokens[lIndex].type == identsym)
			{
				printparseerror(13);
				return -1;
			}
			else
			{
				printparseerror(14);
				return 0;
			}
		}

		lIndex++;
	}

	return varCount;
}// END of var_declaration

void procedure_declaration()
{
	if (errorFlag == 1) return;

	while (tokens[lIndex].type == procsym)
	{
		lIndex++;
		// checks for valid procedure declaration pattern	
		if (tokens[lIndex].type != identsym)
		{
			printparseerror(4);
			return;
		}

		int symbolTableIdx = multipledeclarationcheck(tokens[lIndex].name);
		if (symbolTableIdx != -1)
		{
			printparseerror(18);
			return;
		}

		addToSymbolTable(3, tokens[lIndex].name, 0, level, 0, 0);

		lIndex++;
		if (tokens[lIndex].type != semicolonsym)
		{
			printparseerror(4);
			return;
		}

		lIndex++;
		block();

		if (errorFlag == 1) return;

		// checks if not closed with a semicolon. 
		if (tokens[lIndex].type != semicolonsym)
		{
			printparseerror(14);
			return;
		}

		lIndex++;
		emit(2,0,0);
	}

	return;
}// END of procedure_declaration

void statement()
{
	if (errorFlag == 1) return;
	if (tokens[lIndex].type == identsym)
	{		
		// ensuring that procedure is not being assigned or read
		if (tokens[lIndex + 1].type == becomessym)
		{
			int symbolTableIdx = findsymbol(tokens[lIndex].name, 3);

			if (symbolTableIdx != -1)
			{ 
				printparseerror(6);
				return;
			}		
		} 

		int symbolTableIdx = findsymbol(tokens[lIndex].name, 2);

		if (symbolTableIdx == -1)
		{ 
				// checking for conflicting symbol declarations or undeclared identifier
			if (findsymbol(tokens[lIndex].name, 1) != findsymbol(tokens[lIndex].name, 3))
			{
				printparseerror(18);
				return;
			}
			else
			{
				printparseerror(19);
				return;
			}
		}

		lIndex++;

		// Only changes if arithmetic flag is triggered
		int instructNum = -1;
		// CHECK TO SEE IF THIS IS RIGHT!!!
		if (tokens[lIndex].type != becomessym)
		{
			if (tokens[lIndex].type == addassignsym 	||
				tokens[lIndex].type == subassignsym 	||
				tokens[lIndex].type == timesassignsym 	||
				tokens[lIndex].type == divassignsym 	||
				tokens[lIndex].type == modassignsym		)
			{
				arithmeticFlag = 1;
				// minus 29 for starting at 33 
				// minus 2 for starting at 4
				instructNum = tokens[lIndex].type - 29 - 2;
			}
			else
			{
				printparseerror(5);
				return;
			}
		}

		lIndex++;
		if (arithmeticFlag)
		{
			emit(3, level - table[symbolTableIdx].level, table[symbolTableIdx].addr);
		}
		
		expression();
		if (errorFlag == 1) return;
		
		if (arithmeticFlag)
		{
			arithmeticFlag = 0;
			emit(2, 0, instructNum);
		}
		
		emit(4, level - table[symbolTableIdx].level, table[symbolTableIdx].addr);
		return;
	}
	
	if (tokens[lIndex].type == callsym)
	{
		lIndex++;
		int symbolTableIdx = findsymbol(tokens[lIndex].name, 3);
		if (symbolTableIdx == -1)
		{
			// checking for missing, incorrect, or undeclared identifier
			if ( tokens[lIndex].type == semicolonsym || findsymbol(tokens[lIndex].name, 1) != findsymbol(tokens[lIndex].name, 2))
			{
				printparseerror(7);
				return;
			}
			else
			{
				printparseerror(19);
				return;
			}
		}

		lIndex++;
		emit(5, level - table[symbolTableIdx].level, symbolTableIdx);
		return;
	}
	
	if (tokens[lIndex].type == beginsym)
	{
	
		do
		{
			lIndex++;
			statement();

			if (errorFlag == 1) return;

		}while (tokens[lIndex].type == semicolonsym);

		// checks if begin-end statement is seperated by a semicolon or 
		// if there is no end symbol
		if (tokens[lIndex].type != endsym)
		{
			if (tokens[lIndex].type == identsym 	|| 
				tokens[lIndex].type == beginsym 	||
				tokens[lIndex].type == ifsym		||
				tokens[lIndex].type == whilesym		||
				tokens[lIndex].type == readsym		||
				tokens[lIndex].type == writesym 	||
				tokens[lIndex].type == callsym	)
			{
				printparseerror(15);
				return;
			}
			else
			{
				printparseerror(16);
				return;
			}
		}

		lIndex++;
		return;
	}

	if (tokens[lIndex].type == ifsym)
	{
		lIndex++;
		condition();
		if (errorFlag == 1) return;

		int jpcIdx = cIndex;
		emit(8, 0, jpcIdx);
		// checks if if symbol is not followed by then
		if (tokens[lIndex].type != thensym)
		{
			printparseerror(8);
			return;
		}

		lIndex++;
		statement();

		if (errorFlag == 1) return;

		if (tokens[lIndex].type == elsesym)
		{
			int jmpIdx = cIndex;
			emit(7, 0, jmpIdx);
			code[jpcIdx].m = cIndex * 3;
			lIndex++;
			statement();

			if (errorFlag == 1) return;

			code[jmpIdx].m = cIndex * 3;
		}
		else
		{
			code[jpcIdx].m = cIndex * 3;
		}

		return;		
	}

	if (tokens[lIndex].type == whilesym)
	{
		lIndex++;
		int loopIdx = cIndex;
		condition();
		if (errorFlag == 1) return;

		// checks while symbol is followed by do symbol
		if(tokens[lIndex].type != dosym)
		{
			printparseerror(9);
			return;
		}

		lIndex++;

		int jpcIdx = cIndex;
		emit(8, 0, jpcIdx);
		statement();
		if (errorFlag == 1) return;

		emit(7, 0, loopIdx * 3);
		code[jpcIdx].m = cIndex * 3;

		return;
	}

	if (tokens[lIndex].type == readsym)
	{
	
		lIndex++;

		// identifier is missing or the present one is not a var
		if (tokens[lIndex].type != identsym)
		{
			printparseerror(6); 
			return;
		}

		int symbolTableIdx = findsymbol(tokens[lIndex].name, 2);
		if (symbolTableIdx == -1)
		{
			// identifier is missing or the present one is not a var or undeclared identifier
			if (findsymbol(tokens[lIndex].name, 1) != findsymbol(tokens[lIndex].name, 3))
			{
				printparseerror(6); 
				return;
			}
			else
			{
				printparseerror(19); 
				return;
			}
		}

		lIndex++;
		emit(9, 0, 2);
		emit(4, level - table[symbolTableIdx].level, table[symbolTableIdx].addr);
		return;
	}

	if (tokens[lIndex].type == writesym)
	{
		lIndex++;
		expression();
		if (errorFlag == 1) return;

		emit(9,0,1);
		return;
	}
}// END of statement

void condition()
{
	if (errorFlag == 1) return;
	
	expression();
	if (tokens[lIndex].type == eqlsym)
	{
		lIndex++;
		expression();
		if (errorFlag == 1) return;

		emit(2, 0, 7);
	}
	else if (tokens[lIndex].type == neqsym)
	{
		lIndex++;
		expression();
		if (errorFlag == 1) return;

		emit(2, 0, 8);
	}
	else if (tokens[lIndex].type == lessym)
	{
		lIndex++;
		expression();
		if (errorFlag == 1) return;

		emit(2, 0, 9);
	}
	else if (tokens[lIndex].type == leqsym)
	{
		lIndex++;
		expression();
		if (errorFlag == 1) return;

		emit(2, 0, 10);
	}
	else if (tokens[lIndex].type == gtrsym)
	{
		lIndex++;
		expression();
		if (errorFlag == 1) return;

		emit(2, 0, 11);
	}
	else if (tokens[lIndex].type == geqsym)
	{
		lIndex++;
		expression();
		if (errorFlag == 1) return;

		emit(2, 0, 12);
	}
	else
	{
		printparseerror(10);// relational operator missing
		return;
	}
}

void expression()
{
	if (errorFlag == 1) return;

	if (tokens[lIndex].type == minussym)
	{
		lIndex++;
		term();
		if(errorFlag == 1)
			return;
		emit(2, 0, 1);
		while (tokens[lIndex].type == plussym || tokens[lIndex].type == minussym)
		{
			if(tokens[lIndex].type == plussym)
			{
				lIndex++;
				term();
				if(errorFlag == 1)
					return;
				emit(2, 0, 2);
			}
			else
			{
				lIndex++;
				term();
				if(errorFlag == 1)
					return;
				emit(2, 0, 3);
			}
		}
	}
	else
	{
		if(tokens[lIndex].type == plussym)
			lIndex++;
		term();
		if(errorFlag == 1)
			return;
		while(tokens[lIndex].type == plussym || tokens[lIndex].type == minussym)
		{
			if(tokens[lIndex].type == plussym)
			{
				lIndex++;
				term();
				if(errorFlag == 1)
					return;
				emit(2, 0, 2);
			}
			else
			{
				lIndex++;
				term();
				if(errorFlag == 1)
					return;
				emit(2, 0, 3);
			}
		}
	}

	// Bad arithmetic error
	if(	tokens[lIndex].type == lparentsym 	|| 
		tokens[lIndex].type == identsym  	||
		tokens[lIndex].type == numbersym	)
	{
		printparseerror(17);
		return;	
	}

}// END of expression

void term()
{
	if (errorFlag == 1) return;

	factor();

	if (errorFlag == 1) return;

	while (	tokens[lIndex].type == multsym		||
		tokens[lIndex].type == slashsym		||
		tokens[lIndex].type == modsym 		)
	{
		if (tokens[lIndex].type == multsym)
		{
			lIndex++;
			factor();

			if (errorFlag == 1) return;

			emit(2, 0, 4);
		}
		else if (tokens[lIndex].type == slashsym)
		{
			lIndex++;
			factor();

			if (errorFlag == 1) return;			

			emit(2, 0, 5);
		}
		else
		{
			lIndex++;
			factor();

			if (errorFlag == 1) return;

			emit(2, 0, 6);
		}
	}
}// END of term

void factor()
{
	if (errorFlag == 1) return;

	if (tokens[lIndex].type == identsym)
	{
		int symbolTableIdx2 = findsymbol(tokens[lIndex].name, 2);
		int symbolTableIdx1 = findsymbol(tokens[lIndex].name, 1);

		if (symbolTableIdx2 == -1 && symbolTableIdx1 == -1)
		{
			// checks if curr symbol is neither a number or identifier, or
			// if it is an undeclared identifier
			if (findsymbol(tokens[lIndex].name, 3) != -1)
			{
				printparseerror(11);
				return;
			}
			else
			{
				printparseerror(19);
				return;
			}
		}

		if (symbolTableIdx2 == -1)
		{
			emit(1, 0, table[symbolTableIdx1].value);
		}
		else if (symbolTableIdx1 == -1 || table[symbolTableIdx2].level > table[symbolTableIdx1].level)
		{
			emit(3, level - table[symbolTableIdx2].level, table[symbolTableIdx2].addr);
		}

		lIndex++;
	}
	else if (tokens[lIndex].type == numbersym)
	{
		emit(1, 0, tokens[lIndex].value);
		lIndex++;
	}
	else if (tokens[lIndex].type == lparentsym)
	{
		lIndex++;
		expression();

		if (errorFlag == 1) return;

		// no closing paranthesis found
		if (tokens[lIndex].type != rparentsym)
		{
			printparseerror(12);
			return;
		}

		lIndex++;
	}
	else
	{
		printparseerror(11);
		return;
	}
}// END of factor


// adds an instruction to the end of the code array
void emit(int opname, int level, int mvalue)
{
	code[cIndex].op = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// adds a symbol to the end of the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].value = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

// starts at the end of the table and works backward (ignoring already
// marked entries (symbols of subprocedures)) to mark the symbols of
// the current procedure. it knows it's finished the current procedure
// by looking at level: if level is less than the current level and unmarked
// (meaning it belongs to the parent procedure) it stops
void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

// does a linear pass through the symbol table looking for the symbol
// who's name is the passed argument. it's found a match when the names
// match, the entry is unmarked, and the level is equal to the current
// level. it returns the index of the match. returns -1 if there are no
// matches
int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	}
	return -1;
}

// returns -1 if a symbol matching the arguments is not in the symbol table
// returns the index of the desired entry if found (maximizing the level value)
int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	errorFlag = 1;
	switch (err_code)
	{
		case 1:
		printf("Parser Error: Program must be closed by a period\n");
		break;
		case 2:
		printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
		break;
		case 3:
		printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
		break;
		case 4:
		printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
		break;
		case 5:
		printf("Parser Error: Variables must be assigned using :=, +=, -=, *=, /=, or %%=\n");
		break;
		case 6:
		printf("Parser Error: Only variables may be assigned to or read\n");
		break;
		case 7:
		printf("Parser Error: call must be followed by a procedure identifier\n");
		break;
		case 8:
		printf("Parser Error: if must be followed by then\n");
		break;
		case 9:
		printf("Parser Error: while must be followed by do\n");
		break;
		case 10:
		printf("Parser Error: Relational operator missing from condition\n");
		break;
		case 11:
		printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
		break;
		case 12:
		printf("Parser Error: ( must be followed by )\n");
		break;
		case 13:
		printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
		break;
		case 14:
		printf("Parser Error: Symbol declarations should close with a semicolon\n");
		break;
		case 15:
		printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
		break;
		case 16:
		printf("Parser Error: begin must be followed by end\n");
		break;
		case 17:
		printf("Parser Error: Bad arithmetic\n");
		break;
		case 18:
		printf("Parser Error: Confliciting symbol declarations\n");
		break;
		case 19:
		printf("Parser Error: Undeclared identifier\n");
		break;
		default:
		printf("Implementation Error: unrecognized error code\n");
		break;
	}
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].addr, table[i].mark); 
	printf("\n");
}

void printassemblycode()
{
	int i;
	printf("Assembly Code:\n");
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].op);
		switch (code[i].op)
		{
			case 1:
			printf("LIT\t");
			break;
			case 2:
			switch (code[i].m)
			{
				case 0:
				printf("RTN\t");
				break;
				case 1:
				printf("NEG\t");
				break;
				case 2:
				printf("ADD\t");
				break;
				case 3:
				printf("SUB\t");
				break;
				case 4:
				printf("MUL\t");
				break;
				case 5:
				printf("DIV\t");
				break;
				case 6:
				printf("MOD\t");
				break;
				case 7:
				printf("EQL\t");
				break;
				case 8:
				printf("NEQ\t");
				break;
				case 9:
				printf("LSS\t");
				break;
				case 10:
				printf("LEQ\t");
				break;
				case 11:
				printf("GTR\t");
				break;
				case 12:
				printf("GEQ\t");
				break;
				default:
				printf("err\t");
				break;
			}
			break;
			case 3:
			printf("LOD\t");
			break;
			case 4:
			printf("STO\t");
			break;
			case 5:
			printf("CAL\t");
			break;
			case 6:
			printf("INC\t");
			break;
			case 7:
			printf("JMP\t");
			break;
			case 8:
			printf("JPC\t");
			break;
			case 9:
			printf("SYS\t");
			break;
			default:
			printf("err\t");
			break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	printf("\n");
}

