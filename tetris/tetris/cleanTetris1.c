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

int blocks[7][4][4][4] = {
	{ { 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },
{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0 } },
{ { 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },{ 0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0 },
{ 0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 },{ 0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0 } },
{ { 0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0 },
{ 0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0 },{ 0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0 } },
{ { 0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0 },{ 0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0 },
{ 0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0 },{ 0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0 } },
{ { 0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0 },
{ 0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0 } },
{ { 0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0 },
{ 0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0 },{ 0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0 } },
{ { 0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0 },{ 0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0 },
{ 0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0 },{ 0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0 } }
}; //블록모양 저장 4*4공간에 블록을 표현 blcoks[b_type][b_rotation][i][j]로 사용 

int main_org[MAIN_Y][MAIN_X]; //게임판의 정보를 저장하는 배열 모니터에 표시후에 main_cpy로 복사됨 
int main_cpy[MAIN_Y][MAIN_X]; /* 즉 maincpy는 게임판이 모니터에 표시되기 전의 정보를 가지고 있음
							  main의 전체를 계속 모니터에 표시하지 않고(이렇게 하면 모니터가 깜빡거림)
							  main_cpy와 배열을 비교해서 값이 달라진 곳만 모니터에 고침 */



typedef struct Game {

	// game 정보

	int key;				//키보드로 입력받은 키값을 저장 
	int speed;				//게임진행속도 
	int level;				//현재 level 
	int level_goal;			//다음레벨로 넘어가기 위한 목표점수 
	int cnt;				//현재 레벨에서 제거한 줄 수를 저장 

	int score;				//현재 점수 
	int last_score;			//마지막게임점수 
	int best_score;			//최고게임점수


}Game;

typedef struct Flag {

	// flag 정보

	int new_block;	//새로운 블럭이 필요함을 알리는 flag 
	int crush;		//현재 이동중인 블록이 충돌 상태인지 알려주는 flag 
	int level_up;	//다음레벨로 진행(현재 레벨목표가 완료되었음을) 알리는 flag 
	int space_key;	//hard drop상태임을 알려주는 flag 
}Flag;

typedef struct Block {

	// block 정보

	int bx;					//이동중인 블록의 게임판상의 x좌표를 저장 
	int by;					//이동중인 블록의 게임판상의 y좌표를 저장 
	int type;				//블록 종류를 저장 
	int rotation;			//블록 회전값 저장 
	int next_type;			//다음 블록값 저장
}Block;



void title(void);
void reset(Game *game, Flag *flag);
void draw_map(Game *game);
void reset_main(void);
void draw_main(void);
void set_block(Block *block, int flag);
void new_block(Block *block, Flag *flag);
void check_key(Game *game, Block *block, Flag *flag);
int check_crush(Block *block, int x, int y, int rotation);
void move_block(int dir, Block *block);
void drop_block(Game *game, Flag *flag, Block *block);
void check_line(Game *game, Flag *flag);
void level_up(Game *game, Flag *flag);
void check_game_over(Game *game, Flag *flag);


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
	Flag flag;
	Block block;

	srand((unsigned)time(NULL));	// 난수표생성
	setcursortype(NOCURSOR);		// 커서 없앰
	title();						// 메인타이틀 호출 
	reset(&game, &flag);					// 게임 정보 리셋
	set_block(&block, 1);			// 생성될 블럭 설정
	new_block(&block, &flag);

	while (1) {
		for (i = 0; i < 5; i++) {	//블록이 한칸떨어지는동안 5번 키입력받을 수 있음 
			check_key(&game, &block, &flag);// 1. 키 입력
			draw_main();					// 2. 화면을 그림
			Sleep(game.speed);				// 게임 속도 조절

			if (flag.crush && check_crush(&block, 0, 1, block.rotation) == false) Sleep(50);
			//블록이 충돌중인 경우 추가로 이동 및 회전할 시간을 갖음

			if (flag.space_key == 1) { //스페이스바를 누른 경우(hard drop) 추가로 이동 및 회전할 수 없음 break; 
				flag.space_key = 0;
				break;
			}
		}

		drop_block(&game, &flag, &block);			// 블록을 한칸 내림 
		check_game_over(&game, &flag);				// 게임오버를 체크

		if (flag.new_block == 1) new_block(&block, &flag); // 뉴 블럭 flag가 있는 경우 새로운 블럭 생성 
	}


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

