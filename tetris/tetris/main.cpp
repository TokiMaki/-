#include "GameClient.h"
#include "socket_err.h"

//int STATUS_Y_GOAL; //GOAL ����ǥ����ġY ��ǥ ����
int STATUS_Y_LEVEL; //LEVEL ����ǥ����ġY ��ǥ ����
//int STATUS_Y_SCORE; //SCORE ����ǥ����ġY ��ǥ ����

int blocks[7][4][4][4] = {
{{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},           // 0 ����� ��
 {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},           // 1 1 ��� �� 
 {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
{{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},           // 2 ������� ��
 {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
{{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},           // 3 �� ������� ��
 {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
{{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},           // 4 ������� ��
 {0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
{{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},           // 5 ����� ��
 {0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
{{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},           // 6 �Ǹ�� ��
 {0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
}; //��ϸ�� ���� 4*4������ ����� ǥ�� blcoks[b_type][b_rotation][i][j]�� ���

struct Flag {
    bool new_block_on = 0; //���ο� ���� �ʿ����� �˸��� flag 
    bool crush_on = 0; //���� �̵����� ����� �浹�������� �˷��ִ� flag 
    bool level_up_on = 0; //���������� ����(���� ������ǥ�� �Ϸ�Ǿ�����) �˸��� flag 
    bool space_key_on = 0; //hard drop�������� �˷��ִ� flag 
};


Gamestatus gamestatus;
Flag flag;
KeyInput keydownbuffer;

int key; //Ű����� �Է¹��� Ű���� ����


int level_goal; //���������� �Ѿ�� ���� ��ǥ����
int cnt; //���� �������� ������ �� ���� ����
int score; //���� ����


void title(void); //���ӽ���ȭ��
void reset(void); //������ �ʱ�ȭ 
void reset_main(void); //���� ������(gamestatus.board_org[][]�� �ʱ�ȭ)
void reset_main_cpy(void); //copy ������(gamestatus.board_cpy[][]�� �ʱ�ȭ)
void draw_map(void); //���� ��ü �������̽��� ǥ�� 
void draw_main(void); //�������� �׸� 
void new_block(void); //���ο� ����� �ϳ� ���� 
void check_key(void); //Ű����� Ű�� �Է¹���
void drop_block(float fTimeElapsed = 100.f); //����� �Ʒ��� ����Ʈ�� 
int check_crush(int bx, int by, int rotation); //bx, by��ġ�� rotationȸ������ ���� ��� �浹 �Ǵ� 
void move_block(int dir); //dir�������� ����� ������ 
void check_line(void); //���� ����á������ �Ǵ��ϰ� ���� 
void check_level_up(void); //������ǥ�� �޼��Ǿ������� �Ǵ��ϰ� levelup��Ŵ 
void check_game_over(void); //���ӿ������� �Ǵ��ϰ� ���ӿ����� ���� 
void pause(void);//������ �Ͻ�������Ŵ


typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; //Ŀ������� �Լ��� ���Ǵ� ������ 
void setcursortype(CURSOR_TYPE c) { //Ŀ������� �Լ� 
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
}

int main() {
    int i;

    srand((unsigned)time(NULL)); //����ǥ���� 
    setcursortype(NOCURSOR); //Ŀ�� ���� 
    title(); //����Ÿ��Ʋ ȣ�� 
    reset(); //������ ����
    __int64 LastTime = ::timeGetTime();
    __int64 CurrentTime;
    float fTimeScale = 0.001f;
    float fTimeElapsed = 0.0f;

    while (1) {
        for (i = 0; i < 5; i++) { //����� ��ĭ�������µ��� 5�� Ű�Է¹��� �� ����
            check_key(); //Ű�Է�Ȯ��
            // if (flag.crush_on && check_crush(bx, by + 1, b_rotation) == false);
            //����� �浹���ΰ�� �߰��� �̵��� ȸ���� �ð��� ����
            if (flag.space_key_on == 1) { //�����̽��ٸ� �������(hard drop) �߰��� �̵��� ȸ���Ҽ� ���� break;
                flag.space_key_on = 0;
                break;
            }
        }
        draw_main(); //ȭ���� �׸�
        while (fTimeElapsed < (1.0f / 60.0f)) {
            CurrentTime = ::timeGetTime();
            fTimeElapsed = (CurrentTime - LastTime) * fTimeScale;
        }
        LastTime = CurrentTime;
        gamestatus.fDropBlockTime += fTimeElapsed;
        fTimeElapsed = 0.0f;
        drop_block(gamestatus.fDropBlockTime); // ����� ��ĭ ����
        check_level_up(); // �������� üũ
        check_game_over(); //���ӿ����� üũ
        if (flag.new_block_on == 1) new_block(); // �� �� flag�� �ִ� ��� ���ο� �� ����
    }
}

void title(void) {
    int x = 5; //Ÿ��Ʋȭ���� ǥ�õǴ� x��ǥ 
    int y = 4; //Ÿ��Ʋȭ���� ǥ�õǴ� y��ǥ 
    int cnt; //Ÿ��Ʋ �������� ���� ����  
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return;

    // socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // connect()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    gotoxy(x, y + 0); printf("����������������"); Sleep(100);
    gotoxy(x, y + 1); printf("�����  ����    ������"); Sleep(100);
    gotoxy(x, y + 2); printf("�����              ���  ��"); Sleep(100);
    gotoxy(x, y + 3); printf("������  ��  ��  ������"); Sleep(100);
    gotoxy(x, y + 4); printf("���  �������������"); Sleep(100);
    gotoxy(x + 5, y + 2); printf("T E T R I S"); Sleep(100);
    gotoxy(x, y + 7); printf("Please Enter Any Key to Start..");
    gotoxy(x, y + 9); printf("  ��   : Shift");
    gotoxy(x, y + 10); printf("��  �� : Left / Right");
    gotoxy(x, y + 11); printf("  ��   : Soft Drop");
    gotoxy(x, y + 12); printf(" SPACE : Hard Drop");
    gotoxy(x, y + 13); printf("   P   : Pause");
    gotoxy(x, y + 14); printf("  ESC  : Quit");
    gotoxy(x, y + 16); printf("BONUS FOR HARD DROPS / COMBOS");

    for (cnt = 0;; cnt++) { //cnt�� 1�� ������Ű�鼭 ��� �ݺ�    //�ϳ��� ���߿��� �� ��¦�̴� �ִϸ��̼�ȿ�� 
        if (kbhit()) break; //Ű�Է��� ������ ���ѷ��� ���� 
        if (cnt % 200 == 0) { gotoxy(x + 4, y + 1); printf("��"); }       //cnt�� 200���� ������ �������� ���� ǥ�� 
        if ((cnt % 200 - 100) == 0) { gotoxy(x + 4, y + 1); printf("  "); } //�� ī��Ʈ���� 100ī��Ʈ �������� ���� ���� 
        if ((cnt % 350) == 0) { gotoxy(x + 13, y + 2); printf("��"); } //������ ������ �ð����� ���� 
        if ((cnt % 350 - 100) == 0) { gotoxy(x + 13, y + 2); printf("  "); }
        Sleep(10); // 00.1�� ������  
    }

    while (kbhit()) getch(); //���ۿ� ��ϵ� Ű���� ���� 

}

void reset(void) {
    gamestatus.level = 1; //�������� �ʱ�ȭ 
    score = 0;
    level_goal = 1000;
    key = 0;
    flag.crush_on = 0;
    cnt = 0;
    gamestatus.speed = 1;

    system("cls"); //ȭ������
    reset_main(); // gamestatus.board_org�� �ʱ�ȭ
    draw_map(); // ����ȭ���� �׸�
    draw_main(); // �������� �׸�

    gamestatus.b_type_next = rand() % 7; //�������� ���� ��� ������ �����ϰ� ���� 
    new_block(); //���ο� ����� �ϳ� ����  
}

void reset_main(void) { //�������� �ʱ�ȭ  
    int i, j;

    for (i = 0; i < BOARD_Y; i++) { // �������� 0���� �ʱ�ȭ  
        for (j = 0; j < BOARD_X; j++) {
            gamestatus.board_org[i][j] = 0;
            gamestatus.board_cpy[i][j] = 100;
        }
    }
    for (j = 1; j < BOARD_X; j++) { //y���� 3�� ��ġ�� õ���� ����
        gamestatus.board_org[3][j] = CEILLING;
    }
    for (i = 1; i < BOARD_Y - 1; i++) { //�¿� ���� ����  
        gamestatus.board_org[i][0] = WALL;
        gamestatus.board_org[i][BOARD_X - 1] = WALL;
    }
    for (j = 0; j < BOARD_X; j++) { //�ٴں��� ���� 
        gamestatus.board_org[BOARD_Y - 1][j] = WALL;
    }
}

void reset_main_cpy(void) { //gamestatus.board_cpy�� �ʱ�ȭ 
    int i, j;

    for (i = 0; i < BOARD_Y; i++) {         //�����ǿ� ���ӿ� ������ �ʴ� ���ڸ� ���� 
        for (j = 0; j < BOARD_X; j++) {  //�̴� gamestatus.board_org�� ���� ���ڰ� ���� �ϱ� ���� 
            gamestatus.board_cpy[i][j] = 100;
        }
    }
}

void draw_map(void) { //���� ���� ǥ�ø� ��Ÿ���� �Լ�  
    int y = 3;           // gamestatus.level, goal, score�� �����߿� ���� �ٲ�� �� ���� �� y���� ���� �����ص� 
                         // �׷��� Ȥ�� ���� ���� ǥ�� ��ġ�� �ٲ� �� �Լ����� �ȹٲ㵵 �ǰ�
    gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL = y); printf(" LEVEL : %5d", gamestatus.level);
    // gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL = y + 1); printf(" GOAL  : %5d", 10 - cnt);
    gotoxy(STATUS_X_ADJ, y + 2); printf("��    NEXT    ��");
    gotoxy(STATUS_X_ADJ, y + 3); printf("��            ��");
    gotoxy(STATUS_X_ADJ, y + 4); printf("��            ��");
    gotoxy(STATUS_X_ADJ, y + 5); printf("��            ��");
    gotoxy(STATUS_X_ADJ, y + 6); printf("��            ��");
    gotoxy(STATUS_X_ADJ, y + 7); printf("����������������������������");
    /*
    gotoxy(STATUS_X_ADJ, y + 8); printf(" YOUR SCORE :");
    gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE = y + 9); printf("        %6d", score);
    gotoxy(STATUS_X_ADJ, y + 10); printf(" LAST SCORE :");
    gotoxy(STATUS_X_ADJ, y + 11); printf("        %6d", last_score);
    gotoxy(STATUS_X_ADJ, y + 12); printf(" BEST SCORE :");
    gotoxy(STATUS_X_ADJ, y + 13); printf("        %6d", best_score);
    gotoxy(STATUS_X_ADJ, y + 15); printf("  ��   : Shift        SPACE : Hard Drop");
    gotoxy(STATUS_X_ADJ, y + 16); printf("��  �� : Left / Right   P   : Pause");
    gotoxy(STATUS_X_ADJ, y + 17); printf("  ��   : Soft Drop     ESC  : Quit");
    */
}

void draw_main(void) { //������ �׸��� �Լ� 
    int i, j;

    for (j = 1; j < BOARD_X - 1; j++) { //õ���� ��� ���ο���� �������� �������� ���� �׷��� 
        if (gamestatus.board_org[3][j] == EMPTY) gamestatus.board_org[3][j] = CEILLING;
    }
    for (i = 0; i < BOARD_Y; i++) {
        for (j = 0; j < BOARD_X; j++) {
            if (gamestatus.board_cpy[i][j] != gamestatus.board_org[i][j]) { //cpy�� ���ؼ� ���� �޶��� �κи� ���� �׷���.
                                                //�̰� ������ ��������ü�� ��� �׷��� �������� ��¦�Ÿ�
                gotoxy(BOARD_X_ADJ + j, BOARD_Y_ADJ + i);
                switch (gamestatus.board_org[i][j]) {
                case EMPTY: //��ĭ��� 
                    printf("  ");
                    break;
                case CEILLING: //õ���� 
                    printf(". ");
                    break;
                case WALL: //����� 
                    printf("��");
                    break;
                case INACTIVE_BLOCK: //���� �� ���  
                    printf("��");
                    break;
                case ACTIVE_BLOCK: //�����̰��ִ� �� ���
                    printf("��");
                    break;
                }
            }
        }
    }
    for (i = 0; i < BOARD_Y; i++) { //�������� �׸� �� gamestatus.board_cpy�� ����  
        for (j = 0; j < BOARD_X; j++) {
            gamestatus.board_cpy[i][j] = gamestatus.board_org[i][j];
        }
    }
}

void new_block(void) { //���ο� ��� ����  
    int i, j;

    gamestatus.bx = (BOARD_X / 2) - 1; //��� ���� ��ġx��ǥ(�������� ���) 
    gamestatus.by = 0;  //��� ������ġ y��ǥ(���� ��) 
    gamestatus.b_type = gamestatus.b_type_next; //���������� ������ 
    gamestatus.b_type_next = rand() % 7; //���� ���� ���� 
    gamestatus.b_rotation = 0;  //ȸ���� 0������ ������ 

    flag.new_block_on = 0; //new_block flag�� ��  

    for (i = 0; i < 4; i++) { //������ bx, by��ġ�� ������  
        for (j = 0; j < 4; j++) {
            if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j] = ACTIVE_BLOCK;
        }
    }
    for (i = 1; i < 3; i++) { //���ӻ���ǥ�ÿ� ������ ���ú��� �׸� 
        for (j = 0; j < 4; j++) {
            if (blocks[gamestatus.b_type_next][0][i][j] == 1) {
                gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
                printf("��");
            }
            else {
                gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
                printf("  ");
            }
        }
    }
}

void check_key(void) {
    key = 0; //Ű�� �ʱ�ȭ  

    if (kbhit()) { //Ű�Է��� �ִ� ���  
        key = getch(); //Ű���� ����
        if (key == 224) { //����Ű�ΰ�� 
            do { key = getch(); } while (key == 224);//����Ű���ð��� ���� 
            switch (key) {
            case LEFT: //����Ű ��������
                if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
                    if (check_crush(gamestatus.bx - 1, gamestatus.by, gamestatus.b_rotation) == true) move_block(LEFT);
                }
                break;                            //�������� �� �� �ִ��� üũ �� �����ϸ� �̵�
            case RIGHT: //������ ����Ű ��������- ���� �����ϰ� ó���� 
                if (check_crush(gamestatus.bx + 1, gamestatus.by, gamestatus.b_rotation) == true) move_block(RIGHT);
                break;
            case DOWN: //�Ʒ��� ����Ű ��������-���� �����ϰ� ó���� 
                if (check_crush(gamestatus.bx, gamestatus.by + 1, gamestatus.b_rotation) == true) move_block(DOWN);
                break;
            case UP: //���� ����Ű ��������
                if (check_crush(gamestatus.bx, gamestatus.by, (gamestatus.b_rotation + 1) % 4) == true) move_block(UP);
                //ȸ���� �� �ִ��� üũ �� �����ϸ� ȸ��
                else if (flag.crush_on == 1 && check_crush(gamestatus.bx, gamestatus.by - 1, (gamestatus.b_rotation + 1) % 4) == true) move_block(100);
            }   //�ٴڿ� ���� ��� �������� ��ĭ����� ȸ���� �����ϸ� �׷��� ��(Ư������)
        }
        else { //����Ű�� �ƴѰ�� 
            switch (key) {
            case SPACE: //�����̽�Ű �������� 
                flag.space_key_on = 1; //�����̽�Ű flag�� ��� 
                while (flag.crush_on == 0) { //�ٴڿ� ���������� �̵���Ŵ
                    drop_block();
                    score += gamestatus.level; // hard drop ���ʽ�
                    // gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", score); //���� ǥ��  
                }
                break;
            case P: //P(�빮��) �������� 
            case p: //p(�ҹ���) �������� 
                pause(); //�Ͻ����� 
                break;
            case ESC: //ESC�������� 
                system("cls"); //ȭ���� ����� 
                exit(0); //�������� 
            }
        }
    }
    while (kbhit()) getch(); //Ű���۸� ��� 
}

void drop_block(float fTimeElapsed) {
    int i, j;
    if (flag.crush_on && check_crush(gamestatus.bx, gamestatus.by + 1, gamestatus.b_rotation) == false) { //���� ��������ʰ� crush flag�� ����������
        for (i = 0; i < BOARD_Y; i++) { //���� �������� ���� ���� 
            for (j = 0; j < BOARD_X; j++) {
                if (gamestatus.board_org[i][j] == ACTIVE_BLOCK) gamestatus.board_org[i][j] = INACTIVE_BLOCK;
            }
        }
        flag.crush_on = 0; //flag�� �� 
        check_line(); //����üũ�� �� 
        flag.new_block_on = 1; //���ο� ������ flag�� ��
        return; //�Լ� ���� 
    }
    if (fTimeElapsed >= gamestatus.speed) {
        if (flag.crush_on && check_crush(gamestatus.bx, gamestatus.by + 1, gamestatus.b_rotation) == true) flag.crush_on = 0; //���� ��������� crush flag ��
        if (check_crush(gamestatus.bx, gamestatus.by + 1, gamestatus.b_rotation) == true) move_block(DOWN); //���� ��������� ������ ��ĭ �̵� 
        if (check_crush(gamestatus.bx, gamestatus.by + 1, gamestatus.b_rotation) == false) flag.crush_on++; //������ �̵��� �ȵǸ�  crush flag�� ��
        gamestatus.fDropBlockTime = 0.0f;
    }
    return;
}

int check_crush(int bx, int by, int b_rotation) { //������ ��ǥ�� ȸ�������� �浹�� �ִ��� �˻� 
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) { //������ ��ġ�� �����ǰ� ������� ���ؼ� ��ġ�� false�� ���� 
            if (blocks[gamestatus.b_type][b_rotation][i][j] == 1 && gamestatus.board_org[by + i][bx + j] > 0) return false;
        }
    }
    return true; //�ϳ��� �Ȱ�ġ�� true���� 
};

void move_block(int dir) { //����� �̵���Ŵ 
    int i, j;

    switch (dir) {
    case LEFT: //���ʹ��� 
        for (i = 0; i < 4; i++) { //������ǥ�� ���� ���� 
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) { //�������� ��ĭ���� active block�� ���� 
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j - 1] = ACTIVE_BLOCK;
            }
        }
        gamestatus.bx--; //��ǥ�� �̵� 
        break;

    case RIGHT:    //������ ����. ���ʹ����̶� ���� ������ ���� 
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j + 1] = ACTIVE_BLOCK;
            }
        }
        gamestatus.bx++;
        break;

    case DOWN:    //�Ʒ��� ����. ���ʹ����̶� ���� ������ ����
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i + 1][gamestatus.bx + j] = ACTIVE_BLOCK;
            }
        }
        gamestatus.by++;
        break;

    case UP: //Ű���� ���� �������� ȸ����Ŵ. 
        for (i = 0; i < 4; i++) { //������ǥ�� ���� ����  
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j] = EMPTY;
            }
        }
        gamestatus.b_rotation = (gamestatus.b_rotation + 1) % 4; //ȸ������ 1������Ŵ(3���� 4�� �Ǵ� ���� 0���� �ǵ���) 
        for (i = 0; i < 4; i++) { //ȸ���� ����� ���� 
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j] = ACTIVE_BLOCK;
            }
        }
        break;

    case 100: //����� �ٴ�, Ȥ�� �ٸ� ��ϰ� ���� ���¿��� ��ĭ���� �÷� ȸ���� ������ ��� 
              //�̸� ���۽�Ű�� Ư������ 
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i][gamestatus.bx + j] = EMPTY;
            }
        }
        gamestatus.b_rotation = (gamestatus.b_rotation + 1) % 4;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[gamestatus.b_type][gamestatus.b_rotation][i][j] == 1) gamestatus.board_org[gamestatus.by + i - 1][gamestatus.bx + j] = ACTIVE_BLOCK;
            }
        }
        gamestatus.by--;
        break;
    }
}

