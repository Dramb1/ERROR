#define N_BUTTONS   5 // ���������� ������
#define NAME_SIZE  32 // ����� �������� ������
#define KEY_EXIT   27
#define MAXN 6 // ������������ ���������� �������
#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600
#define _USE_MATH_DEFINES

#include "graphics.h"
#include "Windows.h"
#include <math.h>
#include <stdlib.h>
int first, last;  // ������ � ��������� ����� � ��������
int N_players;// ���������� �������
int N_rounds;// ���������� ����� �������, ������� ���������� ���������� �������, 
                 //����� ���������� ������
int N_chips;// ������� ���������� ����� � ����
int small, big;// ����� ���������, ������� ������ ������
int LengthOfTrace;// ����� ������
double size; // ������ ����� ������

struct Button
{
   int left; // ��������� �
   int top; // ���������� �
   int width;
   int height;
   IMAGE *image;
};
Button buttons[N_BUTTONS];
IMAGE *image_menu, *image_about;

struct Team
{
   char name[10]; // ���
   int r, g, b; //����
   IMAGE *image; // �������� ���������������� ���������
   int  BonusTurns; // ������� � �������� ���������� ����� �������
   int score; // ���������� ����� �������
}; 
Team list_teams[MAXN];
struct Chip
{
   int Num; // ����� �������
   int i, j; // ������� ���������
   int DoneTurn; // ���������� ��������� �����
   int path; // ���������� ���������� �����
   int CurrTurnMin, CurrTurnMax;// ������� ������������ � ����������� ���������� �����
   int DoneRound; // ���������� ���������� ������
   int place; //����� � �����, ������� ������ 
   int flag; // ���� -- ������� ����� ��� ���
   int BonusFlag; // �������� �� ����� ������ ��� ���
};
typedef struct Trace
{
   int x0, x1, y0, y1; // ���������� ��� �����������
   int state;//0 -  ������ ����������/ -1 - �� ���������/1 - ����������
   int cost; // ���� ���� �� ������ ������
   double xd, yd; // ���������� ��� ������ �����-������
}Trace;

// ������� ������, ����������� ��� ��������� ������������ ��������
int block3[3][4] = { 
   {1,  0,  0,   0},
   {1, -1,  0, -1},
   {1,  0,  0,  -1},
};
int block0[3][4] = { 
   {1,  0,  0, -1},
   {1, -1,  0, -1},
   {1,  0,  0,  0},
};
int sdwig[4][13]={
   { 1,  0,   -1,   0, 1,  -1,  -1,   1,  -5,  2,  1,   0,  5},
   { 0,  1,   0,   -1,  0,    0,  5,    -1,  -1,  1,   -5,  2,  1},
   {-4, -1,  4,   1, -4,   0,   0,    0,  0,   0,  0,   0,  0},
   { 1, -4, -1,  4,  1,    0,  0,    0,  0,   0,   0,  0,  0}
};
int kx[4]{1, -1, -1, 1};
int ky[4]{-1, -1, 1, 1};

int flag[6] = {0};// ������ ������ - ���� �� ���-���� �� ������� ����� �� ������ ����� ��� ���
int score[12] ={15, 12, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};// ����� �� �����: 1-�� ����� - 15, ... , 12-�� - 1

void loadbutton();
void start();
void menu();
int select(int, int);
void about();
void close();
void choosemenu();
void teams();
void game(Chip*);
void SetSquareTile(int, int, int, int, Trace**);
void SetRotationTile(int, int, int, int, Trace**, int);
Trace** CreateTrace();
void DrawTrace(int, int, Trace **);
void DestroyTrace(Trace**);
void CoordinateDraw(int, int, int, int, int, int, int, Trace**, double, double);
void Sort(Chip*);
void ColourChip(int, int, int, int, Trace**);
void Spawn(Trace** trace, Chip*);
void ChooseTile(Chip*, int, int, Trace**, int, int, int, int);
void Compare(int, int, Trace**, int, int, int, Chip*, int, int);
int MouseClick(Trace**, Chip*, int);
void CleanTrace(Trace** trace);
void ColourTrace(Trace**, Chip*, int);
void MessageTeam(int, Chip*);
void MessageBonus(int, Chip*);
void MessageFinish(int, Chip*);
void MessageKick(int, Chip*);
void MessageRound(int, Chip*);
void MessageInf(int, Chip*);
void Ranking(Chip*);
void Activate(int i, int j, Trace** trace);
void IntToChar(int, char*);
void Check(Chip*,Trace**);
void FillGap(Trace**, int, int, int);
void Randomize();
void CheckTail(Chip*, Trace**);
void MessageTail(int, Chip*);
void Result(Chip*);
void CountScore(Chip* list_chips);
void PrepareForResult();
void QuantityPlayers();
void AboutTeams();
int main()
{
   SetConsoleCP(1251);
   SetConsoleOutputCP(1251);
   initwindow(WIDTH_WINDOW, HEIGHT_WINDOW);
   loadbutton();
   start();
   menu();
   closegraph();
   return 0;
}

