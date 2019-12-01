#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#pragma warning(disable:4996)

// 키보드 값
#define LEFT 75		//좌로 이동
#define RIGHT 77	//우로 이동 
#define UP 72		//회전 
#define DOWN 80		//soft drop
#define SPACE 32	//hard drop
#define p 112		//일시정지 
#define P 80		//일시정지
#define ESC 27		//게임종료 

#define false 0
#define true 1

#define ACTIVE_BLOCK -2		// 게임판배열에 저장될 블록의 상태들 
#define CEILLING -1			// 블록이 이동할 수 있는 공간은 0 또는 음의 정수로 표현 
#define EMPTY 0				// 블록이 이동할 수 없는 공간은 양수로 표현 
#define WALL 1
#define INACTIVE_BLOCK 2	// 이동이 완료된 블록값 

#define MAIN_X 11 //게임판 가로크기 
#define MAIN_Y 23 //게임판 세로크기 
#define MAIN_X_ADJ 3 //게임판 위치조정 
#define MAIN_Y_ADJ 1 //게임판 위치조정 

#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1 //게임정보표시 위치조정 

int STATUS_Y_GOAL;  // GOAL 정보표시위치Y 좌표 저장 
int STATUS_Y_LEVEL; // LEVEL 정보표시위치Y 좌표 저장
int STATUS_Y_SCORE; // SCORE 정보표시위치Y 좌표 저장

int main_org[MAIN_Y][MAIN_X]; //게임판의 정보를 저장하는 배열 모니터에 표시후에 main_cpy로 복사됨 
int main_cpy[MAIN_Y][MAIN_X]; /* 즉 maincpy는 게임판이 모니터에 표시되기 전의 정보를 가지고 있음
							  main의 전체를 계속 모니터에 표시하지 않고(이렇게 하면 모니터가 깜빡거림)
							  main_cpy와 배열을 비교해서 값이 달라진 곳만 모니터에 고침 */

typedef struct Game {

	// game 정보 설정

	int key;				//키보드로 입력받은 키값을 저장 
	int speed;				//게임진행속도 
	int level;				//현재 level 
	int level_goal;			//다음레벨로 넘어가기 위한 목표점수 
	int cnt;				//현재 레벨에서 제거한 줄 수를 저장 
	int score;				//현재 점수 
	int last_score;			//마지막게임점수 
	int best_score;			//최고게임점수 
	int crush_on;			//현재 이동중인 블록이 충돌 상태인지 알려주는 flag 

}Game;



void title(void);
void reset(Game *game);
void draw_map(Game *game);
void reset_main(void);
void draw_main(void);


// 커서 숨기기

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; //커서숨기는 함수에 사용되는 열거형

void setcursortype(CURSOR_TYPE c) {

	// 커서를 숨기거나 나타낸다.

	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
} // setcursortype 함수