void check_line(void) {
    int i, j, k, l;

    int    block_amount; //������ ��ϰ����� �����ϴ� ���� 
    int combo = 0; //�޺����� �����ϴ� ���� ������ �ʱ�ȭ 

    for (i = BOARD_Y - 2; i > 3;) { //i=MAIN_Y-2 : ���ʺ��� ��ĭ����,  i>3 : õ��(3)�Ʒ����� �˻� 
        block_amount = 0; //��ϰ��� ���� ���� �ʱ�ȭ 
        for (j = 1; j < BOARD_X - 1; j++) { //���� �������� ��ϰ��縦 �� 
            if (gamestatus.board_org[i][j] > 0) block_amount++;
        }
        if (block_amount == BOARD_X - 2) { //����� ���� �� ��� 
            if (flag.level_up_on == 0) { //���������°� �ƴ� ��쿡(�������� �Ǹ� �ڵ� �ٻ����� ����) 
                score += 100 * gamestatus.level; //�����߰� 
                cnt++; //���� �� ���� ī��Ʈ ���� 
                combo++; //�޺��� ����  
            }
            for (k = i; k > 1; k--) { //������ ��ĭ�� ��� ����(������ õ���� �ƴ� ��쿡��) 
                for (l = 1; l < BOARD_X - 1; l++) {
                    if (gamestatus.board_org[k - 1][l] != CEILLING) gamestatus.board_org[k][l] = gamestatus.board_org[k - 1][l];
                    if (gamestatus.board_org[k - 1][l] == CEILLING) gamestatus.board_org[k][l] = EMPTY;
                    //������ õ���� ��쿡�� õ���� ��ĭ ������ �ȵǴϱ� ��ĭ�� ���� 
                }
            }
        }
        else i--;
    }
    if (combo) { //�� ������ �ִ� ��� ������ ���� ��ǥ�� ���� ǥ����  
        if (combo > 1) { //2�޺��̻��� ��� ��� ���ʽ��� �޼����� �����ǿ� ����ٰ� ���� 
            gotoxy(BOARD_X_ADJ + (BOARD_X / 2) - 1, BOARD_Y_ADJ + gamestatus.by - 2); printf("%d COMBO!", combo);
            // Sleep(500);
            // score += (combo * gamestatus.level * 100);
            reset_main_cpy(); //�ؽ�Ʈ�� ����� ���� gamestatus.board_cpy�� �ʱ�ȭ.
        //(gamestatus.board_cpy�� gamestatus.board_org�� ���� �ٸ��Ƿ� ������ draw()ȣ��� ������ ��ü�� ���� �׸��� ��) 
        }
        // gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", (cnt <= 10) ? 10 - cnt : 0);
        // gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", score);
    }
}