void reset(Game *game, Flag *flag) {

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
	game->cnt = 0;
	game->speed = 100;

	flag->new_block = 0;
	flag->crush = 0;
	flag->level_up = 0;
	flag->space_key = 0;

	system("cls"); //화면지움 

	reset_main(); // main_org를 초기화 
	draw_map(game); // 게임화면을 그림

	draw_main(); // 게임판을 그림 
} // reset

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
} // reset_main

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
} // draw_map

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

void set_block(Block *block, int flag) {
	
	/*
	block 정보를 설정하는 함수
	flag = 0 : 현재 블럭 설정 + 다음 블럭 갱신
	flag = 1 : 다음 블럭 설정
	*/

	switch (flag) {

	case 0:
		block->type = block->next_type;
		block->rotation = 0;

	case 1:
		block->next_type = rand() % 7;
		break;

	default:
		break;

	}
} // set_block

void new_block(Block *block, Flag *flag) {

	// 새로운 블럭을 생성하는 함수 

	int i, j;

	block->bx = (MAIN_X / 2) - 1;		// 블록 생성 위치x좌표 (게임판의 가운데) 
	block->by = 0;						// 블록 생성위치 y좌표 (제일 위) 
	
	set_block(block, 0);					// 블럭값 설정

	flag -> new_block = 0; //new_block flag를 끔  

	for (i = 0; i < 4; i++) {	// 게임판 bx, by위치에 블럭 생성  
		for (j = 0; j < 4; j++) {
			if (blocks[block->type][block->rotation][i][j] == 1) main_org[block->by + i][block->bx + j] = ACTIVE_BLOCK;
		}
	}

	for (i = 1; i < 3; i++) {	// 게임 상태 표시에 다음에 나올 블럭을 그림

		for (j = 0; j < 4; j++) {
			if (blocks[block->next_type][0][i][j] == 1) {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("■");
			}
			else {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("  ");
			}
		}
	}
} // new_block 함수

void check_key(Game *game, Block *block, Flag *flag) {

	// 키를 눌렀을 때, 블럭이 이동 가능한지 확인

	game->key = 0;		//키 값 초기화  
	int x_move = 0;
	int y_move = 0;
	int rotation = 0;

	if (kbhit()) {			//키 입력이 있는 경우  

		game->key = getch();		//키 값을 받음

		if (game->key == 224) { // 방향키인경우 

			do { game->key = getch(); } while (game->key == 224);// 방향키 지시값을 버림 

			 switch (game->key) {

			 case LEFT: // 왼쪽 키 눌렀을 때 왼쪽으로 갈 수 있는지 체크 후 가능하면 이동
				x_move = -1;
				break;

			 case RIGHT: // 오른쪽 방향키 눌렀을 때 - 위와 동일하게 처리됨 
				x_move = 1;
				break;

			 case DOWN: // 아래쪽 방향키 눌렀을 때 - 위와 동일하게 처리됨 
				y_move = 1;
				break;
				
			 case UP: //위쪽 방향키 눌렀을때 
				 rotation = 1;
				
				// 회전할 수 있는지 체크 후 가능하면 회전
				/*
				else if (flag->crush == 1 && check_crush(block->bx, block->by - 1, (block->rotation + 1) % 4) == true)
					move_block(100, block);
				// 바닥에 닿은 경우 위쪽으로 한 칸띄워서 회전이 가능하면 그렇게 함(특수동작)
				// TODO 특수동작 확인
				*/
			}
			 if (check_crush(block, x_move, y_move, (block->rotation + rotation) % 4) == true) move_block(game->key, block);
		}
		else { // 방향키가 아닌 경우 

			switch (game->key) {

			case SPACE: //스페이스키 눌렀을때 

				flag->space_key = 1; //스페이스키 flag를 띄움 

				while (flag->crush == 0) { // 바닥에 닿을 때까지 이동시킴 

					drop_block(game, flag, block);
				
					game->score += game->level; // hard drop 보너스
					gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", game->score); //점수 표시  
				}
				break;

			case P: // P(대문자) 눌렀을때 
			case p: // p(소문자) 눌렀을때 
				// pause(); //일시정지 
				break;

			case ESC: // ESC눌렀을때 
				system("cls"); // 화면을 지우고 
				exit(0); // 게임 종료 
			}
		}
	}
	while (kbhit()) getch(); // 키 버퍼를 비움 
} // check_key

int check_crush(Block *block, int x, int y, int rotation) {

	// 지정된 좌표와 회전값으로 충돌이 있는지 검사 

	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) { // 지정된 위치의 게임판과 블럭모양을 비교해서 겹치면 false를 리턴

			if (blocks[block->type][rotation][i][j] == 1 && main_org[(block->by) + y + i][(block->bx) + x + j] > 0) return false;
		}
	}

	return true; // 하나도 안겹치면 true리턴 
}; // check_crush