void loadbutton() // �������� ����������� � ������ � �������
{
   FILE *input;
   char name[NAME_SIZE];
   
   // �������� ������ � �������
   input = fopen("buttons.txt", "r");
   for (int i = 0; i < N_BUTTONS; i++)
   {
      fscanf(input, "%d", &buttons[i].left);
      fscanf(input, "%d", &buttons[i].top);
      fscanf(input, "%s", name);
      buttons[i].image  = loadBMP(name);
      buttons[i].width  = imagewidth(buttons[i].image);
      buttons[i].height = imageheight(buttons[i].image);
   }
   fclose(input);
   
   // �������� ����������� ���� � ������
   image_menu  = loadBMP("window_menu.bmp");
   image_about = loadBMP("window_about.bmp");
}

void start() //���������� ����
{
   IMAGE *image;
   image = loadBMP("window_start.bmp");
   putimage(0, 0, image, COPY_PUT);
   freeimage(image);
   getch();
}

void menu() //������� ����
{
   while (true)
   {
      // ������� �������� ������ �������� ����
      putimage(0, 0, image_menu, COPY_PUT);
      for (int i = 0; i < N_BUTTONS-2; i++)
      {
         putimage(buttons[i].left, buttons[i].top,
                  buttons[i].image, COPY_PUT);
      }
      int i = -1;
      while (i < 0)
      {
         while (mousebuttons() != 1);
         i = select(mousex(), mousey());
      }
      switch (i)
      {
         case 0: about();  break;
         case 1: choosemenu(); break;
         case 2: close(); return;
      }
   }
}

int select(int x, int y) //����������� ������� ������
{
   for (int i = 0; i < N_BUTTONS; i++)
   {
      if (x > buttons[i].left &&
          x < buttons[i].left + buttons[i].width &&
          y > buttons[i].top &&
          y < buttons[i].top + buttons[i].height)
      return i;
   }
   return -1;
} 
void about() //���� � ���������
{
   while(true)
   {
      putimage(0, 0, image_about, COPY_PUT);
      putimage(buttons[3].left, buttons[3].top, buttons[3].image, COPY_PUT);
     int  i = -1;
      while (i < 0)
      {
         while (mousebuttons() != 1);
         i = select(mousex(), mousey());
      }
      if (i == 3) return;
   }
}

void close() //������������ ������ �� �����������
{
   for (int i = 0; i < N_BUTTONS; ++i)
      freeimage(buttons[i].image);
   freeimage(image_menu);
   freeimage(image_about);
   for (int i = 0; i < N_players; ++i)
      freeimage(list_teams[i].image);
}