void check_level_up(void) {
    int i, j;

    if (cnt >= 10) { //�������� 10�پ� ���־���. 10���̻� ���� ��� 
        draw_main();
        flag.level_up_on = 1; //������ flag�� ���
        gamestatus.level += 1; //������ 1 �ø�
        cnt = 0; //���� �ټ� �ʱ�ȭ  
        /*
        for (i = 0; i < 4; i++) {
            gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 3, MAIN_Y_ADJ + 4);
            printf("             ");
            gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 2, MAIN_Y_ADJ + 6);
            printf("             ");
            Sleep(200);

            gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 3, MAIN_Y_ADJ + 4);
            printf("��LEVEL UP!��");
            gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 2, MAIN_Y_ADJ + 6);
            printf("��SPEED UP!��");
            Sleep(200);
        }
        */
        reset_main_cpy(); //�ؽ�Ʈ�� ����� ���� gamestatus.board_cpy�� �ʱ�ȭ.
        //(gamestatus.board_cpy�� gamestatus.board_org�� ���� �ٸ��Ƿ� ������ draw()ȣ��� ������ ��ü�� ���� �׸��� ��) 

        /*
        for (i = MAIN_Y - 2; i > MAIN_Y - 2 - (gamestatus.level - 1); i--) { //�������������� �� ����-1�� ����ŭ �Ʒ��� ���� ������ 
            for (j = 1; j < MAIN_X - 1; j++) {
                gamestatus.board_org[i][j] = INACTIVE_BLOCK; // ���� ������� ��� ä��� 
                gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i); // ���� �����.. �̻����̰� 
                printf("��");
                Sleep(20);
            }
        }
        Sleep(100); //�������� �����ֱ� ���� delay
        check_line(); //������� ��� ä��� �����
        */

//.check_line()�Լ� ���ο��� gamestatus.level up flag�� �����ִ� ��� ������ ����.         
        switch (gamestatus.level) { //�������� �ӵ��� ��������. 
        case 2:
            gamestatus.speed = 0.9;
            break;
        case 3:
            gamestatus.speed = 0.8;
            break;
        case 4:
            gamestatus.speed = 0.7;
            break;
        case 5:
            gamestatus.speed = 0.6;
            break;
        case 6:
            gamestatus.speed = 0.5;
            break;
        case 7:
            gamestatus.speed = 0.4;
            break;
        case 8:
            gamestatus.speed = 0.3;
            break;
        case 9:
            gamestatus.speed = 0.2;
            break;
        case 10:
            gamestatus.speed = 0.1;
            break;
        }
        flag.level_up_on = 0; //������ flag����

        gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL); printf(" LEVEL : %5d", gamestatus.level); //����ǥ�� 
        // gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", 10 - cnt); // ������ǥ ǥ�� 

    }
}

