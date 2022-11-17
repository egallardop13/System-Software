// Emilio Morales and Ernesto Gallardo
// COP3402-22Summer C001

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

struct IR
{
	int op; // Opcode
	int l; // Lexicographical level
	int m; // Modifier
};

int base(int L, int BP, int pas[3000])
{
	int arb = BP;// arb = activation record base
	
	//find base L levels down
	while ( L > 0)  
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

void virtual_machine(instruction *code)
{
	int pas[3000];
	int i = 0;
	// indexer for pas in this loop
	int k = 0;
	while (code[i].op != -1)
	{
		pas[k] = code[i].op;
		pas[k + 1] = code[i].l;
		pas[k + 2] = code[i].m;
		k += 3;
		i++;
	}

	// used for properly printing activation records
	int arIndexes[100];
	int arListIdx = 0;// idx for arIndexes

	int len = i;// MAYBE CHANGE???

	// Fetch-Execute cycle
	int halt = 1;
	
	int sp = k - 1;
	int bp = sp + 1;
	int pc = 0;
	struct IR ir;
	printf("VM Execution: \n");
	printf("\t\tPC \tBP \tSP \tstack\n");
	printf("Initial values: %d \t%d \t%d\n\n", pc, bp, sp);

	while (halt == 1)
	{
		// Fetch
		ir.op = pas[pc];
		ir.l = pas[++pc];
		ir.m = pas[++pc];

		pc++;

		// Execute
		switch(ir.op)
		{
			// LIT 
			// Pushes a constant value (literal) M onto the stack.
			case 1:
			{
				sp++;
				pas[sp] = ir.m;

				printf("LIT  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

				int arCount = 0;// temp idx for arList
				for (int j = k; j <= sp; j++)
				{	
					if ( (j == arIndexes[arCount]))
					{
						arCount++;
						printf("| ");// new activation record	
					} 

					printf("%d ", pas[j]);
				}


				printf("\n");
				break;
			} 

			// OPR 
			// Operation to be performed on the data at the top of the stack. (or return from function)	
			case 2:
			{
				switch(ir.m)
				{
					// RETURN
					case 0:
					{
						sp = bp - 1;
						bp = pas[sp + 2];
						pc = pas[sp + 3];

						printf("RTN  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}

						printf("\n");
						break;	
					}

					// NEGATE
					case 1:
					{
						pas[sp] = pas[sp] * -1;

						printf("NEG  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// ADD
					case 2:
					{
						sp--;
						pas[sp] = pas[sp] + pas[sp + 1];

						printf("ADD  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// SUBTRACT
					case 3:
					{
						sp--;
						pas[sp] = pas[sp] - pas[sp + 1];

						printf("SUB  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// MULTIPLY
					case 4:
					{
						sp--;
						pas[sp] = pas[sp] * pas[sp + 1];
						
						printf("MUL  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);
						
						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// DIVIDE
					case 5:
					{
						sp--;
						pas[sp] = pas[sp] / pas[sp + 1];

						printf("DIV  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// MOD
					case 6:
					{
						sp--;
						pas[sp] = pas[sp] % pas[sp + 1];

						printf("MOD  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// EQUALS
					case 7:
					{	
						sp--;
						pas[sp] = (pas[sp] == pas[sp + 1]);

						printf("EQL  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// NOT EQUAL
					case 8:
					{
						sp--;
						pas[sp] = (pas[sp] != pas[sp + 1]);

						printf("NEQ  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// LESS THAN
					case 9:
					{
						sp--;
						pas[sp] = (pas[sp] < pas[sp + 1]);

						printf("LSS  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// LESS THAN OR EQUAL TO
					case 10:
					{
						sp--;
						pas[sp] = (pas[sp] <= pas[sp + 1]);

						printf("LEQ  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// GREATER THAN
					case 11:
					{
						sp--;
						pas[sp] = (pas[sp] > pas[sp + 1]);

						printf("GTR  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// GREATER THAN OR EQUAL TO
					case 12:
					{
						sp--;
						pas[sp] = (pas[sp] >= pas[sp + 1]);

						printf("GEQ  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}
				}// switch in case 2

				break;	
			}// case 2

			// LOD
			// Load value to top of stack from the stack location at offset M from L lexicographical levels down.
			case 3:
			{
				sp++;
				pas[sp] = pas[base(ir.l, bp, pas) + ir.m];

				printf("LOD  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

				int arCount = 0;// temp idx for arList
				for (int j = k; j <= sp; j++)
				{	
					if ( (j == arIndexes[arCount]))
					{
						arCount++;
						printf("| ");// new activation record	
					} 

					printf("%d ", pas[j]);
				}


				printf("\n");
				break;
			}

			// STO
			// Store value at top of stack in the stack location at offset M from L lexicographical levels down.
			case 4:
			{
				pas[base(ir.l, bp, pas) + ir.m] = pas[sp];
				sp--;

				printf("STO  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

				int arCount = 0;// temp idx for arList
				for (int j = k; j <= sp; j++)
				{	
					if ( (j == arIndexes[arCount]))
					{
						arCount++;
						printf("| ");// new activation record	
					} 

					printf("%d ", pas[j]);
				}


				printf("\n");
				break;
			}

			// CAL
			// Call procedure at code index M (generates new Activation Record and PC = M).
			case 5:
			{
				pas[sp + 1] = base(ir.l, bp, pas);// static link (SL)
				pas[sp + 2] = bp;// dynamic link (DL)
				pas[sp + 3] = pc;// return address (RA)
				bp = sp + 1;
				pc = ir.m;

				// storing startIdx of new activation record
				arIndexes[arListIdx] = bp;
				arListIdx++; 

				printf("CAL  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

				int arCount = 0;// temp idx for arList
				for (int j = k; j <= sp; j++)
				{	
					if ( (j == arIndexes[arCount]))
					{
						arCount++;
						printf("| ");// new activation record	
					} 

					printf("%d ", pas[j]);
				}


				printf("\n");
				break;
			}

			// INC
			// Allocate M memory words (increment SP by M). 
			// First four are reserved to Static Link (SL), Dynamic Link (DL), and Return Address (RA).
			case 6:
			{	
				sp += ir.m;

				printf("INC  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);
				
				int arCount = 0;// temp idx for arList
				for (int j = k; j <= sp; j++)
				{	
					if ( (j == arIndexes[arCount]))
					{
						arCount++;
						printf("| ");// new activation record	
					} 

					printf("%d ", pas[j]);
				}

				printf("\n");
				break;
			}

			// JMP
			// Jump to instruction M (PC = M).
			case 7:
			{
				pc = ir.m;
				printf("JMP  %d %d \t%d \t%d \t%d", ir.l, ir.m, pc, bp, sp);

				int arCount = 0;// temp idx for arList
				for (int j = k; j <= sp; j++)
				{	
					if ( (j == arIndexes[arCount]))
					{
						arCount++;
						printf("| ");// new activation record	
					} 

					printf("%d ", pas[j]);
				}

				printf("\n");
				break;
			}

			// JPC
			// Jump to instruction M if top stack element is 0.
			case 8:
			{
				if (pas[sp] == 0)
				{
					pc = ir.m;
				}

				sp--;

				printf("JPC  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

				int arCount = 0;// temp idx for arList
				for (int j = k; j <= sp; j++)
				{	
					if ( (j == arIndexes[arCount]))
					{
						arCount++;
						printf("| ");// new activation record	
					} 

					printf("%d ", pas[j]);
				}


				printf("\n");
				break;
			}

			// SYS
			case 9:
			{
				switch(ir.m)
				{
					// Write the top stack element to the screen.
					case 1:
					{
						printf("Output result is: %d\n", pas[sp]);
						sp--;

						printf("SYS  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}


						printf("\n");
						break;
					}

					// Read in input from the user and store it on top of the stack.
					case 2:
					{
						sp++;
						printf("Please Enter an Integer: ");
						scanf("%d", &pas[sp]);
						printf("SYS  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);
						
						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}

						printf("\n");
						break;
					}
					// End of program (Set Halt flag to zero).
					case 3:
					{
						halt = 0;

						printf("SYS  %d %d \t%d \t%d \t%d \t", ir.l, ir.m, pc, bp, sp);

						int arCount = 0;// temp idx for arList
						for (int j = k; j <= sp; j++)
						{	
							if ( (j == arIndexes[arCount]))
							{
								arCount++;
								printf("| ");// new activation record	
							} 

							printf("%d ", pas[j]);
						}

						printf("\n");
						break;
					}
				}

				break;
			}// case 9
		}// main switch
	}//while
/*
	for (int i = 0; i <= arListIdx; i++)
	{
		printf("------AR INDEXES------\n");
		printf("%d ", arIndexes[i]);
	}
*/
	return;
}
