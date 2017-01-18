#include <stdio.h>
#include <stdlib.h>

typedef struct board{
	int NumOfMoves;
	int state[4][5];
	struct board *Parent;
	struct board *FirstChild;
	struct board *NextBrother;
} board;

void printBoard(int pstate[][5]);
void initializeStructBoard(
	board *pb,
	int NumOfMoves,
	int state[][5],
	board *Parent,
	board *FirstChild,
	board *NextBrother
);
int checkEmpty(int state[][5], int piece, int direction);
int checkGoal(board *pb);

int main(void){
	int piece, xi, yi; /* ループカウンタ */
	int NumOfMoves;
	int InitialState[4][5] = {
		{5, 5, 2, 3, 4},
		{1, 1, 6, 3, 0},
		{1, 1, 6, 7, 0},
		{8, 8, 9, 7, 10}
	};
	board *proot, *pparent, *pworking;

	proot = malloc(sizeof(board));
	initializeStructBoard(proot, 0, InitialState, NULL, NULL, NULL);
	pparent = proot;

	// while(1){
	// 	// ゴールに達しない限り，ゲームを続ける

	// 	// pparent の子を列挙する
	// 	/* ピース1から10を順に，動かすことができるかどうか確かめていく。 */
	// 	/* 動かせれば，動かして，新しい盤面を子とする */
	// 	for(piece = 1; piece <= 10; piece++){
	// 		/* 上に1マス動かせるか */
	// 		checkEmpty(pparent->state, piece, "up");
	// 	}


	// 	// pparent を，その弟に更新する
	// }
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

void initializeStructBoard(
	board *pb,
	int NumOfMoves,
	int state[][5],
	board *Parent,
	board *FirstChild,
	board *NextBrother
){
	int xi,yi;
	for(yi = 0; yi < 5; yi++){
		for(xi = 0; xi < 4; xi++){
			pb->state[xi][yi] = state[xi][yi];
		}
	}
	pb->NumOfMoves = NumOfMoves;
	pb->Parent = Parent;
	pb->FirstChild = FirstChild;
	pb->NextBrother = NextBrother;
	return;
}

//--------------------------------------------------------------------------
// 関数名	:checkEmpty
// 概要		:
// 戻り値	:0であれば動かせない。1であれば動かせる
// 引数		:int state[][5] (ボードの状況)
// 引数		:int piece (動かしたいピース)
// 引数		:int direction (動かしたい方向 1:up, 2:down, 3:left, 4:right)
//--------------------------------------------------------------------------
int checkEmpty(int state[][5], int piece, int direction){
	int xi, yi; /* ループカウンタ */
	int xdest, ydest; /* destination */
	for(yi = 0; yi < 5; yi++){
		for(xi = 0; xi < 4; xi++){
			/* 座標(xi, yi)に動かしたいピースが置かれていなければ，その座標はスルー */
			if(state[xi][yi] != piece){
				continue;
			}
			/* 動かしたいピースの，動かしたい先の座標を取得 */
			switch(direction){
				case 1:
					xdest = xi;
					ydest = yi - 1;
					break;
				case 2:
					xdest = xi;
					ydest = yi + 1;
					break;
				case 3:
					xdest = xi - 1;
					ydest = yi;
					break;
				case 4:
					xdest = xi + 1;
					ydest = yi;
					break;
			}
			/* 動かしたい先に動かせるかどうか判定。動かせなければ，0を返して関数を終了する */
			if(xdest < 0 || xdest >= 4 || ydest < 0 || ydest >= 5){
				return 0;
			}
			if(state[xdest][ydest] != 0 && state[xdest][ydest] != piece){
				return 0;
			}
		}
	}
	return 1;
}

//--------------------------------------------------------------------------
// 関数名	:checkGoal
// 概要		:2x2のピース (ピース番号1) が一番下の中央に来たら，ゲーム終了とする
// 戻り値	:終了:1, その他:0
// 引数		:*pb (ボードへのポインタ)
//--------------------------------------------------------------------------
int checkGoal(board *pb){
	if(pb->state[1][4] == 1 && pb->state[2][4] == 1){
		return 1;
	} else {
		return 0;
	}
}
