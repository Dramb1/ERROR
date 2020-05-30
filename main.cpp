#define N_BUTTONS   5 // количество кнопок
#define NAME_SIZE  32 // длина названия кнопки
#define KEY_EXIT   27
#define MAXN 6 // максимальное количество игроков
#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600
#define _USE_MATH_DEFINES

#include "graphics.h"
#include "Windows.h"
#include <math.h>
#include <stdlib.h>
int first, last;  // первое и последнее место в рейтинге
int N_players;// количество игроков
int N_rounds;// количество всего дорожек, которое необходимо преодолеть игрокам, 
                 //чтобы достигнуть финиша
int N_chips;// текущее количество фишек в игре
int small, big;// длины маленькой, большой частей трассы
int LengthOfTrace;// длина трассы
double size; // размер ячеек трассы

struct Button
{
   int left; // кордината х
   int top; // координата у
   int width;
   int height;
   IMAGE *image;
};
Button buttons[N_BUTTONS];
IMAGE *image_menu, *image_about;

struct Team
{
   char name[10]; // имя
   int r, g, b; //цвет
   IMAGE *image; // картинка соответствующего персонажа
   int  BonusTurns; // текущее и бонусное количество ходов команды
   int score; // количество очков команды
}; 
Team list_teams[MAXN];
struct Chip
{
   int Num; // номер команды
   int i, j; // текущее положение
   int DoneTurn; // количество сделанных ходов
   int path; // количество пройденных полос
   int CurrTurnMin, CurrTurnMax;// текущее максимальное и минимальное количество ходов
   int DoneRound; // количество пройденных кругов
   int place; //место в гонке, занятое фишкой 
   int flag; // флаг -- сходила фишка или нет
   int BonusFlag; // получила ли фишка бонусы или нет
};
typedef struct Trace
{
   int x0, x1, y0, y1; // координаты для квадратиков
   int state;//0 -  клетка существует/ -1 - не существут/1 - объединена
   int cost; // цена хода на данную клетку
   double xd, yd; // координаты для центра круга-выбора
}Trace;

// массивы сдигов, необходимые при рисовании симметричных объектов
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

int flag[6] = {0};// массив флагов - взял ли кто-либо из игроков бонус на данном круге или нет
int score[12] ={15, 12, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};// баллы за места: 1-ое место - 15, ... , 12-ое - 1

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

void loadbutton() // загрузка изображений и данных о кнопках
{
   FILE *input;
   char name[NAME_SIZE];
   
   // загрузка данных о кнопках
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
   
   // загрузка изображений меню и правил
   image_menu  = loadBMP("window_menu.bmp");
   image_about = loadBMP("window_about.bmp");
}

void start() //стартового окна
{
   IMAGE *image;
   image = loadBMP("window_start.bmp");
   putimage(0, 0, image, COPY_PUT);
   freeimage(image);
   getch();
}