void choosemenu(){// ���� ������
   int i, j;
   int x1[MAXN], x2[MAXN], y1[MAXN], y2[MAXN];
   QuantityPlayers();
   
   //�������� ������
   FILE * input = fopen("choosemenu_buttons.txt", "r");
   char c[2];
   for(j = 0; j < MAXN; j++){
      fscanf(input, "%d%d%d%d%*c", &x1[j], &y1[j], &x2[j], &y2[j]);
      if (j != MAXN-1){
         bar(x1[j], y1[j], x2[j], y2[j]);
         IntToChar(j+2, c);
         outtextxy((x1[j]+x2[j])/2 - 10, (y1[j]+y2[j])/2 - 10 , c);
      }
   }
   fclose(input);
      
   // ����������, ����� ������ ������
   N_players = -1;
   while (N_players < 0){
      while (mousebuttons() != 1);
      int x = mousex(), y = mousey();
      for (int j = 0; j < MAXN-1; j++)
         if (x >= x1[j] && x <= x2[j])
            if (y >= y1[j] && y <= y2[j]) N_players = j+2;
   }
   N_chips = N_players*2;
   Chip * list_chips = (Chip*)malloc((N_players*2) * sizeof(Chip));
   
   AboutTeams();
   for (j = 0; j < N_players; ++j){
      putimage(x1[j], y1[j], list_teams[j].image, COPY_PUT);
      outtextxy(x1[j]+100, y1[j], "�������");
      outtextxy(x1[j]+100, y1[j]+15, list_teams[j].name);
      outtextxy(x1[j]+100, y1[j]+30, "����:");
      setfillstyle(SOLID_FILL, COLOR(list_teams[j].r, list_teams[j].g, list_teams[j].b));
      bar(x1[j]+100, y1[j]+45, x2[j], y2[j]);
   }
   // ������ �����
   putimage(buttons[4].left, buttons[4].top, buttons[4].image, COPY_PUT);
   while (true){
      while (mousebuttons() != 1);
      int x = mousex(), y = mousey();
      if (x > buttons[4].left && x < buttons[4].left + buttons[4].width
         && y > buttons[4].top && y < buttons[4].top + buttons[4].height)
      break;
   }
   game(list_chips);
}
void QuantityPlayers()//��������� � ������ ���-�� �������
{
   cleardevice();
   setfillstyle(SOLID_FILL, COLOR(243, 244, 252));
   bar(0, 0, getmaxx(), getmaxy());
         
   //��������� ������ � �����
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   setbkcolor(COLOR(65, 105, 225));
   setfillstyle(SOLID_FILL, COLOR(65, 105, 225));
   outtextxy(50, 50, "�������� ���������� �������");
      
}
void AboutTeams()//���������� � ���������
{
   setbkcolor(COLOR(243, 244, 252));
   cleardevice();
   
   // ��������� ������ � �������� �� �������� �����
   char picture[NAME_SIZE];
   FILE * input1 = fopen("teams.txt", "r");
   for (int i = 0; i < N_players+2 ;++ i)
   {
      fscanf(input1, "%s", list_teams[i].name);
      fscanf(input1, "%d%d%d", &list_teams[i].r, &list_teams[i].g, &list_teams[i].b);
      fscanf(input1, "%s", picture);
      list_teams[i].image  = loadBMP(picture);
      list_teams[i%N_players].score = 0;
   }
   fclose(input1);
   
   //������� ���������� ������ � ��������
   setbkcolor(COLOR(65, 105, 225));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   outtextxy(0, 50, "������, ������������ � ���������");
   outtextxy(50, 100, "����� �������� ����� 3 2 1..");
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
}
void game(Chip * list_chips)
{
   //�������� ������� ����, ������� �������� ��������������� �������
   setbkcolor(COLOR(195, 176, 145));
   cleardevice();
   Trace** trace = CreateTrace();
   DrawTrace(280, 300, trace);
   Randomize();
   Spawn(trace, list_chips);
   while(N_chips >= 2)
   {
      for (int I = 0; I < N_chips; ++I)
      {
         delay(500);
         Ranking(list_chips);
         list_chips[I].flag = 0;
         
         //����������� ���-�� ����� �����
         if(I == 0)
         {
            list_chips[0].CurrTurnMin = 1;
            list_chips[0].CurrTurnMax = 4;
         }
         else if (I == 1)
         {
            list_chips[1].CurrTurnMin = list_chips[0].DoneTurn - 1;
            list_chips[1].CurrTurnMax = list_chips[0].DoneTurn + 2;
         }
         else 
         {
            list_chips[I].CurrTurnMin = list_chips[I-1].DoneTurn - 1;
            list_chips[I].CurrTurnMax = list_chips[I-1].DoneTurn + 1;
         }
         ChooseTile(list_chips, list_chips[I].i,  list_chips[I].j, trace, I, 0,  list_teams[list_chips[I].Num].BonusTurns, 0);
         ColourTrace(trace, list_chips, I);
         Activate(list_chips[I].i,  list_chips[I].j, trace);
         MessageTeam(I, list_chips); 
         if (list_chips[I].flag == 1)// ���� ����� ���� ���� ������, ���� ����� �����
            while(MouseClick(trace, list_chips, I));       
      }
      Check(list_chips, trace);
      Sort(list_chips);
      CheckTail(list_chips, trace);
   }
   DestroyTrace(trace);
   Result(list_chips);
   return;
}
void SetSquareTile(int x, int y, int i, int j, Trace** trace)//���������� ������
{
   // ���� ��� ��� ������ ��� ������ ������������ ����� � ������ ����� �������
   if (i == LengthOfTrace-big/2 || i == LengthOfTrace-1)
   {
      if (j % 2 == 0) setfillstyle(SOLID_FILL, WHITE);
      else setfillstyle(SOLID_FILL, BLACK);
   }
   else setfillstyle(SOLID_FILL, COLOR(128,128,128));
   
   bar(x, y, x+size, y+size);
   setcolor(BLACK);
   setlinestyle(SOLID_LINE, 0, 2);
   rectangle(x, y, x+size, y+size);
   
   //���������� ���������� ������
   trace[i][j].xd = x+0.5*size; 
   trace[i][j].yd = y+0.5*size;
   trace[i][j].x0 = x;
   trace[i][j].x1 = x+size; 
   trace[i][j].y0 = y; 
   trace[i][j].y1 = y+size; 
}
void SetRotationTile(int N, int x, int y, Trace** trace, int i)//������ ��������
{
   //��������� ��������� �������� � ���������� ����������. ����� ������� ������������
   int d = N*90; // ���� ��������
   int i0, i1;
   setcolor(BLACK); 
   setlinestyle(SOLID_LINE, 0, 2); 
   switch(N)
   {
      case 0:
         CoordinateDraw(x+3*size, y, 0+d, 90+d, 2*size, i+2, 3, trace, x+4.5*size, y-0.5*size);
         break;
      case 1:
         CoordinateDraw(x-3*size, y, 0+d, 90+d, 2*size, i, 3, trace, x-4.5*size, y-0.5*size);
         break;
      case 2:
         CoordinateDraw(x-3*size, y, 0+d, 90+d, 2*size, i+2, 3, trace, x-4.5*size, y+0.5*size);
         break;
      case 3:
         CoordinateDraw(x+3*size, y, 0+d, 90+d, 2*size, i, 3, trace, x+4.5*size, y+0.5*size);
         break;
   }
   if ( N%2 == 0) 
   {
      i0 = i+2;
      i1 = i;
   }
   else 
   {
      i0 = i; 
      i1 = i+2;
   }
   CoordinateDraw(x, y, 0+d, 30+d, 4*size, i0, 2, trace, x+3.5*kx[N]*size, y+0.5*ky[N]*size);
   CoordinateDraw(x, y, 30+d, 60+d, 4*size, i+1, 2, trace, x+2.5*kx[N]*size, y+2.5*ky[N]*size );
   CoordinateDraw(x, y, 60+d, 90+d, 4*size, i1, 2, trace, x+0.5*kx[N]*size, y+3.5*ky[N]*size);
   CoordinateDraw(x, y, 0+d, 45+d, 3*size, i0, 1, trace, x+2.5*kx[N]*size, y+0.5*ky[N]*size);
   CoordinateDraw(x, y, 45+d, 90+d, 3*size, i1, 1, trace, x+0.5*kx[N]*size, y+2.5*ky[N]*size );
   for (int I = i; I < i+3; ++I) 
      CoordinateDraw(x, y, 0+d, 90+d, 2*size, I, 0, trace, x+1.5*kx[N]*size, y+0.5*ky[N]*size);
   
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145));
   pieslice(x, y, 0+d, 90+d, size);
   setcolor(COLOR(195, 176, 145));
   pieslice(x, y, 90+d, 360+d, size-2);
}
Trace** CreateTrace()//�������� ������������� ���������� ������� ��������
{
   int i, j, k;
   if (N_players == 2) N_rounds = 1;
      else N_rounds = 5;
   size = 26-N_players;
   small = 1+N_players;
   big = (2*N_players-1)*2;
   LengthOfTrace = 2*(big+small) + 4*3;
   Trace ** trace = (Trace**)malloc(LengthOfTrace * sizeof(Trace*));
   
   //������������� ���� ����� ���������
   for( i = 0; i < LengthOfTrace; ++i)
   {
      trace[i] = (Trace*)malloc(4 * sizeof(Trace));
      for (j = 0; j < 4; ++j) 
      {
         trace[i][j].state = 0;
         trace[i][j].x0 = trace[i][j].x1 = 0; 
         trace[i][j].y0 = trace[i][j].y1 = 0; 
         trace[i][j].xd = trace[i][j].yd = 0;
         trace[i][j].cost = 0;
      }
   }
   
   //���������� ��������� ������ �������� � ������� �������
   for(k = 0,  i = big/2; i < (big/2+3); ++i, ++k)
      for (j = 0; j < 4; ++j)  trace[i][j].state = block3[k][j];
   
   for(k = 0,  i = (big/2+small+3);   i < (big/2+small+6);  ++i, ++k)
      for (j = 0; j < 4; ++j)  trace[i][j].state = block0[k][j];
   
   for(k = 0,  i = (LengthOfTrace-big/2-3); i < (LengthOfTrace-big/2); ++i, ++k)
      for (j = 0; j < 4; ++j)  trace[i][j].state = block0[k][j];
   
   for(k = 0,  i = (LengthOfTrace-big/2-small-6);   i < (LengthOfTrace-big/2-small-3); ++i, ++k)
      for (j = 0; j < 4; ++j)  trace[i][j].state = block3[k][j];
   
   for(i = (big/2+3); i < ((big/2+3)+small); ++i)
      trace[i][3].state = -1;
   for(i = (LengthOfTrace - 3 - small - big/2); i < (LengthOfTrace - 3 - big/2); ++i)
      trace[i][3].state = -1;
   return trace;
}
void DrawTrace(int x, int y, Trace ** trace)//��������� ������
{
   int NumberOfRotation = 3; //�������� ������������� ����������, �� ����������� �������� �����
   int type = 0, t;
   for(int i = 0; i < LengthOfTrace; ++i)
   {
      if(trace[i][0].state == 1)// ���� ������� �������
      {
         // ������ ����� ����� �������
         SetRotationTile(NumberOfRotation, x+sdwig[0][5+type]*size, y+sdwig[1][5+type]*size, trace, i);
         x+=sdwig[0][6+type]*size; 
         y+=sdwig[1][6+type]*size;
         NumberOfRotation--;
         t = 3-NumberOfRotation;
         type+=2;
         i+=2;
      }
      else
      {
         // ����� ������ ���������� ��������
         for(int j = 0; j < 4; j++)
         {
            t = 3-NumberOfRotation;
            if (trace[i][j].state != 0) 
            {
               x+=sdwig[0][t]*size; 
               y+=sdwig[1][t]*size;
               continue;
            }
            SetSquareTile(x, y, i, j, trace); 
            x+=sdwig[0][t]*size; y+=sdwig[1][t]*size;
         }
      }
      x+=sdwig[2][t]*size; 
      y+=sdwig[3][t]*size;
   }
}
void DestroyTrace(Trace ** trace)// ������������ ������
{
   for (int i = 0; i < LengthOfTrace; ++i)
      free(trace[i]);
   free(trace);
}
void CoordinateDraw(int x, int y, int d1, int d2, int r1, int i, int j, Trace** trace, double xd, double yd)
{
   pieslice(x, y, d1, d2, r1);//��������� �������� ��������
   
   //���������� ��������� ���� ������
   trace[i][j].x0 = x; 
   trace[i][j].y0 = y;
   trace[i][j].xd = xd; 
   trace[i][j].yd = yd;
}
void Sort(Chip * list_chips)//���������� �����
{
   Chip temp;
   // ���������� ��������� ����� �� ���-�� ���������� �������
   for (int i = 1; i < N_players*2; ++i)
      for (int j = i; j>= 1 &&  list_chips[j-1].path <= list_chips[j].path; j--)
      {
         //���� ����� ������ ���������� ���������, �� ���� ��������� ����� ������
         // ��� ����� ������ ������ ���������
          if (list_chips[i-1].path == list_chips[i].path && list_chips[i-1].j > list_chips[i].j || 
             list_chips[i-1].path != list_chips[i].path)
          {
            temp =  list_chips[j-1];
            list_chips[j-1] =  list_chips[j];
            list_chips[j] = temp; 
          }
      }
      
   for (int i = 1; i < N_players*2; ++i)
   if (list_chips[i-1].path == list_chips[i].path && list_chips[i-1].j > list_chips[i].j )
   {
      temp = list_chips[i-1];
      list_chips[i-1] = list_chips[i];
      list_chips[i] = temp;
   }
}

