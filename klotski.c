#include <stdio.h>
#include <stdlib.h>

typedef struct board{
	int NumOfMoves;
	int state[5][4];
	struct board *Parent;
	struct board *FirstChild;
	struct board *NextBrother;
} board;

void printBoard(int pstate[][5]);
// void initializeStructBoard(int NumOfMoves, char state[][4], board *Parent, board *FirstChild, board *NextBrother);

int main(void){
	int InitialState[4][5] = {
		{1, 1, 2, 3, 4},
		{5, 5, 6, 3, 0},
		{5, 5, 6, 7, 0},
		{8, 8, 9, 7, 10}
	};
	// board *initial = malloc(sizeof(board));

	// initializeStructBoard(initial, 0, InitialState, NULL, NULL, NULL);

	printBoard(InitialState);
	return 0;
}

void printBoard(int pstate[][5]){
	int xi, yi;
	for(yi = 0; yi < 5; yi++){
		for(xi = 0; xi < 4; xi++){
			printf("%4d", pstate[xi][yi]);
		}
		printf("\n");
	}
	printf("--------------------\n");
}

// void initializeStructBoard(board *pb, int NumOfMoves, char state[][4], board *Parent, board *FirstChild, board *NextBrother){
// 	int xi,yi;
// 	for(yi = 0; yi < 5; yi)
// 	pb->NumOfMoves = pb;
// 	pb->Parent = Parent;
// 	pb->FirstChild = FirstChild;
// 	pb->NextBrother = NextBrother;
// }