void check_game_over(void) {
    int i;

    int x = 5;
    int y = 5;

    for (i = 1; i < BOARD_X - 2; i++) {
        if (gamestatus.board_org[3][i] > 0) { //õ��(������ ����° ��)�� inactive�� �����Ǹ� ���� ���� 
            gotoxy(x, y + 0); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�"); //���ӿ��� �޼��� 
            gotoxy(x, y + 1); printf("��                              ��");
            gotoxy(x, y + 2); printf("��  +-----------------------+   ��");
            gotoxy(x, y + 3); printf("��  |  G A M E  O V E R..   |   ��");
            gotoxy(x, y + 4); printf("��  +-----------------------+   ��");
            gotoxy(x, y + 5); printf("��   YOUR SCORE: %6d         ��", score);
            gotoxy(x, y + 6); printf("��                              ��");
            gotoxy(x, y + 7); printf("��  Press any key to restart..  ��");
            gotoxy(x, y + 8); printf("��                              ��");
            gotoxy(x, y + 9); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");

            Sleep(1000);
            while (kbhit()) getch();
            key = getch();
            reset();
        }
    }
}

void pause(void) { //���� �Ͻ����� �Լ� 
    int i, j;

    int x = 5;
    int y = 5;

    for (i = 1; i < BOARD_X - 2; i++) { //���� �Ͻ����� �޼��� 
        gotoxy(x, y + 0); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");
        gotoxy(x, y + 1); printf("��                              ��");
        gotoxy(x, y + 2); printf("��  +-----------------------+   ��");
        gotoxy(x, y + 3); printf("��  |       P A U S E       |   ��");
        gotoxy(x, y + 4); printf("��  +-----------------------+   ��");
        gotoxy(x, y + 5); printf("��  Press any key to resume..   ��");
        gotoxy(x, y + 6); printf("��                              ��");
        gotoxy(x, y + 7); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");
    }
    getch(); //Ű�Է½ñ��� ��� 

    system("cls"); //ȭ�� ����� ���� �׸� 
    reset_main_cpy();
    draw_main();
    draw_map();

    for (i = 1; i < 3; i++) { // ������� �׸� 
        for (j = 0; j < 4; j++) {
            if (blocks[gamestatus.b_type_next][0][i][j] == 1) {
                gotoxy(BOARD_X + BOARD_X_ADJ + 3 + j, i + 6);
                printf("��");
            }
            else {
                gotoxy(BOARD_X + BOARD_X_ADJ + 3 + j, i + 6);
                printf("  ");
            }
        }
    }
} //��! 