void move_block(int dir, Block *block) {

	// 블록을 dir 방향으로 이동시킨다.

	int i, j;

	for (i = 0; i < 4; i++) {

		// 현재 좌표의 블럭을 지움 
		for (j = 0; j < 4; j++) {
			if (blocks[block->type][block->rotation][i][j] == 1) main_org[block->by + i][block->bx + j] = EMPTY;
		}
	}

	switch (dir) {

	case LEFT:
		block->bx--;
		break;

	case RIGHT:
		block->bx++;
		break;

	case DOWN:
		block->by++;
		break;

	case UP:
		block->rotation = (block->rotation + 1) % 4; //회전값을 1증가시킴(3에서 4가 되는 경우는 0으로 되돌림) 
		break;

	case 100:
		block->rotation = (block->rotation + 1) % 4;
		block->by--;
		break;
	}

	for (i = 0; i < 4; i++) { //왼쪽으로 한칸가서 active block을 찍음 
		for (j = 0; j < 4; j++) {
			if (blocks[block->type][block->rotation][i][j] == 1) main_org[block->by + i][block->bx + j] = ACTIVE_BLOCK;
		}
	}
} // move_block

void drop_block(Game *game, Flag *flag, Block *block) {

	int i, j;

	if (check_crush(block, 0, 1, block->rotation) == true) {
		move_block(DOWN, block);
	}
	else {
		/*

		밑이 비어있지 않다. == 더 이상 내려갈 수 없다.

		crush == 0 : 유예 시간 주기 전
		crush == 1 : 유예 시간 준 후

		현재 블럭을 비활성화 시킨 후, 가득 찬 줄이 있는지 확인한다.
		새로운 블럭 생성flag 를 킨다.

		*/

		if (flag->crush == 1) {
			for (i = 0; i < MAIN_Y; i++) {
				for (j = 0; j < MAIN_X; j++) {
					if (main_org[i][j] == ACTIVE_BLOCK) main_org[i][j] = INACTIVE_BLOCK;
				}
			}

			flag->crush = 0;		// flag를 끔 
			check_line(game, flag);			// 라인 체크를 함
			if (game->cnt >= 10) {
				printf("1");
				level_up(game, flag);
			}
			
			flag->new_block = 1;	// 새로운 블럭생성 flag를 켬    
			return;					//함수 종료 
		}
		flag->crush++;
	}
} // drop block

void check_line(Game *game, Flag *flag) {
	int i, j, k, l;

	int block_amount;		// 한줄의 블록 갯수를 저장하는 변수 
	int combo = 0;			// 콤보갯수 저장하는 변수 지정및 초기화 

	for (i = MAIN_Y - 2; i > 3;) {	// i=MAIN_Y-2 : 밑쪽벽의 윗칸부터,  i>3 : 천장(3)아래까지 검사 

		block_amount = 0; //블록갯수 저장 변수 초기화 

		for (j = 1; j < MAIN_X - 1; j++) { // 벽과 벽사이의 블록갯루를 셈 
			if (main_org[i][j] > 0) block_amount++;
		}

		if (block_amount == MAIN_X - 2) {			// 블록이 가득 찬 경우 

			if (flag->level_up == 0) {				// 레벨업상태가 아닌 경우에 (레벨업이 되면 자동 줄삭제가 있음) 

				game->score += 100 * game->level;	// 점수 추가 
				game->cnt++;						// 지운 줄 갯수 카운트 증가 
				combo++;							// 콤보수 증가
			}

			for (k = i; k > 1; k--) {				// 윗줄을 한칸씩 모두 내림(윗줄이 천장이 아닌 경우에만) 

				for (l = 1; l < MAIN_X - 1; l++) {

					if (main_org[k - 1][l] != CEILLING)
						 main_org[k][l] = main_org[k - 1][l];

					else main_org[k][l] = EMPTY;
					// 윗줄이 천장인 경우에는 천장을 한칸 내리면 안되니까 빈칸을 넣음 
				}
			}
		}
		else i--;
	}


	if (combo) { // 줄 삭제가 있는 경우 점수와 레벨 목표를 새로 표시함  
		if (combo > 1) { // 2콤보 이상인 경우 보너스 및 메세지를 게임판에 띄웠다가 지움 
			gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 1, MAIN_Y+2); printf("%d COMBO!", combo);
			Sleep(500);
			game->score += (combo * game->level * 100);
			gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 1, MAIN_Y + 2); printf("        ");
		}

		gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", (game->cnt <= 10) ? 10 - game->cnt : 0);
		gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", game->score);
	}
} // check_line