void ColourChip(int condition, int NumTeam, int i, int j, Trace** trace)// ������� ��������
{
   switch(condition)
   {
      case 0://"�������"(���������) ����� �� ����
         setcolor(COLOR(128,128,128));// ���� ����
         setfillstyle(SOLID_FILL, COLOR(128,128,128));
         break;
      case 1://��������� ����� ��� �������������
         setcolor(BLACK);
         setfillstyle(SLASH_FILL, GREEN); 
         setbkcolor(COLOR(154, 205, 50));
         break;
      case 2://"�����������" ����� �� ����� �����
         setcolor(BLACK);
         setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
         FillGap(trace, i, j, 2);
         break;
      case 3://"�������"(���������) ����� �� ������
         setcolor(WHITE);
         setfillstyle(SOLID_FILL, WHITE);
         break;
      case 4: //"�������"(���������) ����� �� ������
         setcolor(BLACK);
         setfillstyle(SOLID_FILL, BLACK);
         break;
      case 5:// ��������� �����, ��� ��������
         setcolor(BLACK);
         setfillstyle(SLASH_FILL, RED); 
         setbkcolor(COLOR(255, 73, 108));
         break;
   }
   circle(trace[i][j].xd, trace[i][j].yd, size/2-2);
   fillellipse(trace[i][j].xd, trace[i][j].yd, size/2-2, size/2-2);
}
void Spawn(Trace** trace, Chip * list_chips)// ���������� � ����
{
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
   int i = LengthOfTrace-big/2;
   setbkcolor(COLOR(255, 73, 108));//������� ��� ��� ����� �����
   outtextxy(trace[i][0].x0-90, trace[i][0].y0, "�����");
   i = LengthOfTrace-1;
   setbkcolor(COLOR(154, 205, 50));// ������� ��� ��� ����� �����
   outtextxy(trace[i][0].x0-90, trace[i][0].y0, "�����");
   
   //������������� ����� ��������� �����
   int k = (LengthOfTrace-1)%LengthOfTrace;
   for(int j = 0; j < N_players*2; j++)
   {
      ColourChip(2, j%N_players, k, 3, trace);//��������� ����� ����� �� ����
      list_chips[j].Num = j%N_players;
      list_chips[j].i = k;
      list_chips[j].j = 3;
      list_chips[j].path = -j - big/2+1;
      list_chips[j].DoneRound = 0 ;
      list_chips[j].flag = 0;
      list_chips[j].BonusFlag = 0;
      list_chips[j].place = 0;
      k = (k+1)%LengthOfTrace; 
   }
   first = 1, last = 2*N_players; //������������� ����, ������� ����� �������������� � �������� ��������
}
void ChooseTile(Chip* list_chips, int i, int j, Trace** trace, int N, int NumTurn, int BonusTurns, int NumBon)
{
   if (j != 0)//���� ��� �� ����� ���������� ������� ������, �� ����� ������� �����
      Compare(i, j-1, trace, N, NumTurn, 1, list_chips, BonusTurns, NumBon);
   
   if (j != 3)//���� ��� �� ����� ������� ������� ������, �� ����� ������� ������
      Compare(i, j+1, trace, N, NumTurn, 3, list_chips, BonusTurns, NumBon);
   
   //���� ����� ������� ������, � ������� ��� ������, �� "�������������" �� ���������(������ 2-�� ������� ��������)
   if(trace[i][j].state == 0 && trace[(i-1+LengthOfTrace)%LengthOfTrace][j].state == -1)
      Compare((i-2+LengthOfTrace)%LengthOfTrace, j, trace, N, NumTurn, 1, list_chips, BonusTurns, NumBon);
   
   //���� ��� ������ ������ ������������ ������, �� ����� ���� ������ ����� ������� ��� ��������� ������ �����������
   else if (trace[i][j].state == 1 && trace[(i-2+LengthOfTrace)%LengthOfTrace][j].state == 1)
      ChooseTile( list_chips, (i-2+LengthOfTrace)%LengthOfTrace, j, trace, N, NumTurn, BonusTurns, NumBon);
   
   // ����� ����� ������
   else 
      Compare((i-1+LengthOfTrace)%LengthOfTrace, j, trace, N, NumTurn, 1, list_chips, BonusTurns, NumBon);
}
void Compare(int i, int j, Trace** trace, int N, int NumTurn, int k, Chip* list_chips, int BonusTurns, int NumBon)
{
   //������� ����������, ����� �� �������� ��� ������ � �������� ������������� ��� ���� ��� ���
   // k - ����������� ���������� �����, ��� �������� �� ���������� ������� k = 1, ��� �������� �� ������� k = 3
   int  CurrTurnsMax =  list_chips[N].CurrTurnMax;
   if (trace[i][j].state != -1 && trace[i][j].cost != 2) 
   {
      if(NumTurn + k <= CurrTurnsMax)// ���� �������� ���� � ������
      {
         NumTurn+=k;
         
         // ���� ������ ��� �� ������� ��� �������������
         // ��� ����� ������� "�������" �� ��� ������
         if (trace[i][j].cost >= NumTurn*10 || trace[i][j].cost == 0)
            FillGap(trace, i, j, NumTurn*10);
         
         //�������� ������� ������ ��������� ������������� ������ ��� ����
         ChooseTile(list_chips, i, j, trace, N, NumTurn, BonusTurns, NumBon);
      }
      else
      {
         if(NumTurn == CurrTurnsMax )//���� ��������� ����. ���-�� �����
         {
            if (NumBon+k <= BonusTurns)//���� � ������� ���� ������
            {
               NumBon+=k; 
               // ���� ����� � ������� ������� ������� "�������"
               // ��� ���� ������ ��� �� ������� ��� �������������
               if((trace[i][j].cost >= NumBon*1000 && trace[i][j].cost >= 1000) || trace[i][j].cost == 0) 
               {
                  FillGap(trace, i, j, NumBon*1000); 
                  ChooseTile(list_chips, i, j, trace, N, NumTurn, BonusTurns, NumBon);
               }
            }
         }
      }
   }
}
int MouseClick(Trace** trace, Chip* list_chips, int N)// ����������� ��������� ������
{
      FillGap(trace,list_chips[N].i,list_chips[N].j, 10);
      int NumTeam = list_chips[N].Num;   
      while (mousebuttons() != 1);
      int x = mousex(), y = mousey();
   
      for (int i = 0; i < LengthOfTrace; ++i)
      {
         for (int j = 0; j<4; ++j)
            if (trace[i][j].cost >= 10 && 
               pow(x - trace[i][j].xd, 2) + pow(y - trace[i][j].yd, 2) <= pow(size/2, 2))// ���� ���� ����� � ��������
            {
               list_chips[N].path += (list_chips[N].i - i + LengthOfTrace) % LengthOfTrace;// ���������� ���������� ���������� �������
               list_chips[N].i = i;
               list_chips[N].j = j;
               if (trace[i][j].cost / 1000 > 0)// ���� ��� �������� ������
               {
                  list_chips[N].DoneTurn = list_chips[N].CurrTurnMax;// ���� ������ �����, �� ���-�� ���������� ������ - ���-�� ����. �����
                  list_teams[NumTeam].BonusTurns -= trace[i][j].cost / 1000;
               }
               else
                  list_chips[N].DoneTurn = trace[i][j].cost / 10;
               ColourChip(2, NumTeam, i, j, trace);//����������� ������ ������� ����� ����� �����
               CleanTrace(trace);
               return 0;
            }
      }
      return 1;
}
void CleanTrace(Trace** trace)// �������� ������ �� ���� ���������� ��� ���� ������
{
   for (int i = 0; i < LengthOfTrace; ++i)
      for (int j = 0; j<4; ++j)
         if (trace[i][j].cost >= 10)
         {
            trace[i][j].cost = 0; // ��������� ���� ������
            if (i == LengthOfTrace-big/2 || i == LengthOfTrace-1)// ��������� �������� ����� ������ � ������
            {
                  if (j % 2 == 0) ColourChip(3, -1, i, j, trace);
                  else ColourChip(4, -1, i, j, trace);
            }
            else ColourChip(0, -1, i, j, trace); //��������� ������� ������
         }
}
void ColourTrace(Trace** trace, Chip * list_chips, int N)// �������� ���� ������������� ������ ��� ����
{
   for (int i = 0; i < LengthOfTrace; ++i)
      for (int j = 0; j<4; ++j)
         if (trace[i][j].cost > 2) //���� ������ �� ������ ������ ������
         {
            if (trace[i][j].cost < 1000)// ���� ��� �� �������� ���
            {
               if (trace[i][j].cost / 10 >= list_chips[N].CurrTurnMin)// ���� ����� ��������� ������ �����, ��� ���. ���-��
               {
                  ColourChip(1, -1, i, j, trace);
                  list_chips[N].flag = 1; //����� ���� ���� ������
               }
            }
            else // �������� ���
            {
               ColourChip(5, -1, i, j, trace);
               list_chips[N].flag = 1; // ����� ���� ���� ������
            }
         }
} 
void MessageTeam(int N, Chip* list_chips)//��������� � ����
{
   char c[3];
   MessageInf(N, list_chips);
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
   
   if (list_chips[N].flag == 1)// ���� ����� ���� ���� ������
   {
      IntToChar(list_chips[N].CurrTurnMin, c);
      outtextxy(390, 100, "� ��� ���� �������");
      outtextxy(720, 100, c);
      outtextxy(390, 130, "� �������� ");
      IntToChar(list_chips[N].CurrTurnMax, c);
      outtextxy(580, 130, c);
      outtextxy(625, 130, "���a(-��)");
      MessageRound(N, list_chips);
      if (list_teams[list_chips[N].Num].BonusTurns > 0)//���� � ����� ���� ������, �� ������������ ��������� � ���
         MessageBonus(N, list_chips);
   }
   else// ���� ������ ������
   {
      outtextxy(390, 100, "� ���! ��� ������ ������!");
      outtextxy(390, 130, "���� ����� ����� �������!");
      delay(5000);
   }
}
void MessageBonus(int N, Chip* list_chips)//��������� � ���������� ������� �������
{
   char c[3];
   setcolor(BLACK);
   setbkcolor(COLOR(255, 73, 108));
   setfillstyle(SOLID_FILL, COLOR(255, 73, 108));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
   bar(390, 150, 800, 250);
   
   outtextxy(390, 160, "���! �� ����� �������:");
   IntToChar(list_teams[list_chips[N].Num].BonusTurns, c);
   outtextxy(780, 160, c);
   outtextxy(390, 190, "����� ���������������");
   outtextxy(390, 220, "������ �� ����� �����!");
}
void MessageRound(int N, Chip* list_chips)// ��������� � ���������� ���������� ������ ������
{
   char c[3];
   int x = 280 - size*small/2-1.5*size, y = 300 + size*big/4;
   setcolor(BLACK);
   setbkcolor(COLOR(195, 176, 145));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   
   IntToChar(list_chips[N].DoneRound+1, c);
   outtextxy(x-25, y, c);
   outtextxy(x, y, "/");
   IntToChar(N_rounds+1, c);
   outtextxy(x+25, y, c);
}
void MessageFinish(int N, Chip* list_chips)//��������� � ��������� ����� ��� ������ �����
{
   MessageInf(N, list_chips);
   int NumTeam = list_chips[N].Num;
   
   setcolor(BLACK);
   setbkcolor(COLOR(154, 205, 50));
   setfillstyle(SOLID_FILL, (COLOR(154, 205, 50)));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14 );
   bar(390, 250, 800, 300);
   outtextxy(390, 260, "�����");
   setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   fillellipse(500, 260, size/2-2, size/2-2);
   outtextxy(390, 280, "��������� �����");
   
   delay(5000);
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145));
   bar(390, 250, 800, 360 );
}
void MessageKick(int N, Chip* list_chips)// ��������� � �������� ����� � �����
{
   IMAGE* nyanya;
   nyanya  = loadBMP("nyanya.bmp");
   int NumTeam = list_chips[N].Num;
   
   setcolor(BLACK);
   setbkcolor(COLOR(240, 205, 91));
   setfillstyle(SOLID_FILL, (COLOR(240, 205, 91)));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14 );
   bar(390, 150, 800, 270);
   
   outtextxy(500, 165, "�������� ����");
   outtextxy(500, 185, "�������� �����!");
   putimage(390, 165, nyanya, COPY_PUT);
   setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   fillellipse(400, 165, size/2-2, size/2-2);
   
   delay(5000);
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145));
   bar(390, 150, 800, 270 );
   freeimage(nyanya);
}
void MessageInf(int N, Chip* list_chips)// ��������� � �������(����, ���, �����������)
{
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145)); 
   bar(390, 0, 800, 250);
   int NumTeam = list_chips[N].Num;
   
    // ������������ �� ����� ������� ���������� ���� ������
   setcolor(BLACK);
   if (list_teams[NumTeam].g == 64 || list_teams[NumTeam].r == 0) 
      setcolor(WHITE); 
   
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   setbkcolor(COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   
   bar(390, 0, 800, 150 );
   outtextxy(390, 0, "�������");
   outtextxy(390, 50, list_teams[NumTeam].name);
   putimage(700, 0, list_teams[NumTeam].image, COPY_PUT);
}
void MessageTail(int N, Chip* list_chips)//��������� ����������
{
   MessageInf(N, list_chips);
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14 );
   outtextxy(390, 100, "��� ��������� �� �����!");
   outtextxy(390, 130, "-1 ����a:(");
   MessageKick(N, list_chips);
}
void Ranking(Chip* list_chips)// ������������� ������� �����
{
   int x = 500, y = 600-size*2*N_players;
   char c[3];
   int NumTeam;
   
   //��������� ����������� ����������
   setcolor(BLACK);
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145)); 
   bar(x, y, 800, 600);
   setbkcolor(COLOR(195, 176, 145));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
   
   //����� �������� � ���� �������
   outtextxy(x, y-20, "�������");
   for (int i = 0; i < N_chips; ++i)
   {
      NumTeam = list_chips[i].Num;
      setcolor(BLACK);
      setbkcolor(COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
      setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
      
      bar(x, y, x+300, y+size);
      rectangle(x, y, x+300, y+size);
      IntToChar(i+1, c);
      outtextxy(x+3, y+3, c);
      y+=size;
      x+=10;
   }
}
void Activate(int i, int j, Trace** trace)//����� �����, ������� ����� ��� ������� ����� ������ � ������
{
   setcolor(BLACK);
   circle(trace[i][j].xd, trace[i][j].yd, 2);
}
void IntToChar(int x, char* c) //������� ������ �������������� ����������� ����� � ������ ��������
{
    if (x > 9)
      {
         c[0] =x/10 + '0'; 
         c[1] =x%10 + '0';
         c[2] = 0;
      }
      else 
      {
         c[0] = x + '0';
         c[1] = 0;
      }
}
void Check(Chip* list_chips, Trace** trace) // �������� ������ ����� ����, ��� ��� ����� �������
{
   int N = N_chips;
   for (int i = 0; i < N; ++i)
   {
      if (list_chips[i].path / LengthOfTrace > list_chips[i].DoneRound) // ���� ����� ��������� ��������� ����
      {
         if (list_chips[i].DoneRound == N_rounds) //���� ����� �������� ��� ���������, ��� ���������� ������� 
         {
            list_chips[i].place = first;
            list_chips[i].path = 0;
            first++;
            N_chips--;
            MessageFinish(i, list_chips);
            FillGap(trace, list_chips[i].i,list_chips[i].j, 10);
            CleanTrace(trace); 
         }
         else
         {
            if (list_teams[list_chips[i].Num].BonusTurns > 0)// ��������� �������, ���� ��� ����
               if (list_chips[i].BonusFlag == 1)
               {
                     list_teams[list_chips[i].Num].BonusTurns =  0;
                     list_chips[i].BonusFlag = 0;
               }
            if (flag[list_chips[i].DoneRound]== 0)//��������� �������, ���� �� ��� ����� �� ���� �� ���� ����
               {
                  list_teams[list_chips[i].Num].BonusTurns = 3; 
                  flag[list_chips[i].DoneRound] = 1;
                  list_chips[i].BonusFlag = 1; 
               }
            list_chips[i].DoneRound++;
         }
      }
      if (list_chips[i].flag == 0) //���� ����� �� ������� �� ���� �����, �� ��� ���������� �������
      {
         list_chips[i].place = last;
         list_chips[i].path = -100;
         last--;
         N_chips--;
         MessageInf(i, list_chips);
         MessageKick(i, list_chips);
         FillGap(trace, list_chips[i].i,list_chips[i].j, 10);
         CleanTrace(trace);
      }
   }
}
void FillGap(Trace** trace, int i, int j, int x) // ������ � ������������ ������� ��������
{
   //������� �������� ������ ��������� ���� ������ �����������
   // ���� ���� � ����� ���������� ������ ��������, ������� � ��������� - ��� ������ �������, � �� ����
   if (trace[i][j].state == 1 && trace[(i-2+LengthOfTrace)%LengthOfTrace][j].state == 1) 
      trace[(i-2+LengthOfTrace)%LengthOfTrace][j].cost = x;
   else 
   {
      if  (trace[i][j].state == 1 && trace[(i+2+LengthOfTrace)%LengthOfTrace][j].state == 1)
      trace[(i+2+LengthOfTrace)%LengthOfTrace][j].cost = x;
   }
   trace[i][j].cost = x;
}
void Randomize() // ��������� ����������� ����������� ����� ������
{
   Team temp;
   for (int i = 0; i< N_players; ++i)
   {
      int j = rand()%N_players;
      temp = list_teams[i];
      list_teams[i] = list_teams[j];
      list_teams[j] = temp;
   }
}
void CheckTail(Chip* list_chips, Trace** trace) // ��������� ���������
{
   int i = N_chips - 1;
   while(list_chips[0].path - list_chips[i].path >= LengthOfTrace) // ���� ������� ����������� ���������� ����� ������� � ���������
   {                                                                                     // ������ ����� ������, �� �������� ��������� � �����
      list_chips[i].place = last;
      list_chips[i].path = -100;
      last--;
      N_chips--;
      Activate(list_chips[i].i, list_chips[i].j, trace);
      MessageTail(i, list_chips);
      FillGap(trace, list_chips[i].i,list_chips[i].j, 10);
      CleanTrace(trace);
      i--;
   }
}
void Result(Chip* list_chips) // ����� �����������
{
   CountScore(list_chips);
   PrepareForResult();
   
   //����� ������ � ���� ������� - �� ������� ������ �����
   char c[3];
   int x = 80, y = 180;
   for (int i = 1; i <= N_players; ++i)
   {
      setcolor(BLACK);
      setbkcolor(COLOR(list_teams[i-1].r, list_teams[i-1].g, list_teams[i-1].b));
      setfillstyle(SOLID_FILL, COLOR(list_teams[i-1].r, list_teams[i-1].g, list_teams[i-1].b));
      
      bar(x, y, x+i*130, y+size*1.8);
      rectangle(x, y, x+i*130, y+size*1.8);
      IntToChar(list_teams[i-1].score, c);
      outtextxy(x+50, y+3, c);
      y+=size*1.5;
   } 
   delay(100);
   //������ ������ � ������� ����
   putimage(buttons[4].left, buttons[4].top, buttons[4].image, COPY_PUT);
   int i = -1;
   while (i < 0)
   {
      while (mousebuttons() != 1);
      i = select(mousex(), mousey()); 
      if (i == 4) {return;}
         else i = -1;
   }
}
void PrepareForResult() // ���������� ������� � ���������� �����������
{
   //������� �������� ������ 
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145));
   bar(390, 0, 800, 600);
   putimage(buttons[4].left, buttons[4].top, buttons[4].image, COPY_PUT);
   
   //����������� ������� ������ �����
   int i = -1;
   while (i < 0)
   {
      while (mousebuttons() != 1);
      i = select(mousex(), mousey()); 
      if (i == 4) break;
      else i = -1;
   }
   
   // ����� ���� � ������������
   setfillstyle(SOLID_FILL, COLOR(243, 244, 252));
   cleardevice();
   bar(0, 0, getmaxx(), getmaxy());
   int x = 80, y = 150;
   setcolor(BLACK);
   setbkcolor(COLOR(243, 244, 252));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   outtextxy(100, 50, "����������� �����������!");
   outtextxy(x, 100, "����:");
}
void CountScore(Chip* list_chips) // ������� ����� ������
{
   // ��������� ������� - ����� ����� ���� ����� �� ���������� �����
   for (int i = 0; i < N_players*2; ++i)
   if (list_chips[i].place == 0) list_teams[list_chips[i].Num].score += score[last-1];
      else list_teams[list_chips[i].Num].score += score[list_chips[i].place-1];
   
   //���������� ��������� �� �������� �����
   Team temp;
   for (int i = 1; i < N_players; ++i)
      for (int j = i; j>= 1 &&  list_teams[j-1].score <= list_teams[j].score; j--)
      {
         temp =  list_teams[j-1];
         list_teams[j-1] =  list_teams[j];
         list_teams[j] = temp; 
      }
}