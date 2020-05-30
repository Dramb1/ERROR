#define N_BUTTONS   5 // количество кнопок
#define NAME_SIZE  32 // длина названия кнопки
#define KEY_EXIT   27
#define MAXN 6 // максимальное количество игроков
#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600
#define _USE_MATH_DEFINES





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

void choosemenu()
{
   int i, j;
   cleardevice();
   int x1[MAXN], x2[MAXN], y1[MAXN], y2[MAXN];
   //окно и фон
   setfillstyle(SOLID_FILL, COLOR(243, 244, 252));
   bar(0, 0, getmaxx(), getmaxy());
         
   //настройка шрифта и цвета
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 15);
   setbkcolor(COLOR(65, 105, 225));
   setfillstyle(SOLID_FILL, COLOR(65, 105, 225));
   outtextxy(50, 50, "ВЫБЕРИТЕ КОЛИЧЕСТВО ИГРОКОВ");
      
   //создание кнопок
   FILE * input = fopen("choosemenu_buttons.txt", "r");
   char c[2];
   for(j = 0; j < MAXN; j++)
   {
      fscanf(input, "%d%d%d%d%*c", &x1[j], &y1[j], &x2[j], &y2[j]);
      if (j != MAXN-1)
      {
         bar(x1[j], y1[j], x2[j], y2[j]);
         c[0] =(j+2)+ '0'; 
         c[1] = 0;
         outtextxy((x1[j]+x2[j])/2 - 10, (y1[j]+y2[j])/2 - 10 , c);
      }
   }
   fclose(input);
      
   // определяем, какая кнопка нажата
   N_players = -1;
   while (N_players < 0)
   {
      while (mousebuttons() != 1);
      int x = mousex(), y = mousey();
      for (int j = 0; j < MAXN-1; j++)
         if (x >= x1[j] && x <= x2[j])
            if (y >= y1[j] && y <= y2[j]) N_players = j+2;
   }
   N_chips = N_players*2;
   Chip * list_chips = (Chip*)malloc((N_players*2) * sizeof(Chip));
   setbkcolor(COLOR(243, 244, 252));
   cleardevice();
   
   // загружаем данные о командах из текстого файла
   char picture[NAME_SIZE];
   FILE * input1 = fopen("teams.txt", "r");
   for (i = 0; i < N_players+2 ;++ i)
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
   for (j = 0; j < N_players; ++j)
   {
      putimage(x1[j], y1[j], list_teams[j].image, COPY_PUT);
      outtextxy(x1[j]+100, y1[j], "команда");
      outtextxy(x1[j]+100, y1[j]+15, list_teams[j].name);
      outtextxy(x1[j]+100, y1[j]+30, "цвет:");
      setfillstyle(SOLID_FILL, COLOR(list_teams[j].r, list_teams[j].g, list_teams[j].b));
      bar(x1[j]+100, y1[j]+45, x2[j], y2[j]);
   }
   size = 26-N_players;
   putimage(buttons[4].left, buttons[4].top, buttons[4].image, COPY_PUT);
   while (true)
   {
      while (mousebuttons() != 1);
      int x = mousex(), y = mousey();
      if (x > buttons[4].left &&
          x < buttons[4].left + buttons[4].width &&
          y > buttons[4].top &&
          y < buttons[4].top + buttons[4].height)
      break;
   }
   game(list_chips);
}
void game(Chip * list_chips)
{
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
         for (int i = 0; i < LengthOfTrace; ++i)
         {
            for (int j = 0; j < 4; ++j)
               printf("%04d  ", trace[i][j].cost);
            printf("\n");
         }
         printf("\n");
         ColourTrace(trace, list_chips, I);
         Activate(list_chips[I].i,  list_chips[I].j, trace);
         MessageTeam(I, list_chips); 
         if (list_chips[I].flag == 1) 
            while(MouseClick(trace, list_chips, I));       
      }
      Check(list_chips, trace);
      Sort(list_chips);
      CheckTail(list_chips, trace);
   }
   DestroyTrace(trace);
   Result(list_chips);
   menu();
}
void SetSquareTile(int x, int y, int i, int j, Trace** trace)
{
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
   trace[i][j].xd = x+0.5*size; 
   trace[i][j].yd = y+0.5*size;
   trace[i][j].x0 = x;
   trace[i][j].x1 = x+size; 
   trace[i][j].y0 = y; 
   trace[i][j].y1 = y+size; 
}
void SetRotationTile(int N, int x, int y, Trace** trace, int i)
{
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
      i0 = i+2; i1 = i;
   }
   else 
   {
      i0 = i; i1 = i+2;
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
Trace** CreateTrace()
{
   int i, j, k;
   if (N_players == 2) N_rounds = 1;
      else N_rounds = 5;
   small = 1+N_players;
   big = (2*N_players-1)*2;
   LengthOfTrace = 2*(big+small) + 4*3;
   Trace ** trace = (Trace**)malloc(LengthOfTrace * sizeof(Trace*));
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
void _abracadabra_cast(DrawTrace(int x, int y, Trace ** trace));