void level_up(Game *game, Flag *flag) {
	int i, j;

	
		draw_main();
		flag->level_up = 1;
		game->level += 1; //레벨을 1 올림 
		game->cnt = 0; //지운 줄수 초기화   

		for (i = 0; i < 4; i++) {
			

			gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 3, MAIN_Y_ADJ + 4);
			printf("☆LEVEL UP!☆");
			gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 2, MAIN_Y_ADJ + 6);
			printf("☆SPEED UP!☆");
			Sleep(200);

			gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 3, MAIN_Y_ADJ + 4);
			printf("             ");
			gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 2, MAIN_Y_ADJ + 6);
			printf("             ");

		}


		for (i = MAIN_Y - 2; i > MAIN_Y - 2 - (game->level - 1); i--) { //레벨업보상으로 각 레벨-1의 수만큼 아랫쪽 줄을 지워줌 
			for (j = 1; j < MAIN_X - 1; j++) {
				main_org[i][j] = INACTIVE_BLOCK; // 줄을 블록으로 모두 채우고 
				gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i); // 별을 찍어줌.. 이뻐보이게 
				printf("★");
			}
		}

		Sleep(200);					//별찍은거 보여주기 위해 delay 
		check_line(game, flag);		//블록으로 모두 채운것 지우기
									//check_line()함수 내부에서 level up flag가 켜져있는 경우 점수는 없음.         

		switch (game->level) { //레벨별로 속도를 조절해줌. 
		case 2:
			game->speed = 50;
			break;
		case 3:
			game->speed = 25;
			break;
		case 4:
			game->speed = 10;
			break;
		case 5:
			game->speed = 5;
			break;
		case 6:
			game->speed = 4;
			break;
		case 7:
			game->speed = 3;
			break;
		case 8:
			game->speed = 2;
			break;
		case 9:
			game->speed = 1;
			break;
		case 10:
			game->speed = 0;
			break;
		}
		flag->level_up = 0; //레벨업 flag꺼줌

		gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL); printf(" LEVEL : %5d", game->level);		//레벨표시 
		gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", 10 - game->cnt);	// 레벨목표 표시 

} // check_level_up

void check_game_over(Game *game, Flag *flag) {
	int i;

	int x = 5;
	int y = 5;

	for (i = 1; i < MAIN_X - 2; i++) {
		if (main_org[3][i] > 0) { //천장(위에서 세번째 줄)에 inactive가 생성되면 게임 오버 
			gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤"); //게임오버 메세지 
			gotoxy(x, y + 1); printf("▤                              ▤");
			gotoxy(x, y + 2); printf("▤  +-----------------------+   ▤");
			gotoxy(x, y + 3); printf("▤  |  G A M E  O V E R..   |   ▤");
			gotoxy(x, y + 4); printf("▤  +-----------------------+   ▤");
			gotoxy(x, y + 5); printf("▤   YOUR SCORE: %6d         ▤", game->score);
			gotoxy(x, y + 6); printf("▤                              ▤");
			gotoxy(x, y + 7); printf("▤     Press r to restart..     ▤");
			gotoxy(x, y + 8); printf("▤     Press esc to quit..      ▤");
			gotoxy(x, y + 9); printf("▤                              ▤");
			gotoxy(x, y + 10); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
			game->last_score = game->score; //게임점수를 옮김 

			if (game->score > game->best_score) { //최고기록 갱신시 
				FILE* file = fopen("score.dat", "wt"); //score.dat에 점수 저장                

				gotoxy(x, y + 6); printf("▤  ★★★ BEST SCORE! ★★★   ▤  ");

				if (file == 0) { //파일 에러메세지  
					gotoxy(0, 0);
					printf("FILE ERROR: SYSTEM CANNOT WRITE BEST SCORE ON \"SCORE.DAT\"");
				}
				else {
					fprintf(file, "%d", game->score);
					fclose(file);
				}
			}
			Sleep(1000);

			while (kbhit()) getch();	//키 버퍼를 비움
			game->key = getch();		//키 입력

			switch (game->key) {
			case 114:					//키 = r 이면
				reset(game, flag);		//reset
				break;
			case ESC:					//키 = esc 이면 exit
				system("cls");
				exit(0);
				break;
			}
		}
	}
} // game_over