void menu() //главное меню
{
   while (true)
   {
      // вставка основных кнопок главного меню
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

int select(int x, int y) //определение нажатой кнопки
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
void about() //окно с правилами
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

void close() //освобождение памяти от изображений
{
   for (int i = 0; i < N_BUTTONS; ++i)
      freeimage(buttons[i].image);
   freeimage(image_menu);
   freeimage(image_about);
   for (int i = 0; i < N_players; ++i)
      freeimage(list_teams[i].image);
}

void choosemenu(){// меню выбора
   int i, j;
   int x1[MAXN], x2[MAXN], y1[MAXN], y2[MAXN];
   QuantityPlayers();
   
   //создание кнопок
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
      
   // определяем, какая кнопка нажата
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
      outtextxy(x1[j]+100, y1[j], "команда");
      outtextxy(x1[j]+100, y1[j]+15, list_teams[j].name);
      outtextxy(x1[j]+100, y1[j]+30, "цвет:");
      setfillstyle(SOLID_FILL, COLOR(list_teams[j].r, list_teams[j].g, list_teams[j].b));
      bar(x1[j]+100, y1[j]+45, x2[j], y2[j]);
   }
   // кнопка далее
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
void QuantityPlayers()//сообщение о выборе кол-ва игроков
{
   cleardevice();
   setfillstyle(SOLID_FILL, COLOR(243, 244, 252));
   bar(0, 0, getmaxx(), getmaxy());
         
   //настройка шрифта и цвета
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   setbkcolor(COLOR(65, 105, 225));
   setfillstyle(SOLID_FILL, COLOR(65, 105, 225));
   outtextxy(50, 50, "ВЫБЕРИТЕ КОЛИЧЕСТВО ИГРОКОВ");
      
}
void AboutTeams()//информация о командалх
{
   setbkcolor(COLOR(243, 244, 252));
   cleardevice();
   
   // загружаем данные о командах из текстого файла
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
   
   //выводим необходимы данные о командах
   setbkcolor(COLOR(65, 105, 225));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   outtextxy(0, 50, "ИГРОКИ, ОПРЕДЕЛИТЕСЬ С КОМАНДАМИ");
   outtextxy(50, 100, "ГОНКА НАЧНЕТСЯ ЧЕРЕЗ 3 2 1..");
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
}
void game(Chip * list_chips)
{
   //основная функция игры, которая вызывает вспомогательные функции
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
         
         //определение кол-ва ходов фишки
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
         if (list_chips[I].flag == 1)// если фишке есть куда ходить, ждем клика мышки
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
void SetSquareTile(int x, int y, int i, int j, Trace** trace)//квадратные клетки
{
   // если это ряд финиша или старта используются белые и черные цвета заливки
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
   
   //заполнение координаты клетки
   trace[i][j].xd = x+0.5*size; 
   trace[i][j].yd = y+0.5*size;
   trace[i][j].x0 = x;
   trace[i][j].x1 = x+size; 
   trace[i][j].y0 = y; 
   trace[i][j].y1 = y+size; 
}
void SetRotationTile(int N, int x, int y, Trace** trace, int i)//клетки поворота
{
   //отрисовка элементов поворота и заполнение соответств. ячеек массива координатами
   int d = N*90; // угол поворота
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
Trace** CreateTrace()//создание динамического двумерного массива структур
{
   int i, j, k;
   if (N_players == 2) N_rounds = 1;
      else N_rounds = 5;
   size = 26-N_players;
   small = 1+N_players;
   big = (2*N_players-1)*2;
   LengthOfTrace = 2*(big+small) + 4*3;
   Trace ** trace = (Trace**)malloc(LengthOfTrace * sizeof(Trace*));
   
   //инициализация всех полей структуры
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
   
   //заполнение состояния клеток поворота с помощью массива
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
void DrawTrace(int x, int y, Trace ** trace)//отрисовка трассы
{
   int NumberOfRotation = 3; //повороты пронумерованы поубыванию, по направлению движения фишек
   int type = 0, t;
   for(int i = 0; i < LengthOfTrace; ++i)
   {
      if(trace[i][0].state == 1)// если начался поворот
      {
         // рисуем сразу целый поворот
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
         // иначе рисуем квадратную клеточку
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
void DestroyTrace(Trace ** trace)// освобождение памяти
{
   for (int i = 0; i < LengthOfTrace; ++i)
      free(trace[i]);
   free(trace);
}
void CoordinateDraw(int x, int y, int d1, int d2, int r1, int i, int j, Trace** trace, double xd, double yd)
{
   pieslice(x, y, d1, d2, r1);//отрисовка элемента поворота
   
   //заполнение координат этой клетки
   trace[i][j].x0 = x; 
   trace[i][j].y0 = y;
   trace[i][j].xd = xd; 
   trace[i][j].yd = yd;
}
void Sort(Chip * list_chips)//сортировка фишек
{
   Chip temp;
   // сортировка вставками фишек по кол-ву пройденных дорожек
   for (int i = 1; i < N_players*2; ++i)
      for (int j = i; j>= 1 &&  list_chips[j-1].path <= list_chips[j].path; j--)
      {
         //если фишки прошли одинаковую дистанцию, но одна находится левее другой
         // или фишки прошли разную дистанцию
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

void ColourChip(int condition, int NumTeam, int i, int j, Trace** trace)// окраска кружочка
{
   switch(condition)
   {
      case 0://"удалить"(закрасить) место на поле
         setcolor(COLOR(128,128,128));// цвет поля
         setfillstyle(SOLID_FILL, COLOR(128,128,128));
         break;
      case 1://закрасить место как потенциальное
         setcolor(BLACK);
         setfillstyle(SLASH_FILL, GREEN); 
         setbkcolor(COLOR(154, 205, 50));
         break;
      case 2://"передвинуть" фишку на новое место
         setcolor(BLACK);
         setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
         FillGap(trace, i, j, 2);
         break;
      case 3://"удалить"(закрасить) место на финише
         setcolor(WHITE);
         setfillstyle(SOLID_FILL, WHITE);
         break;
      case 4: //"удалить"(закрасить) место на финише
         setcolor(BLACK);
         setfillstyle(SOLID_FILL, BLACK);
         break;
      case 5:// закрасить место, как бонусное
         setcolor(BLACK);
         setfillstyle(SLASH_FILL, RED); 
         setbkcolor(COLOR(255, 73, 108));
         break;
   }
   circle(trace[i][j].xd, trace[i][j].yd, size/2-2);
   fillellipse(trace[i][j].xd, trace[i][j].yd, size/2-2, size/2-2);
}
void Spawn(Trace** trace, Chip * list_chips)// подготовка к игре
{
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
   int i = LengthOfTrace-big/2;
   setbkcolor(COLOR(255, 73, 108));//красный фон для слова финиш
   outtextxy(trace[i][0].x0-90, trace[i][0].y0, "ФИНИШ");
   i = LengthOfTrace-1;
   setbkcolor(COLOR(154, 205, 50));// зеленый фон для слова старт
   outtextxy(trace[i][0].x0-90, trace[i][0].y0, "СТАРТ");
   
   //инициализация полей структуры фишек
   int k = (LengthOfTrace-1)%LengthOfTrace;
   for(int j = 0; j < N_players*2; j++)
   {
      ColourChip(2, j%N_players, k, 3, trace);//отрисовка самих фишек на поле
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
   first = 1, last = 2*N_players; //инициализация мест, которые будут использоваться в итоговом рейтинге
}
void ChooseTile(Chip* list_chips, int i, int j, Trace** trace, int N, int NumTurn, int BonusTurns, int NumBon)
{
   if (j != 0)//если это не самая внутренняя дорожка трассы, то можно сходить влево
      Compare(i, j-1, trace, N, NumTurn, 1, list_chips, BonusTurns, NumBon);
   
   if (j != 3)//если это не самая внешняя дорожка трассы, то можно сходить вправо
      Compare(i, j+1, trace, N, NumTurn, 3, list_chips, BonusTurns, NumBon);
   
   //если нужно сходить вперед, а впереди нет клетки, то "перепрыгиваем" на следующую(случай 2-ой дорожки поворота)
   if(trace[i][j].state == 0 && trace[(i-1+LengthOfTrace)%LengthOfTrace][j].state == -1)
      Compare((i-2+LengthOfTrace)%LengthOfTrace, j, trace, N, NumTurn, 1, list_chips, BonusTurns, NumBon);
   
   //если это первая ячейка объединенной клетки, то выбор хода вперед нужно сделать для последней ячейки объединения
   else if (trace[i][j].state == 1 && trace[(i-2+LengthOfTrace)%LengthOfTrace][j].state == 1)
      ChooseTile( list_chips, (i-2+LengthOfTrace)%LengthOfTrace, j, trace, N, NumTurn, BonusTurns, NumBon);
   
   // иначе ходим вперед
   else 
      Compare((i-1+LengthOfTrace)%LengthOfTrace, j, trace, N, NumTurn, 1, list_chips, BonusTurns, NumBon);
}
void Compare(int i, int j, Trace** trace, int N, int NumTurn, int k, Chip* list_chips, int BonusTurns, int NumBon)
{
   //Функция определяет, можно ли отметить эту клетку в качестве потенциальной для хода или нет
   // k - необходимое количество ходов, для движения на внутренние дорожки k = 1, для движения на внешние k = 3
   int  CurrTurnsMax =  list_chips[N].CurrTurnMax;
   if (trace[i][j].state != -1 && trace[i][j].cost != 2) 
   {
      if(NumTurn + k <= CurrTurnsMax)// если остались ходы в запасе
      {
         NumTurn+=k;
         
         // если клетку еще не выбрана как потенциальная
         // или можно сходить "дешевле" на эту клетку
         if (trace[i][j].cost >= NumTurn*10 || trace[i][j].cost == 0)
            FillGap(trace, i, j, NumTurn*10);
         
         //вызываем функцию выбора следующих потенциальных клеток для хода
         ChooseTile(list_chips, i, j, trace, N, NumTurn, BonusTurns, NumBon);
      }
      else
      {
         if(NumTurn == CurrTurnsMax )//если потрачено макс. кол-во ходов
         {
            if (NumBon+k <= BonusTurns)//если у команды есть бонусы
            {
               NumBon+=k; 
               // если можно с помощью бонусов сходить "дешевле"
               // или если клетка еще не выбрана как потенциальная
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
int MouseClick(Trace** trace, Chip* list_chips, int N)// определение выбранной клетки
{
      FillGap(trace,list_chips[N].i,list_chips[N].j, 10);
      int NumTeam = list_chips[N].Num;   
      while (mousebuttons() != 1);
      int x = mousex(), y = mousey();
   
      for (int i = 0; i < LengthOfTrace; ++i)
      {
         for (int j = 0; j<4; ++j)
            if (trace[i][j].cost >= 10 && 
               pow(x - trace[i][j].xd, 2) + pow(y - trace[i][j].yd, 2) <= pow(size/2, 2))// если клик попал в кружочек
            {
               list_chips[N].path += (list_chips[N].i - i + LengthOfTrace) % LengthOfTrace;// прибавляем количество пройденных дорожек
               list_chips[N].i = i;
               list_chips[N].j = j;
               if (trace[i][j].cost / 1000 > 0)// если это бонусная клетка
               {
                  list_chips[N].DoneTurn = list_chips[N].CurrTurnMax;// если выбран бонус, то кол-во пройденных клеток - кол-во макс. ходов
                  list_teams[NumTeam].BonusTurns -= trace[i][j].cost / 1000;
               }
               else
                  list_chips[N].DoneTurn = trace[i][j].cost / 10;
               ColourChip(2, NumTeam, i, j, trace);//закрашиваем цветом команды новое место фишки
               CleanTrace(trace);
               return 0;
            }
      }
      return 1;
}
void CleanTrace(Trace** trace)// очистить трассу от всех помеченных для хода клеток
{
   for (int i = 0; i < LengthOfTrace; ++i)
      for (int j = 0; j<4; ++j)
         if (trace[i][j].cost >= 10)
         {
            trace[i][j].cost = 0; // обнуление цены клетки
            if (i == LengthOfTrace-big/2 || i == LengthOfTrace-1)// особенное внимание линии старта и финиша
            {
                  if (j % 2 == 0) ColourChip(3, -1, i, j, trace);
                  else ColourChip(4, -1, i, j, trace);
            }
            else ColourChip(0, -1, i, j, trace); //закрасить обычную клетку
         }
}
void ColourTrace(Trace** trace, Chip * list_chips, int N)// закраска всех потенциальных клеток для хода
{
   for (int i = 0; i < LengthOfTrace; ++i)
      for (int j = 0; j<4; ++j)
         if (trace[i][j].cost > 2) //если клетка не занята другой фишкой
         {
            if (trace[i][j].cost < 1000)// если это не бонусный ход
            {
               if (trace[i][j].cost / 10 >= list_chips[N].CurrTurnMin)// если нужно потратить больше ходов, чем мин. кол-во
               {
                  ColourChip(1, -1, i, j, trace);
                  list_chips[N].flag = 1; //фишке есть куда ходить
               }
            }
            else // бонусный ход
            {
               ColourChip(5, -1, i, j, trace);
               list_chips[N].flag = 1; // фишке есть куда ходить
            }
         }
} 
void MessageTeam(int N, Chip* list_chips)//сообщение о ходе
{
   char c[3];
   MessageInf(N, list_chips);
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
   
   if (list_chips[N].flag == 1)// если фишке есть куда ходить
   {
      IntToChar(list_chips[N].CurrTurnMin, c);
      outtextxy(390, 100, "У вас есть минимум");
      outtextxy(720, 100, c);
      outtextxy(390, 130, "и максимум ");
      IntToChar(list_chips[N].CurrTurnMax, c);
      outtextxy(580, 130, c);
      outtextxy(625, 130, "ходa(-ов)");
      MessageRound(N, list_chips);
      if (list_teams[list_chips[N].Num].BonusTurns > 0)//если у фишки есть бонусы, то показывается сообщение о них
         MessageBonus(N, list_chips);
   }
   else// если некуда ходить
   {
      outtextxy(390, 100, "О нет! Вам некуда ходить!");
      outtextxy(390, 130, "Вашу фишку скоро заберут!");
      delay(5000);
   }
}
void MessageBonus(int N, Chip* list_chips)//сообщение о количестве бонусов команды
{
   char c[3];
   setcolor(BLACK);
   setbkcolor(COLOR(255, 73, 108));
   setfillstyle(SOLID_FILL, COLOR(255, 73, 108));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
   bar(390, 150, 800, 250);
   
   outtextxy(390, 160, "ЫАУ! На счету бонусов:");
   IntToChar(list_teams[list_chips[N].Num].BonusTurns, c);
   outtextxy(780, 160, c);
   outtextxy(390, 190, "Успей воспользоваться");
   outtextxy(390, 220, "шансом до конца круга!");
}
void MessageRound(int N, Chip* list_chips)// сообщение о количестве пройденных фишкой кругов
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
void MessageFinish(int N, Chip* list_chips)//сообщение о окончании гонки для данной фишки
{
   MessageInf(N, list_chips);
   int NumTeam = list_chips[N].Num;
   
   setcolor(BLACK);
   setbkcolor(COLOR(154, 205, 50));
   setfillstyle(SOLID_FILL, (COLOR(154, 205, 50)));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14 );
   bar(390, 250, 800, 300);
   outtextxy(390, 260, "Фишка");
   setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   fillellipse(500, 260, size/2-2, size/2-2);
   outtextxy(390, 280, "завершает гонку");
   
   delay(5000);
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145));
   bar(390, 250, 800, 360 );
}
void MessageKick(int N, Chip* list_chips)// сообщение о удаление фишки с гонки
{
   IMAGE* nyanya;
   nyanya  = loadBMP("nyanya.bmp");
   int NumTeam = list_chips[N].Num;
   
   setcolor(BLACK);
   setbkcolor(COLOR(240, 205, 91));
   setfillstyle(SOLID_FILL, (COLOR(240, 205, 91)));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14 );
   bar(390, 150, 800, 270);
   
   outtextxy(500, 165, "Железная няня");
   outtextxy(500, 185, "забирает фишку!");
   putimage(390, 165, nyanya, COPY_PUT);
   setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   fillellipse(400, 165, size/2-2, size/2-2);
   
   delay(5000);
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145));
   bar(390, 150, 800, 270 );
   freeimage(nyanya);
}
void MessageInf(int N, Chip* list_chips)// сообщение о команде(цвет, имя, изображение)
{
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145)); 
   bar(390, 0, 800, 250);
   int NumTeam = list_chips[N].Num;
   
    // взависимости от цвета команды выбирается цвет шрифта
   setcolor(BLACK);
   if (list_teams[NumTeam].g == 64 || list_teams[NumTeam].r == 0) 
      setcolor(WHITE); 
   
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   setbkcolor(COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   setfillstyle(SOLID_FILL, COLOR(list_teams[NumTeam].r, list_teams[NumTeam].g, list_teams[NumTeam].b));
   
   bar(390, 0, 800, 150 );
   outtextxy(390, 0, "Команда");
   outtextxy(390, 50, list_teams[NumTeam].name);
   putimage(700, 0, list_teams[NumTeam].image, COPY_PUT);
}
void MessageTail(int N, Chip* list_chips)//сообщение отстающему
{
   MessageInf(N, list_chips);
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14 );
   outtextxy(390, 100, "Вам наступили на хвост!");
   outtextxy(390, 130, "-1 фишкa:(");
   MessageKick(N, list_chips);
}
void Ranking(Chip* list_chips)// обновляющийся рейтинг фишек
{
   int x = 500, y = 600-size*2*N_players;
   char c[3];
   int NumTeam;
   
   //настройка графических параметров
   setcolor(BLACK);
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145)); 
   bar(x, y, 800, 600);
   setbkcolor(COLOR(195, 176, 145));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 14);
   
   //вывод рейтинга в виде лесенки
   outtextxy(x, y-20, "Рейтинг");
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
void Activate(int i, int j, Trace** trace)//метка фишки, которая ходит или которую скоро удалят с трассы
{
   setcolor(BLACK);
   circle(trace[i][j].xd, trace[i][j].yd, 2);
}
void IntToChar(int x, char* c) //перевод целого положительного двузначного числа в массив символов
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
void Check(Chip* list_chips, Trace** trace) // проверка правил после того, как все фишки сходили
{
   int N = N_chips;
   for (int i = 0; i < N; ++i)
   {
      if (list_chips[i].path / LengthOfTrace > list_chips[i].DoneRound) // если фишка закончила очередной круг
      {
         if (list_chips[i].DoneRound == N_rounds) //если фишка проехала всю дистанцию, она прекращает участие 
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
            if (list_teams[list_chips[i].Num].BonusTurns > 0)// обнуление бонусов, если они есть
               if (list_chips[i].BonusFlag == 1)
               {
                     list_teams[list_chips[i].Num].BonusTurns =  0;
                     list_chips[i].BonusFlag = 0;
               }
            if (flag[list_chips[i].DoneRound]== 0)//получение бонусов, если их еще никто не взял за этот круг
               {
                  list_teams[list_chips[i].Num].BonusTurns = 3; 
                  flag[list_chips[i].DoneRound] = 1;
                  list_chips[i].BonusFlag = 1; 
               }
            list_chips[i].DoneRound++;
         }
      }
      if (list_chips[i].flag == 0) //если фишка не сходила на этом круге, то она прекращает участие
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
void FillGap(Trace** trace, int i, int j, int x) // работа с объединенной клеткой поворота
{
   //Функция помогает менять стоимость всех клеток объединения
   // речь идет о самой внутренней клетке поворота, которая в программе - три ячейки массива, а не одна
   if (trace[i][j].state == 1 && trace[(i-2+LengthOfTrace)%LengthOfTrace][j].state == 1) 
      trace[(i-2+LengthOfTrace)%LengthOfTrace][j].cost = x;
   else 
   {
      if  (trace[i][j].state == 1 && trace[(i+2+LengthOfTrace)%LengthOfTrace][j].state == 1)
      trace[(i+2+LengthOfTrace)%LengthOfTrace][j].cost = x;
   }
   trace[i][j].cost = x;
}
void Randomize() // случайное определение очередности ходов команд
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
void CheckTail(Chip* list_chips, Trace** trace) // проверить отстающих
{
   int i = N_chips - 1;
   while(list_chips[0].path - list_chips[i].path >= LengthOfTrace) // если разница пройденного расстояния между лидером и отстающим
   {                                                                                     // больше длины трассы, то отсающий удаляется с гонки
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
void Result(Chip* list_chips) // вывод результатов
{
   CountScore(list_chips);
   PrepareForResult();
   
   //вывод баллов в виде лесенки - на вершине первое место
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
   //кнопка выхода в главное меню
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
void PrepareForResult() // подготовка экранов к объявлению результатов
{
   //очистка игрового экрана 
   setfillstyle(SOLID_FILL, COLOR(195, 176, 145));
   bar(390, 0, 800, 600);
   putimage(buttons[4].left, buttons[4].top, buttons[4].image, COPY_PUT);
   
   //определение нажатия кнопки далее
   int i = -1;
   while (i < 0)
   {
      while (mousebuttons() != 1);
      i = select(mousex(), mousey()); 
      if (i == 4) break;
      else i = -1;
   }
   
   // новое окно с результатами
   setfillstyle(SOLID_FILL, COLOR(243, 244, 252));
   cleardevice();
   bar(0, 0, getmaxx(), getmaxy());
   int x = 80, y = 150;
   setcolor(BLACK);
   setbkcolor(COLOR(243, 244, 252));
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   outtextxy(100, 50, "Поздравляем победителей!");
   outtextxy(x, 100, "балл:");
}
void CountScore(Chip* list_chips) // подсчёт очков команд
{
   // результат команды - сумма очков двух фишек за полученные места
   for (int i = 0; i < N_players*2; ++i)
   if (list_chips[i].place == 0) list_teams[list_chips[i].Num].score += score[last-1];
      else list_teams[list_chips[i].Num].score += score[list_chips[i].place-1];
   
   //сортировка вставками по убыванию очков
   Team temp;
   for (int i = 1; i < N_players; ++i)
      for (int j = i; j>= 1 &&  list_teams[j-1].score <= list_teams[j].score; j--)
      {
         temp =  list_teams[j-1];
         list_teams[j-1] =  list_teams[j];
         list_teams[j] = temp; 
      }
}