void gotoxy(int x, int y) {
	/*
	커서를 (x, y)로 이동한다.
	x : 이동할 x 좌표
	y : 이동할 y 좌표
	*/
	COORD pos = { 2 * x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
} // gotoxy 함수 

int main() {
	int i;
	Game game;

	srand((unsigned)time(NULL));	// 난수표생성
	setcursortype(NOCURSOR);		// 커서 없앰
	title();						// 메인타이틀 호출 
	reset(&game);						// 게임 정보 리셋




	return 0;

} // main 함수

void title(void) {
	// 게임 타이틀을 나타내는 함수

	int x = 5;	//타이틀화면이 표시되는 x좌표 
	int y = 4;	//타이틀화면이 표시되는 y좌표 
	int cnt;	//타이틀 프레임을 세는 변수 

	gotoxy(x, y + 0); printf("■□□□■■■□□■■□□■■"); Sleep(100);
	gotoxy(x, y + 1); printf("■■■□  ■□□    ■■□□■"); Sleep(100);
	gotoxy(x, y + 2); printf("□□□■              □■  ■"); Sleep(100);
	gotoxy(x, y + 3); printf("■■□■■  □  ■  □□■□□"); Sleep(100);
	gotoxy(x, y + 4); printf("■■  ■□□□■■■□■■□□"); Sleep(100);
	gotoxy(x, y + 5); printf("      blog.naver.com/azure0777"); Sleep(100);
	gotoxy(x + 5, y + 2); printf("T E T R I S"); Sleep(100);
	gotoxy(x, y + 7); printf("Please Enter Any Key to Start..");
	gotoxy(x, y + 9); printf("  △   : Shift");
	gotoxy(x, y + 10); printf("◁  ▷ : Left / Right");
	gotoxy(x, y + 11); printf("  ▽   : Soft Drop");
	gotoxy(x, y + 12); printf(" SPACE : Hard Drop");
	gotoxy(x, y + 13); printf("   P   : Pause");
	gotoxy(x, y + 14); printf("  ESC  : Quit");
	gotoxy(x, y + 16); printf("BONUS FOR HARD DROPS / COMBOS");

	for (cnt = 0;; cnt++) { // cnt를 1씩 증가시키면서 계속 반복    //하나도 안중요한 별 반짝이는 애니메이션효과 
		if (kbhit()) break; //키입력이 있으면 무한 루프 종료 
		if (cnt % 200 == 0) { gotoxy(x + 4, y + 1); printf("★"); }       //cnt가 200으로 나누어 떨어질때 별을 표시 
		if ((cnt % 200 - 100) == 0) { gotoxy(x + 4, y + 1); printf("  "); } //위 카운트에서 100카운트 간격으로 별을 지움 
		if ((cnt % 350) == 0) { gotoxy(x + 13, y + 2); printf("☆"); } //윗별과 같지만 시간차를 뒀음 
		if ((cnt % 350 - 100) == 0) { gotoxy(x + 13, y + 2); printf("  "); }
		Sleep(10); // 00.1초 딜레이  
	}

	while (kbhit()) getch(); //버퍼에 기록된 키값을 버림 
} // title 함수


void reset(Game *game) {

	FILE* file = fopen("score.dat", "rt"); // score.dat파일을 연결 
	if (file == 0) { game->best_score = 0; } //파일이 없으면 걍 최고점수에 0을 넣음 
	else {
		fscanf(file, "%d", game->best_score); // 파일이 열리면 최고점수를 불러옴 
		fclose(file); //파일 닫음 
	}

	game->level = 1; //각종변수 초기화 
	game->score = 0;
	game->level_goal = 1000;
	game->key = 0;
	game->crush_on = 0;
	game->cnt = 0;
	game->speed = 100;

	system("cls"); //화면지움 

	
	reset_main(); // main_org를 초기화 
	draw_map(game); // 게임화면을 그림

	draw_main(); // 게임판을 그림 
	/*
	b_type_next = rand() % 7;
	new_block();

	*/
}

void reset_main(void) { //게임판을 초기화  
	int i, j;

	for (i = 0; i < MAIN_Y; i++) { // 게임판을 0으로 초기화  
		for (j = 0; j < MAIN_X; j++) {
			main_org[i][j] = 0;
			main_cpy[i][j] = 100;
		}
	}
	for (j = 1; j < MAIN_X; j++) { //y값이 3인 위치에 천장을 만듦 
		main_org[3][j] = CEILLING;
	}
	for (i = 1; i < MAIN_Y - 1; i++) { //좌우 벽을 만듦  
		main_org[i][0] = WALL;
		main_org[i][MAIN_X - 1] = WALL;
	}
	for (j = 0; j < MAIN_X; j++) { //바닥벽을 만듦 
		main_org[MAIN_Y - 1][j] = WALL;
	}
}

void draw_map(Game *game) { //게임 상태 표시를 나타내는 함수  
	int y = 3;             // level, goal, score만 게임중에 값이 바뀔수 도 있음 그 y값을 따로 저장해둠 
						   // 그래서 혹시 게임 상태 표시 위치가 바뀌어도 그 함수에서 안바꿔도 되게.. 
	gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL = y); printf(" LEVEL : %5d", game->level);
	gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL = y + 1); printf(" GOAL  : %5d", 10 - game->cnt);
	gotoxy(STATUS_X_ADJ, y + 2); printf("+-  N E X T  -+ ");
	gotoxy(STATUS_X_ADJ, y + 3); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 4); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 5); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 6); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 7); printf("+-- -  -  - --+ ");
	gotoxy(STATUS_X_ADJ, y + 8); printf(" YOUR SCORE :");
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE = y + 9); printf("        %6d", game->score);
	gotoxy(STATUS_X_ADJ, y + 10); printf(" LAST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 11); printf("        %6d", game->last_score);
	gotoxy(STATUS_X_ADJ, y + 12); printf(" BEST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 13); printf("        %6d", game->best_score);
	gotoxy(STATUS_X_ADJ, y + 15); printf("  △   : Shift        SPACE : Hard Drop");
	gotoxy(STATUS_X_ADJ, y + 16); printf("◁  ▷ : Left / Right   P   : Pause");
	gotoxy(STATUS_X_ADJ, y + 17); printf("  ▽   : Soft Drop     ESC  : Quit");
	gotoxy(STATUS_X_ADJ, y + 20); printf("blog.naver.com/azure0777");
}

void draw_main(void) {

	// 게임판을 그리는 함수

	int i, j;

	for (j = 1; j < MAIN_X - 1; j++) {

		// 천장은 계속 새로운 블럭이 지나가서 지워지면 새로 그려줌 
		if (main_org[3][j] == EMPTY) main_org[3][j] = CEILLING;
	}

	for (i = 0; i < MAIN_Y; i++) {

		for (j = 0; j < MAIN_X; j++) {

			if (main_cpy[i][j] != main_org[i][j]) {
				// cpy랑 비교해서 값이 달라진 부분만 새로 그려줌
				// 이게 없으면 게임판 전체를 계속 그려서 느려지고 반짝거림

				gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i);

				switch (main_org[i][j]) {

				case EMPTY:				//빈칸모양 
					printf("  ");
					break;
				case CEILLING:			//천장모양 
					printf(". ");
					break;
				case WALL:				//벽모양 
					printf("▩");
					break;
				case INACTIVE_BLOCK:	//굳은 블럭 모양  
					printf("□");
					break;
				case ACTIVE_BLOCK:		//움직이고있는 블럭 모양  
					printf("■");
					break;
				default:
					break;

					main_cpy[i][j] = main_org[i][j];	// main_cpy 갱신
				}
			}
		}
	}
} // draw_main



