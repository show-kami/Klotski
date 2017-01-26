#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define QUEUE 1000000
#define HASH 99999

// #define DEBUG

// define structs
typedef struct board{
	int NumOfMoves;
	int state[4][5];
	struct board *Parent;
	struct board *FirstChild;
	struct board *NextBrother;
} board;
typedef struct hash{
	struct hash *next;
	board *pboard;
} hash;

// prototype declaration of functions
void *mymalloc(int size);
board **executeKlotskiGame(int InitialState[][5], board **result);
void printQueue(board **queue, int head, int tail);
void printBoard(int pstate[][5]);
void initializeBoard(
	board *pb,
	int NumOfMoves,
	int state[][5],
	board *Parent,
	board *FirstChild,
	board *NextBrother
);
int checkEmpty(int state[][5], int piece, int direction, int MoveOrNot);
int checkGoal(board *pb);
void appendIntoQueue(board *queue[], board *value, int *head, int *tail);
board *pickFromQueue(board *queue[], int *head, int *tail);
int compareStateWithAnother(int pstate1[][5],int pstate2[][5]);
unsigned int calculateHashKey(int pstate[][5]);
int checkHashTable(board *IsThisNew);
void printHashTable(void);

// declaration of global variables
hash *HashTable[HASH];

int main(void){
	int ii;
	int ResultNumOfMoves = 0;
	int InitialState[4][5] = {
		{5, 5, 2, 3, 4},
		{1, 1, 6, 3, 0},
		{1, 1, 6, 7, 0},
		{8, 8, 9, 7, 10}
	};
	
	board **result = NULL;

	// グローバル変数の初期化
	for(ii = 0; ii < HASH; ii++){
		HashTable[ii] = NULL;
	}

	result = executeKlotskiGame(InitialState, result);
	ResultNumOfMoves = result[0]->NumOfMoves;
	
	return 0;
}

void *mymalloc(int size){
	void *pointer = malloc(size);
	if(pointer == NULL){
		fprintf(stderr, "ERROR: allocation failed.\n");
		exit(1);
	}
	return pointer;
}

// 実行部分
board **executeKlotskiGame(int InitialState[][5], board **result){
	// 必要な変数の宣言
	int ii, piece, direction; /* ループカウンタ */
	board *proot, *pworking;
	board *queue[QUEUE];
	board *pnew = NULL;
	int head = 0;
	int tail = 0;
	int ResultNumOfMoves = 0;

	proot = mymalloc(sizeof(board));
	initializeBoard(proot, 0, InitialState, NULL, NULL, NULL);
	for(ii = 0; ii < QUEUE; ii++){
		queue[ii] = NULL;
	}
	pworking = proot;

	// 各世代ごとにループを回す
	while(1){
		int generation;
		if(queue[head] == NULL){
			generation = 0;
		} else {
			generation = queue[head]->NumOfMoves;
		} 
		printf("-----\nNEW GENERATION %d th\n", generation);
		printf("queue: %d ~ %d\n", head, tail);

		// 子作り。第F世代の次世代を全て探す
		do{
			if(head != tail){
				pworking = pickFromQueue(queue, &head, &tail);
			}
			/* pworkingの子を探していく */
			board *pyoungest = NULL; /* pworkingのこの中で一番最近作られたもの */
			/* ピース1から10を順に，動かすことができるかどうか確かめていく。 */
			/* 動かせれば，動かして，新しい盤面を子とする */
			for(piece = 1; piece <= 10; piece++){
				for(direction = 1; direction <= 4; direction++){
					/* 上下左右，それぞれ1マス動かせるか。動かせれば動かす。 */
					int check = checkEmpty(pworking->state, piece, direction, FALSE);
					if(check == 1){
						/* 新しい構造体を用意し，初期化。 */
						pnew = mymalloc(sizeof(board));
						initializeBoard(pnew, (pworking->NumOfMoves)+1, pworking->state, pworking, NULL, NULL);
						/* 新しく作った構造体において，ピースの移動を実行 */
						checkEmpty(pnew->state, piece, direction, TRUE);
						/* 移動したあとが今まで見たことない盤面であるかどうかチェック */
						if(checkHashTable(pnew) != 0){
							free(pnew);
							continue;
						}
						appendIntoQueue(queue, pnew, &head, &tail);
						/* 兄や親のNextBroやFirstChldを更新 */
						if(pworking->FirstChild == NULL){
							pworking->FirstChild = pnew;
						} else {
							pyoungest->NextBrother = pnew;
						}
						/* 今作られた子がpworkingのこの中で一番若い */
						pyoungest = pnew;
						/* こいつがゴールであれば，計算打ち切っちゃってよい。 */
						if(checkGoal(pnew) == 1){
							goto OUT;
						}
					}
				}
			}
		} while(queue[head]->NumOfMoves == generation);
	}
	OUT:
	pworking = pnew;
	ResultNumOfMoves = pworking->NumOfMoves;
	result = mymalloc(ResultNumOfMoves * sizeof(board *));
	if(result == NULL){
		fprintf(stderr, "ERROR @%d: allocation failed\n", __LINE__);
		exit(1);
	}
	for(ii = 0; ii < ResultNumOfMoves; ii++){
		result[ii] = pworking;
		pworking = pworking->Parent;
	}
	return result;
}

