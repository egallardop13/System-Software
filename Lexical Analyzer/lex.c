// Ernesto Gallardo and Emilio Morales 
// COP3402-22Summer C001

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum token_type
{ 
	elsesym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, slashsym = 7, modsym = 8, 
	eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, 
	commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, 
	whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31, readsym = 32
}token_type;

typedef struct lexeme
{
	token_type type;
	char name[12];// must be less than a length of 11 characters.
	int value;// must be less than 5 digits long
	int error_type;// 0 means no error. 1-5 is which error.
}lexeme;

int main(int argc, char **argv)
{
	FILE *f = fopen(argv[1], "r");
	
	char input[500] = {0};
	int i = 0;
	char temp;
	while (fscanf(f, "%c", &temp) == 1)
	{
		input[i] = temp;
		i++;
	}
	
	int len = i + 1;
	fclose(f);

	char list_of_errors [5][20] = 
	{
		"Invalid Identifier",
		"Number Length",
		"Identifier Length",
		"Invalid Symbol",
		"Never-ending Comment"		
	};
	
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");

	lexeme token_list[500];
	int tokenIdx = 0;
	for (int k = 0; k < len; k++)
	{
		char buffer[100];
		// ignores invisible characters 
		while (isspace(input[k]) || iscntrl(input[k])) k++;
		// check if it is division or comment
		if (input[k]=='/')
		{
			int j = 0;
			buffer[j] = input[k];
			j++;
			k++;

			// its a comment
			if (input[k]=='*')
			{
				buffer[j] = input[k];
				j++;
				k++;
				buffer[j] = '\0';

				while(input[k] != '*')
				{
			 		// checks for neverending comment
					if (k >= len)
					{
						token_list[tokenIdx].error_type = 5;
						break;
					}

					k++;
				} 

			 	// neverending comment
				if (k >= len)
				{
					printf("Lexical Analyzer Error: %s\n", list_of_errors[token_list[tokenIdx].error_type - 1]);
					break;
				} 

			 	// end of comment
				if (input[k] =='/')
				{
					token_list[tokenIdx].type = 7;
					strcpy(token_list[tokenIdx].name, buffer);
					token_list[tokenIdx].error_type = 0;
				}
			}

			// its division
			else
			{
				token_list[tokenIdx].type = 7;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}

			tokenIdx++;
		}
		// Checking for NEQ or invalid
		if (input[k]=='!')
		{
			int j = 0;
			buffer[j] = input[k];
			j++;
			k++;
			buffer[j] = '\0';

			// invalid
			if (input[k] != '=')
			{
				token_list[tokenIdx].error_type = 4;
			}
			// is NEQ
			else
			{
				buffer[j] = input[k];
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 10;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				k++;
			}

			if (token_list[tokenIdx].error_type > 0 )
			{
				printf("Lexical Analyzer Error: %s\n", list_of_errors[token_list[tokenIdx].error_type - 1]);
			}
			else
			{
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);			
			}

			tokenIdx++;
		}

		//Check for GTR or GEQ
		if (input[k]=='>')
		{
			int j = 0;
			buffer[j] = input[k];
			j++;
			k++;
			buffer[j] = '\0';

			// is GTR
			if (input[k] != '=')
			{
				token_list[tokenIdx].type = 13;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			// is GEQ
			else
			{
				buffer[j] = input[k];
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 14;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			
			printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
			tokenIdx++;			
		}

		//Check for LSS or LEQ
		if (input[k]=='<')
		{
			int j = 0;
			buffer[j] = input[k];
			j++;
			k++;
			buffer[j] = '\0';

			// is LSS
			if (input[k] != '=')
			{
				token_list[tokenIdx].type = 11;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;	
			}
			// is LEQ
			else
			{
				buffer[j] = '=';
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 12;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				k++;
			}

			printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);			
			tokenIdx++;
		}

		//Check for letter
		if (isalpha(input[k]))
		{
			int j = 0;
			buffer[j] = input[k];
			j++;
			k++;
			while (isalpha(input[k]) || isdigit(input[k]))
			{
				buffer[j] = input[k];
				k++;
				j++;
			}
			buffer[j] = '\0';

			// if j > 11, then this is error type of 3
			if (j > 11)
			{
				token_list[tokenIdx].error_type = 3;
			}
			// checking if we have a reserved word.
			else if (strcmp(buffer, "begin") == 0)
			{
				token_list[tokenIdx].type = 21;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "end") == 0)
			{
				token_list[tokenIdx].type = 22;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "if") == 0)
			{
				token_list[tokenIdx].type = 23;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "then") == 0)
			{
				token_list[tokenIdx].type = 24;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "while") == 0)
			{
				token_list[tokenIdx].type = 25;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "do") == 0)
			{
				token_list[tokenIdx].type = 26;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "call") == 0)
			{
				token_list[tokenIdx].type = 27;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "const") == 0)
			{
				token_list[tokenIdx].type = 28;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "var") == 0)
			{
				token_list[tokenIdx].type = 29;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "procedure") == 0)
			{
				token_list[tokenIdx].type = 30;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "write") == 0)
			{
				token_list[tokenIdx].type = 31;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			else if (strcmp(buffer, "read") == 0)
			{
				token_list[tokenIdx].type = 32;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			// was not a reserved word 
			else
			{
				token_list[tokenIdx].type = 2;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
			}
			
			if (token_list[tokenIdx].error_type > 0 )
			{
				printf("Lexical Analyzer Error: %s\n", list_of_errors[token_list[tokenIdx].error_type - 1]);
			}
			else
			{
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
			}

			tokenIdx++;
		}//Check for digit
		else if (isdigit(input[k]))
		{
			int j = 0;
			buffer[j] = input[k];
			j++;
			k++;
			//getting number
			while (isdigit(input[k]))
			{
				buffer[j] = input[k];
				k++;
				j++;
			}

			buffer[j] = '\0';
			//check for letter
			if (isalpha(input[k]))
			{
				token_list[tokenIdx].error_type = 1;
			}
			else if (j > 5)
			{
				token_list[tokenIdx].error_type = 2;
			}
			else
			{
				token_list[tokenIdx].type = 3;// identsym
				token_list[tokenIdx].value = atoi(buffer);
				token_list[tokenIdx].error_type = 0;
			}

			if (token_list[tokenIdx].error_type > 0 )
			{
				printf("Lexical Analyzer Error: %s\n", list_of_errors[token_list[tokenIdx].error_type - 1]);
			}
			else
			{
				if (token_list[tokenIdx].type == 3)
				{
					printf("\t%d\t%d\n", token_list[tokenIdx].value, token_list[tokenIdx].type);
				}
				else
				{
					printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				}
			}

			tokenIdx++;
		}
		//Setting up J and buffer for special cases 
		int j = 0;
		buffer[j] = input[k];
		switch(buffer[j])
		{
			case '+':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 4;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case '-':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 5;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;	
			}

			case '*':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 6;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case '%':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 8;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;	
			}

			case '=':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 9;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case '(':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 15;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case ')':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 16;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case ',':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 17;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case ';':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 18;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case '.':
			{
				buffer[++j] = '\0';
				token_list[tokenIdx].type = 19;
				strcpy(token_list[tokenIdx].name, buffer);
				token_list[tokenIdx].error_type = 0;
				//printf("%d\n", token_list[tokenIdx].type);
				printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
				tokenIdx++;
				break;
			}

			case ':':
			{
				k++;
				if (input[k] == '=')
				{	
					j++;
					buffer[j] = input[k];
					buffer[++j] = '\0';

					token_list[tokenIdx].type = 20;
					strcpy(token_list[tokenIdx].name, buffer);
					token_list[tokenIdx].error_type = 0;
					//printf("%d\n", token_list[tokenIdx].type);
					printf("\t%s\t%d\n", token_list[tokenIdx].name, token_list[tokenIdx].type);
					tokenIdx++;
				}	

				break;
			}
		}// end of switch	
	}// end of for

	printf("Token List: \n");
	int tokenNum = tokenIdx;
	for (int i = 0; i < tokenNum; i++)
	{
		if (token_list[i].type == 2)
		{
			printf("%d %s ", token_list[i].type, token_list[i].name);
		}
		else if (token_list[i].type == 3)
		{
			printf("%d %d ", token_list[i].type, token_list[i].value);	
		}
		else
		{
			printf("%d ", token_list[i].type);	
		}	
	}

	return 0;
}	