// キューをプリントするための関数を作成
void printQueue(board **queue, int head, int tail){
	int ii;
	for(ii = head; ii <= tail; ii++){
		if(head > tail){
			tail += QUEUE;
		}
		if(ii >= QUEUE){
			printf("%p, ", (void *)queue[ii - QUEUE]);
		} else {
			printf("%p, ", (void *)queue[ii]);
		}
	}
	printf("\n");
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

void initializeBoard(
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
// 引数		:int MoveOrNot (1: 動かしたい方向に動かす。 0: 動かさない)
//--------------------------------------------------------------------------
int checkEmpty(int state[][5], int piece, int direction, int MoveOrNot){
	int xi, yi; /* ループカウンタ */
	int x,y;
	int xdest, ydest; /* destination */
	for(yi = 0; yi < 5; yi++){
		for(xi = 0; xi < 4; xi++){
			/* 移動方向が走査方向と一致してしまっている下移動・右移動では，走査方向を逆転させたい */
			if(direction == 2 || direction == 4){
				x = 3-xi;
				y = 4-yi;
			} else {
				x = xi;
				y = yi;
			}
			/* 座標(x, y)に動かしたいピースが置かれていなければ，その座標はスルー */
			if(state[x][y] != piece){
				continue;
			}
			/* 動かしたいピースの，動かしたい先の座標を取得 */
			switch(direction){
				case 1:
					xdest = x;
					ydest = y - 1;
					break;
				case 2:
					xdest = x;
					ydest = y + 1;
					break;
				case 3:
					xdest = x - 1;
					ydest = y;
					break;
				case 4:
					xdest = x + 1;
					ydest = y;
					break;
			}
			/* 動かしたい先に動かせるかどうか判定。動かせなければ，0を返して関数を終了する */
			if(xdest < 0 || xdest >= 4 || ydest < 0 || ydest >= 5){
				return 0;
			}
			if(state[xdest][ydest] != 0 && state[xdest][ydest] != piece){
				return 0;
			}

			/* ピースを動かす指定があれば，動かす */
			if (MoveOrNot == TRUE){
				int tmp = state[xdest][ydest];
				state[xdest][ydest] = state[x][y];
				state[x][y] = tmp;
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

void appendIntoQueue(board *queue[], board *value, int *head, int *tail){
	if((*head) == (*tail) + 1){
		fprintf(stderr, "ERROR @%d: queue is too small (head: %d, tail: %d)\n", __LINE__, *head, *tail);
		exit(1);
	}
	if(queue[*tail] != NULL){
		(*tail)++;
	}
	if((*tail) >= QUEUE){
		(*tail) = (*tail) - QUEUE;
	}
	queue[*tail] = value;
	return;
}

board *pickFromQueue(board *queue[], int *head, int *tail){
	if((*head) == (*tail)){
		fprintf(stderr, "ERROR @%d: invalid picking from queue.\n", __LINE__);
		exit(1);
	}
	int tmp = *head;
	(*head)++;
	if((*head) >= QUEUE){
		(*head) = (*head) - QUEUE;
	}
	return queue[tmp];
}

//--------------------------------------------------------------------------
// 関数名	:compareStateWithAnother
// 概要		:盤面の状態1と状態2とが同じであるか，同じでないかを判定する
// 戻り値	:1: 同じ。0: 違う。
// 引数		:状態1および状態2
//--------------------------------------------------------------------------
int compareStateWithAnother(int pstate1[][5],int pstate2[][5]){
	int xi, yi;
	for(yi = 0; yi < 5; yi++){
		for(xi = 0; xi < 4; xi++){
			if(pstate1[xi][yi] != pstate2[xi][yi]){
				return 0;
			}
		}
	}
	return 1;
}

//--------------------------------------------------------------------------
// 関数名	:calculateHashKey
// 概要		:ハッシュキーを計算する
// 戻り値	:ハッシュキー
// 引数		:盤面の状態
//--------------------------------------------------------------------------
unsigned int calculateHashKey(int pstate[][5]){
	// ハッシュは，ピース番号2, 4, 9, 10の位置によって決めることにする
	unsigned int key = 0;
	int pos[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int xi,yi,pi;
	for(yi = 0; yi < 5; yi++){
		for(xi = 0; xi < 4; xi++){
			if(pstate[xi][yi] != 0){
				pos[pstate[xi][yi] - 1] = xi + 4 * yi;
			}
		}
	}
	for(pi = 1; pi <= 10; pi++){
		key += pos[pi-1] * pow(4, pi);
	}
	key = key % HASH;
	return key;
}

//--------------------------------------------------------------------------
// 関数名	:checkHashTable
// 概要		:新しく生成した盤面IsThisNewが，今までに出てきたことがないかどうかを確認する。
// 戻り値	:int (0: 出てきたことはない。 0以外: 出てきたことがある)
// 引数		:hash **HashTable (ハッシュテーブルである配列)
// 引数		:board *IsThisNew (判定をしたい盤面)
//--------------------------------------------------------------------------
int checkHashTable(board *IsThisNew){
	int identifier = 0;
	unsigned int key = calculateHashKey(IsThisNew->state);
	hash *new;

	// ハッシュテーブルに格納するためのハッシュ構造体を新しく作成する
	new = mymalloc(sizeof(hash));
	new->next = NULL;
	new->pboard = IsThisNew;

	// 盤面が新しいかどうか判定＆ハッシュテーブルに新しい構造体を格納
	if(HashTable[key] == NULL){
		HashTable[key] = new;
	} else {
		hash *searching = HashTable[key];
		hash *LastSearched = NULL;
		do{
			identifier = compareStateWithAnother(IsThisNew->state, (searching->pboard)->state);
			if(identifier == 1){
				break;
			}
			LastSearched = searching;
			searching = searching->next;
		} while (searching != NULL);
		if(identifier == 0){
			LastSearched->next = new;
		} else {
			free(new);
		}
	}
	
	// 戻る
	return identifier;
}

void printHashTable(void){
	unsigned int ki;
	hash *printed;

	for(ki = 0; ki < HASH; ki++){
		printf("key %u: ", ki);
		if(HashTable[ki] == NULL){
			printf("NULL. \n\n");
			continue;
		}
		printed = HashTable[ki];
		do{
			printf("%p -> ", (void *)printed);
			printed = printed->next;
		} while (printed != NULL);
		printf("key %u end. \n\n", ki);
	}
	printf("End of printing hash. \n");
	return;
}
