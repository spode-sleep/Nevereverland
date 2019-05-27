#include <iostream>
#include <cmath>

#include <GL/glut.h>

#include "GL_movement.h"   // здесть удобный класс 3д точки и действия с камерой
#pragma warning(disable:4996)
#include <vector>
#include <fstream>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif


#ifdef _WIN32
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define pi 3.14
#include <ctime>
#include <cstdlib>

#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <wchar.h>

#include <FTGL/ftgl.h>
#include <FTFont.h>
#include <FTGLBitmapFont.h>

#ifndef __FONT_H
#define __FONT_H
#endif

#include <irrKlang.h>
#include <ik_ISoundEngine.h>

#include "Fall_murasaki_neverever.cpp"

#include "readBMP.h"

// меня замучали предупреждения о преобразованиях double во float так что будем игнорировать эти предупреждения

#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  



// ключевое слово extern информирует компиллятор,
// что это внешние переменные и их значение доступно другим файлам
// используется внутри GL_movement

extern GLfloat alX = 0, alY = 0; // угол поворота

// начальные данные о положении и совершенном перемещении мыши
extern int lx0 = 0, lx1 = 0, ly0 = 0, ly1 = 0; // левая клавиша
extern int rx0 = 0, rx1 = 0, ry0 = 0, ry1 = 0; // правая клавиша
extern int dveX0 = 0, dveY0 = 0, dveX1 = 0, dveY1 = 0; //две клавиши нажатые вместе

/*
 предопредленый угол зрения (FOV -Field of View) в перспективном режиме
 или величина приближения(в абсолютных единицах) в ортогональном режиме
 для приближения камеры(увеличения pzoom) удобно вызывать zoom(+3);
 для отдаления камеры(уменьшения pzoom) удобно вызывать zoom(-3);
 */
extern int pzoom = 60;

// тип используемой камеры по умолчанию
// 1 - ортогональная 2-перспективная                        
extern int view = 1;

extern bool m1 = false; // нажата ли левая клавиша мыши
extern bool m2 = false; // нажата ли правая клавиша мыши

// ининциализация глобавльных переменных
extern GLdouble w = 900, h = 900, px = 0, py = 0;

extern MyPoint e(10, 20, 10);
extern MyPoint c(0, 0, 0);
extern MyPoint u(0, 0, 1);

MyPoint Light_pos(10, 0, 5);
bool plosk_smooth = true; // сглаживать изображение плоскости?



// указатель на текстуру
GLubyte *resImage = 0;
// номер текстуры
GLuint texture1;

// переменная для списка отображения
GLuint Item1;

int resImageWidth, resImageHeight;

int mode = 0;

static void Resize(int width, int height);
static void Draw(void);

void  DrawModel(void);

GLint windowW, windowH;

bool isEndOfScript;


// функция задания свойст источников освещения
void InitLight(void)
{

	GLfloat amb[] = { 0.1,0.1,0.1,1. };
	GLfloat dif[] = { 1.,1.,1.,1. };
	GLfloat spec[] = { 1.,1.,1.,1. };
	GLfloat pos[] = { Light_pos.x,Light_pos.y,Light_pos.z,1. };

	// параметры источника света
	// позиция источника
	glLightfv(GL_LIGHT0, GL_POSITION, pos);



	// характеристики излучаемого света
	// фоновое освещение (рассеянный свет)
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	// диффузная составляющая света
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	// зеркально отражаемая составляющая света
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glPointSize(10);
	glColor3ub(255, 255, 0);
	glBegin(GL_POINTS);
	glVertex3fv(Light_pos);
	glEnd();
	glEnable(GL_LIGHTING);

}


// функции обработки событий

//extern GLfloat alX=0, alY=0; // угол поворота

void view_select(void)
{
	view++;
	if (view > 2)
		view = 1;

	pzoom = 80;
	c.Set(0, 0, 0);
	e.Set(10, 20, 10);
	zoom(0);
}

void Light_in_Camera(void)
{
	if (view == 2) // если в перспективе
	{
		Light_pos = e;
	}
}

void plosk_select(void)
{
	plosk_smooth = !plosk_smooth;
}

struct texture_dictionary
{
	char path[100];
	int* srcImage;
};

texture_dictionary tex_dict[100];
int count_of_textures;


void loadImage(char *str)
{
	int i, j;
	const char* bmpFileName = str;
	int *srcImage;
	srcImage = loadBMP(bmpFileName, resImageWidth, resImageHeight);

	if (!srcImage)
	{
		std::cout << "Could not load an image from the file " << bmpFileName
			<< "\ncheck if the file is present in the working directory.\n";
		return;
	}

	//Выделяем память под наше изображение
	delete[] resImage;
	resImage = new unsigned char[resImageWidth * resImageHeight * 4];

	//Процессим изображение (циклы по линиям и по столбцам)
	for (i = 0; i < resImageHeight; i++)
	{
		for (j = 0; j < resImageWidth; j++)
		{
			//Переприсваиваем в нашем изображении цветовые значения исходного

			int pixelValue = srcImage[i * resImageWidth + j];
			unsigned char red = pixelValue % 256;
			pixelValue >>= 8;
			unsigned char green = pixelValue % 256;
			unsigned char blue = pixelValue >> 8;

			resImage[i*resImageWidth * 4 + j * 4] = red;
			resImage[i*resImageWidth * 4 + j * 4 + 1] = green;
			resImage[i*resImageWidth * 4 + j * 4 + 2] = blue;

			// если цвет их всех равен 0, то сделать альфа компонент =0;
			resImage[i*resImageWidth * 4 + j * 4 + 3] =
				(red || green || blue) ? 255 : 0;
		}
	}
	delete srcImage;
}

void Textura_use(void)
{


	// устанавливаем формат хранения пикселей
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//  Команды glGenTextures() и glBindTexture() именуют и создают текстурный объект для изображения текстуры.
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// устанавливаем параметры повторения на краях
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// параметры отображения текстур если происходит уменьшение или увеличение текстуры
	//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	// даем команду на пересылку текстуры в память видеокарты
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth, resImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage);


	// способ наложения текстуры GL_MODULATE, GL_DECAL, and GL_BLEND.
	// GL_MODULATE - умножение на цвет от освещения
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// создадим два новых списка отображения
	Item1 = glGenLists(2);

	glNewList(Item1, GL_COMPILE);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);

	glTexCoord2f(0, 0);
	glVertex3f(-30, -30, -3);


	glTexCoord2f(1, 0);
	glVertex3f(30, -30, -3);

	glTexCoord2f(1, 1);
	glVertex3f(30, 30, -3);

	glTexCoord2f(0, 1);
	glVertex3f(-30, 30, -3);

	glEnd();



	glEndList();

	MyPoint A(-10, -10, 0), B(-10, 10, 0), C(10, -10, 0), D(10, 10, 0), S(0, 0, 20), N1, N2, N3, N4, T1, T2;

	T1 = B - A;
	T2 = S - A;
	// опереатором *= я переопределил векторное умнежение векторов
	N1 = T2 *= T1;
	N1.Normalize_Self();

	T1 = (D - B);
	T2 = (S - B);
	N2 = T2 *= T1;
	N2.Normalize_Self();

	T1 = (A - C);
	T2 = (S - C);
	N3 = T2 *= T1;
	N3.Normalize_Self();

	T1 = (C - D);
	T2 = (S - D);
	N4 = T2 *= T1;
	N4.Normalize_Self();

	glNewList(Item1 + 1, GL_COMPILE);

	//Рисуем пирамидку

	glBegin(GL_TRIANGLES);

	glNormal3fv(N1);
	glTexCoord2f(1, 1); glVertex3fv(B);
	glTexCoord2f(0, 1); glVertex3fv(A);
	glTexCoord2f(0.5, 0); glVertex3fv(S);

	glNormal3fv(N2);
	glTexCoord2f(1, 1); glVertex3fv(D);
	glTexCoord2f(0, 1); glVertex3fv(B);
	glTexCoord2f(0.5, 0); glVertex3fv(S);

	glNormal3fv(N3);
	glTexCoord2f(1, 1); glVertex3fv(C);
	glTexCoord2f(0, 1); glVertex3fv(D);
	glTexCoord2f(0.5, 0); glVertex3fv(S);

	glNormal3fv(N4);
	glTexCoord2f(1, 1); glVertex3fv(A);
	glTexCoord2f(0, 1); glVertex3fv(C);
	glTexCoord2f(0.5, 0); glVertex3fv(S);

	glEnd();

	glEndList();
	mode = 1;
}

void texture_mode(void)
{
	static bool t = 0;
	if (t)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	t = !t;
}

void Key_DOWN(void)
{
	if (view == 1)
		jamp(-3, 0);
	else
		strate(-3, 0);
}
void Key_UP(void)
{
	if (view == 1)
		jamp(3, 0);
	else
		strate(3, 0);
}
void Key_LEFT(void)
{
	jamp(0, -3);
}
void Key_RIGHT(void)
{
	jamp(0, 3);
}

// Общий обработчик нажатия клавиш.





void TextureStart(char *s)
{
	glEnable(GL_TEXTURE_2D);
	// рисуем прямоугольник
	// формулу вычисления освещения пикселя см в справке


	loadImage(s);

	// устанавливаем формат хранения пикселей
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//  Команды glGenTextures() и glBindTexture() именуют и создают текстурный объект для изображения текстуры.
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// устанавливаем параметры повторения на краях
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// параметры отображения текстур если происходит уменьшение или увеличение текстуры
	//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	// даем команду на пересылку текстуры в память видеокарты
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth, resImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage);


	// способ наложения текстуры GL_MODULATE, GL_DECAL, and GL_BLEND.
	// GL_MODULATE - умножение на цвет от освещения
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture1);
}

void TextureEnd()
{
	glDeleteTextures(1, &texture1);
	glDisable(GL_TEXTURE_2D);
}

void Cilinder(double xc, double yc, double R, double h, char* tname, bool flipped = true, int shag = 50)
{
	MyPoint N = MyPoint(0, 0, 1);

	if (flipped == true)
	{
		glRotatef(90, 1, 0, 0);
	}


	double fs = 0;
	double ff = 2 * 3.149;
	double delta = (ff - fs) / shag;
	double f = fs;

	double xp = xc + R * cos(f);
	double yp = yc + R * sin(f);
	double xi, yi;


	double xpt = 0.5 + 0.5*cos(f);
	double ypt = 0.5 + 0.5*sin(f);
	double xit, yit;

	f = fs + delta;
	TextureStart(tname);
	for (int i = 0; i < shag; i++, f = f + delta)
	{
		xi = xc + R * cos(f);
		yi = yc + R * sin(f);

		xit = 0.5 + 0.5*cos(f);
		yit = 0.5 + 0.5*sin(f);



		glBegin(GL_TRIANGLES);
		//glColor3f(0.0f, 0.0f, 0.0f); 
		glNormal3fv(N);
		glTexCoord2f(0.5, 0.5);
		glVertex3f(xc, yc, 0);
		glTexCoord2f(xpt, ypt);
		glVertex3f(xp, yp, 0);
		glTexCoord2f(xit, yit);
		glVertex3f(xi, yi, 0);
		glEnd();

		glBegin(GL_TRIANGLES);
		//glColor3f(0.0f, 0.0f, 0.0f); 
		glNormal3fv(N);
		glTexCoord2f(0.5, 0.5);
		glVertex3f(xc, yc, h);
		glTexCoord2f(xpt, ypt);
		glVertex3f(xp, yp, h);
		glTexCoord2f(xit, yit);
		glVertex3f(xi, yi, h);
		glEnd();




		xp = xi;
		yp = yi;

		xpt = xit;
		ypt = yit;
	}
	TextureEnd();


	f = fs + delta;

	for (int i = 0; i < shag; i++, f = f + delta)
	{
		xi = xc + R * cos(f);
		yi = yc + R * sin(f);

		xit = 0.5 + 0.5*cos(f);
		yit = 0.5 + 0.5*sin(f);

		glBegin(GL_QUADS);
		glNormal3fv(N);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(xi, yi, 0);
		glVertex3f(xi, yi, h);
		glVertex3f(xp, yp, h);
		glVertex3f(xp, yp, 0);
		glEnd();

		xp = xi;
		yp = yi;

		xpt = xit;
		ypt = yit;
	}

	if (flipped == true)
	{
		glRotatef(-90, 0, 1, 0);
	}

	//xi = xc + 2 * cos(f); 
	//yi = yc + 2 * sin(f); 
}












double Ugol = 0;
bool up = 0;
int i, j;

bool addIce;
bool addAge;
bool resetDrink;
bool serve;

bool ShowWalls;

static void Resize(int width, int height);
static void Draw(void);

void  DrawModel(void);

struct Point
{
	float x;
	float y;
};

struct Vector3D {
	float x;
	float y;
	float z;
};


//лаба с прямыми ----------------------------------------------------------
Vector3D InitialData[4][4];
Vector3D line[100];
Vector3D grad;
Vector3D ort[4];
int time_scale;
int grad_coef;
Point min_cross;
Point max_cross;
int counter_of_chars;

int whichfont, thefont;


irrklang::ISoundEngine* se;

void InitMusic()
{
	char filepath[30];
	char number_of_song[3];

	se = irrklang::createIrrKlangDevice();
	if (!se)
			printf("Error: Could not create Sound Engine");


	/*for (int i = 0; i < 8; i++)
	{
		filepath[0] = '\0';
		strcat(filepath ,"jukebox_sound/");
		itoa(i+1, number_of_song, 10);
		strcat(filepath, number_of_song);
		strcat(filepath, ".mp3");
		songs[i][0] = '\0';
		strcat(songs[i], filepath);
	}*/
}


class shaker_containment
{
	int red;
	int yellow;
	int blue;
	int green;
	int white;
	bool isWithIce;
	bool isAged;
	bool isShaked;
	bool isShakedHard;

public:

	shaker_containment() {
		red = 0;
		yellow = 0;
		blue = 0;
		green = 0;
		white = 0;
		isWithIce = 0;
		isAged = 0;
		isShaked = 0;
		isShakedHard = 0;
	}

	void UpdateComponents(int index_of_bottle);

	void SetIce(bool cond);
	void SetAge(bool cond);
	void SetShake(bool cond);
	void SetHardShake(bool cond);

	void GetComponentsToShow(int comp[6]);

	int GetSumOfComponents();

	void GetAllComponentsToCheck(int comp[8]);

	void Reset();
};

void shaker_containment::GetComponentsToShow(int comp[6])
{
	comp[0] = red;
	comp[1] = blue;
	comp[2] = white;
	comp[3] = yellow;
	comp[4] = green;
	comp[5] = red + blue + white + yellow + green;
}

void shaker_containment::GetAllComponentsToCheck(int comp[8])
{
	comp[0] = red;
	comp[1] = yellow;
	comp[2] = blue;
	comp[3] = green;
	comp[4] = white;
	comp[5] = isWithIce;
	comp[6] = isAged;
	if (isShaked)
		comp[7] = 1;
	else if (isShakedHard)
		comp[7] = 2;
	else
		comp[7] = 0;
}

int shaker_containment::GetSumOfComponents()
{
	return red + blue + white + yellow + green;
}

void shaker_containment::UpdateComponents(int index_of_bottle)
{
	if (red + blue + white + yellow + green < 20)
		switch (index_of_bottle)
		{
		case 0:
			red++;
			break;
		case 1:
			blue++;
			break;
		case 2:
			white++;
			break;
		case 3:
			yellow++;
			break;
		case 4:
			green++;
			break;
		}
}

void shaker_containment::SetIce(bool cond)
{
	isWithIce = cond;
}
void shaker_containment::SetAge(bool cond)
{
	isAged = cond;
}
void shaker_containment::SetShake(bool cond)
{
	isShaked = cond;
}
void shaker_containment::SetHardShake(bool cond)
{
	isShakedHard = cond;
}

void shaker_containment::Reset() {
	red = 0;
	yellow = 0;
	blue = 0;
	green = 0;
	white = 0;
	isWithIce = 0;
	isAged = 0;
	isShaked = 0;
	isShakedHard = 0;
}

shaker_containment shaker_cont;


FTFont * mainTextFont, *charNameFont, *barFont,*titleFont;

void initFonts(void)
{
	mainTextFont = new FTGLBitmapFont("AnonymousPro-Regular.ttf");
	charNameFont = new FTGLBitmapFont("sinaloa_rus.ttf");
	barFont = new FTGLBitmapFont("AnonymousPro-Regular.ttf");
	titleFont = new FTGLBitmapFont("3447.ttf");

	if (!mainTextFont->FaceSize(18)) {
		printf("Failed to load main font\n");
		return;
	}
	if (!charNameFont->FaceSize(30)) {
		printf("Failed to load char font\n");
		return;
	}
	if (!barFont->FaceSize(11)) {
		printf("Failed to load bar font\n");
		return;
	}

	if (!titleFont->FaceSize(20)) {
		printf("Failed to load bar font\n");
		return;
	}
}

void DrawCharName(WCHAR* name)
{
	glPushMatrix();
	glRasterPos3f(24.1, 3, 13.5);
	glEnable(GL_BLEND);
	charNameFont->Render(name);
	glDisable(GL_BLEND);
	glPopMatrix();
}


void DrawTalk(WCHAR* text)
{

	glEnable(GL_BLEND);
	mainTextFont->Render(text);
	glDisable(GL_BLEND);
}



void DrawComplexTalk(WCHAR* text, int pivot)
{
	WCHAR line[60];
	float z = 13;
	int draw_i = 0;

	for (int i = 0; i < pivot; i++)
		if (text[i] == '\n')
		{
			if (text[i + 1] != '\0')
			{
				line[draw_i] = '\0';
				glPushMatrix();
				glRasterPos3f(24.1, 3, z);
				DrawTalk(line);
				glPopMatrix();
				z -= 0.2;
				draw_i = 0;
			}
		}
		else if (!(text[i] == '\n'  && text[i + 1] == '\0'))
		{
			line[draw_i] = text[i];
			draw_i++;
		}
	line[draw_i] = '\0';
	glPushMatrix();
	glRasterPos3f(24.1, 3, z);
	DrawTalk(line);
	glPopMatrix();
}




float GetDistance(Point a, Point b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}


struct speech
{
	char charName[10];
	char str[1010];
};

int current_chapter;
int current_line;
int count_of_lines_in_chapter;
speech* talks;
bool start_of_chapter;
WCHAR* Talker;
WCHAR* Talk;
bool isVNModeOn;



void PrepareTalk(char cyphered_speech[1000], speech& talk)
{
	bool isNameReaded = false;
	int i = 0;
	char reading_char = cyphered_speech[i];
	while (cyphered_speech[i] != '\0' && !isNameReaded)
	{
		if (cyphered_speech[i] == ']')
		{
			isNameReaded = true;
			talk.charName[i] = '\0';
		}
		else
			talk.charName[i] = cyphered_speech[i];
		i++;
	}
	ToLayoutFormat(cyphered_speech + sizeof(char)*i, talk.str, 51);
}

void ReadChapterFromFile(int chapter, speech* &talk)
{
	char filepath[20] = "script/ch", number_of_ch[2];
	char current_char, current_speech[1000];
	int i = 0, number_of_talk = 0;
	FILE* script_file;
	itoa(chapter, number_of_ch, 10);
	strcat(filepath, number_of_ch);
	strcat(filepath, ".txt");
	script_file = fopen(filepath, "rt");
	count_of_lines_in_chapter = 0;
	current_char = fgetc(script_file);
	while (!feof(script_file))
	{
		if (current_char != '\n')
		{
			current_speech[i] = current_char;
			i++;
			current_char = fgetc(script_file);
		}
		else
		{
			current_speech[i] = '\0';
			talk = (speech*)realloc(talk, (number_of_talk + 1) * sizeof(speech));
			PrepareTalk(current_speech, talk[number_of_talk]);
			number_of_talk++;
			i = 0;
			count_of_lines_in_chapter++;
			current_char = fgetc(script_file);
		}
	}
	current_speech[i] = '\0';
	printf("\n%s", current_speech);
	talk = (speech*)realloc(talk, (number_of_talk + 1) * sizeof(speech));
	PrepareTalk(current_speech, talk[number_of_talk]);
	count_of_lines_in_chapter++;
	fclose(script_file);
}



void EndChapter()
{
	free(talks);
	talks = NULL;
}



struct scriptline
{
	char function[30];
	char arguments[1010];
};



Vector3D getErmitPoint(float t, Vector3D P1, Vector3D P4, Vector3D R1, Vector3D R4)
{
	Vector3D Ermit;
	Ermit.x = P1.x*(2 * pow(t, 3) - 3 * pow(t, 2) + 1) + P4.x*(-2 * pow(t, 3) + 3 * pow(t, 2)) + R1.x*(pow(t, 3) - 2 * pow(t, 2) + t) + R4.x*(pow(t, 3) - pow(t, 2));
	Ermit.y = P1.y*(2 * pow(t, 3) - 3 * pow(t, 2) + 1) + P4.y*(-2 * pow(t, 3) + 3 * pow(t, 2)) + R1.y*(pow(t, 3) - 2 * pow(t, 2) + t) + R4.y*(pow(t, 3) - pow(t, 2));
	Ermit.z = P1.z*(2 * pow(t, 3) - 3 * pow(t, 2) + 1) + P4.z*(-2 * pow(t, 3) + 3 * pow(t, 2)) + R1.z*(pow(t, 3) - 2 * pow(t, 2) + t) + R4.z*(pow(t, 3) - pow(t, 2));
	return Ermit;
}

Vector3D getBezierPoint(float t, Vector3D P1, Vector3D P2, Vector3D P3, Vector3D P4)
{
	Vector3D Bezier;
	Bezier.x = pow(1 - t, 3)*P1.x + 3 * t*pow(1 - t, 2)*P2.x + 3 * pow(t, 2)*(1 - t)*P3.x + pow(t, 3)*P4.x;
	Bezier.y = pow(1 - t, 3)*P1.y + 3 * t*pow(1 - t, 2)*P2.y + 3 * pow(t, 2)*(1 - t)*P3.y + pow(t, 3)*P4.y;
	Bezier.z = pow(1 - t, 3)*P1.z + 3 * t*pow(1 - t, 2)*P2.z + 3 * pow(t, 2)*(1 - t)*P3.z + pow(t, 3)*P4.z;
	return Bezier;
}

Vector3D getTestPoint(float t)
{
	Vector3D result;
	result.x = t;
	result.y = pow(t, 2);
	result.z = pow(t, 2);
	return result;
}

void generateErmitLine(Vector3D *line, Vector3D P1, Vector3D P4, Vector3D R1, Vector3D R4)
{
	for (int i = 0; i < 100; i++)
	{
		line[i] = getErmitPoint(i*0.01, P1, P4, R1, R4);
	}
}

void generateBezierLine(Vector3D line[100], Vector3D P1, Vector3D P2, Vector3D P3, Vector3D P4)
{
	/*printf("P1:%f %f %f\n", P1.x, P1.y, P1.z);
	printf("P2:%f %f %f\n", P2.x, P2.y, P2.z);
	printf("P3:%f %f %f\n", P3.x, P3.y, P3.z);
	printf("P4:%f %f %f\n", P4.x, P4.y, P4.z);*/
	for (int i = 0; i < 100; i++)
	{
		line[i] = getBezierPoint(i*0.01, P1, P2, P3, P4);

	}
}

void generateTestLine(Vector3D *line)
{
	for (int i = 0; i < 100; i++)
	{
		line[i] = getTestPoint(i*0.1);
	}
}

void InitilizePoints()
{
	InitialData[0][0].x = 2.2; InitialData[0][0].y = 6.4; InitialData[0][0].z = 4.3;
	InitialData[0][1].x = 0.5; InitialData[0][1].y = 2.9; InitialData[0][1].z = 5;
	InitialData[0][2].x = 9.2; InitialData[0][2].y = 5.3; InitialData[0][2].z = 5;
	InitialData[0][3].x = 4.4; InitialData[0][3].y = 6.1; InitialData[0][3].z = 9.8;

	InitialData[1][0].x = 3.2; InitialData[1][0].y = 2.1; InitialData[1][0].z = 5.1;
	InitialData[1][1].x = 2; InitialData[1][1].y = 4.5; InitialData[1][1].z = 8.2;
	InitialData[1][2].x = 5.4; InitialData[1][2].y = 8.2; InitialData[1][2].z = 2.3;
	InitialData[1][3].x = 0.9; InitialData[1][3].y = 2.5; InitialData[1][3].z = 9.3;

	InitialData[2][0].x = 8.3; InitialData[2][0].y = 7.4; InitialData[2][0].z = 4.6;
	InitialData[2][1].x = 7.1; InitialData[2][1].y = 5.1; InitialData[2][1].z = 6.7;
	InitialData[2][2].x = -6.9; InitialData[2][2].y = 1.5; InitialData[2][2].z = -7.8;
	InitialData[2][3].x = -7.2; InitialData[2][3].y = -1.5; InitialData[2][3].z = 11.7;

	InitialData[3][0].x = 5; InitialData[3][0].y = 5.2; InitialData[3][0].z = 1.8;
	InitialData[3][1].x = 4.9; InitialData[3][1].y = 7.7; InitialData[3][1].z = 6.3;
	InitialData[3][2].x = 8.1; InitialData[3][2].y = 1.5; InitialData[3][2].z = -5.4;
	InitialData[3][3].x = 7.2; InitialData[3][3].y = 12; InitialData[3][3].z = 6.3;
}

Vector3D getGradient(Vector3D A, Vector3D B)
{
	Vector3D result;
	result.x = B.x - A.x;
	result.y = B.y - A.y;
	result.z = B.z - A.z;
	return result;
}

float GetScalarMult(Vector3D a, Vector3D b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

float GetVectorLen(Vector3D a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

float GetAngle(Vector3D a, Vector3D b)
{
	return acosf(GetScalarMult(a, b) / (GetVectorLen(a)*GetVectorLen(b)));
}

void GetCircle(Point points[360], Point center, float rad)
{
	float step = (2 * pi) / 360;
	for (int i = 0; i < 360; i++)
	{
		points[i].x = rad * cos(i*step) + center.x;
		points[i].y = rad * sin(i*step) + center.y;
	}
}

struct Point3D
{
	float x;
	float y;
	float z;
};

void GetCircleXZ(Point3D points[360], Point3D center, float rad)
{
	float step = (2 * pi) / 360;
	for (int i = 0; i < 360; i++)
	{
		points[i].x = rad * cos(i*step) + center.x;
		points[i].z = rad * sin(i*step) + center.z;
		points[i].y = center.y;
	}
}

void GetCircleYZ(Point3D points[360], Point3D center, float rad)
{
	float step = (2 * pi) / 360;
	for (int i = 0; i < 360; i++)
	{
		points[i].y = rad * cos(i*step) + center.y;
		points[i].z = rad * sin(i*step) + center.z;
		points[i].x = center.x;
	}
}

//лаба с текстурами -----------------------------------------------------



GLuint raw_texture_load(const char *filename, int width, int height)
{
	GLuint texture;
	unsigned char *data;
	FILE *file;

	// open texture data
	file = fopen(filename, "rb");
	if (file == NULL) return 0;

	// allocate buffer
	data = (unsigned char*)malloc(width * height * 4);

	// read texture data
	fread(data, width * height * 4, 1, file);
	fclose(file);

	// allocate a texture name
	glGenTextures(1, &texture);

	// select our current texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	// when texture area is large, bilinear filter the first mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// texture should tile
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// build our texture mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// free buffer
	free(data);

	return texture;
}

GLuint LoadTexture(const char * filename)
{

	GLuint texture;

	int width, height;

	unsigned char * data;

	FILE * file;

	file = fopen(filename, "rb");

	if (file == NULL) return 0;
	width = 1024;
	height = 512;
	data = (unsigned char *)malloc(width * height * 3);
	//int size = fseek(file,);
	fread(data, width * height * 3, 1, file);
	fclose(file);

	for (int i = 0; i < width * height; ++i)
	{
		int index = i * 3;
		unsigned char B, R;
		B = data[index];
		R = data[index + 2];

		data[index] = R;
		data[index + 2] = B;

	}


	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}


int gen_textures = 0;
GLuint texture[15];
GLuint texture_bar_wall;
GLuint texture_common_wall;



// функция задания свойств источников освещения


Point3D GetNormalVector(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	float ab_x = x2 - x1, ab_y = y2 - y1, ab_z = z2 - z1, ac_x = x3 - x1, ac_y = y3 - y1, ac_z = z3 - z1;
	Point3D normal_vector;
	normal_vector.x = ab_y * ac_z - ac_y * ab_z;
	normal_vector.y = ac_x * ab_z - ab_x * ac_z;
	normal_vector.z = ab_x * ac_y - ac_x * ab_y;
	return normal_vector;
}


void glaStencilWindow(int stencil)									//это поможет с впуклыми дугами-------------------------------------
{
	//Enable stenciling in the scene, and clear the stencil buffer
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);

	//Draw to the stencil mask when anything is drawn
	glStencilFunc(GL_ALWAYS, stencil, stencil);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	//Disable drawing to color or depth
	glColorMask(false, false, false, false);
	glDepthMask(false);
}

void glaStencilWall(int stencil)
{
	//Now enable drawing to color and depth
	glColorMask(true, true, true, true);
	glDepthMask(true);

	//Set the stencil function to fail when the first stencil mask is met
	//And this failure will result in the old value being kept
	glStencilFunc(GL_NOTEQUAL, stencil, stencil);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void glaStencilEnd()
{
	glDisable(GL_STENCIL_TEST);
}



typedef Point Vector;

Point circle_points[360];
Point circle_points_deck[9][360];
Point center_of_circle;
Point3D center_of_circle_3D;
Point3D circle_points_3D[360];


class ArcCustom
{
	Point start_of_arc;
	Point end_of_arc;
	float radius;

public:
	ArcCustom(float sx, float sy, float ex, float ey, float radius) :radius(radius)
	{
		start_of_arc.x = sx;
		start_of_arc.y = sy;
		end_of_arc.x = ex;
		end_of_arc.y = ey;
	}
	void GenerateArc(Point* &poi_of_arc, int& count_of_elems, bool convex);
};

void ArcCustom::GenerateArc(Point* &poi_of_arc, int& count_of_elems, bool convex = true)
{
	float len_of_chord = sqrt(pow(end_of_arc.x - start_of_arc.x, 2) + pow(end_of_arc.y - start_of_arc.y, 2));
	float distance_to_center = sqrt(pow(radius, 2) - pow(len_of_chord / 2, 2));
	Point center_of_chord;
	center_of_chord.x = (end_of_arc.x + start_of_arc.x) / 2;
	center_of_chord.y = (end_of_arc.y + start_of_arc.y) / 2;
	Vector eAB;
	eAB.x = (end_of_arc.x - center_of_chord.x) / (len_of_chord / 2);
	eAB.y = (end_of_arc.y - center_of_chord.y) / (len_of_chord / 2);
	Vector altAC;
	if (convex)
	{
		altAC.x = -eAB.y * distance_to_center;
		altAC.y = eAB.x * distance_to_center;
	}
	else
	{
		altAC.x = eAB.y * distance_to_center;
		altAC.y = -eAB.x * distance_to_center;
	}
	Point center_of_circle;
	center_of_circle.x = altAC.x + center_of_chord.x;
	center_of_circle.y = altAC.y + center_of_chord.y;
	float angle_of_start, angle_of_end;
	if (convex)
	{
		angle_of_start = atan2(start_of_arc.y - center_of_circle.y, start_of_arc.x - center_of_circle.x);
		angle_of_end = atan2(end_of_arc.y - center_of_circle.y, end_of_arc.x - center_of_circle.x);
	}
	else
	{
		angle_of_end = atan2(start_of_arc.y - center_of_circle.y, start_of_arc.x - center_of_circle.x);
		angle_of_start = atan2(end_of_arc.y - center_of_circle.y, end_of_arc.x - center_of_circle.x);
	}
	float current_angle = angle_of_start;
	poi_of_arc = (Point*)malloc((int((angle_of_end - angle_of_start) / 0.002) + 2) * sizeof(Point));
	poi_of_arc[0].x = start_of_arc.x;
	poi_of_arc[0].y = start_of_arc.y;
	int i = 1;
	while (current_angle < angle_of_end)
	{
		current_angle += 0.002;
		poi_of_arc[i].x = center_of_circle.x + radius * cos(current_angle);
		poi_of_arc[i].y = center_of_circle.y + radius * sin(current_angle);
		i++;
	}
	//poi_of_arc = (Point*)realloc(poi_of_arc, i * sizeof(Point));
	count_of_elems = i;
}




class drop
{
	float current_x;
	float current_y;
	float current_z;
	float radius_x;
	float radius_y;
	float angle;
	float parallel_height;
	float speed;
	int state;
	bool delete_it;			//c помощью этого проверяем нужно ли удалить
	float step;
	float radius_of_jukebox;
	float center_of_jukebox_y;

public:

	drop(float start_x, float start_y, float radius1, float radius2, float parallel_height, float center_of_jukebox_y) :current_x(start_x), current_y(start_y), radius_x(radius1), radius_y(radius2), parallel_height(parallel_height), center_of_jukebox_y(center_of_jukebox_y)
	{
		angle = pi;
		state = 1;
		speed = 0.5;
		current_z = 1;
		delete_it = false;
		step = pi / 180 * speed * 10;
		radius_of_jukebox = abs(start_y - center_of_jukebox_y);
	}
	void Move();

	bool IsDeletable();
};

void drop::Move()
{
	if (state == 3)
	{
		current_z -= speed;
		glPushMatrix();
		glTranslatef(current_x, current_y, current_z);
		glColor4d(248 / 255., 180 / 255., 148 / 255., 1);
		glScalef(radius_x, radius_y, 0.25);
		glutSolidSphere(1, 15, 15);
		glPopMatrix();
		if (current_z <= 1)
			delete_it = true;
	}
	if (state == 2)
	{
		angle -= step;
		current_y = radius_of_jukebox * cos(angle) + center_of_jukebox_y;
		current_z = radius_of_jukebox * sin(angle) + parallel_height;
		glPushMatrix();
		glTranslatef(current_x, current_y, current_z);
		glRotatef(angle * 180 / pi, 1, 0, 0);
		glColor4d(248 / 255., 180 / 255., 148 / 255., 1);
		glScalef(radius_x, radius_y, 0.25);
		glutSolidSphere(1, 15, 15);
		glPopMatrix();
		if (angle <= 0)
			state = 3;
	}
	if (state == 1)
	{
		current_z += speed;
		glPushMatrix();
		glTranslatef(current_x, current_y, current_z);
		glColor4d(248 / 255., 180 / 255., 148 / 255., 1);
		glScalef(radius_x, radius_y, 0.25);				//радиусы по x,y,z
		glutSolidSphere(1, 15, 15);
		glPopMatrix();
		if (current_z >= parallel_height)
			state = 2;
	}
}

bool drop::IsDeletable()
{
	return delete_it;
}

drop* array_of_drops;
float gacha_var[100];
int gacha_index;
int count_of_drops;
int i_dr;


bool unplug;
bool plug;
int current_disc;
int new_disc;
const float speed_of_spin = 5;
float direction_of_rotation_of_disc_automat;
float current_angle_of_disc_automat;
float new_angle;
bool isFirstTimeChangingDiscs;
bool isSameDiscPicked;
bool isJukeboxStopped;

class DiscHolder
{
	int serial_number;
	bool isDiscPlugged;							//установлен ли диск
	float DiscAngle;
	float angle;
	float lower_holder_height;

public:

	DiscHolder(int serial) :serial_number(serial)
	{
		isDiscPlugged = false;
		DiscAngle = 0;
		angle = 0;
		lower_holder_height = 0.3;
	}

	DiscHolder() {}

	void DrawDiscHolder(int& requested_holder, bool& plug, bool &unplug);
};
// функции обработки событий

//extern GLfloat alX=0, alY=0; // угол поворота
int mx0, mx1, my0, my1;

bool IsPointInsidePolygon(Point *p, int Number, float x, float y)
{
	float N, S, S1, S2, S3;
	int i1, i2, n;
	bool flag;
	N = Number;
	for (n = 0; n < N; n++)
	{
		flag = 0;
		i1 = n < N - 1 ? n + 1 : 0;
		while (flag == 0)
		{
			i2 = i1 + 1;
			if (i2 >= N)
				i2 = 0;
			if (i2 == (n < N - 1 ? n + 1 : 0))
				break;
			S = abs(p[i1].x * (p[i2].y - p[n].y) +
				p[i2].x * (p[n].y - p[i1].y) +
				p[n].x  * (p[i1].y - p[i2].y));
			S1 = abs(p[i1].x * (p[i2].y - y) +
				p[i2].x * (y - p[i1].y) +
				x * (p[i1].y - p[i2].y));
			S2 = abs(p[n].x * (p[i2].y - y) +
				p[i2].x * (y - p[n].y) +
				x * (p[n].y - p[i2].y));
			S3 = abs(p[i1].x * (p[n].y - y) +
				p[n].x * (y - p[i1].y) +
				x * (p[i1].y - p[n].y));
			if (S == S1 + S2 + S3)
			{
				flag = 1;
				break;
			}
			i1 = i1 + 1;
			if (i1 >= N)
				i1 = 0;
		}
		if (flag == 0)
			break;
	}
	return flag;
}

class Player
{
	Point3D position;
	bool isAbleToPlayPiano;
	bool isAbleToUseJukebox;
	bool isAbleToBartendering;
	bool isAbleToLeave;

	void CheckAbilities()
	{
		if (position.y <= -20 && position.y >= -30 && position.x >= 33 && position.x <= 36)
			isAbleToLeave = true;
		else
			isAbleToLeave = false;

		if (position.y <= -24 && position.y >= -30 && position.x >= -2 && position.x <= 8)
			isAbleToUseJukebox = true;
		else
			isAbleToUseJukebox = false;

		if (position.y <= -10 && position.y >= -16 && position.x >= 27 && position.x <= 36)
			isAbleToBartendering = true;
		else
			isAbleToBartendering = false;
		if (position.y <= -2.4*position.x - 50.4)
		{
			isAbleToPlayPiano = true;
			position.z = 2;
		}
		else
		{
			isAbleToPlayPiano = false;
			position.z = 0;
		}

		printf("%f %f\n", position.x, position.y);
	}

public:

	Player(float x, float y);

	Player() {}

	void DrawPlayer();

	void MovePlayer(int direction);

	bool GetAbilityToLeave();

	bool GetAbilityToBartendering();

	bool GetAbilityToUseJukebox();

	bool GetAbilityToPlayPiano();
};

bool Player::GetAbilityToLeave()
{
	return isAbleToLeave;
}

bool Player::GetAbilityToBartendering()
{
	return isAbleToBartendering;
}

bool Player::GetAbilityToUseJukebox()
{
	return isAbleToUseJukebox;
}

bool Player::GetAbilityToPlayPiano()
{
	return isAbleToPlayPiano;
}

Player::Player(float x, float y) {
	position.x = x;
	position.y = y;
	isAbleToLeave = true;
	isAbleToPlayPiano = false;
	isAbleToUseJukebox = false;
	isAbleToBartendering = false;
	position.z = 0;
}

void Player::MovePlayer(int direction)
{
	bool clip_flag = false;
	float borders[11][4] = { {-12,35,16.5,36},
	{1,12.5,11,16.5},
	{20,26.5,9.5,15},
	{22,28.5,-8.5,-0.5},
	{11.5,17.5,-3,4.5},
	{9,21,-12,-3},
	{1.5,8.5,-11,-4},
	{-36,-28,-2.5,9},
	{13.5,15,-34,-29},
	{14,21,-21.5,-17}
	};
	Point piano_points[7];
	piano_points[0].x = -19; piano_points[0].y = -7;
	piano_points[1].x = -11.5; piano_points[1].y = -30;
	piano_points[2].x = -19; piano_points[2].y = -30;
	piano_points[3].x = -32.5; piano_points[3].y = -21;
	piano_points[4].x = -34; piano_points[4].y = -13;
	piano_points[5].x = -24.5; piano_points[5].y = -9.5;
	piano_points[6].x = -19; piano_points[6].y = -7.5;
	Point piano_chair_points[4];
	piano_chair_points[0].x = -26; piano_chair_points[0].y = -4.5;
	piano_chair_points[1].x = -25; piano_chair_points[1].y = -7.5;
	piano_chair_points[2].x = -30.5; piano_chair_points[2].y = -10;
	piano_chair_points[3].x = -31.5; piano_chair_points[3].y = -6.5;
	Point monitor_1_points[4];
	monitor_1_points[0].x = -31; monitor_1_points[0].y = -29.5;
	monitor_1_points[1].x = -28; monitor_1_points[1].y = -27.5;
	monitor_1_points[2].x = -26.5; monitor_1_points[2].y = -34.5;
	monitor_1_points[3].x = -28; monitor_1_points[3].y = -34;
	switch (direction)
	{
	case GLUT_KEY_UP:
		for (int i = 0; i < 11; i++)
			if (position.y - 0.5 > borders[i][0] && position.y - 0.5 < borders[i][1] && position.x > borders[i][2] && position.x < borders[i][3])
			{
				clip_flag = true;
				printf("%d\n", i);
				break;
			}
		if (IsPointInsidePolygon(piano_points, 7, position.x, position.y - 0.5))
			clip_flag = true;
		if (IsPointInsidePolygon(piano_chair_points, 4, position.x, position.y - 0.5))
			clip_flag = true;
		if (IsPointInsidePolygon(monitor_1_points, 4, position.x, position.y - 0.5))
			clip_flag = true;
		if (position.y - 0.5 <= -35.5)
			clip_flag = true;
		if (!clip_flag)
			position.y -= 0.5;

		break;
	case GLUT_KEY_DOWN:
		for (int i = 0; i < 11; i++)
			if (position.y + 0.5 > borders[i][0] && position.y + 0.5 < borders[i][1] && position.x > borders[i][2] && position.x < borders[i][3])
			{
				printf("%d\n", i);
				clip_flag = true;
				break;
			}
		if (IsPointInsidePolygon(piano_points, 7, position.x, position.y + 0.5))
			clip_flag = true;
		if (IsPointInsidePolygon(piano_chair_points, 4, position.x, position.y + 0.5))
			clip_flag = true;
		if (IsPointInsidePolygon(monitor_1_points, 4, position.x, position.y + 0.5))
			clip_flag = true;
		if (position.y + 0.5 >= 35.5)
			clip_flag = true;
		if (!clip_flag)
			position.y += 0.5;
		break;
	case GLUT_KEY_RIGHT:
		for (int i = 0; i < 11; i++)
			if (position.y > borders[i][0] && position.y < borders[i][1] && position.x - 0.5 > borders[i][2] && position.x - 0.5 < borders[i][3])
			{
				printf("%d\n", i);
				clip_flag = true;
				break;
			}
		if (IsPointInsidePolygon(piano_points, 7, position.x - 0.5, position.y))
			clip_flag = true;
		if (IsPointInsidePolygon(piano_chair_points, 4, position.x - 0.5, position.y))
			clip_flag = true;
		if (IsPointInsidePolygon(monitor_1_points, 4, position.x - 0.5, position.y))
			clip_flag = true;
		if (position.x - 0.5 <= -35.5)
			clip_flag = true;
		if (!clip_flag)
			position.x -= 0.5;
		break;
	case GLUT_KEY_LEFT:
		for (int i = 0; i < 11; i++)
			if (position.y > borders[i][0] && position.y < borders[i][1] && position.x + 0.5 > borders[i][2] && position.x + 0.5 < borders[i][3])
			{
				printf("%d\n", i);
				clip_flag = true;
				break;
			}
		if (IsPointInsidePolygon(piano_points, 7, position.x + 0.5, position.y))
			clip_flag = true;
		if (IsPointInsidePolygon(piano_chair_points, 4, position.x + 0.5, position.y))
			clip_flag = true;
		if (IsPointInsidePolygon(monitor_1_points, 4, position.x + 0.5, position.y))
			clip_flag = true;
		if (position.x + 0.5 >= 35.5)
			clip_flag = true;
		if (!clip_flag)
			position.x += 0.5;
		break;
	}
	CheckAbilities();
}

void Player::DrawPlayer()
{
	glBegin(GL_QUAD_STRIP);
	if (isAbleToLeave || isAbleToBartendering || isAbleToPlayPiano || isAbleToUseJukebox)
		glColor3f(223 / 255., 133 / 255., 123 / 255.);
	else
		glColor3f(1, 1, 1);
	glVertex3f(-1 + position.x, -1 + position.y, 17 + position.z);
	glVertex3f(-1 + position.x, 1 + position.y, 17 + position.z);
	glVertex3f(1 + position.x, 1 + position.y, 17 + position.z);
	glVertex3f(1 + position.x, -1 + position.y, 17 + position.z);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	if (isAbleToLeave || isAbleToBartendering || isAbleToPlayPiano || isAbleToUseJukebox)
		glColor3f(223 / 255., 133 / 255., 123 / 255.);
	else
		glColor3f(1, 1, 1);
	glVertex3f(-1 + position.x, -1 + position.y, 17 + position.z);
	glVertex3f(-1 + position.x, 1 + position.y, 17 + position.z);
	glVertex3f(position.x, position.y, position.z);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	if (isAbleToLeave || isAbleToBartendering || isAbleToPlayPiano || isAbleToUseJukebox)
		glColor3f(223 / 255., 133 / 255., 123 / 255.);
	else
		glColor3f(1, 1, 1);
	glVertex3f(-1 + position.x, 1 + position.y, 17 + position.z);
	glVertex3f(1 + position.x, 1 + position.y, 17 + position.z);
	glVertex3f(position.x, position.y, position.z);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	if (isAbleToLeave || isAbleToBartendering || isAbleToPlayPiano || isAbleToUseJukebox)
		glColor3f(223 / 255., 133 / 255., 123 / 255.);
	else
		glColor3f(1, 1, 1);
	glVertex3f(1 + position.x, 1 + position.y, 17 + position.z);
	glVertex3f(1 + position.x, -1 + position.y, 17 + position.z);
	glVertex3f(position.x, position.y, position.z);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	if (isAbleToLeave || isAbleToBartendering || isAbleToPlayPiano || isAbleToUseJukebox)
		glColor3f(223 / 255., 133 / 255., 123 / 255.);
	else
		glColor3f(1, 1, 1);
	glVertex3f(-1 + position.x, -1 + position.y, 17 + position.z);
	glVertex3f(1 + position.x, -1 + position.y, 17 + position.z);
	glVertex3f(position.x, position.y, position.z);
	glEnd();
}



Player bartender;

struct drink
{
	char name[30];
	int red;
	int yellow;
	int blue;
	int green;
	int white;
	int ice;
	int age;
	int state;
	int OptKarmotrine;
};

drink drinktionary[30];
int len_of_drinktionary;

void ReadDrinktionaryFromFile(drink drinktionary[30], int* len_of_drinktionary)
{
	FILE* f = fopen("drinktionary.txt", "rt");
	int i = 0;
	while (!feof(f))
	{
		fscanf(f, "%s %d %d %d %d %d %d %d %d %d", drinktionary[i].name, &drinktionary[i].red,
			&drinktionary[i].yellow, &drinktionary[i].blue,
			&drinktionary[i].green, &drinktionary[i].white,
			&drinktionary[i].ice, &drinktionary[i].age,
			&drinktionary[i].state, &drinktionary[i].OptKarmotrine);
		for (int j = 0; j < strlen(drinktionary[i].name); j++)
			if (drinktionary[i].name[j] == '_')
				drinktionary[i].name[j] = ' ';
		i++;
	}
	*len_of_drinktionary = i;
	fclose(f);
}


class button
{
	Point3D position;
	float width;
	float height;
	bool* value;
	float color_passive[3];
	float color_active[3];
	Point hitbox[4];

public:

	button() {}

	button(float x, float y, float z, float width, float height, bool* value, float color_p[3], float color_a[3], Point hitbox_of_button[4]) :width(width), height(height), value(value)
	{
		for (int i = 0; i < 3; i++)
		{
			color_passive[i] = color_p[i];
			color_active[i] = color_a[i];
		}
		for (int i = 0; i < 4; i++)
			hitbox[i] = hitbox_of_button[i];
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void DrawButton();

	void isClicked(int x, int y);
};

void button::DrawButton()
{
	glBegin(GL_QUAD_STRIP);
	if (*value)
		glColor4d(color_active[0], color_active[1], color_active[2], 0.8);
	else
		glColor4d(color_passive[0], color_passive[1], color_passive[1], 0.8);
	glVertex3f(position.x, position.y, position.z);
	glVertex3f(position.x, position.y, position.z + height);
	glVertex3f(position.x, position.y + width, position.z);
	glVertex3f(position.x, position.y + width, position.z + height);
	glEnd();
}

void button::isClicked(int x, int y)
{
	if (IsPointInsidePolygon(hitbox, 4, x, y))
		if (*value)
			*value = false;
		else
			*value = true;
}


button IceButton;
button AgeButton;
button EraseButton;
button ServeButton;


int current_octave = 0;
int current_key = -1;

void PianoKeyboard(char key)
{
	current_key = tolower(key);
}

void ChangeOctave(char key)
{
	if (key == '-')
		if (current_octave != -4)
			current_octave--;
		else
			current_octave = 4;
	if (key == '=')
		if (current_octave != 4)
			current_octave++;
		else
			current_octave = -4;
}

void JukeboxDiscChange(char key)
{
	new_disc = key - '0';
	new_angle = (new_disc - 1) * 45;
	if (current_angle_of_disc_automat != new_angle)
		if (abs(current_angle_of_disc_automat - new_angle) <= 180)
		{
			printf("first\n");
			direction_of_rotation_of_disc_automat = (current_angle_of_disc_automat - new_angle > 0 ? -1 : 1);
		}
		else
		{
			printf("second\n");
			direction_of_rotation_of_disc_automat = (current_angle_of_disc_automat - new_angle > 0 ? 1 : -1);
		}
	else
		isSameDiscPicked = true;

	if (isFirstTimeChangingDiscs)
		isFirstTimeChangingDiscs = false;
	else
		unplug = true;
	plug = false;
}

void JukeboxBreak()
{
	unplug = true;
	isJukeboxStopped = true;
}

void DrawBottleType1(float footer_height, float color[3])
{
	Point header_circle_points[360];

	center_of_circle.x = 0;										//нижняя часть
	center_of_circle.y = -0.5;
	GetCircle(circle_points, center_of_circle, 0.5);
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color[0], color[1], color[2]);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(color[0], color[1], color[2]);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glEnd();

	GetCircle(header_circle_points, center_of_circle, 0.1);						//средняя часть

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 3.2);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 3.2);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 3.2);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 3.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glEnd();


	for (int i = 0; i < 359; i++)										//пробка
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(0, 0, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 3.4);
		glVertex3f(center_of_circle.x, center_of_circle.y, 3.4);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 3.4);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(0, 0, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 3.4);
	glVertex3f(center_of_circle.x, center_of_circle.y, 3.4);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 3.4);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 3.4);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 3.2);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 3.4);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 3.2);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glNormal3f(-2, 4, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 3.4);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 3.2);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 3.4);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 3.2);
	glEnd();
}

void DrawBottleType2(float footer_height, float color[3])
{
	glBegin(GL_QUAD_STRIP);					//нижняя часть
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -1, 0);
	glVertex3f(0.5, -1, 0);
	glVertex3f(0.5, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -1, 0);
	glVertex3f(0, 0, footer_height);
	glVertex3f(0, -1, footer_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -1, 0);
	glVertex3f(0.5, 0, 0);
	glVertex3f(0.5, -1, footer_height);
	glVertex3f(0.5, 0, footer_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -1, 0);
	glVertex3f(0.5, -1, 0);
	glVertex3f(0, -1, footer_height);
	glVertex3f(0.5, -1, footer_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0.5, 0, 0);
	glVertex3f(0, 0, footer_height);
	glVertex3f(0.5, 0, footer_height);
	glEnd();

	ArcCustom concave = ArcCustom(0, 0, 0.4, -0.4, 0.8);						//изгиб
	Point* concave_points = NULL;
	int count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave, false);

	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_TRIANGLE_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(0, 0, 1);
		glVertex3f(0, -0.5, footer_height);
		glVertex3f(0, -concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0, -concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glEnd();


	}

	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_TRIANGLE_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(0, 0, 1);
		glVertex3f(0.5, -0.5, footer_height);
		glVertex3f(0.5, -concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0.5, -concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glEnd();
	}

	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(0, 0, 1);
		glVertex3f(0, -concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0, -concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glVertex3f(0.5, -concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0.5, -concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glEnd();
	}

	for (i = 0; i < count_of_elems_concave - 1; i++)						//в другую сторону
	{

		glBegin(GL_TRIANGLE_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(0, 0, 1);
		glVertex3f(0, -0.5, footer_height);
		glVertex3f(0, -1 + concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0, -1 + concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glEnd();


	}

	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_TRIANGLE_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(0, 0, 1);
		glVertex3f(0.5, -0.5, footer_height);
		glVertex3f(0.5, -1 + concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0.5, -1 + concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glEnd();
	}

	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(0, 0, 1);
		glVertex3f(0, -1 + concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0, -1 + concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glVertex3f(0.5, -1 + concave_points[i].x, -concave_points[i].y + footer_height);
		glVertex3f(0.5, -1 + concave_points[i + 1].x, -concave_points[i + 1].y + footer_height);
		glEnd();
	}

	free(concave_points);

	glBegin(GL_QUAD_STRIP);					//горло
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.4, footer_height);
	glVertex3f(0, -0.4, footer_height + 0.6);
	glVertex3f(0, -0.6, footer_height);
	glVertex3f(0, -0.6, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -0.4, footer_height);
	glVertex3f(0.5, -0.4, footer_height + 0.6);
	glVertex3f(0.5, -0.6, footer_height);
	glVertex3f(0.5, -0.6, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -0.6, footer_height);
	glVertex3f(0.5, -0.6, footer_height + 0.6);
	glVertex3f(0, -0.6, footer_height);
	glVertex3f(0, -0.6, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.4, footer_height);
	glVertex3f(0, -0.4, footer_height + 0.6);
	glVertex3f(0.5, -0.4, footer_height);
	glVertex3f(0.5, -0.4, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.4, footer_height + 0.6);
	glVertex3f(0, -0.6, footer_height + 0.6);
	glVertex3f(0.5, -0.4, footer_height + 0.6);
	glVertex3f(0.5, -0.6, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//горло
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.4, footer_height);
	glVertex3f(0, -0.4, footer_height + 0.6);
	glVertex3f(0, -0.6, footer_height);
	glVertex3f(0, -0.6, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -0.4, footer_height);
	glVertex3f(0.5, -0.4, footer_height + 0.6);
	glVertex3f(0.5, -0.6, footer_height);
	glVertex3f(0.5, -0.6, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -0.6, footer_height);
	glVertex3f(0.5, -0.6, footer_height + 0.6);
	glVertex3f(0, -0.6, footer_height);
	glVertex3f(0, -0.6, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.4, footer_height);
	glVertex3f(0, -0.4, footer_height + 0.6);
	glVertex3f(0.5, -0.4, footer_height);
	glVertex3f(0.5, -0.4, footer_height + 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.4, footer_height + 0.6);
	glVertex3f(0, -0.6, footer_height + 0.6);
	glVertex3f(0.5, -0.4, footer_height + 0.6);
	glVertex3f(0.5, -0.6, footer_height + 0.6);
	glEnd();


}

void DrawBottleType3(float footer_height, float color[3])
{
	Point header_circle_points[360];

	glPushMatrix();									//средняя часть
	glTranslatef(0, -0.5, footer_height);
	glScalef(0.5, 0.5, footer_height / 2);
	glColor3d(color[0], color[1], color[2]);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();

	center_of_circle.x = 0;										//нижняя часть
	center_of_circle.y = -0.5;
	GetCircle(circle_points, center_of_circle, 0.5);
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color[0], color[1], color[2]);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(color[0], color[1], color[2]);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	GetCircle(header_circle_points, center_of_circle, 0.1);


	for (int i = 0; i < 359; i++)								//верхняя часть
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(0, 0, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, footer_height * 3 / 2. + 0.3);
		glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3 / 2. + 0.3);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, footer_height * 3 / 2. + 0.3);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(0, 0, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, footer_height * 3 / 2. + 0.3);
	glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3 / 2. + 0.3);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, footer_height * 3 / 2. + 0.3);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, footer_height * 3 / 2. + 0.1);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 0);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, footer_height * 3 / 2. + 0.1);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, footer_height * 3 / 2. + 0.1);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 0);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, footer_height * 3 / 2. + 0.1);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, footer_height * 3 / 2. + 0.1);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, footer_height * 3 / 2. + 0.3);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, footer_height * 3 / 2. + 0.1);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, footer_height * 3 / 2. + 0.3);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glNormal3f(-2, 4, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, footer_height * 3 / 2. + 0.1);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, footer_height * 3 / 2. + 0.3);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, footer_height * 3 / 2. + 0.1);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, footer_height * 3 / 2. + 0.3);
	glEnd();

}

void GetOGLPos(Point3D& world_coords, int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble worldX, worldY, worldZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);

	world_coords.x = worldX;
	world_coords.y = worldY;
	world_coords.z = worldZ;
}

class active_bottle
{
protected:
	Point3D initial_position;
	Point3D current_position;
	float initial_angle;
	float current_angle;
	int type;
	float height;
	float color_of_bottle[3];
	bool isBottleCatched;

	Point hitbox_of_bottle[10];
	int count_of_points_in_hitbox;

	float true_height;

public:
	active_bottle() {}

	active_bottle(float x, float y, float z, float angle, int type, float height, float color_r, float color_g, float color_b, Point hitbox[10], int count_of_points, float true_height) : initial_angle(angle), type(type), height(height), true_height(true_height)
	{
		initial_position.x = x;
		initial_position.y = y;
		initial_position.z = z;
		current_position.x = x;
		current_position.y = y;
		current_position.z = z;
		color_of_bottle[0] = color_r;
		color_of_bottle[1] = color_g;
		color_of_bottle[2] = color_b;
		isBottleCatched = false;
		count_of_points_in_hitbox = count_of_points;
		for (int i = 0; i < count_of_points_in_hitbox; i++)
			hitbox_of_bottle[i] = hitbox[i];
	}

	void DrawActiveBottle();

	void CheckIfClickedOnBottle(int x, int y);

	virtual void ChangePosition(int mouse_x, int mouse_y);

	void ChangeAngle(int mouse_initial_x, int mouse_initial_y, int mouse_current_x, int mouse_current_y);

	void ReturnToInitialAngle();

	bool isBottleActive();

	float GetCurrentAngle();

	float GetTrueHeight();

	Point3D GetCurrentPosition();

	Point3D GetInitialPosition();

	void ReturnToInitialPosition();
};

void active_bottle::DrawActiveBottle()
{
	glPushMatrix();
	if (isBottleCatched)
	{
		glTranslatef(current_position.x, current_position.y, current_position.z);
		glRotatef(current_angle, 1, 0, 0);
	}
	else
		glTranslatef(initial_position.x, initial_position.y, initial_position.z);
	switch (type)
	{
	case 1:
		glPushMatrix();
		glScalef(0.5, 0.5, 0.5);
		DrawBottleType1(height, color_of_bottle);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glScalef(0.5, 0.5, 0.5);
		DrawBottleType2(height, color_of_bottle);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glScalef(0.5, 0.5, 0.5);
		DrawBottleType3(height, color_of_bottle);
		glPopMatrix();
		break;
	}
	glPopMatrix();
}

bool isLeftButtonPressed, isLeftButtonJustReleased;

void active_bottle::CheckIfClickedOnBottle(int x, int y)
{
	if (!isLeftButtonPressed)
	{
		isBottleCatched = IsPointInsidePolygon(hitbox_of_bottle, count_of_points_in_hitbox, x, y);
		printf(isBottleCatched ? "catch" : "fali");
		if (isBottleCatched)
			current_angle = initial_angle;
	}
}

void active_bottle::ReturnToInitialPosition()
{
	current_position.x = initial_position.x;
	current_position.y = initial_position.y;
	current_position.z = initial_position.z;
	isBottleCatched = false;
}

bool active_bottle::isBottleActive()
{
	return isBottleCatched;
}

void active_bottle::ChangePosition(int mouse_x, int mouse_y)
{
	Point3D world_coords;
	GetOGLPos(world_coords, mouse_x, mouse_y);
	current_position.x = world_coords.x;
	if (current_position.x >= 22.5)
		current_position.x = 22.5;
	current_position.y = world_coords.y;
	current_position.z = world_coords.z;
	printf("%f %f\n", current_position.y, current_position.z);
}

void active_bottle::ChangeAngle(int mouse_initial_x, int mouse_initial_y, int mouse_current_x, int mouse_current_y)
{
	float distantion = sqrt(pow(mouse_initial_x - mouse_current_x, 2) + pow(mouse_initial_y - mouse_current_y, 2));
	if (distantion > 300)
		distantion = 300;
	if (mouse_initial_x < mouse_current_x)
		current_angle = -180 * distantion / 300.;
	else
		current_angle = 180 * distantion / 300.;
}

void active_bottle::ReturnToInitialAngle()
{
	current_angle = initial_angle;
}

float active_bottle::GetCurrentAngle()
{
	return current_angle;
}

float active_bottle::GetTrueHeight()
{
	return true_height;
}

Point3D active_bottle::GetCurrentPosition()
{
	return current_position;
}

Point3D active_bottle::GetInitialPosition()
{
	return initial_position;
}

class alco_drop
{
	Point3D position;

public:
	alco_drop() {}

	alco_drop(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void DrawAlcoDrop();
};

void alco_drop::DrawAlcoDrop()
{
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glColor3d(1, 0, 0);
	glutSolidSphere(0.03, 10, 10);
	glPopMatrix();

	position.z -= 2;					//временно
}

class active_shaker :public active_bottle
{
	float height;
	float total_distance;
	int state;
	Point old_position;
	Point new_position;
	int index_of_drink;

public:
	active_shaker() {}

	active_shaker(float x, float y, float z, float height, float angle, Point hitbox[10], int count_of_points) :height(height)
	{
		active_bottle::initial_position.x = x;
		active_bottle::initial_position.y = y;
		active_bottle::initial_position.z = z;
		active_bottle::initial_angle = angle;
		active_bottle::count_of_points_in_hitbox = count_of_points;
		active_bottle::isBottleCatched = false;
		active_bottle::current_position.x = x;
		total_distance = 0;
		state = 0;
		index_of_drink = -1;
		for (int i = 0; i < count_of_points_in_hitbox; i++)
			active_bottle::hitbox_of_bottle[i] = hitbox[i];
	}

	void DrawActiveShaker();

	void StartCountingPath(int x, int y);

	void ChangeState(int x, int y);

	void ChangePosition(int mouse_x, int mouse_y);

	void TryToDetermineDrink(drink drinktionary[30], int len_of_drinktionary);

	void Reset();

	int GetResultDrinkIndex();
};

int active_shaker::GetResultDrinkIndex()
{
	return index_of_drink;
}

void active_shaker::Reset()
{
	index_of_drink = -1;
	total_distance = 0;
	state = 0;
}

void active_shaker::StartCountingPath(int x, int y)
{
	new_position.x = x;
	new_position.y = y;
}

void active_shaker::TryToDetermineDrink(drink drinktionary[30], int len_of_drinktionary)
{
	int statement_of_shaker[8];
	shaker_cont.GetAllComponentsToCheck(statement_of_shaker);
	index_of_drink = -1;
	for (int i = 0; i < len_of_drinktionary; i++)
	{
		//printf("\n%d\n", statement_of_shaker[7]);
		if (statement_of_shaker[0] == drinktionary[i].red &&
			statement_of_shaker[1] == drinktionary[i].yellow &&
			statement_of_shaker[2] == drinktionary[i].blue &&
			statement_of_shaker[3] == drinktionary[i].green &&
			statement_of_shaker[5] == drinktionary[i].ice &&
			statement_of_shaker[6] == drinktionary[i].age &&
			statement_of_shaker[7] == drinktionary[i].state)
		{
			if (!drinktionary[i].OptKarmotrine)
			{
				if (statement_of_shaker[3] == drinktionary[i].green)
				{
					index_of_drink = i;
					break;
				}
			}
			else
			{
				index_of_drink = i;
				break;
			}
		}
	}
}

void active_shaker::ChangePosition(int mouse_x, int mouse_y)
{
	Point3D world_coords;
	GetOGLPos(world_coords, mouse_x, mouse_y);
	current_position.x = world_coords.x;
	if (current_position.x >= 22.5)
		current_position.x = 22.5;
	current_position.y = world_coords.y;
	current_position.z = world_coords.z;
	printf("%f %f\n", current_position.y, current_position.z);

	ChangeState(mouse_x, mouse_y);
	TryToDetermineDrink(drinktionary, len_of_drinktionary);
}


void active_shaker::ChangeState(int x, int y)
{
	old_position = new_position;
	new_position.x = x;
	new_position.y = y;
	total_distance += GetDistance(old_position, new_position);
	printf("%f\n %d", total_distance, state);
	if (total_distance > 4000)
		state = 2;
	else if (total_distance > 1800)
		state = 1;
	switch (state)
	{
	case 0:
		shaker_cont.SetShake(0);
		shaker_cont.SetHardShake(0);
		break;
	case 1:
		shaker_cont.SetShake(1);
		shaker_cont.SetHardShake(0);
		break;
	case 2:
		shaker_cont.SetShake(0);
		shaker_cont.SetHardShake(1);
		break;
	}
}



void active_shaker::DrawActiveShaker()
{
	Point bottom_circle_points[360];
	Point header_circle_points[360];
	Point cap_circle_points[360];



	Point3D position = active_bottle::isBottleCatched ? active_bottle::current_position : active_bottle::initial_position;
	float indicator_color[3];
	switch (state)
	{
	case 0:
		indicator_color[0] = 78 / 255.;
		indicator_color[1] = 67 / 255.;
		indicator_color[2] = 71 / 255.;
		break;
	case 1:
		indicator_color[0] = 234 / 255.;
		indicator_color[1] = 128 / 255.;
		indicator_color[2] = 107 / 255.;
		break;
	case 2:
		indicator_color[0] = 1;
		indicator_color[1] = 0;
		indicator_color[2] = 0;
		break;
	}

	glPushMatrix();
	if (index_of_drink != -1)
		glColor4d(234 / 255., 128 / 255., 107 / 255., 1);
	else
		glColor4d(78 / 255., 67 / 255., 71 / 255., 1);
	glRasterPos3f(position.x, position.y - 1, position.z + 2.3);
	if (index_of_drink == -1)
		barFont->Render("???");
	else
		barFont->Render(drinktionary[index_of_drink].name);
	glPopMatrix();


	glPushMatrix();
	if (isBottleCatched)
	{
		glTranslatef(position.x, position.y, position.z);
		glRotatef(current_angle, 1, 0, 0);
		glTranslatef(-position.x, -position.y, -position.z);
	}

	if (isBottleCatched)
	{
		glPushMatrix();									//средняя часть
		glTranslatef(position.x, position.y - 0.5, height + position.z);
		glScalef(0.4, 0.4, height / 2 - 0.2);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glutSolidSphere(1, 10, 10);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();									//средняя часть (но когда шейкер неактивен
		glTranslatef(position.x - 2, position.y - 1.5, position.z);
		glScalef(0.4, 0.4, height / 2 - 0.2);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glutSolidSphere(1, 10, 10);
		glPopMatrix();
	}


	glPushMatrix();
	glTranslatef(0, 0, position.z);

	center_of_circle.x = position.x;										//нижняя часть
	center_of_circle.y = position.y - 0.5;
	GetCircle(circle_points, center_of_circle, 0.4);
	GetCircle(bottom_circle_points, center_of_circle, 0.3);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(bottom_circle_points[359].x, bottom_circle_points[359].y, 0);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, height);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, height);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, height);
	glVertex3f(bottom_circle_points[359].x, circle_points[359].y, 0);
	glEnd();



	if (isBottleCatched)
	{
		GetCircle(header_circle_points, center_of_circle, 0.1);

		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(-2, 4, 0);
			glColor3d(indicator_color[0], indicator_color[1], indicator_color[2]);
			glVertex3f(circle_points[i].x, circle_points[i].y, height);
			glVertex3f(center_of_circle.x, center_of_circle.y, height);
			glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[0].x, circle_points[0].y, height);
		glVertex3f(center_of_circle.x, center_of_circle.y, height);
		glVertex3f(circle_points[359].x, circle_points[359].y, height);
		glEnd();

		for (int i = 0; i < 359; i++)					//верхняя часть
		{
			glBegin(GL_QUAD_STRIP);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glNormal3f(-2, 4, 0);
			glVertex3f(header_circle_points[i].x, header_circle_points[i].y, height * 3 / 2.);
			glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 0);
			glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, height * 3 / 2.);
			glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 0);
			glEnd();
		}

		glBegin(GL_QUAD_STRIP);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[0].x, header_circle_points[0].y, height * 3 / 2.);
		glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 0);
		glVertex3f(header_circle_points[359].x, header_circle_points[359].y, height * 3 / 2.);
		glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 0);
		glEnd();

		GetCircle(cap_circle_points, center_of_circle, 0.12);


		for (int i = 0; i < 359; i++)					//крышка
		{
			glBegin(GL_QUAD_STRIP);
			glColor3d(indicator_color[0], indicator_color[1], indicator_color[2]);
			glNormal3f(-2, 4, 0);
			glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, height * 3 / 2.);
			glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, height * 3 / 2. - 0.1);
			glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, height * 3 / 2.);
			glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, height * 3 / 2. - 0.1);
			glEnd();
		}

		glBegin(GL_QUAD_STRIP);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, height * 3 / 2.);
		glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, height * 3 / 2. - 0.1);
		glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, height * 3 / 2.);
		glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, height * 3 / 2. - 0.1);
		glEnd();


		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(-2, 4, 0);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, height * 3 / 2.);
			glVertex3f(center_of_circle.x, center_of_circle.y, height * 3 / 2.);
			glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, height * 3 / 2.);
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, height * 3 / 2.);
		glVertex3f(center_of_circle.x, center_of_circle.y, height * 3 / 2.);
		glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, height * 3 / 2.);
		glEnd();


		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(-2, 4, 0);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, height * 3 / 2. - 0.1);
			glVertex3f(center_of_circle.x, center_of_circle.y, height * 3 / 2. - 0.1);
			glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, height * 3 / 2. - 0.1);
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, height * 3 / 2. - 0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, height * 3 / 2. - 0.1);
		glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, height * 3 / 2. - 0.1);
		glEnd();
	}
	else
	{
		GetCircle(header_circle_points, center_of_circle, 0.1);

		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(-2, 4, 0);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glVertex3f(circle_points[i].x - 2, circle_points[i].y - 1, 0);
			glVertex3f(center_of_circle.x - 2, center_of_circle.y - 1, 0);
			glVertex3f(circle_points[i + 1].x - 2, circle_points[i + 1].y - 1, 0);
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[0].x - 2, circle_points[0].y - 1, 0);
		glVertex3f(center_of_circle.x - 2, center_of_circle.y - 1, 0);
		glVertex3f(circle_points[359].x - 2, circle_points[359].y - 1, 0);
		glEnd();

		for (int i = 0; i < 359; i++)					//верхняя часть
		{
			glBegin(GL_QUAD_STRIP);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glNormal3f(-2, 4, 0);
			glVertex3f(header_circle_points[i].x - 2, header_circle_points[i].y - 1, height * 1 / 2.);
			glVertex3f(header_circle_points[i].x - 2, header_circle_points[i].y - 1, 0);
			glVertex3f(header_circle_points[i + 1].x - 2, header_circle_points[i + 1].y - 1, height * 1 / 2.);
			glVertex3f(header_circle_points[i + 1].x - 2, header_circle_points[i + 1].y - 1, 0);
			glEnd();
		}

		glBegin(GL_QUAD_STRIP);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[0].x - 2, header_circle_points[0].y - 1, height * 1 / 2.);
		glVertex3f(header_circle_points[0].x - 2, header_circle_points[0].y - 1, 0);
		glVertex3f(header_circle_points[359].x - 2, header_circle_points[359].y - 1, height * 1 / 2.);
		glVertex3f(header_circle_points[359].x - 2, header_circle_points[359].y - 1, 0);
		glEnd();

		GetCircle(cap_circle_points, center_of_circle, 0.12);


		for (int i = 0; i < 359; i++)					//крышка
		{
			glBegin(GL_QUAD_STRIP);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glNormal3f(-2, 4, 0);
			glVertex3f(cap_circle_points[i].x - 2, cap_circle_points[i].y - 1, height * 1 / 2.);
			glVertex3f(cap_circle_points[i].x - 2, cap_circle_points[i].y - 1, height * 1 / 2. - 0.1);
			glVertex3f(cap_circle_points[i + 1].x - 2, cap_circle_points[i + 1].y - 1, height * 1 / 2.);
			glVertex3f(cap_circle_points[i + 1].x - 2, cap_circle_points[i + 1].y - 1, height * 1 / 2. - 0.1);
			glEnd();
		}

		glBegin(GL_QUAD_STRIP);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(cap_circle_points[0].x - 2, cap_circle_points[0].y - 1, height * 1 / 2.);
		glVertex3f(cap_circle_points[0].x - 2, cap_circle_points[0].y - 1, height * 1 / 2. - 0.1);
		glVertex3f(cap_circle_points[359].x - 2, cap_circle_points[359].y - 1, height * 1 / 2.);
		glVertex3f(cap_circle_points[359].x - 2, cap_circle_points[359].y - 1, height * 1 / 2. - 0.1);
		glEnd();


		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(-2, 4, 0);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glVertex3f(cap_circle_points[i].x - 2, cap_circle_points[i].y - 1, height * 1 / 2.);
			glVertex3f(center_of_circle.x - 2, center_of_circle.y - 1, height * 1 / 2.);
			glVertex3f(cap_circle_points[i + 1].x - 2, cap_circle_points[i + 1].y - 1, height * 1 / 2.);
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(cap_circle_points[0].x - 2, cap_circle_points[0].y - 1, height * 1 / 2.);
		glVertex3f(center_of_circle.x - 2, center_of_circle.y - 1, height * 1 / 2.);
		glVertex3f(cap_circle_points[359].x - 2, cap_circle_points[359].y - 1, height * 1 / 2.);
		glEnd();


		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(-2, 4, 0);
			glColor3d(78 / 255., 67 / 255., 71 / 255.);
			glVertex3f(cap_circle_points[i].x - 2, cap_circle_points[i].y - 1, height * 1 / 2. - 0.1);
			glVertex3f(center_of_circle.x - 2, center_of_circle.y - 1, height * 1 / 2. - 0.1);
			glVertex3f(cap_circle_points[i + 1].x - 2, cap_circle_points[i + 1].y - 1, height * 1 / 2. - 0.1);
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(cap_circle_points[0].x - 2, cap_circle_points[0].y - 1, height * 1 / 2. - 0.1);
		glVertex3f(center_of_circle.x - 2, center_of_circle.y - 1, height * 1 / 2. - 0.1);
		glVertex3f(cap_circle_points[359].x - 2, cap_circle_points[359].y - 1, height * 1 / 2. - 0.1);
		glEnd();
	}
	glPopMatrix();
	glPopMatrix();
}




class active_glass :public active_bottle
{
	float height;


	float color_of_drink[3];



public:
	bool isFilled;

	active_glass() {}

	active_glass(float x, float y, float z, float height, float angle, Point hitbox[10], int count_of_points) :height(height)
	{
		active_bottle::initial_position.x = x;
		active_bottle::initial_position.y = y;
		active_bottle::initial_position.z = z;
		active_bottle::initial_angle = angle;
		active_bottle::count_of_points_in_hitbox = count_of_points;
		active_bottle::isBottleCatched = false;
		active_bottle::current_position.x = x;
		isFilled = false;
		for (int i = 0; i < count_of_points_in_hitbox; i++)
			active_bottle::hitbox_of_bottle[i] = hitbox[i];
	}

	void DrawActiveGlass();

	void CalculateColorOfDrink();
};

void active_glass::CalculateColorOfDrink()
{
	int shaker_components[6];
	float count_of_components;
	shaker_cont.GetComponentsToShow(shaker_components);
	color_of_drink[0] = (float)(shaker_components[0] + shaker_components[2] + shaker_components[3]) / shaker_components[5];
	color_of_drink[1] = (float)(shaker_components[2] + shaker_components[3] + shaker_components[4]) / shaker_components[5];
	color_of_drink[2] = (float)(shaker_components[1]) / shaker_components[5];
}

void active_glass::DrawActiveGlass()
{

	Point3D position = active_bottle::isBottleCatched ? active_bottle::current_position : active_bottle::initial_position;




	glPushMatrix();
	if (isBottleCatched)
	{
		glTranslatef(position.x, position.y, position.z);
		glRotatef(current_angle, 1, 0, 0);
		glTranslatef(-position.x, -position.y, -position.z);
	}

	glPushMatrix();
	glTranslatef(0, 0, position.z);

	center_of_circle.x = position.x;
	center_of_circle.y = position.y - 0.5;

	if (isFilled)
	{

		CalculateColorOfDrink();
		GetCircle(circle_points, center_of_circle, 0.39);

		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_QUAD_STRIP);
			glColor3d(color_of_drink[0], color_of_drink[1], color_of_drink[2]);
			glNormal3f(-2, 4, 0);
			glVertex3f(circle_points[i].x, circle_points[i].y, height - 0.1);
			glVertex3f(circle_points[i].x, circle_points[i].y, height*0.1);
			glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height - 0.1);
			glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height*0.1);
			glEnd();
		}

		glBegin(GL_QUAD_STRIP);
		glColor3d(color_of_drink[0], color_of_drink[1], color_of_drink[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[0].x, circle_points[0].y, height - 0.1);
		glVertex3f(circle_points[0].x, circle_points[0].y, height*0.1);
		glVertex3f(circle_points[359].x, circle_points[359].y, height - 0.1);
		glVertex3f(circle_points[359].x, circle_points[359].y, height*0.1);
		glEnd();

		for (int i = 0; i < 359; i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(-2, 4, 0);
			glColor3d(color_of_drink[0], color_of_drink[1], color_of_drink[2]);
			glVertex3f(circle_points[i].x, circle_points[i].y, height*0.1);
			glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
			glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height*0.1);
			glEnd();
		}
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color_of_drink[0], color_of_drink[1], color_of_drink[2]);
		glVertex3f(circle_points[0].x, circle_points[0].y, height*0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
		glVertex3f(circle_points[359].x, circle_points[359].y, height*0.1);
		glEnd();
	}

	GetCircle(circle_points, center_of_circle, 0.4);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glVertex3f(circle_points[i].x, circle_points[i].y, height*0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height*0.1);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(circle_points[0].x, circle_points[0].y, height*0.1);
	glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
	glVertex3f(circle_points[359].x, circle_points[359].y, height*0.1);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, height);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, height);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, height);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();



	glPopMatrix();
	glPopMatrix();
}


bool isBartenderingModeOn;
bool isOnlyZoomAble;
int view_buffer, pzoom_buffer;
bool isBartenderModeJustEnded;

active_bottle active_bottles[5];
active_shaker shaker;
active_glass glass;
Point hitboxes[11][10];

int count_of_alco_drops;
float stream_generation_timer_gen_time;
float stream_generation_timer_current_time;
alco_drop stream[10000];



void DrawBarTitles()
{
	int counts_of_coctail_elements[6];
	char counts_of_coctail_elements_str[6][4];

	shaker_cont.GetComponentsToShow(counts_of_coctail_elements);

	for (int i = 0; i < 6; i++)
		itoa(counts_of_coctail_elements[i], counts_of_coctail_elements_str[i], 10);

	/*active_bottles[0] = active_bottle(22.5, 9.3, 14.5, 0, 1, 3, 1, 0, 0, hitboxes[0], 4, 3.4);
	active_bottles[1] = active_bottle(22, 9.3, 17.5, 0, 2, 0.5, 0, 0, 1, hitboxes[1], 4, 1);
	active_bottles[2] = active_bottle(22, 11, 17.5, 0, 1, 0.1, 1, 1, 1, hitboxes[2], 4, 3.4);
	active_bottles[3] = active_bottle(22, 12.7, 17.5, 0, 3, 1, 1, 1, 0, hitboxes[3], 4, 1.8);
	active_bottles[4] = active_bottle(22.5, 12.7, 14.5, 0, 3, 2, 0, 1, 0, hitboxes[4], 4, 3.4);*/

	glPushMatrix();
	glRasterPos3f(22.5, 8.6, 20.5);
	glEnable(GL_BLEND);
	titleFont->Render("NEVEREVERLAND");
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(22.5, 8.6, 16.4);
	glEnable(GL_BLEND);
	barFont->Render("Adelhyde");
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(22.5, 9, 14.2);
	glEnable(GL_BLEND);
	barFont->Render(counts_of_coctail_elements_str[0]);
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 8.3, 18.4);
	glEnable(GL_BLEND);
	barFont->Render("Powdrd. Delta");
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(22.5, 9, 17.2);
	glEnable(GL_BLEND);
	barFont->Render(counts_of_coctail_elements_str[1]);
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 9.7, 19.1);
	glEnable(GL_BLEND);
	barFont->Render("Karmotrine");
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 10.3, 17.2);
	glEnable(GL_BLEND);
	barFont->Render(counts_of_coctail_elements_str[2]);
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 11, 18.4);
	glEnable(GL_BLEND);
	barFont->Render("Bronson Ext.");
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 11.7, 17.2);
	glEnable(GL_BLEND);
	barFont->Render(counts_of_coctail_elements_str[3]);
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 11.4, 16.4);
	glEnable(GL_BLEND);
	barFont->Render("Flanergide");
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(22.5, 12.4, 14.2);
	glEnable(GL_BLEND);
	barFont->Render(counts_of_coctail_elements_str[4]);
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 11, 15.8);
	glEnable(GL_BLEND);
	barFont->Render(L"Лёд");
	glDisable(GL_BLEND);
	glPopMatrix();

	glPushMatrix();
	glRasterPos3f(23, 10.5, 14.9);
	glEnable(GL_BLEND);
	barFont->Render(L"Выдержка");
	glDisable(GL_BLEND);
	glPopMatrix();


	/*EraseButton = button(22.5, 11.3, 13.5, 1.5, 0.5, &resetDrink, color1, color2, hitboxes[8]);
	ServeButton = button(22.5, 11.3, 12.7, 1.5, 0.5, &serve, color1, color2, hitboxes[8]);*/

	glPushMatrix();
	glRasterPos3f(23, 11.7, 14);
	glEnable(GL_BLEND);
	barFont->Render(L"Заново");
	glDisable(GL_BLEND);
	glPopMatrix();

	if (glass.isFilled)
	{
		glPushMatrix();
		glRasterPos3f(23, 11.7, 13.25);
		glEnable(GL_BLEND);
		barFont->Render(L"Подать");
		glDisable(GL_BLEND);
		glPopMatrix();
	}

	glPushMatrix();
	if (counts_of_coctail_elements[5] == 20)
		glColor4d(1, 0, 0, 1.0);
	glRasterPos3f(23, 8.9, 12.2);
	glEnable(GL_BLEND);
	barFont->Render(counts_of_coctail_elements_str[5]);
	glDisable(GL_BLEND);
	glPopMatrix();
}

bool isRightButtonPressed;
bool isRightButtonReleased;

void StopChangingAngle()
{
	int active_bottle_index = -1;
	for (int i = 0; i < 5; i++)
		if (active_bottles[i].isBottleActive())
		{
			active_bottle_index = i;
			break;
		}
	if (active_bottle_index != -1)
	{
		if (isRightButtonReleased)
		{
			active_bottles[active_bottle_index].ReturnToInitialAngle();
			isRightButtonReleased = false;
			isRightButtonPressed = false;
		}
	}
	else if (shaker.isBottleActive())
	{
		if (isRightButtonReleased)
		{
			shaker.ReturnToInitialAngle();
			isRightButtonReleased = false;
			isRightButtonPressed = false;
		}
	}
	if (glass.isBottleActive())
	{
		if (isRightButtonReleased)
		{
			glass.ReturnToInitialAngle();
			isRightButtonReleased = false;
			isRightButtonPressed = false;
		}
	}
}

void GenerateAlcoDrops()
{
	Point3D cup_position, center;
	int active_bottle_index = -1;
	for (int i = 0; i < 5; i++)
		if (active_bottles[i].isBottleActive())
		{
			active_bottle_index = i;
			break;
		}
	if (active_bottle_index != -1)
		if (fabs(active_bottles[active_bottle_index].GetCurrentAngle()) > 45)
		{
			stream_generation_timer_gen_time = 135 / fabs(active_bottles[active_bottle_index].GetCurrentAngle());
			if (stream_generation_timer_gen_time <= stream_generation_timer_current_time)
			{
				float angle = active_bottles[active_bottle_index].GetCurrentAngle();
				printf("%d\n", count_of_alco_drops);
				center = active_bottles[active_bottle_index].GetCurrentPosition();
				float radius = sqrt(pow(0.5, 2) + pow(active_bottles[active_bottle_index].GetTrueHeight(), 2));
				cup_position.x = 23;
				cup_position.y = center.y + radius * cos((angle + 90)*pi / 180);
				cup_position.z = center.z + radius * sin((angle + 90)*pi / 180);
				if (angle > 0)
					cup_position.z -= 1;

				stream_generation_timer_current_time = 0;
				count_of_alco_drops++;
				stream[count_of_alco_drops - 1] = alco_drop(cup_position.x, cup_position.y, cup_position.z);
			}
			else
				stream_generation_timer_current_time += 1;
		}
}

struct drop_components
{
	float red;
	float white;
	float yellow;
	float green;
	float blue;
};



void renderBitmapString(float x, float y, float z, void *font, char *string) {

	char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void ChangePositionOfObjectsDuringLeftMousePress(int x, int y)
{
	if (isLeftButtonPressed)
	{
		int active_bottle_index = -1;
		for (int i = 0; i < 5; i++)
			if (active_bottles[i].isBottleActive())
			{
				active_bottle_index = i;
				break;
			}
		if (active_bottle_index != -1)
		{
			if (isLeftButtonPressed && y < 540)
			{
				active_bottles[active_bottle_index].ChangePosition(x, y);

			}
			/*if (isActiveBottleCollidesShaker(active_bottle_index, x, y))
			{
				shaker_cont.UpdateComponents(active_bottle_index);
				active_bottles[active_bottle_index].ReturnToInitialPosition();
			}*/
		}
		else
			if (shaker.isBottleActive())
			{
				if (isLeftButtonPressed && y < 540)
					shaker.ChangePosition(x, y);
			}
			else
				if (glass.isBottleActive())
				{
					if (isLeftButtonPressed && y < 540)
						glass.ChangePosition(x, y);
				}
	}
}

int rightclickedmouse_x, rightclickedmouse_y, leftclickedmouse_x, leftclickedmouse_y;


class scriptprocessor
{
	scriptline *script;
	int current_chapter;
	int length_of_script;
	int reading_pointer;
	int current_drink_index;
	int counter_of_chars;
	bool isNeedToGo;

	WCHAR* Talker;
	WCHAR* Talk;



	void InterpretateLoadNextFile();
	void InterpretateStartBartendering();
	int InterpretateJump(int line_number);
	int InterpretateJump(char mark[30]);
	int InterpretateIfDrink(int line_number, int current_drink_index);
	bool InterpretateLineOfScript();
	void DivideScriptLineToFunctionAndArguments(char line[1000], scriptline& line_of_code);
	void ReadScriptFromFile();

	void ChangeSpeech();

	void DrawVNwindow();
	void DrawBartendering();

public:
	scriptprocessor()
	{
		script = NULL;
		current_chapter = 0;
		length_of_script = 0;
		reading_pointer = -1;
		current_drink_index = -1;
		counter_of_chars = 0;
		isNeedToGo = true;
		ReadScriptFromFile();
	}

	void SpacePressed();
	void DrawTextComponent();
};

void scriptprocessor::SpacePressed()
{
	isNeedToGo = true;
}

void scriptprocessor::ChangeSpeech()
{
	counter_of_chars = 0;

	char name_of_talker[30];
	for (int i = 1; i < strlen(script[reading_pointer].function); i++)
		name_of_talker[i - 1] = script[reading_pointer].function[i];
	name_of_talker[strlen(script[reading_pointer].function) - 1] = '\0';

	setlocale(LC_ALL, "Russian");
	//printf("%s %s\n", name_of_talker, script[reading_pointer].arguments);
	size_t size = strlen(name_of_talker) + 1;
	Talker = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, Talker, size, name_of_talker, size - 1);

	size = strlen(script[reading_pointer].arguments) + 1;
	Talk = new wchar_t[size];
	mbstowcs_s(&outSize, Talk, size, script[reading_pointer].arguments, size - 1);
}

void scriptprocessor::DrawVNwindow()
{
	glBegin(GL_QUAD_STRIP);
	glColor4d(0, 0, 0, 0.8);
	glVertex3f(24, 0, 11);
	glVertex3f(24, 36, 11);
	glVertex3f(24, 0, 14);
	glVertex3f(24, 36, 14);
	glEnd();

	glColor3f(223 / 255., 133 / 255., 123 / 255.);
	DrawCharName(Talker);
	if (Talk[counter_of_chars] != '\0')
		counter_of_chars++;
	glColor3f(1, 1, 1);
	DrawComplexTalk(Talk, counter_of_chars);
}

void scriptprocessor::DrawBartendering()
{
	shaker.DrawActiveShaker();
	for (int i = 0; i < 5; i++)
	{
		if (i < 3)
			active_bottles[i].DrawActiveBottle();
		else
		{
			glPushMatrix();
			Point3D pos = active_bottles[i].isBottleActive() ? active_bottles[i].GetCurrentPosition() : active_bottles[i].GetInitialPosition();
			glTranslatef(pos.x, pos.y, pos.z);
			if (i == 4)
				glRotatef(3, 1, 0, 0);
			else
				glRotatef(2, 1, 0, 0);
			glTranslatef(-pos.x, -pos.y, -pos.z);
			active_bottles[i].DrawActiveBottle();
			glPopMatrix();
		}
	}

	IceButton.DrawButton();
	AgeButton.DrawButton();
	EraseButton.DrawButton();
	if (glass.isFilled)
		ServeButton.DrawButton();

	shaker_cont.SetIce(addIce);
	shaker_cont.SetAge(addAge);

	if (resetDrink)
	{
		shaker_cont.Reset();
		shaker.Reset();
		addIce = false;
		addAge = false;
		glass.isFilled = false;
	}

	resetDrink = false;
	if (serve)
	{
		printf("Drink index:%d\n", shaker.GetResultDrinkIndex());
		isNeedToGo = true;
		current_drink_index = shaker.GetResultDrinkIndex();
		counter_of_chars = 0;
	}
	serve = false;

	glBegin(GL_QUAD_STRIP);
	glColor4d(0, 0, 0, 0.8);
	glVertex3f(22.5, 8.5, 11);
	glVertex3f(22.5, 36, 11);
	glVertex3f(20.5, 8.5, 30);
	glVertex3f(20.5, 36, 30);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor4d(0, 0, 0, 0);
	glVertex3f(22.5, 8.5, 11);
	glVertex3f(22.5, 0, 11);
	glVertex3f(20.5, 8.5, 30);
	glVertex3f(20.5, 0, 30);
	glEnd();

	ChangePositionOfObjectsDuringLeftMousePress(leftclickedmouse_x, leftclickedmouse_y);

	glColor3d(1, 1, 1);
	DrawBarTitles();
}


void scriptprocessor::DrawTextComponent()
{
	if (isBartenderingModeOn)
	{
		if (isNeedToGo)
		{
			reading_pointer++;
			printf("here %s %s %d\n", script[reading_pointer].function,script[reading_pointer].arguments,reading_pointer);
			while (!InterpretateLineOfScript())
				if (reading_pointer == -1)
					reading_pointer = 0;
			isNeedToGo = false;
		}
		if (!isEndOfScript)
		{
			if (isVNModeOn)
				DrawVNwindow();
			else
				DrawBartendering();
		}
	}
}

void scriptprocessor::InterpretateLoadNextFile()
{
	current_chapter = atoi(script[reading_pointer].arguments);
	printf("%s %d\n", script[reading_pointer].arguments, current_chapter);
	ReadScriptFromFile();
}



void scriptprocessor::InterpretateStartBartendering()
{
	shaker.Reset();
	shaker_cont.Reset();
	addIce = false;
	addAge = false;
	glass.isFilled = false;
	isVNModeOn = false;
}

int scriptprocessor::InterpretateJump(int line_number)
{
	char catcher[30];
	strcpy(catcher, script[line_number].function);
	catcher[0] = '#';
	printf("marks:%s\n", catcher);
	for (int i = 0; i < length_of_script; i++)
		if (!strcmp(script[i].function, catcher))
		{
			
			return i + 1;
		}
}

int scriptprocessor::InterpretateJump(char mark[30])
{
	char catcher[30];
	strcpy(catcher, mark);
	catcher[0] = '#';
	for (int i = 0; i < length_of_script; i++)
		if (!strcmp(script[i].function, catcher))
			return i + 1;
}

int scriptprocessor::InterpretateIfDrink(int line_number, int current_drink_index)
{
	int i = 0;
	int j;
	int needed_drink;
	char needed_drink_str[4];
	char mark_to_follow[30];
	while (script[line_number].arguments[i] != ' ')
	{
		needed_drink_str[i] = script[line_number].arguments[i];
		i++;
	}
	needed_drink_str[i] = '\0';
	needed_drink = atoi(needed_drink_str);
	i++;
	if (needed_drink != current_drink_index)
	{
		while (script[line_number].arguments[i] != '|')
			i++;
		i++;
	}
	j = 0;
	while (script[line_number].arguments[i] != '|' && script[line_number].arguments[i] != '\0')
	{
		mark_to_follow[j] = script[line_number].arguments[i];
		i++;
		j++;
	}
	mark_to_follow[j] = '\0';
	return InterpretateJump(mark_to_follow);
}

bool scriptprocessor::InterpretateLineOfScript()
{

	if (!strcmp(script[reading_pointer].function, "?ifdrink"))
	{
		reading_pointer = InterpretateIfDrink(reading_pointer, current_drink_index);
		return false;
	}
	else if (script[reading_pointer].function[0] == '^')
	{
		reading_pointer = InterpretateJump(script[reading_pointer].function);
		return false;
	}
	else if (script[reading_pointer].function[0] == '#')
	{
		reading_pointer++;
		return false;
	}
	else if (!strcmp(script[reading_pointer].function, "*bartendering"))
	{
		InterpretateStartBartendering();
		return true;
	}
	else if (script[reading_pointer].function[0] == '$')
	{
		isVNModeOn = true;
		ChangeSpeech();
		return true;
	}
	else if (!strcmp(script[reading_pointer].function, "!load"))
	{
		InterpretateLoadNextFile();
		return false;
	}
	else if (!strcmp(script[reading_pointer].function, "!end"))
	{
		view = view_buffer;
		pzoom = pzoom_buffer;
		zoom((int)(dveX1 + dveY1));
		isBartenderingModeOn = false;
		isEndOfScript = true;
		return true;
	}
}

void scriptprocessor::DivideScriptLineToFunctionAndArguments(char line[1000], scriptline& line_of_code)
{
	bool isFuncReaded = false;
	int i = 0, j = 0;
	char reading_char = line[i];
	
			
	while (line[i] != '\0' && !isFuncReaded)
	{
		if (line[i] == ']')
		{
			isFuncReaded = true;
			line_of_code.function[i] = '\0';
		}
		else
			line_of_code.function[i] = line[i];
		i++;
	}
	line_of_code.arguments[0] = '\0';
	if (line[0] == '$')
	{
		ToLayoutFormat(line + sizeof(char)*i, line_of_code.arguments, 51);
		printf("%s\n", line_of_code.arguments);
	}
	else
	{
		while (line[i] != '\0')
		{
			line_of_code.arguments[j] = line[i];
			i++;
			j++;
		}
		line_of_code.arguments[j] = '\0';
	}
		

}

void scriptprocessor::ReadScriptFromFile()
{
	char filepath[20] = "script/ch", number_of_ch[2];
	char current_char, current_line[1000];
	int i = 0;

	if (script != NULL)
	{
		free(script);
		script = NULL;
	}

	FILE* script_file;
	itoa(current_chapter, number_of_ch, 10);
	strcat(filepath, number_of_ch);
	strcat(filepath, ".txt");
	script_file = fopen(filepath, "rt");
	current_char = fgetc(script_file);
	length_of_script = 0;
	while (!feof(script_file))
	{
		if (current_char != '\n')
		{
			current_line[i] = current_char;
			i++;
			current_char = fgetc(script_file);
		}
		else
		{
			current_line[i] = '\0';
			script = (scriptline*)realloc(script, (length_of_script + 1) * sizeof(scriptline));
			DivideScriptLineToFunctionAndArguments(current_line, script[length_of_script]);
			length_of_script++;
			i = 0;
			current_char = fgetc(script_file);
		}
	}
	current_line[i] = '\0';
	script = (scriptline*)realloc(script, (length_of_script + 1) * sizeof(scriptline));
	DivideScriptLineToFunctionAndArguments(current_line, script[length_of_script]);
	length_of_script++;

	reading_pointer = -1;
	isNeedToGo = true;

	fclose(script_file);
}

scriptprocessor vn;

// Общий обработчик нажатия клавиш.

/*
char keys_white[] = { 'a','s','d','f','g','h','j' };
	char keys_black[] = { 'w','e','t','y','u'};*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		if (!isBartenderingModeOn)
			view_select();
		else
			vn.SpacePressed();
		break;
	case 'a':
	case 'A':
	case 's':
	case 'S':
	case 'd':
	case 'D':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
	case 'h':
	case 'H':
	case 'j':
	case 'J':
	case 'w':
	case 'W':
	case 'e':
	case 'E':
	case 't':
	case 'T':
	case 'y':
	case 'Y':
	case 'U':
	case 'u':
		if (bartender.GetAbilityToPlayPiano())
			PianoKeyboard(key);
		break;
	case '-':
	case '=':
		if (bartender.GetAbilityToPlayPiano())
			ChangeOctave(key);
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
		if (bartender.GetAbilityToUseJukebox())
			JukeboxDiscChange(key);
		break;
	case '0':
		if (bartender.GetAbilityToUseJukebox())
			JukeboxBreak();
		break;
	case 'l':
	case 'L':
		ShowWalls = !ShowWalls;
		break;
	case 13:
		if (bartender.GetAbilityToBartendering() && !isEndOfScript)
		{
			printf("start bar mode");
			isOnlyZoomAble = true;
			isBartenderingModeOn = true;
		}
		else if (bartender.GetAbilityToLeave())
		{
			glutDestroyWindow(1);
		}
		break;
	case 27:
		if (bartender.GetAbilityToBartendering())
		{
			printf("end bar mode");
			view = view_buffer;
			pzoom = pzoom_buffer;
			zoom((int)(dveX1 + dveY1));
			isBartenderingModeOn = false;
		}
		break;
	
	}
}

void keyUnpressed(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
	case 'A':
	case 's':
	case 'S':
	case 'd':
	case 'D':
	case 'f':
	case 'F':
	case 'g':
	case 'G':
	case 'h':
	case 'H':
	case 'j':
	case 'J':
	case 'w':
	case 'W':
	case 'e':
	case 'E':
	case 't':
	case 'T':
	case 'y':
	case 'Y':
	case 'U':
	case 'u':
		if (bartender.GetAbilityToPlayPiano())
			PianoKeyboard('i');
		break;
	}
}


void specialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN:
		if (!isBartenderingModeOn)
			bartender.MovePlayer(GLUT_KEY_DOWN);
		break;
	case GLUT_KEY_LEFT:
		if (!isBartenderingModeOn)
			bartender.MovePlayer(GLUT_KEY_LEFT);
		break;
	case GLUT_KEY_RIGHT:
		if (!isBartenderingModeOn)
			bartender.MovePlayer(GLUT_KEY_RIGHT);
		break;
	case GLUT_KEY_UP:
		if (!isBartenderingModeOn)
			bartender.MovePlayer(GLUT_KEY_UP);
		break;
	}
}


bool isActiveBottleCollidesShaker(int index_of_bottle, int mouse_x, int mouse_y)
{
	int diff_x = mouse_x - hitboxes[index_of_bottle][3].x;
	int diff_y = mouse_y - hitboxes[index_of_bottle][3].y;

	if (hitboxes[5][1].x > (hitboxes[index_of_bottle][3].x + diff_x) || (hitboxes[index_of_bottle][1].x + diff_x) > hitboxes[5][3].x)
		return false;

	if (hitboxes[5][1].y > (hitboxes[index_of_bottle][3].y + diff_y) || (hitboxes[index_of_bottle][1].y + diff_y) > hitboxes[5][3].y)
		return false;

	return true;
}

bool isShakerCollidesGlass(int mouse_x, int mouse_y)
{
	int diff_x = mouse_x - hitboxes[5][3].x;
	int diff_y = mouse_y - hitboxes[5][3].y;

	if (hitboxes[6][1].x > (hitboxes[5][3].x + diff_x) || (hitboxes[5][1].x + diff_x) > hitboxes[6][3].x)
		return false;

	if (hitboxes[6][1].y > (hitboxes[5][3].y + diff_y) || (hitboxes[5][1].y + diff_y) > hitboxes[6][3].y)
		return false;

	return true;
}


void CheckCollisionInLeftButtonRelease(int mouse_x, int mouse_y)
{
	int active_bottle_index = -1;
	for (int i = 0; i < 5; i++)
		if (active_bottles[i].isBottleActive())
		{
			active_bottle_index = i;
			break;
		}
	if (active_bottle_index != -1)
	{
		if (isActiveBottleCollidesShaker(active_bottle_index, mouse_x, mouse_y))
			shaker_cont.UpdateComponents(active_bottle_index);
		active_bottles[active_bottle_index].ReturnToInitialPosition();
	}
	if (shaker.isBottleActive())
	{
		if (isShakerCollidesGlass(mouse_x, mouse_y))
			glass.isFilled = true;
		shaker.ReturnToInitialPosition();
	}
}




// действие по нажатию или отпусканию кнопки мыши.
void mouseEvent(int button, int state, int x, int y)
{

	printf(",%d,%d\n", x, y);
	//GetOGLPos(x, y);



	// действие по нажатию левой кнопки мыши
	if (!isBartenderingModeOn)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			m1 = true;                           // дать пометку, что левая кнопка была нажата
			lx0 = x;                           // сохранить х координату мышки
			ly0 = y;                           // сохранить y координату мышки

			if ((m1) && (m2))                         // если обе(и правая и левая) кнопки мыши были нажаты
			{                                       // то сохранить х и y координаты мыши в отдельные переменные,
				dveX0 = x;                            //  отвечающие за действие приближения (в орто проекции)  
				dveY0 = y;                            //  или  изменения угла зрения в перспективной проекции  

			}

		}
		// действие по событию отпускания левой кнопки мыши
		else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			m1 = false;
		}
		// действие по событию нажатия правой кнопки
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		{
			m2 = 1;                             // дать пометку, что правая кнопка была нажата
			rx0 = x;                          // сохранить х координату мышки
			ry0 = y;                          // сохранить y координату мышки

			if ((m1) && (m2))
			{                                          // если обе(и правая и левая) кнопки мыши были нажаты
				dveX0 = x;                                // то сохранить х и y координаты мыши в отдельные переменные,
				dveY0 = y;                                //  отвечающие за действие приближения (в орто проекции)  
			}                                       //  или  изменения угла зрения в перспективной проекции 
		}
		// действие по событию отпускания правой кнопки мыши
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		{
			m2 = 0;
		}
	}
	else if (!isVNModeOn)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			for (int i = 0; i < 5; i++)
				active_bottles[i].CheckIfClickedOnBottle(x, y);

			if (shaker_cont.GetSumOfComponents())
				shaker.CheckIfClickedOnBottle(x, y);
			if (shaker.isBottleActive())
				shaker.StartCountingPath(x, y);

			//glass.CheckIfClickedOnBottle(x, y);
			IceButton.isClicked(x, y);
			AgeButton.isClicked(x, y);
			EraseButton.isClicked(x, y);
			ServeButton.isClicked(x, y);

			isLeftButtonPressed = true;
			leftclickedmouse_x = x;
			leftclickedmouse_y = y;
		}

		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			isLeftButtonPressed = false;
			CheckCollisionInLeftButtonRelease(x, y);
		}
	}

}



// общий обработчик вызывающийся при движении мыши
void mouse_move(int x, int y)
{
	if (!isBartenderingModeOn)
	{
		if ((m1) && (m2))                                  // а не было ли жвижения мышки с нажатыми обоими кнопками?
		{
			dveX1 = x - dveX0;
			dveY1 = y - dveY0;

			zoom((int)(dveX1 + dveY1));                  // если да то применить приближение/изменения угла зрения

			dveX0 = x;
			dveY0 = y;
			return;
		}
		else
		{
			if (m1)                                        // может была нажата левая кнопка мыши?
			{
				lx1 = x - lx0;
				ly1 = y - ly0;

				jamp(-ly1, lx1);                  // тогда сдвинуться вверх и влево(по перемещению) 

				lx0 = x;
				ly0 = y;
			}
			else if (m2)                                  // или может была нажата правая кнопка мыши?
			{

				rx1 = x - rx0;
				ry1 = y - ry0;

				look_around(-rx1 / 10.0, ry1 / 30.0);              // тогда повернуть камеру

				rx0 = x;
				ry0 = y;
			}
		}
	}
	else if (!isVNModeOn)
	{
		if (isLeftButtonPressed)
		{
			leftclickedmouse_x = x;
			leftclickedmouse_y = y;
		}
	}
}




void TimerCallback(int fictive)
{
	glutPostRedisplay();
}

void GetSubCircleYZ(Point3D points[360], Point3D center, float rad)
{
	float step = (2 * pi) / 360;
	for (int i = 0; i < 180; i++)
	{
		points[i].y = rad * cos(i*step) + center.y;
		points[i].z = rad * sin(i*step) + center.z;
		points[i].x = center.x;
	}
}

Point3D arched_points_jukebox[360][180];

void InitArc()
{
	center_of_circle.x = 0;										//ободок - дуга
	center_of_circle.y = -8.9;
	GetCircle(circle_points, center_of_circle, 1);
	ArcCustom arched(0, 0, 0, 0, 0);
	Point* arched_points_old;
	Point3D center;
	center.y = -4.1;
	center.z = 0;
	int count_of_elems_arched_old;
	for (int i = 0; i < 360; i++)										//5.1 - центр
	{
		/*arched = ArcCustom(circle_points[i].x, circle_points[i].y, circle_points[i].x, -9.6 - circle_points[i].y, 3.8 - 2 * ((7.9 + circle_points[i].y) / 2));
		arched.GenerateArc(arched_points_jukebox[i], count_of_elems_arched_jukebox[i]);*/
		center.x = circle_points[i].x;
		GetSubCircleYZ(arched_points_jukebox[i], center, 3.1 - 2 * ((7.9 + circle_points[i].y) / 2));
	}
}




void DrawGlass(float height)
{
	center_of_circle.x = 0;										//нижняя часть
	center_of_circle.y = -0.5;
	GetCircle(circle_points, center_of_circle, 0.4);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glVertex3f(circle_points[i].x, circle_points[i].y, height*0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height*0.1);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(circle_points[0].x, circle_points[0].y, height*0.1);
	glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
	glVertex3f(circle_points[359].x, circle_points[359].y, height*0.1);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, height);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, height);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, height);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

}

void DrawMiniGlass(float height)
{
	center_of_circle.x = 0;										//нижняя часть
	center_of_circle.y = -0.5;
	GetCircle(circle_points, center_of_circle, 0.2);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glVertex3f(circle_points[i].x, circle_points[i].y, height*0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height*0.1);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(circle_points[0].x, circle_points[0].y, height*0.1);
	glVertex3f(center_of_circle.x, center_of_circle.y, height*0.1);
	glVertex3f(circle_points[359].x, circle_points[359].y, height*0.1);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, height);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, height);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, height);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

}

void DrawShaker(float footer_height, float color[3])
{
	Point bottom_circle_points[360];
	Point header_circle_points[360];
	Point cap_circle_points[360];

	glPushMatrix();									//средняя часть
	glTranslatef(0, -0.5, footer_height);
	glScalef(0.4, 0.4, footer_height / 2 - 0.2);
	glColor3d(color[0], color[1], color[2]);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();

	center_of_circle.x = 0;										//нижняя часть
	center_of_circle.y = -0.5;
	GetCircle(circle_points, center_of_circle, 0.4);
	GetCircle(bottom_circle_points, center_of_circle, 0.3);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color[0], color[1], color[2]);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(color[0], color[1], color[2]);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(bottom_circle_points[359].x, bottom_circle_points[359].y, 0);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glVertex3f(bottom_circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	GetCircle(header_circle_points, center_of_circle, 0.1);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color[0] + 1, color[1], color[2]);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(center_of_circle.x, center_of_circle.y, footer_height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(center_of_circle.x, center_of_circle.y, footer_height);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glEnd();


	for (int i = 0; i < 359; i++)					//верхняя часть
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, footer_height * 3 / 2.);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 0);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, footer_height * 3 / 2.);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, footer_height * 3 / 2.);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 0);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, footer_height * 3 / 2.);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 0);
	glEnd();

	GetCircle(cap_circle_points, center_of_circle, 0.12);


	for (int i = 0; i < 359; i++)					//крышка
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, footer_height * 3 / 2.);
		glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, footer_height * 3 / 2. - 0.1);
		glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, footer_height * 3 / 2.);
		glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, footer_height * 3 / 2. - 0.1);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(234 / 255., 128 / 255., 107 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, footer_height * 3 / 2.);
	glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, footer_height * 3 / 2. - 0.1);
	glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, footer_height * 3 / 2.);
	glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, footer_height * 3 / 2. - 0.1);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, footer_height * 3 / 2.);
		glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3 / 2.);
		glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, footer_height * 3 / 2.);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, footer_height * 3 / 2.);
	glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3 / 2.);
	glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, footer_height * 3 / 2.);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(cap_circle_points[i].x, cap_circle_points[i].y, footer_height * 3 / 2. - 0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3 / 2. - 0.1);
		glVertex3f(cap_circle_points[i + 1].x, cap_circle_points[i + 1].y, footer_height * 3 / 2. - 0.1);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(cap_circle_points[0].x, cap_circle_points[0].y, footer_height * 3 / 2. - 0.1);
	glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3 / 2. - 0.1);
	glVertex3f(cap_circle_points[359].x, cap_circle_points[359].y, footer_height * 3 / 2. - 0.1);
	glEnd();
}

void DrawVortex(float footer_height, float color[3])
{
	Point center_circle_points[360];

	center_of_circle.x = 0;
	center_of_circle.y = -0.5;
	GetCircle(circle_points, center_of_circle, 0.2);
	GetCircle(center_circle_points, center_of_circle, 0.1);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_circle_points[i].x, center_circle_points[i].y, footer_height / 2.);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glVertex3f(center_circle_points[i + 1].x, center_circle_points[i + 1].y, footer_height / 2.);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_circle_points[0].x, center_circle_points[0].y, footer_height / 2.);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glVertex3f(center_circle_points[359].x, center_circle_points[359].y, footer_height / 2.);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(center_circle_points[i].x, center_circle_points[i].y, footer_height / 2.);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glVertex3f(center_circle_points[i + 1].x, center_circle_points[i + 1].y, footer_height / 2.);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(center_circle_points[0].x, center_circle_points[0].y, footer_height / 2.);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glVertex3f(center_circle_points[359].x, center_circle_points[359].y, footer_height / 2.);
	glEnd();
}

void DrawBlender(float footer_height, float color[3])
{
	Point bottom_circle_points[360];
	Point header_circle_points[360];
	Point cap_circle_points[360];

	center_of_circle.x = 0;										//нижняя часть
	center_of_circle.y = -0.5;
	GetCircle(circle_points, center_of_circle, 0.6);
	GetCircle(bottom_circle_points, center_of_circle, 0.4);

	for (int i = 0; i < 359; i++)											//дно
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color[0], color[1], color[2]);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(color[0], color[1], color[2]);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(bottom_circle_points[359].x, bottom_circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color[0], color[1], color[2]);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(color[0], color[1], color[2]);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glVertex3f(bottom_circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(color[0] + 1, color[1], color[2]);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(center_of_circle.x, center_of_circle.y, footer_height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(center_of_circle.x, center_of_circle.y, footer_height);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glEnd();

	GetCircle(header_circle_points, center_of_circle, 0.1);						//трубка внутри


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(color[0], color[1], color[2]);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, footer_height * 3 / 2. - 0.4);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 0);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, footer_height * 3 / 2. - 0.4);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(color[0], color[1], color[2]);
	glNormal3f(-2, 4, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, footer_height * 3 / 2. - 0.4);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 0);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, footer_height * 3 / 2. - 0.4);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.3, -0.4, footer_height * 3 / 2. - 0.4);
	glVertex3f(-0.3, -0.6, footer_height * 3 / 2. - 0.4);
	glVertex3f(0.3, -0.4, footer_height * 3 / 2. - 0.4);
	glVertex3f(0.3, -0.6, footer_height * 3 / 2. - 0.4);
	glEnd();



	for (int i = 0; i < 359; i++)								//стекло, верхняя часть
	{
		glBegin(GL_QUAD_STRIP);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, footer_height);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, footer_height * 3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, footer_height);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, footer_height * 3);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, footer_height);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, footer_height * 3);
	glVertex3f(circle_points[359].x, circle_points[359].y, footer_height);
	glVertex3f(bottom_circle_points[359].x, circle_points[359].y, footer_height * 3);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
		glVertex3f(bottom_circle_points[i].x, bottom_circle_points[i].y, footer_height * 3);
		glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3);
		glVertex3f(bottom_circle_points[i + 1].x, bottom_circle_points[i + 1].y, footer_height * 3);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(bottom_circle_points[0].x, bottom_circle_points[0].y, footer_height * 3);
	glVertex3f(center_of_circle.x, center_of_circle.y, footer_height * 3);
	glVertex3f(bottom_circle_points[359].x, bottom_circle_points[359].y, footer_height * 3);
	glEnd();


	center_of_circle_3D.x = 0;								//Кнопка
	center_of_circle_3D.y = 0;
	center_of_circle_3D.z = 0.7;
	GetCircleXZ(circle_points_3D, center_of_circle_3D, 0.1);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 1.2, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 1.2, circle_points_3D[i].z);
		glEnd();
	}
}



struct Bottle
{
	int type;
	float height;
	float colors[3];
	Point3D position;
};

class RowOfBottles
{
	Bottle* array_of_bottles;
	int count_of_bottles;

public:

	RowOfBottles(int count) :count_of_bottles(count)
	{
		array_of_bottles = (Bottle*)malloc(sizeof(Bottle)*count_of_bottles);
	};

	RowOfBottles() {};

	void GenerateRow();

	void GetBottlesArray(Bottle* data)
	{
		data = (Bottle*)malloc(sizeof(Bottle)*count_of_bottles);
		for (int i = 0; i < count_of_bottles; i++)
			data[i] = array_of_bottles[i];
	}

	void DrawBottlesArray();
};

void RowOfBottles::GenerateRow()
{
	for (int i = 0; i > -count_of_bottles; i--)
	{
		array_of_bottles[-i].type = rand() % 3;
		array_of_bottles[-i].position.x = 0.5;
		array_of_bottles[-i].position.y = i;
		array_of_bottles[-i].colors[0] = rand() % 255 / 255.;
		array_of_bottles[-i].colors[1] = rand() % 255 / 255.;
		array_of_bottles[-i].colors[2] = rand() % 255 / 255.;
		switch (array_of_bottles[-i].type)
		{
		case 0:array_of_bottles[-i].height = rand() % 3; break;
		case 1:
		case 2:
			array_of_bottles[-i].height = rand() % 2 + 1; break;
		}
	}
}

void RowOfBottles::DrawBottlesArray()
{
	for (int i = 0; i < count_of_bottles; i++)
	{
		glPushMatrix();
		glTranslatef(array_of_bottles[i].position.x, array_of_bottles[i].position.y, 0);
		switch (array_of_bottles[i].type)
		{
		case 0:DrawBottleType1(array_of_bottles[i].height, array_of_bottles[i].colors); break;
		case 1:DrawBottleType2(array_of_bottles[i].height, array_of_bottles[i].colors); break;
		case 2:DrawBottleType3(array_of_bottles[i].height, array_of_bottles[i].colors); break;
		}
		glPopMatrix();
	}
}

void InitScreenshotList(GLuint screenshots[16])
{
	char number_of_screenshot[5];
	char filename[25];
	for (int i = 0; i < 16; i++)
	{
		itoa(i, number_of_screenshot, 10);
		filename[0] = '\0';
		strcat(filename, "rainclouds/");
		strcat(filename, number_of_screenshot);
		strcat(filename, ".bmp");
		//screenshots[i] = raw_texture_load(filename,897, 1594);
		screenshots[i] = LoadTexture(filename);
	}
}

void GenerateTheSetOfScreenshotsNumbers(int filenumbers_list[6])
{
	char number_of_screenshot[5];
	for (int i = 0; i < 6; i++)
		filenumbers_list[i] = rand() % 16;
}

void InitHitboxes(Point box[11][10])
{
	box[0][0].x = 385; box[0][0].y = 388;
	box[0][1].x = 387; box[0][1].y = 302;
	box[0][2].x = 420; box[0][2].y = 299;
	box[0][3].x = 419; box[0][3].y = 389;


	box[1][0].x = 384; box[1][0].y = 243;
	box[1][1].x = 384; box[1][1].y = 204;
	box[1][2].x = 415; box[1][2].y = 205;
	box[1][3].x = 419; box[1][3].y = 244;


	box[2][0].x = 467; box[2][0].y = 237;
	box[2][1].x = 462; box[2][1].y = 147;
	box[2][2].x = 504; box[2][2].y = 150;
	box[2][3].x = 498; box[2][3].y = 237;


	box[3][0].x = 546; box[3][0].y = 240;
	box[3][1].x = 547; box[3][1].y = 187;
	box[3][2].x = 581; box[3][2].y = 187;
	box[3][3].x = 582; box[3][3].y = 239;


	box[4][0].x = 553; box[4][0].y = 391;
	box[4][1].x = 554; box[4][1].y = 302;
	box[4][2].x = 586; box[4][2].y = 301;
	box[4][3].x = 587; box[4][3].y = 395;


	box[5][0].x = 410; box[5][0].y = 562;
	box[5][1].x = 405; box[5][1].y = 463;
	box[5][2].x = 458; box[5][2].y = 465;
	box[5][3].x = 459; box[5][3].y = 565;



	box[6][0].x = 512; box[6][0].y = 568;
	box[6][1].x = 510; box[6][1].y = 485;
	box[6][2].x = 566; box[6][2].y = 485;
	box[6][3].x = 569; box[6][3].y = 566;

	box[7][0].x = 440; box[7][0].y = 338;
	box[7][1].x = 440; box[7][1].y = 313;
	box[7][2].x = 544; box[7][2].y = 315;
	box[7][3].x = 541; box[7][3].y = 338;

	box[8][0].x = 439; box[8][0].y = 388;
	box[8][1].x = 440; box[8][1].y = 362;
	box[8][2].x = 538; box[8][2].y = 363;
	box[8][3].x = 538; box[8][3].y = 385;

	box[9][0].x = 510; box[9][0].y = 432;
	box[9][1].x = 510; box[9][1].y = 411;
	box[9][2].x = 584; box[9][2].y = 410;
	box[9][3].x = 582; box[9][3].y = 431;

	box[10][0].x = 507; box[10][0].y = 471;
	box[10][1].x = 510; box[10][1].y = 448;
	box[10][2].x = 580; box[10][2].y = 447;
	box[10][3].x = 579; box[10][3].y = 469;
}





DiscHolder disc_holders[8];
RowOfBottles rows_of_bottles[3];
float change_timer;
GLuint texture_array[16];
int screens_texture_numbers[6];



int main(int argc, char *argv[])
{



	// регистрация функций обработки событий от мыши и от клавиатуры
	setlocale(LC_ALL, "Russian");
	std::cout
		<< "Управление:\n"
		<< "[Стрелки] Управление персонажем\n"
		<< "[Ввод] Взаимодействие с барной стойкой и выходом\n"
		<< "[ESC] Выход из-за барной стойки\n"
		<< "[Пробел] В режиме ВН - перелистнуть, иначе - приближение\n"
		<< "[1-8] Рядом с музыкальным автоматом - включить музыку\n"
		<< "[0] Рядом с музыкальным автоматом - отключить музыку\n"
		<< "[a-j] Рядом с базз-роялем - белые клавиши\n"
		<< "[w,e,t-u] Рядом с базз-роялем - чёрные клавиши\n"
		<< "[+-] Рядом с базз-роялем - сменить октаву\n"
		<< "[L] Убрать стены\n"
		<< "Данный код написан топ тян ИДБ-16-02. Счастливого пользования!!!";




	glutInit(&argc, argv);

	// размер окна OpenGL
	windowW = 600;
	windowH = 600;

	// расположение окна OpenGL на экране
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(windowW, windowH);

	// инициализация окна OpenGL с заголовком Title
	glutCreateWindow("NEVEREVERLAND Pre-Alpha");

	glutMouseFunc(mouseEvent);

	// для события смещения - общий обработчик и при ненажатых кнопках, и при нажатых.
	// один для нажатия правой, левой и одновременно обоих нажатых кнопок
	glutMotionFunc(mouse_move);
	glutPassiveMotionFunc(mouse_move);

	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUnpressed);
	glutSpecialFunc(specialKeyPressed);

	// установка основных параметров работы OpenGL
	// цветовой режим RGB | включение Z-буфера для сортировки по глубине

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// регистрация функции, которая вызывается при изменении размеров окна
	//  Resize() - функция пользователя
	glutReshapeFunc(Resize);

	// регистрация функции, которая вызывается при перерисовке 
	// и запуск цикла обработки событий
	// Draw() - функция пользователя

	glutDisplayFunc(Draw);
	texture[0] = raw_texture_load("D:/pics/some_bmp.bmp", 482, 326);
	texture[1] = LoadTexture("D:/pics/text_5.bmp");
	texture[2] = LoadTexture("D:/pics/text_7.bmp");
	texture_bar_wall = LoadTexture("wall_textures/bar_wall.bmp");
	InitilizePoints();
	ort[0].x = 1; ort[0].y = 0; ort[0].z = 0;
	ort[1].x = 0; ort[1].y = 1; ort[1].z = 0;
	ort[2].x = 0; ort[2].y = 0; ort[2].z = 1;
	ort[3].x = 1; ort[3].y = 1; ort[2].z = 1;
	time_scale = 3;
	grad_coef = 0;
	InitArc();
	srand(time(0));
	for (int i = 0; i < 100; i++)
		gacha_var[i] = rand() % 100;
	gacha_index = 0;
	for (int i = 0; i < 8; i++)
		disc_holders[i] = DiscHolder(i + 1);
	unplug = false;
	plug = false;
	current_angle_of_disc_automat = 0;
	new_angle = 0;
	current_disc = 1;
	isFirstTimeChangingDiscs = true;
	isSameDiscPicked = false;
	rows_of_bottles[0] = RowOfBottles(35);
	rows_of_bottles[0].GenerateRow();
	rows_of_bottles[1] = RowOfBottles(35);
	rows_of_bottles[1].GenerateRow();
	rows_of_bottles[2] = RowOfBottles(16);
	rows_of_bottles[2].GenerateRow();
	bartender = Player(35, -25);
	change_timer = 10;
	InitScreenshotList(texture_array);
	isBartenderingModeOn = false;
	isOnlyZoomAble = false;
	initFonts();
	current_chapter = 0;
	current_line = 0;
	start_of_chapter = true;
	isVNModeOn = false;

	InitHitboxes(hitboxes);

	glPushMatrix();

	active_bottles[0] = active_bottle(22.5, 9.3, 14.5, 0, 1, 3, 1, 0, 0, hitboxes[0], 4, 3.4);
	active_bottles[1] = active_bottle(22, 9.3, 17.5, 0, 2, 0.5, 0, 0, 1, hitboxes[1], 4, 1);
	active_bottles[2] = active_bottle(22, 11, 17.5, 0, 1, 0.1, 1, 1, 1, hitboxes[2], 4, 3.4);
	active_bottles[3] = active_bottle(22, 12.7, 17.5, 0, 3, 1, 1, 1, 0, hitboxes[3], 4, 1.8);
	active_bottles[4] = active_bottle(22.5, 12.7, 14.5, 0, 3, 2, 0, 1, 0, hitboxes[4], 4, 3.4);
	shaker = active_shaker(23.2, 10, 11.5, 1.5, 0, hitboxes[5], 4);

	glPopMatrix();

	glass = active_glass(23.2, 12, 11.5, 1, 0, hitboxes[6], 4);

	/*glBegin(GL_QUAD_STRIP);
		glColor4d(78 / 255., 67 / 255., 71 / 255., 0.8);
		glVertex3f(22.5, 9.8, 14.5);
		glVertex3f(22.5, 11.8, 14.5);
		glVertex3f(22.5, 9.8, 15);
		glVertex3f(22.5, 11.8, 15);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor4d(78 / 255., 67 / 255., 71 / 255., 0.8);
		glVertex3f(22.5, 9.8, 15.5);
		glVertex3f(22.5, 11.8, 15.5);
		glVertex3f(22.5, 9.8, 16);
		glVertex3f(22.5, 11.8, 16);
		glEnd();*/

	float color1[3] = { 78 / 255., 67 / 255., 71 / 255. };
	float color2[3] = { 234 / 255., 128 / 255., 107 / 255. };
	float color3[3] = { 0, 1, 1 };
	addIce = false;
	addAge = false;
	resetDrink = false;
	serve = false;

	IceButton = button(22.5, 9.8, 15.5, 2, 0.5, &addIce, color1, color3, hitboxes[7]);
	AgeButton = button(22.5, 9.8, 14.5, 2, 0.5, &addAge, color1, color2, hitboxes[8]);
	EraseButton = button(22.5, 11.3, 13.5, 1.5, 0.5, &resetDrink, color1, color2, hitboxes[9]);
	ServeButton = button(22.5, 11.3, 12.7, 1.5, 0.5, &serve, color1, color2, hitboxes[10]);

	isLeftButtonPressed = false;
	isLeftButtonJustReleased = false;

	isEndOfScript = false;

	count_of_textures = 0;

	ReadDrinktionaryFromFile(drinktionary, &len_of_drinktionary);

	InitMusic();

	ShowWalls = true;

	glutMainLoop();
	return 0;
}



static void Resize(int width, int height) // создается пользователем
{
	//1. получение текущих координат окна 


	w = width;
	h = height;
	// сохраняем размеры окна вывода w,h  
	if (h == 0)
		h = 1;
	// установка новых размеров окна вывода
	glViewport(0, 0, w, h);


	// в ней мы установим тип проекции (ортогональная или перспективная)
	zoom(0);

	if (width != windowW || height != windowH)						//Не позволяет изменять размер окна
	{
		glutReshapeWindow(windowW, windowH);
	}
}



static void Draw(void) // создается пользователем
{
	// 1. очистка буферов (цвета, глубины и возможно др.)

	  // установка цвета фона
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);

	// очистка всех буферов
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//2.установка режимов рисования

	  // нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// включение режима сортировки по глубине
	glEnable(GL_DEPTH_TEST);

	// тип рисования полигонов
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// первый параметр: рисовать все грани, или только внешние или внутренние
	// GL_FRONT_AND_BACK , GL_FRONT , GL_BACK  
	// второй: как рисовать 
	// GL_POINT (точки) GL_LINE(линии на границе), GL_FILL(заполнять)



  // 3.установка видового преобразования

  // выбор видовой матрицы в качестве текущей

	glMatrixMode(GL_MODELVIEW);

	// Сохранение текущего значения матрицы в стеке
	  //glPushMatrix();

	// загрузка единичной матрицы
	glLoadIdentity();

	// установка точки наблюдения

	if (!isBartenderingModeOn)
		gluLookAt(e.x, e.y, e.z, c.x, c.y, c.z, u.x, u.y, u.z);
	else
	{
		if (isOnlyZoomAble)
		{
			view_buffer = view;
			pzoom_buffer = pzoom;
			isOnlyZoomAble = false;
		}
		view = 2;
		pzoom = 80;
		zoom(0);
		gluLookAt(29.5, 7, 17, 1, 7, 14, 0.0f, 0.0f, 1.0f);
	}

	// 4. вызов модели
	DrawModel();

	// 5. завершение видового преобразования
	  // Restore the view matrix
	//  glPopMatrix();

	//6. Завершить все вызванные на данный момент операции OpenGL
	glFinish();
	glutSwapBuffers();
	int delay = 0;
	if ((!m1) && (!m2))
		delay = 0;
	glutTimerFunc(delay, TimerCallback, 0);
}

void Ugol_update()
{
	Ugol++;
	if (Ugol >= 360)
		Ugol = 0;

}

int GetPianoButtonFreq(int octave,char key)
{
	int simplyfied_octave = octave + 4;
	char keys[12] = { 'a','w','s','e','d','f','t','g','y','h','u','j' };
	int number_of_key = (strchr(keys, key) - keys);
	number_of_key = simplyfied_octave * 12 + number_of_key - 8;
	printf("%d\n", (int)(pow(2, (float)(number_of_key - 49)/12) * 440));
	return pow(2, (float)(number_of_key - 49)/12) * 440;
}

void DrawWhiteButton(char key, int octave)
{
	if (current_key == key && current_octave == octave)
	{
		glRotatef(-9, 0, 1, 0);
		Beep(GetPianoButtonFreq(octave, key), 1000 / 4);
	}
	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.2);
	glVertex3f(-1.45, 0, 0.2);
	glVertex3f(0, -0.23, 0.2);
	glVertex3f(-1.45, -0.23, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.2);
	glVertex3f(-1.45, 0, 0.2);
	glVertex3f(0, 0, 0);
	glVertex3f(-1.45, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.45, 0, 0.2);
	glVertex3f(-1.45, -0.23, 0.2);
	glVertex3f(-1.45, 0, 0);
	glVertex3f(-1.45, -0.23, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.45, -0.23, 0.2);
	glVertex3f(0, -0.23, 0.2);
	glVertex3f(-1.45, -0.23, 0);
	glVertex3f(0, -0.23, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(4, -2, 0);
	glVertex3f(0, 0, 0.2);
	glVertex3f(0, -0.23, 0.2);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.23, 0);
	glEnd();
}

void DrawBlackButton(char key, int octave)
{
	if (current_key == key && current_octave == octave)
	{
		glRotatef(-5, 0, 1, 0);
		Beep(GetPianoButtonFreq(octave, key), 1000 / 4);
	}
	glBegin(GL_QUAD_STRIP);
	glColor3f(-.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.3);
	glVertex3f(-0.75, 0, 0.3);
	glVertex3f(0, -0.09, 0.3);
	glVertex3f(-0.75, -0.09, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);				//должна быть не перпендикулярна основанию
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.75, 0, 0.3);
	glVertex3f(-0.75, -0.09, 0.3);
	glVertex3f(-0.85, 0, 0.2);
	glVertex3f(-0.85, -0.09, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.3);
	glVertex3f(-0.75, 0, 0.3);
	glVertex3f(0, 0, 0.2);
	glVertex3f(-0.85, 0, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.75, -0.09, 0.3);
	glVertex3f(0, -0.09, 0.3);
	glVertex3f(-0.85, -0.09, 0.2);
	glVertex3f(0, -0.09, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.3);
	glVertex3f(0, -0.09, 0.3);
	glVertex3f(0, 0, 0.2);
	glVertex3f(0, -0.09, 0.2);
	glEnd();
}

void DrawOctave(int num_of_octave)
{
	char keys_white[] = { 'a','s','d','f','g','h','j' };
	char keys_black[] = { 'w','e','t','y','u' };
	for (int i = 0; i < 7; i++)
	{
		glPushMatrix();
		glTranslatef(0, -i * 0.23, 0);
		DrawWhiteButton(keys_white[i], num_of_octave);
		glPopMatrix();
	}

	for (int i = 0; i < 2; i++)
	{
		glPushMatrix();
		glTranslatef(0, -0.185 - i * 0.23, 0);
		DrawBlackButton(keys_black[i], num_of_octave);
		glPopMatrix();
	}

	for (int i = 2; i < 5; i++)
	{
		glPushMatrix();
		glTranslatef(0, -0.875 - (i - 2) * 0.23, 0);
		DrawBlackButton(keys_black[i], num_of_octave);
		glPopMatrix();
	}
}

void DrawPianoKeyboard()
{
	int octave_counter = 0;

	glPushMatrix();
	DrawWhiteButton('h', -4);				//субконтроктава
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -0.185, 0);
	DrawBlackButton('u', -4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -0.23, 0);
	DrawWhiteButton('j', -4);
	glPopMatrix();

	for (int i = -3; i < 4; i++)							//контроктава - четвёртая
	{
		glPushMatrix();
		glTranslatef(0, -0.46 + octave_counter * -1.61, 0);
		DrawOctave(i);
		glPopMatrix();
		octave_counter++;
	}

	glPushMatrix();											//пятая
	glTranslatef(0, -0.46 + octave_counter * -1.61, 0);
	DrawWhiteButton('a', 4);
	glPopMatrix();
}

void DrawDamper(int count_of_strings, char key, int octave)
{
	if (current_key == key && current_octave == octave)
	{
		glTranslatef(0, -0.07, 0);
	}
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.05, 0);
	glVertex3f(-0.4, -0.05, 0);
	glVertex3f(0, -0.05, 0.14);
	glVertex3f(-0.4, -0.05, 0.14);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.05, 0);
	glVertex3f(0, -0.2, 0);
	glVertex3f(0, -0.05, 0.14);
	glVertex3f(0, -0.2, 0.14);
	glEnd();

	ArcCustom arched = ArcCustom(-0.4, -0.05, -0.2, -0.1, 0.2);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);


	ArcCustom concave = ArcCustom(-0.2, -0.1, -0.1, -0.15, 0.5);
	Point* concave_points = NULL;
	int count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave, false);

	ArcCustom arched2 = ArcCustom(-0.1, -0.15, 0, -0.2, 0.3);
	Point* arched2_points = NULL;
	int count_of_elems_arched2;
	arched2.GenerateArc(arched2_points, count_of_elems_arched2);

	for (i = 0; i < count_of_elems_arched; i++)
	{

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.14);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, -0.05, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.14);
		glVertex3f(arched_points[i].x, -0.05, 0.14);
		glEnd();
	}


	for (i = 0; i < count_of_elems_concave; i++)
	{

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.14);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(concave_points[i].x, -0.05, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.14);
		glVertex3f(concave_points[i].x, -0.05, 0.14);
		glEnd();
	}

	for (i = 0; i < count_of_elems_arched2; i++)
	{

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 0);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 0.14);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 0);
		glVertex3f(arched2_points[i].x, -0.05, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 0.14);
		glVertex3f(arched2_points[i].x, -0.05, 0.14);
		glEnd();
	}

	free(arched2_points);
	free(arched_points);
	free(concave_points);

	if (count_of_strings == 1)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.14);
		glVertex3f(-0.4, -0.05, 0.14);
		glVertex3f(0, 0, 0.14);
		glVertex3f(-0.4, 0, 0.14);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, 0, 0.14);
		glVertex3f(-0.4, 0, 0.14);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(-0.4, -0.02, 0.07);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(-0.4, -0.02, 0.07);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.4, 0, 0);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.4, 0, 0);
		glVertex3f(0, -0.05, 0);
		glVertex3f(-0.4, -0.05, 0);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.14);
		glVertex3f(0, 0, 0.14);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(0, -0.05, 0.07);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.07);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(0, 0, 0);
		glVertex3f(0, -0.05, 0);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(-0.4, -0.05, 0.14);
		glVertex3f(-0.4, 0, 0.14);
		glVertex3f(-0.4, -0.02, 0.07);
		glVertex3f(-0.4, -0.05, 0.07);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(-0.4, -0.05, 0.07);
		glVertex3f(-0.4, -0.02, 0.07);
		glVertex3f(-0.4, 0, 0);
		glVertex3f(-0.4, -0.05, 0);
		glEnd();

	}

	if (count_of_strings == 2)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.14);
		glVertex3f(-0.4, -0.05, 0.14);
		glVertex3f(0, 0, 0.07);
		glVertex3f(-0.4, 0, 0.07);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, 0, 0.07);
		glVertex3f(-0.4, 0, 0.07);
		glVertex3f(0, -0.05, 0);
		glVertex3f(-0.4, -0.05, 0);
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.14);
		glVertex3f(0, 0, 0.07);
		glVertex3f(0, -0.05, 0);
		glEnd();


		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(-0.4, -0.05, 0.14);
		glVertex3f(-0.4, 0, 0.07);
		glVertex3f(-0.4, -0.05, 0);
		glEnd();
	}

	if (count_of_strings == 3)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.14);
		glVertex3f(-0.4, -0.05, 0.14);
		glVertex3f(0, 0, 0.093);
		glVertex3f(-0.4, 0, 0.093);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, 0, 0.093);
		glVertex3f(-0.4, 0, 0.093);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(-0.4, -0.02, 0.07);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(-0.4, -0.02, 0.07);
		glVertex3f(0, 0, 0.046);
		glVertex3f(-0.4, 0, 0.046);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, 0, 0.046);
		glVertex3f(-0.4, 0, 0.046);
		glVertex3f(0, -0.05, 0);
		glVertex3f(-0.4, -0.05, 0);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.14);
		glVertex3f(0, 0, 0.093);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(0, -0.05, 0.07);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(0, -0.05, 0.07);
		glVertex3f(0, -0.02, 0.07);
		glVertex3f(0, 0, 0.046);
		glVertex3f(0, -0.05, 0);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(-0.4, -0.05, 0.14);
		glVertex3f(-0.4, 0, 0.093);
		glVertex3f(-0.4, -0.02, 0.07);
		glVertex3f(-0.4, -0.05, 0.07);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(-0.4, -0.05, 0.07);
		glVertex3f(-0.4, -0.02, 0.07);
		glVertex3f(-0.4, 0, 0.046);
		glVertex3f(-0.4, -0.05, 0);
		glEnd();
	}
}

void DrawDamperArray()
{
	char keys[] = { 'a','w','s','e','d','f','t','g','y','h','u','j' };
	//'j', 'u', 'h', 'y', 'g', 't', 'f', 'd', 'e', 's', 'w','a'
	int octave_counter = -4;
	int key_position = 9;
	//зона 1
	for (int i = 0; i < 19; i++)
	{
		glPushMatrix();
		glTranslatef(0.2, -0.707 - i * 0.14 - 0.2, 9.3);
		glRotatef(-90, 1, 0, 0);
		glRotatef(11 + 180, 0, 1, 0);
		if (key_position == 12)
		{
			key_position = 0;
			octave_counter++;
		}
		if (i < 8)
			DrawDamper(1, keys[key_position], octave_counter);
		else
			DrawDamper(2, keys[key_position], octave_counter);
		key_position++;
		glPopMatrix();
	}

	//зона 2
	for (int i = 0; i < 31; i++)
	{
		glPushMatrix();
		glTranslatef(0.2, -3.75 - i * 0.14, 9);
		glRotatef(-90, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		if (key_position == 12)
		{
			key_position = 0;
			octave_counter++;
		}
		if (i < 6)
			DrawDamper(2, keys[key_position], octave_counter);
		else
			DrawDamper(3, keys[key_position], octave_counter);
		key_position++;
		glPopMatrix();
	}

	//зона 3
	for (int i = 0; i < 17; i++)
	{
		glPushMatrix();
		glTranslatef(0.2, -8.31 - i * 0.14, 9);
		glRotatef(-90, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		if (key_position == 12)
		{
			key_position = 0;
			octave_counter++;
		}
		DrawDamper(3, keys[key_position], octave_counter);
		key_position++;
		glPopMatrix();
	}
}

void DrawString(int count_of_strings, float startx, float starty, float endx, float endy, float z, bool isLonger)
{
	const float shift = 0.1;
	const float shifty = 0.05;
	if (count_of_strings == 1)
	{
		glBegin(GL_LINES);
		glLineWidth(0.02);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(startx, starty, z);
		glVertex3f(endx, endy, z);
		glEnd();
	}
	if (count_of_strings == 2)
	{
		glBegin(GL_LINES);
		glLineWidth(0.02);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(startx, starty - 0.04, z);
		glVertex3f(endx, endy - 0.04, z);
		glEnd();

		glBegin(GL_LINES);
		glLineWidth(0.02);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(startx, starty, z);
		glVertex3f(endx, endy, z);
		glEnd();
	}

	if (count_of_strings == 3)
	{
		glBegin(GL_LINES);
		glLineWidth(0.01);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(startx, starty - 0.03, z);
		glVertex3f(endx, endy - 0.03, z);
		glEnd();

		glBegin(GL_LINES);
		glLineWidth(0.01);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(startx, starty - 0.06, z);
		glVertex3f(endx, endy - 0.06, z);
		glEnd();

		glBegin(GL_LINES);
		glLineWidth(0.01);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(startx, starty - 0.09, z);
		glVertex3f(endx, endy - 0.09, z);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z - 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z - 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.09 + (float)(isLonger ? shifty : 0), z - 0.1);
	glVertex3f(endx + 0.1 - (float)(isLonger ? shift : 0), endy - 0.03 + (float)(isLonger ? shifty : 0), z - 0.1);
	glEnd();
}



float GetDistanceBetweenStringsFinalPoints(Point* needed_points, int needed_points_length, int count_of_strings)
{
	float sum_of_distances = 0, result;
	for (int i = 0; i < needed_points_length - 1; i++)
	{
		sum_of_distances += GetDistance(needed_points[i], needed_points[i + 1]);
	}
	return sum_of_distances / count_of_strings;
}

Point* GetArrayOfStringsFinalPoints(Point* whole_points, int whole_points_length, int count_of_strings, float leftest_point_y, float rightest_point_y)
{
	Point* needed_points = NULL;
	Point* result = NULL;
	int needed_points_length = 0, result_length = 0;
	float distance_between_strings, distance_iteration_buffer;

	for (int i = 0; i < whole_points_length; i++)
	{
		if (whole_points[i].y <= leftest_point_y && whole_points[i].y >= rightest_point_y)
		{
			needed_points_length++;
			needed_points = (Point*)realloc(needed_points, needed_points_length * sizeof(Point));
			needed_points[needed_points_length - 1] = whole_points[i];
		}
	}
	/*printf("-------START\n");
	for (int i = 0; i < needed_points_length; i++)
		printf("%f,%f\n", needed_points[i].x, needed_points[i].y);
	printf("-------END\n");*/
	distance_between_strings = GetDistanceBetweenStringsFinalPoints(needed_points, needed_points_length, count_of_strings);
	distance_iteration_buffer = distance_between_strings - 0.1;
	for (int i = 0; i < needed_points_length - 1; i++)
	{
		distance_iteration_buffer -= GetDistance(needed_points[i], needed_points[i + 1]);
		if (distance_iteration_buffer <= 0)
		{
			result_length++;
			result = (Point*)realloc(result, result_length * sizeof(Point));
			result[result_length - 1] = needed_points[i + 1];
			distance_iteration_buffer = distance_between_strings + distance_iteration_buffer;
		}
	}
	if (result_length < count_of_strings)
	{
		result_length++;
		result = (Point*)realloc(result, result_length * sizeof(Point));
		result[result_length - 1] = needed_points[needed_points_length - 1];
	}
	free(needed_points);
	return result;
}

void DrawStringsArray(Point* deck_points, int count_of_deck_points)
{
	//зона 1
	Point* need_deck_points = NULL;
	for (int i = 0; i < 19; i++)
	{
		if (i < 8)
			DrawString(1, 0, -0.707 - i * 0.14 - 0.2 - 0.05, 14 - 1 * (i / 19.), -0.707 - i * 0.14 - 0.2 - 2.7 - 0.03, 9.3, true);
		else
			DrawString(2, 0, -0.707 - i * 0.14 - 0.2 - 0.01, 14 - 1 * (i / 19.), -0.707 - i * 0.14 - 0.2 - 2.7, 9.3, true);
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(14.1, -0.707 - 0.2 - 2.5, 9.2);
	glVertex3f(13.9, -0.707 - 0.2 - 2.5, 9.2);
	glVertex3f(13.1, -0.707 - 2.66 - 0.2 - 2.5, 9.2);
	glVertex3f(12.9, -0.707 - 2.66 - 0.2 - 2.5, 9.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(14.1, -0.707 - 0.2 - 2.5, 8.9);
	glVertex3f(13.9, -0.707 - 0.2 - 2.5, 8.9);
	glVertex3f(14.1, -0.707 - 0.2 - 2.5, 9.2);
	glVertex3f(13.9, -0.707 - 0.2 - 2.5, 9.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(13.1, -0.707 - 2.66 - 0.2 - 2.5, 9.2);
	glVertex3f(12.9, -0.707 - 2.66 - 0.2 - 2.5, 9.2);
	glVertex3f(13.1, -0.707 - 2.66 - 0.2 - 2.5, 8.9);
	glVertex3f(12.9, -0.707 - 2.66 - 0.2 - 2.5, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(14.1, -0.707 - 0.2 - 2.5, 9.2);
	glVertex3f(13.1, -0.707 - 2.66 - 0.2 - 2.5, 9.2);
	glVertex3f(14.1, -0.707 - 0.2 - 2.5, 8.9);
	glVertex3f(13.1, -0.707 - 2.66 - 0.2 - 2.5, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(13.9, -0.707 - 0.2 - 2.5, 9.2);
	glVertex3f(12.9, -0.707 - 2.66 - 0.2 - 2.5, 9.2);
	glVertex3f(13.9, -0.707 - 0.2 - 2.5, 8.9);
	glVertex3f(12.9, -0.707 - 2.66 - 0.2 - 2.5, 8.9);
	glEnd();

	//зона 2
	need_deck_points = GetArrayOfStringsFinalPoints(deck_points, count_of_deck_points, 31, -0.707, -8.11);	//0.2, -3.75 - i * 0.14, 9
	for (int i = 0; i < 31; i++)
		if (i < 6)
			DrawString(2, 0.2, -3.75 - i * 0.14 - 0.11, need_deck_points[i].x, need_deck_points[i].y, 9, false);			//-8.11 + i * 0.14 
		else
			DrawString(3, 0.2, -3.75 - i * 0.14 - 0.07 + 0.07*((i - 6) / 25.), need_deck_points[i].x, need_deck_points[i].y, 9, false);
	free(need_deck_points);
	//зона 3
	need_deck_points = GetArrayOfStringsFinalPoints(deck_points, count_of_deck_points, 17, -8.31, -10.6236);
	for (int i = 0; i < 17; i++)
		DrawString(3, 0.2, -8.31 - i * 0.14, need_deck_points[i].x, need_deck_points[i].y, 9, false);				//-10.6236 + i * 0.14
	free(need_deck_points);
	//зона 4
	need_deck_points = GetArrayOfStringsFinalPoints(deck_points, count_of_deck_points, 21, -10.8, -13.986);
	for (int i = 0; i < 21; i++)
		DrawString(3, 0.2, -10.8 - i * 0.14, need_deck_points[i].x, need_deck_points[i].y, 9, false);					//-14.293 + i * 0.14
	free(need_deck_points);
}

Point* GetDeckPointsArray(Point* arched_points, int count_of_arched, Point* concave_point, int count_of_concave)
{
	Point* result = (Point*)malloc((count_of_arched + count_of_concave) * sizeof(Point));
	for (int i = count_of_concave - 1; i >= 0; i--)
		result[i] = concave_point[count_of_concave - i - 1];
	for (int i = 0; i < count_of_arched; i++)
		result[count_of_concave + i] = arched_points[i];
	return result;
}

void DrawSupport()
{

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.2);
	glVertex3f(0, -0.5, 0.2);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.5, 0.2);
	glVertex3f(-1, -0.5, 0.2);
	glVertex3f(0, -0.5, 0);
	glVertex3f(-1, -0.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.2);
	glVertex3f(-1, 0, 0.2);
	glVertex3f(0, 0, 0);
	glVertex3f(-1, 0, 0);
	glEnd();

	int count_of_elems_concave;
	ArcCustom concave = ArcCustom(-1, 0, -2, -0.1, 2);
	Point* concave_points = NULL;
	concave.GenerateArc(concave_points, count_of_elems_concave, false);

	int count_of_elems_concave2;
	ArcCustom concave2 = ArcCustom(-1, -0.5, -2, -0.4, 2);
	Point* concave2_points = NULL;
	concave2.GenerateArc(concave2_points, count_of_elems_concave2);

	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.2);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 0);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 0.2);
		glEnd();

	}

	for (i = 0; i < count_of_elems_concave2 - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 0);
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 0.2);
		glVertex3f(concave2_points[i + 1].x, concave2_points[i + 1].y, 0);
		glVertex3f(concave2_points[i + 1].x, concave2_points[i + 1].y, 0.2);
		glEnd();

	}

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -0.1, 0.2);
	glVertex3f(-8, -0.3, 0.2);
	glVertex3f(-2, -0.1, 0);
	glVertex3f(-8, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -0.4, 0.2);
	glVertex3f(-8, -0.3, 0.2);
	glVertex3f(-2, -0.4, 0);
	glVertex3f(-8, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(concave_points[0].x, concave_points[0].y, 0.2);
	glVertex3f(-2, -0.1, 0.2);
	glVertex3f(concave_points[0].x, concave_points[0].y, 0);
	glVertex3f(-2, -0.1, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(concave2_points[0].x, concave2_points[0].y, 0.2);
	glVertex3f(-1, -0.5, 0.2);
	glVertex3f(concave2_points[0].x, concave2_points[0].y, 0);
	glVertex3f(-1, -0.5, 0);
	glEnd();

	glaStencilWindow(1);

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.2);
	}
	glVertex3f(-2, -0.1, 0.2);
	glEnd();

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave2; i++)
	{
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 0.2);
	}
	glVertex3f(-1, -0.5, 0.2);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_POLYGON);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);

	glVertex3f(-2, -0.1, 0.2);
	glVertex3f(-1, 0, 0.2);
	glVertex3f(-1, -0.5, 0.2);
	glVertex3f(-2, -0.4, 0.2);
	glEnd();

	glaStencilEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0.2);
	glVertex3f(0, -0.5, 0.2);
	glVertex3f(-1, 0, 0.2);
	glVertex3f(-1, -0.5, 0.2);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glLineWidth(0.01);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -0.4, 0.2);
	glVertex3f(-8, -0.3, 0.2);
	glVertex3f(-2, -0.1, 0.2);
	glEnd();

	glaStencilWindow(1);

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
	}
	glVertex3f(-2, -0.1, 0);
	glEnd();

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave2; i++)
	{
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 0);
	}
	glVertex3f(-1, -0.5, 0);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);

	glVertex3f(-2, -0.1, 0);
	glVertex3f(-1, 0, 0);
	glVertex3f(-1, -0.5, 0);
	glVertex3f(-2, -0.4, 0);
	glEnd();

	glaStencilEnd();

	glBegin(GL_QUAD_STRIP);
	glLineWidth(0.01);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glVertex3f(-1, 0, 0);
	glVertex3f(-1, -0.5, 0);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glLineWidth(0.01);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -0.4, 0);
	glVertex3f(-8, -0.3, 0);
	glVertex3f(-2, -0.1, 0);
	glEnd();

	free(concave_points);
	free(concave2_points);
}

void DrawCover()
{
	ArcCustom arched = ArcCustom(16.0, -7.5, 16.0, 0, 9);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	ArcCustom concave = ArcCustom(16.0, -7.5, 4.3, -15.0, 16);
	Point* concave_points = NULL;
	int count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave);

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(16.0, 0, 0);
	glVertex3f(0, 0, 0.4);
	glVertex3f(16.0, 0, 0.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -15.0, 0);
	glVertex3f(0, 0, 0.4);
	glVertex3f(0, -15.0, 0.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -15.0, 0);
	glVertex3f(4.3, -15.0, 0);
	glVertex3f(0, -15.0, 0.4);
	glVertex3f(4.3, -15.0, 0.4);
	glEnd();


	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.4);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 0);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 0.4);
		glEnd();


	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(concave_points[0].x, concave_points[0].y, 0);
	glVertex3f(arched_points[0].x, arched_points[0].y, 0);
	glVertex3f(concave_points[0].x, concave_points[0].y, 0.4);
	glVertex3f(arched_points[0].x, arched_points[0].y, 0.4);
	glEnd();


	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.4);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.4);
		glEnd();


	}

	glaStencilWindow(1);

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
	}
	glVertex3f(16.0, -7.5, 0);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -15.0, 0);
	glVertex3f(4.3, -15.0, 0);
	glVertex3f(16.0, 0, 0);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
	}
	glVertex3f(0, 0, 0);
	glEnd();

	glaStencilEnd();

	glaStencilWindow(1);

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.4);
	}
	glVertex3f(16.0, -7.5, 0.4);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -15.0, 0.4);
	glVertex3f(4.3, -15.0, 0.4);
	glVertex3f(16.0, 0, 0.4);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.4);
	}
	glVertex3f(0, 0, 0.4);
	glEnd();

	glaStencilEnd();

	free(concave_points);
	free(arched_points);
}

void DrawHolders()
{
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	ArcCustom arched = ArcCustom(0, 0, -0.507, 0, 0.26);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.25);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.5);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.25);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.5);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.25);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.5);
	}
	glEnd();

	free(arched_points);

	arched = ArcCustom(0, 0, -0.507, 0, 0.26);
	arched_points = NULL;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.25);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1.25);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 1);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.25);
	}
	glEnd();

	free(arched_points);

	glPopMatrix();
}

void GetPainting(Point3D* points, float center_x, float center_y, float center_z, float radius, float angle)
{
	float step = (2 * pi) / 360, x_buffer, y_buffer, z_buffer;
	for (int i = 0; i < 360; i++)
	{
		points[i].x = radius * cos(i*step) + center_x;
		points[i].y = radius * sin(i*step) + center_y;
		points[i].z = center_z;
	}
	for (int i = 0; i < 360; i++)
	{
		x_buffer = points[i].x;
		z_buffer = points[i].z;
		y_buffer = points[i].y;
		points[i].y = cos(angle*pi / 180.)*y_buffer + cos(angle*pi / 180.)*z_buffer;
		points[i].z = -sin(angle*pi / 180.)*z_buffer + sin(angle*pi / 180.)*y_buffer;
	}
}


void DiscHolder::DrawDiscHolder(int& requested_holder, bool &plug, bool& unplug)
{

	char filepath[30] = "jukebox_sound/n.mp3";
	filepath[14] = serial_number + '0';

	glPushMatrix();									//поворот на соответствующий угол
	glTranslatef(0.15, 0.25, 0);
	glRotatef((serial_number - 1) * 45, 0, 0, 1);
	glTranslatef(-0.15, -0.25, 0);


	glPushMatrix();									//дуги
	glTranslatef(0.3, -1.7, 0);
	glRotatef(90, 1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	ArcCustom arched = ArcCustom(-0.7, 0, -1, 0, 0.15);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.05);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.05);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.05);
	}
	glEnd();

	free(arched_points);

	arched = ArcCustom(-0.7, 0, -1, 0, 0.15);
	arched_points = NULL;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.25);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.3);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.25);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.3);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.25);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.3);
	}
	glEnd();

	free(arched_points);

	glPopMatrix();

	glPushMatrix();
	if (requested_holder == serial_number && (plug || unplug))
	{

		if (unplug || (unplug && plug))
		{
			isDiscPlugged = false;
			if (angle < 0)
			{
				angle += speed_of_spin;
				lower_holder_height -= 0.0375;
				printf("unplug\n");
				se->stopAllSounds();
			}
			else
			{
				unplug = false;
				printf("unplug finished\n");
			}
		}
		else if (plug)
		{
			isJukeboxStopped = false;
			if (angle > -60)
			{
				angle -= speed_of_spin;
				lower_holder_height += 0.0375;
				printf("plug\n");
			}
			else
			{
				plug = false;
				isDiscPlugged = true;
				printf("plug finished\n");
				se->play2D(filepath);
			}
		}
	}
	glTranslatef(0.16, -0.85, 0.2);
	glRotatef(-angle, 1, 0, 0);
	glTranslatef(-0.16, 0.85, -0.2);

	glBegin(GL_QUAD_STRIP);							//нижний держатель(он должен изменять высоту при движении)
	glColor3f(0.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -0.7, lower_holder_height);
	glVertex3f(0.25, -0.7, lower_holder_height);
	glVertex3f(0.05, -1, lower_holder_height);
	glVertex3f(0.25, -1, lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -0.7, lower_holder_height);
	glVertex3f(0.25, -0.7, lower_holder_height);
	glVertex3f(0.05, -0.7, 0.1);
	glVertex3f(0.25, -0.7, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -1, lower_holder_height);
	glVertex3f(0.25, -1, lower_holder_height);
	glVertex3f(0.05, -1, 0.1);
	glVertex3f(0.25, -1, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.25, -0.7, lower_holder_height);
	glVertex3f(0.25, -1, lower_holder_height);
	glVertex3f(0.25, -0.7, 0.1);
	glVertex3f(0.25, -1, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -0.7, lower_holder_height);
	glVertex3f(0.05, -1, lower_holder_height);
	glVertex3f(0.05, -0.7, 0.1);
	glVertex3f(0.05, -1, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -0.7, 0.1);
	glVertex3f(0.25, -0.7, 0.1);
	glVertex3f(0.05, -1, 0.1);
	glVertex3f(0.25, -1, 0.1);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//основной держатель (левый)
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -0.7, 1 + lower_holder_height);
	glVertex3f(0.14, -0.7, 1 + lower_holder_height);
	glVertex3f(0.05, -1, 1 + lower_holder_height);
	glVertex3f(0.14, -1, 1 + lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -1, lower_holder_height);
	glVertex3f(0.14, -1, lower_holder_height);
	glVertex3f(0.05, -1, 1 + lower_holder_height);
	glVertex3f(0.14, -1, 1 + lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -0.7, 1 + lower_holder_height);
	glVertex3f(0.14, -0.7, 1 + lower_holder_height);
	glVertex3f(0.05, -0.7, lower_holder_height);
	glVertex3f(0.14, -0.7, lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.05, -0.7, 1 + lower_holder_height);
	glVertex3f(0.05, -1, 1 + lower_holder_height);
	glVertex3f(0.05, -0.7, lower_holder_height);
	glVertex3f(0.05, -1, lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.14, -0.7, 1 + lower_holder_height);
	glVertex3f(0.14, -1, 1 + lower_holder_height);
	glVertex3f(0.14, -0.7, lower_holder_height);
	glVertex3f(0.14, -1, lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//основной держатель (правый)
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.25, -0.7, 1 + lower_holder_height);
	glVertex3f(0.16, -0.7, 1 + lower_holder_height);
	glVertex3f(0.25, -1, 1 + lower_holder_height);
	glVertex3f(0.16, -1, 1 + lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.25, -1, lower_holder_height);
	glVertex3f(0.16, -1, lower_holder_height);
	glVertex3f(0.25, -1, 1 + lower_holder_height);
	glVertex3f(0.16, -1, 1 + lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.25, -0.7, 1 + lower_holder_height);
	glVertex3f(0.16, -0.7, 1 + lower_holder_height);
	glVertex3f(0.25, -0.7, lower_holder_height);
	glVertex3f(0.16, -0.7, lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.25, -0.7, 1 + lower_holder_height);
	glVertex3f(0.25, -1, 1 + lower_holder_height);
	glVertex3f(0.25, -0.7, lower_holder_height);
	glVertex3f(0.25, -1, lower_holder_height);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.16, -0.7, 1 + lower_holder_height);
	glVertex3f(0.16, -1, 1 + lower_holder_height);
	glVertex3f(0.16, -0.7, lower_holder_height);
	glVertex3f(0.16, -1, lower_holder_height);
	glEnd();

	glPushMatrix();
	if (isDiscPlugged)
	{
		DiscAngle += 60;
		if (DiscAngle >= 360)
			DiscAngle = 0;
	}
	glTranslatef(0.16, -0.85, lower_holder_height + 0.95);
	glRotatef(DiscAngle, 1, 0, 0);
	glTranslatef(-0.16, 0.85, -lower_holder_height - 0.95);

	center_of_circle_3D.x = 0.16;							//диск (должен крутиться когда установлен)
	center_of_circle_3D.y = -0.85;
	center_of_circle_3D.z = lower_holder_height + 0.95;
	GetCircleYZ(circle_points_3D, center_of_circle_3D, 0.9);

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.02, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.02, circle_points_3D[i].z);
		glEnd();
	}
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}



void DrawDiscPlatform()
{
	center_of_circle.x = 0;
	center_of_circle.y = 0;
	GetCircle(circle_points, center_of_circle, 1.25);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 0.0f);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.3);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.3);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.3);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.3);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0.3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.3);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.3);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0.3);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.3);
	glEnd();


	glPushMatrix();
	glTranslatef(-0.15, -0.25, 0.3);
	for (int i = 0; i < 8; i++)
		disc_holders[i].DrawDiscHolder(current_disc, plug, unplug);
	glPopMatrix();
}

void DrawDiscReader()
{
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1.17);
	glVertex3f(0, -0.64, 0.53);
	glVertex3f(0, -0.64, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0.3, 0, 1.17);
	glVertex3f(0.3, -0.64, 0.53);
	glVertex3f(0.3, -0.64, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.3, -0.64, 0);
	glVertex3f(0, -0.64, 0);
	glVertex3f(0.3, -0.64, 0.53);
	glVertex3f(0, -0.64, 0.53);


	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0.3, 0, 1.17);
	glVertex3f(0, 0, 1.17);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.3, 0, 1.17);
	glVertex3f(0, 0, 1.17);
	glVertex3f(0, -0.64, 0.53);
	glVertex3f(0.3, -0.64, 0.53);

	glEnd();
}

void DrawChairHolders()
{

	ArcCustom arched = ArcCustom(0, 0, -0.28, 0, 0.14);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.05);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.05);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.05);
	}
	glEnd();

	free(arched_points);

	arched = ArcCustom(0, 0, -0.28, 0, 0.14);
	arched_points = NULL;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.3);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.35);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.3);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0.35);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.3);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.35);
	}
	glEnd();

	free(arched_points);
}

void GeneratePixelRepresentanionOfCurrentPixels(bool repr[5][6])
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	int current_seconds = ltm->tm_sec;
	bool digits_pixel_representation[10][5][3] =
	{ {{1,1,1},{1,0,1},{1,0,1},{1,0,1},{1,1,1}},						//0
	  {{0,0,1},{0,0,1},{0,0,1} ,{0,0,1} ,{0,0,1}},						//1
	  {{1,1,1},{0,0,1},{1,1,1},{1,0,0},{1,1,1}},						//2
	  {{1,1,1},{0,0,1},{1,1,1},{0,0,1},{1,1,1}},						//3
	  {{1,0,1},{1,0,1},{1,1,1},{0,0,1},{0,0,1}},						//4
	  {{1,1,1},{1,0,0},{1,1,1},{0,0,1},{1,1,1}},						//5
	  {{1,1,1},{1,0,0},{1,1,1},{1,0,1},{1,1,1}},						//6
	  {{1,1,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},						//7
	  {{1,1,1},{1,0,1},{1,1,1},{1,0,1},{1,1,1}},						//8
	  {{1,1,1},{1,0,1},{1,1,1},{0,0,1},{1,1,1}}							//9
	};
	int seconds_in_digits[2];
	seconds_in_digits[1] = current_seconds % 10;
	current_seconds /= 10;
	seconds_in_digits[0] = current_seconds % 10;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			repr[i][j] = digits_pixel_representation[seconds_in_digits[0]][i][j];
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			repr[i][j + 3] = digits_pixel_representation[seconds_in_digits[1]][i][j];
}

void DrawMonitorCRT(GLuint texture)
{
	Point header_circle_points[360];

	glBegin(GL_QUAD_STRIP);					//рамка монитора
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 0.99);
	glVertex3f(0, 0, 4.42);
	glVertex3f(0, -4.42, 4.42);
	glVertex3f(0, -4.42, 0.99);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(1.2, 0, 0.99);
	glVertex3f(1.2, 0, 4.12);
	glVertex3f(1.2, -4.42, 0.99);
	glVertex3f(1.2, -4.42, 4.12);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 0.99);
	glVertex3f(0, 0, 4.42);
	glVertex3f(1.2, 0, 0.99);
	glVertex3f(1.2, 0, 4.12);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(1.2, -4.42, 4.12);
	glVertex3f(1.2, -4.42, 0.99);
	glVertex3f(0, -4.42, 4.42);
	glVertex3f(0, -4.42, 0.99);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(1.2, 0, 0.99);
	glVertex3f(1.2, -4.42, 0.99);
	glVertex3f(0, 0, 0.99);
	glVertex3f(0, -4.42, 0.99);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 4.42);
	glVertex3f(0, -4.42, 4.42);
	glVertex3f(1.2, 0, 4.12);
	glVertex3f(1.2, -4.42, 4.12);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUAD_STRIP);					//экран (для текстур)
	glTexCoord2f(0, 0);
	glVertex3f(-0.01, -0.265, 1.255);
	glTexCoord2f(1, 0);
	glVertex3f(-0.01, -4.155, 1.255);
	glTexCoord2f(1, 1);
	glVertex3f(-0.01, -0.265, 4.155);
	glTexCoord2f(0, 1);
	glVertex3f(-0.01, -4.155, 4.155);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUAD_STRIP);					//панель для кнопок и охладителя
	glColor3d(1, 1, 1);
	glVertex3f(0.4, -0.265, 0.495);
	glVertex3f(0.4, -0.265, 1.279);
	glVertex3f(0.4, -4.155, 0.495);
	glVertex3f(0.4, -4.155, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(3.6, -0.265, 0.495);
	glVertex3f(3.6, -0.265, 1.279);
	glVertex3f(3.6, -4.155, 0.495);
	glVertex3f(3.6, -4.155, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.4, -4.155, 0.495);
	glVertex3f(0.4, -4.155, 1.279);
	glVertex3f(3.6, -4.155, 0.495);
	glVertex3f(3.6, -4.155, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(3.6, -0.265, 0.495);
	glVertex3f(3.6, -0.265, 1.279);
	glVertex3f(0.4, -0.265, 0.495);
	glVertex3f(0.4, -0.265, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(3.6, -0.265, 1.279);
	glVertex3f(3.6, -4.155, 1.279);
	glVertex3f(0.4, -0.265, 1.279);
	glVertex3f(0.4, -4.155, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(3.6, -0.265, 0.495);
	glVertex3f(3.6, -4.155, 0.495);
	glVertex3f(0.4, -0.265, 0.495);
	glVertex3f(0.4, -4.155, 0.495);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//задний "бак"
	glColor3d(1, 0, 1);
	glVertex3f(1.2, 0, 3.679);
	glVertex3f(1.2, -4.42, 3.679);
	glVertex3f(4.42, -0.265, 3.279);
	glVertex3f(4.42, -4.155, 3.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(1.2, 0, 0.99);
	glVertex3f(1.2, -4.42, 0.99);
	glVertex3f(4.42, -0.265, 1.279);
	glVertex3f(4.42, -4.155, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(4.42, -0.265, 3.279);
	glVertex3f(4.42, -4.155, 3.279);
	glVertex3f(4.42, -0.265, 1.279);
	glVertex3f(4.42, -4.155, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(1.2, 0, 3.679);
	glVertex3f(4.42, -0.265, 3.279);
	glVertex3f(1.2, 0, 0.99);
	glVertex3f(4.42, -0.265, 1.279);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(1.2, -4.42, 3.679);
	glVertex3f(4.42, -4.155, 3.279);
	glVertex3f(1.2, -4.42, 0.99);
	glVertex3f(4.42, -4.155, 1.279);
	glEnd();

	center_of_circle.x = 1.2;										//нижняя часть
	center_of_circle.y = -2.21;
	GetCircle(circle_points, center_of_circle, 1.35);
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(0, 0, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(0, 0, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	GetCircle(header_circle_points, center_of_circle, 0.3);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glNormal3f(-2, 4, 0);
		glVertex3f(header_circle_points[i].x, header_circle_points[i].y, 1);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(header_circle_points[i + 1].x, header_circle_points[i + 1].y, 1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glNormal3f(-2, 4, 0);
	glVertex3f(header_circle_points[0].x, header_circle_points[0].y, 1);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(header_circle_points[359].x, header_circle_points[359].y, 1);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();


	GetCircle(circle_points, center_of_circle, 1);

	for (int i = 0; i < 359; i++)												//петля
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.5);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.2);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.5);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.2);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.5);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.5);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.2);
	glEnd();
}

void DrawMonitorLC(GLuint texture)
{
	glBegin(GL_QUAD_STRIP);					//рамка монитора
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 0.6);
	glVertex3f(0, 0, 3.9);
	glVertex3f(0, -5, 0.6);
	glVertex3f(0, -5, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.2, 0, 0.6);
	glVertex3f(0.2, 0, 3.9);
	glVertex3f(0.2, -5, 0.6);
	glVertex3f(0.2, -5, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.2, 0, 0.6);
	glVertex3f(0.2, 0, 3.9);
	glVertex3f(0, 0, 0.6);
	glVertex3f(0, 0, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, -5, 3.9);
	glVertex3f(0, -5, 0.6);
	glVertex3f(0.2, -5, 3.9);
	glVertex3f(0.2, -5, 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 0.6);
	glVertex3f(0, -5, 0.6);
	glVertex3f(0.2, 0, 0.6);
	glVertex3f(0.2, -5, 0.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 3.9);
	glVertex3f(0, -5, 3.9);
	glVertex3f(0.2, 0, 3.9);
	glVertex3f(0.2, -5, 3.9);
	glEnd();


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUAD_STRIP);					//экран (ДЛЯ ТЕКСТУР)
	glTexCoord2f(0, 0);
	glVertex3f(-0.01, 0, 0.9);
	glTexCoord2f(0, 1);
	glVertex3f(-0.01, 0, 3.9);
	glTexCoord2f(1, 0);
	glVertex3f(-0.01, -5, 0.9);
	glTexCoord2f(1, 1);
	glVertex3f(-0.01, -5, 3.9);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUAD_STRIP);					//ножка и место для проводов
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.35, 1);
	glVertex3f(0.2, -2.35, 0.2);
	glVertex3f(0.2, -2.65, 1);
	glVertex3f(0.2, -2.65, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.4, -2.35, 1);
	glVertex3f(0.4, -2.35, 0.2);
	glVertex3f(0.4, -2.65, 1);
	glVertex3f(0.4, -2.65, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.65, 1);
	glVertex3f(0.2, -2.65, 0.2);
	glVertex3f(0.4, -2.65, 1);
	glVertex3f(0.4, -2.65, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.4, -2.35, 1);
	glVertex3f(0.4, -2.35, 0.2);
	glVertex3f(0.2, -2.35, 1);
	glVertex3f(0.2, -2.35, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.35, 0.2);
	glVertex3f(0.2, -2.65, 0.2);
	glVertex3f(0.4, -2.35, 0.2);
	glVertex3f(0.4, -2.65, 0.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.35, 1);
	glVertex3f(0.2, -2.65, 1);
	glVertex3f(0.4, -2.35, 1);
	glVertex3f(0.4, -2.65, 1);
	glEnd();

	center_of_circle.x = 0.3;										//нижняя часть
	center_of_circle.y = -2.5;
	GetCircle(circle_points, center_of_circle, 0.7);
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(0, 0, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(0, 0, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(0, 0, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.2);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0.2);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.2);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(0, 0, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.2);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.2);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.2);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.2);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.2);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	GetCircle(circle_points, center_of_circle, 0.2);

	for (int i = 0; i < 359; i++)												//петля
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.3);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.2);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.2);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.3);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.3);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.2);
	glEnd();
}

void DrawMonitorLCC(GLuint texture)
{
	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 0.5);
	glVertex3f(0, 0, 3.93);
	glVertex3f(0, -5.17, 0.5);
	glVertex3f(0, -5.17, 3.93);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.2, 0, 0.5);
	glVertex3f(0.2, 0, 3.93);
	glVertex3f(0.2, -5.17, 0.5);
	glVertex3f(0.2, -5.17, 3.93);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.2, 0, 0.5);
	glVertex3f(0.2, 0, 3.93);
	glVertex3f(0, 0, 0.5);
	glVertex3f(0, 0, 3.93);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, -5.17, 3.93);
	glVertex3f(0, -5.17, 0.5);
	glVertex3f(0.2, -5.17, 3.93);
	glVertex3f(0.2, -5.17, 0.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.2, 0, 3.93);
	glVertex3f(0.2, -5.17, 3.93);
	glVertex3f(0, 0, 3.93);
	glVertex3f(0, -5.17, 3.93);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 0.5);
	glVertex3f(0, -5.17, 0.5);
	glVertex3f(0.2, 0, 0.5);
	glVertex3f(0.2, -5.17, 0.5);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUAD_STRIP);				//экран (для текстур)
	glTexCoord2d(0, 0);
	glVertex3f(-0.01, 0, 0.7);
	glTexCoord2d(0, 1);
	glVertex3f(-0.01, 0, 3.93);
	glTexCoord2d(1, 0);
	glVertex3f(-0.01, -5.17, 0.7);
	glTexCoord2d(1, 1);
	glVertex3f(-0.01, -5.17, 3.93);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUAD_STRIP);					//ножка и место для проводов
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.35, 1);
	glVertex3f(0.2, -2.35, 0.1);
	glVertex3f(0.2, -2.65, 1);
	glVertex3f(0.2, -2.65, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.4, -2.35, 1);
	glVertex3f(0.4, -2.35, 0.1);
	glVertex3f(0.4, -2.65, 1);
	glVertex3f(0.4, -2.65, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.65, 1);
	glVertex3f(0.2, -2.65, 0.1);
	glVertex3f(0.4, -2.65, 1);
	glVertex3f(0.4, -2.65, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.4, -2.35, 1);
	glVertex3f(0.4, -2.35, 0.1);
	glVertex3f(0.2, -2.35, 1);
	glVertex3f(0.2, -2.35, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.35, 0.1);
	glVertex3f(0.2, -2.65, 0.1);
	glVertex3f(0.4, -2.35, 0.1);
	glVertex3f(0.4, -2.65, 0.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 1);
	glVertex3f(0.2, -2.35, 1);
	glVertex3f(0.2, -2.65, 1);
	glVertex3f(0.4, -2.35, 1);
	glVertex3f(0.4, -2.65, 1);
	glEnd();

	center_of_circle.x = -0.3;										//нижняя часть
	center_of_circle.y = -2.5;
	GetCircle(circle_points, center_of_circle, 1);
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(0, 0, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}
	/*glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(0, 0, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();*/

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3d(0, 0, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0.1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.1);
		glEnd();
	}
	/*glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3d(0, 0, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.1);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0.1);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.1);
	glEnd();*/

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.1);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.2);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0);
	glEnd();

	ArcCustom arched = ArcCustom(0, -0.5, 2, -3.93, 4);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched, false);

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3d(1, 1, 1);
		glVertex3f(0, 0, 3.93);
		glVertex3f(-arched_points[i].x, 0, -arched_points[i].y);
		glVertex3f(-arched_points[i + 1].x, 0, -arched_points[i + 1].y);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 3.93);
	glVertex3f(-2, 0, 3.93);
	glVertex3f(-arched_points[count_of_elems_arched - 1].x, 0, -arched_points[count_of_elems_arched - 1].y);
	glEnd();

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3d(1, 1, 1);
		glVertex3f(0, -5.17, 3.93);
		glVertex3f(-arched_points[i].x, -5.17, -arched_points[i].y);
		glVertex3f(-arched_points[i + 1].x, -5.17, -arched_points[i + 1].y);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, -5.17, 3.93);
	glVertex3f(-2, -5.17, 3.93);
	glVertex3f(-arched_points[count_of_elems_arched - 1].x, -5.17, -arched_points[count_of_elems_arched - 1].y);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0, 0, 3.93);
	glVertex3f(-2, 0, 3.93);
	glVertex3f(0, -5.17, 3.93);
	glVertex3f(-2, -5.17, 3.93);
	glEnd();

	free(arched_points);
}

void DrawLampCRT(float height, GLuint texture)
{
	glPushMatrix();
	glTranslatef(0, 0, height);
	glRotatef(170, 0, 1, 0);
	DrawMonitorCRT(texture);
	glPopMatrix();

	center_of_circle.x = 0.25;										//верёвка
	center_of_circle.y = -2.2;
	GetCircle(circle_points, center_of_circle, 0.1);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 30);
		glVertex3f(center_of_circle.x, center_of_circle.y, 30);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 30);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 30);
	glVertex3f(center_of_circle.x, center_of_circle.y, 30);
	glVertex3f(circle_points[359].x, circle_points[359].y, 30);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, height);
		glVertex3f(center_of_circle.x, center_of_circle.y, height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, height);
	glVertex3f(center_of_circle.x, center_of_circle.y, height);
	glVertex3f(circle_points[359].x, circle_points[359].y, height);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 30);
		glVertex3f(circle_points[i].x, circle_points[i].y, height);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 30);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 30);
	glVertex3f(circle_points[0].x, circle_points[0].y, height);
	glVertex3f(circle_points[359].x, circle_points[359].y, 30);
	glVertex3f(circle_points[359].x, circle_points[359].y, height);
	glEnd();



	for (int i = 0; i < 359; i++)												//бок
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x - 0.5, circle_points[i].y, height - 0.5);
		glVertex3f(circle_points[i].x, circle_points[i].y, height);
		glVertex3f(circle_points[i + 1].x - 0.5, circle_points[i + 1].y, height - 0.5);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x - 0.5, circle_points[0].y, height - 0.5);
	glVertex3f(circle_points[0].x, circle_points[0].y, height);
	glVertex3f(circle_points[359].x - 0.5, circle_points[359].y, height - 0.5);
	glVertex3f(circle_points[359].x, circle_points[359].y, height);
	glEnd();


	glPushMatrix();
	glRotatef(-15, 0, 1, 0);



	glPopMatrix();


	Vector3D common_start_point, P2, P3, P4;				//тюльпаны и питание - кривые Безье
	Vector3D bezier_line[100];
	common_start_point.x = 0.25; common_start_point.y = -2.2; common_start_point.z = height + 5;
	P2.x = 1; P2.y = 1.6; P2.z = height + 4;							//жёлтый
	P3.x = -6; P3.y = 2; P3.z = height - 2;
	P4.x = -3; P4.y = -2.4; P4.z = height - 1.3;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3d(1, 1, 0);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}

	P2.x = 1; P2.y = -4; P2.z = height + 4;							//белый
	P3.x = -9; P3.y = -2; P3.z = height - 1.5;
	P4.x = -3; P4.y = -2; P4.z = height - 1.3;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3d(1, 1, 1);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}

	P2.x = 7; P2.y = -4; P2.z = height + 4;							//красный
	P3.x = -6; P3.y = -6; P3.z = height - 1.5;
	P4.x = -3.5; P4.y = -2.2; P4.z = height - 1.3;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3d(1, 0, 0);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}

	P2.x = -3; P2.y = -4; P2.z = height + 4;							//питание
	P3.x = -6; P3.y = -6; P3.z = height - 1.5;
	P4.x = -3.5; P4.y = -2.8; P4.z = height - 1.3;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}
}

void DrawLampLC(float height, GLuint texture)
{
	glPushMatrix();
	glTranslatef(0, 0, height);
	glRotatef(175, 0, 1, 0);
	DrawMonitorLC(texture);
	glPopMatrix();

	center_of_circle.x = 0.5;										//верёвка
	center_of_circle.y = -2.4;
	GetCircle(circle_points, center_of_circle, 0.1);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 30);
		glVertex3f(center_of_circle.x, center_of_circle.y, 30);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 30);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 30);
	glVertex3f(center_of_circle.x, center_of_circle.y, 30);
	glVertex3f(circle_points[359].x, circle_points[359].y, 30);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, height - 0.2);
		glVertex3f(center_of_circle.x, center_of_circle.y, height - 0.2);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height - 0.2);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, height - 0.2);
	glVertex3f(center_of_circle.x, center_of_circle.y, height - 0.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, height - 0.2);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 30);
		glVertex3f(circle_points[i].x, circle_points[i].y, height - 0.2);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 30);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height - 0.2);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 30);
	glVertex3f(circle_points[0].x, circle_points[0].y, height - 0.2);
	glVertex3f(circle_points[359].x, circle_points[359].y, 30);
	glVertex3f(circle_points[359].x, circle_points[359].y, height - 0.2);
	glEnd();

	center_of_circle_3D.x = 0.6;
	center_of_circle_3D.y = -2.4;
	center_of_circle_3D.z = height - 0.2;
	GetCircleYZ(circle_points_3D, center_of_circle_3D, 0.05);

	glBegin(GL_POLYGON);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x - 1, circle_points_3D[i].y, circle_points_3D[i].z - 0.1);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x - 1, circle_points_3D[i].y, circle_points_3D[i].z - 0.1);
		glEnd();
	}


	Vector3D common_start_point, P2, P3, P4;				//HDMI и питание
	Vector3D bezier_line[100];
	common_start_point.x = 0.25; common_start_point.y = -2.43; common_start_point.z = height + 5;
	P2.x = 3; P2.y = -4; P2.z = height;							//HDMI
	P3.x = -6; P3.y = -6; P3.z = height - 1.5;
	P4.x = 0; P4.y = -2.3; P4.z = height - 1;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3d(74 / 255., 118 / 255., 168 / 255.);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}

	P2.x = -3; P2.y = -4; P2.z = height + 4;							//питание
	P3.x = -6; P3.y = 0; P3.z = height - 1.5;
	P4.x = 0; P4.y = -2.4; P4.z = height - 1;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}
}

void DrawLampLCC(float height, GLuint texture)
{
	glPushMatrix();
	glTranslatef(0, 0, height);
	glRotatef(175, 0, 1, 0);
	DrawMonitorLCC(texture);
	glPopMatrix();

	center_of_circle.x = 1.4;										//верёвка
	center_of_circle.y = -2.4;
	GetCircle(circle_points, center_of_circle, 0.1);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 30);
		glVertex3f(center_of_circle.x, center_of_circle.y, 30);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 30);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 30);
	glVertex3f(center_of_circle.x, center_of_circle.y, 30);
	glVertex3f(circle_points[359].x, circle_points[359].y, 30);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, height - 0.1);
		glVertex3f(center_of_circle.x, center_of_circle.y, height - 0.1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height - 0.1);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, height - 0.1);
	glVertex3f(center_of_circle.x, center_of_circle.y, height - 0.1);
	glVertex3f(circle_points[359].x, circle_points[359].y, height - 0.1);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 30);
		glVertex3f(circle_points[i].x, circle_points[i].y, height - 0.1);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 30);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, height - 0.1);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 30);
	glVertex3f(circle_points[0].x, circle_points[0].y, height - 0.1);
	glVertex3f(circle_points[359].x, circle_points[359].y, 30);
	glVertex3f(circle_points[359].x, circle_points[359].y, height - 0.1);
	glEnd();

	center_of_circle_3D.x = 1.4;
	center_of_circle_3D.y = -2.4;
	center_of_circle_3D.z = height - 0.2;
	GetCircleYZ(circle_points_3D, center_of_circle_3D, 0.05);

	glBegin(GL_POLYGON);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z + 0.15);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x - 1.8, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z + 0.15);
		glVertex3f(circle_points_3D[i].x - 1.8, circle_points_3D[i].y, circle_points_3D[i].z);
		glEnd();
	}


	Vector3D common_start_point, P2, P3, P4;				//HDMI и питание
	Vector3D bezier_line[100];
	common_start_point.x = 1.4; common_start_point.y = -2.43; common_start_point.z = height + 5;
	P2.x = 4; P2.y = -4.4; P2.z = height;							//HDMI
	P3.x = -6; P3.y = 6; P3.z = height - 1.5;
	P4.x = 0; P4.y = -2.7; P4.z = height - 1;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3d(1, 0, 1);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}

	P2.x = -3; P2.y = 3; P2.z = height + 4;							//питание
	P3.x = -6; P3.y = -6; P3.z = height - 5;
	P4.x = 0; P4.y = -2.4; P4.z = height - 0.8;

	generateBezierLine(bezier_line, common_start_point, P2, P3, P4);
	for (int i = 0; i < 99; i++)
	{
		glBegin(GL_LINES);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		//printf("%f %f %f\n", bezier_line[i].x, bezier_line[i].y, bezier_line[i].z);
		glVertex3f(bezier_line[i + 1].x, bezier_line[i + 1].y, bezier_line[i + 1].z);
		glEnd();
	}
}



void DrawPiano()
{
	//Рояль
	//Корпус

	ArcCustom arched = ArcCustom(16.0, -7.5, 16.0, 0, 9);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	ArcCustom concave = ArcCustom(16.0, -7.5, 4.3, -15.0, 16);
	Point* concave_points = NULL;
	int count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave);

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 6.3);
	glVertex3f(16.0, 0, 6.3);
	glVertex3f(0, 0, 10.2);
	glVertex3f(16.0, 0, 10.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 6.3);
	glVertex3f(0, -15.0, 6.3);
	glVertex3f(0, 0, 9.6);
	glVertex3f(0, -15.0, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -15.0, 6.3);
	glVertex3f(4.3, -15.0, 6.3);
	glVertex3f(0, -15.0, 10.2);
	glVertex3f(4.3, -15.0, 10.2);
	glEnd();


	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 6.3);
		glVertex3f(concave_points[i].x, concave_points[i].y, 10.2);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 6.3);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 10.2);
		glEnd();


	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(concave_points[0].x, concave_points[0].y, 6.3);
	glVertex3f(arched_points[0].x, arched_points[0].y, 6.3);
	glVertex3f(concave_points[0].x, concave_points[0].y, 10.2);
	glVertex3f(arched_points[0].x, arched_points[0].y, 10.2);
	glEnd();


	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 6.3);
		glVertex3f(arched_points[i].x, arched_points[i].y, 10.2);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 6.3);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 10.2);
		glEnd();


	}

	glaStencilWindow(1);

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{
		glVertex3f(concave_points[i].x, concave_points[i].y, 6.3);
	}
	glEnd();

	glaStencilWall(1);

	glBegin(GL_POLYGON);
	glColor3f(100.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -15.0, 6.3);
	glVertex3f(4.3, -15.0, 6.3);
	glVertex3f(16.0, 0, 6.3);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 6.3);
	}
	glVertex3f(0, 0, 6.3);
	glEnd();

	glaStencilEnd();

	ArcCustom arched2 = ArcCustom(15.493, -6.993, 15.493, -0.507, 9);							//второй слой
	Point* arched2_points = NULL;
	int count_of_elems_arched2;
	arched2.GenerateArc(arched2_points, count_of_elems_arched2);

	ArcCustom concave2 = ArcCustom(15.493, -6.993, 3.793, -14.493, 16);
	Point* concave2_points = NULL;
	int count_of_elems_concave2;
	concave2.GenerateArc(concave2_points, count_of_elems_concave2);

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.507, 6.3);
	glVertex3f(15.493, -0.507, 6.3);
	glVertex3f(0, -0.507, 10.2);
	glVertex3f(15.493, -0.507, 10.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -14.493, 6.3);
	glVertex3f(3.793, -14.493, 6.3);
	glVertex3f(0, -14.493, 10.2);
	glVertex3f(3.793, -14.493, 10.2);
	glEnd();


	for (i = 0; i < count_of_elems_concave2 - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 6.3);
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 10.2);
		glVertex3f(concave2_points[i + 1].x, concave2_points[i + 1].y, 6.3);
		glVertex3f(concave2_points[i + 1].x, concave2_points[i + 1].y, 10.2);

		glEnd();


	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(concave2_points[0].x, concave2_points[0].y, 6.3);
	glVertex3f(arched2_points[0].x, arched2_points[0].y, 10.2);
	glVertex3f(concave2_points[0].x, concave2_points[0].y, 10.2);
	glVertex3f(arched2_points[0].x, arched2_points[0].y, 6.3);
	glEnd();

	for (i = 0; i < count_of_elems_arched2 - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 6.3);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 10.2);
		glVertex3f(arched2_points[i + 1].x, arched2_points[i + 1].y, 10.2);
		glVertex3f(arched2_points[i + 1].x, arched2_points[i + 1].y, 6.3);
		glEnd();


	}



	glBegin(GL_QUAD_STRIP);								//поверхность корпуса сверху
	glColor3f(0, 1, 1);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -14.493, 10.2);
	glVertex3f(3.793, -14.493, 10.2);
	glVertex3f(0, -15, 10.2);
	glVertex3f(4.3, -15, 10.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.507, 10.2);
	glVertex3f(15.493, -0.507, 10.2);
	glVertex3f(0, 0, 10.2);
	glVertex3f(16.0, 0, 10.2);
	glEnd();

	for (i = 0; i < count_of_elems_concave2 - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0, 1, 1);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 10.2);
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 10.2);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 10.2);
		glVertex3f(concave2_points[i + 1].x, concave2_points[i + 1].y, 10.2);
		glEnd();


	}


	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(arched_points[0].x, arched_points[0].y, 10.2);
	glVertex3f(arched2_points[0].x, arched2_points[0].y, 10.2);
	glVertex3f(concave_points[0].x, concave_points[0].y, 10.2);
	glVertex3f(concave2_points[0].x, concave2_points[0].y, 10.2);
	glEnd();


	for (i = 0; i < count_of_elems_arched2 - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(0, 1, 1);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 10.2);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 10.2);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 10.2);
		glVertex3f(arched2_points[i + 1].x, arched2_points[i + 1].y, 10.2);
		glEnd();


	}

	for (i = count_of_elems_arched2; i < count_of_elems_arched; i++)
	{

		glBegin(GL_LINES);
		glColor3f(0, 1, 1);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 10.2);
		glVertex3f(15.493, -0.507, 10.2);
		glEnd();


	}

	max_cross.y = -1212;
	glBegin(GL_POLYGON);							//саббортик рядом с клавиатурой (правый)
	glColor3f(0, 1, 1);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -14.493, 10);
	glVertex3f(-2, -13.986, 10);
	for (int i = 0; i < count_of_elems_concave2; i++)
		if (concave2_points[i].y <= -13.986 && concave2_points[i].y >= -14.493)
		{
			glVertex3f(concave2_points[i].x, concave2_points[i].y, 10);
			if (concave2_points[i].y > max_cross.y)
			{
				max_cross.y = concave2_points[i].y;
				max_cross.x = concave2_points[i].x;
			}
		}
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0, 1, 1);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -14.493, 9.6);
	glVertex3f(-2, -13.986, 9.6);
	for (int i = 0; i < count_of_elems_concave2; i++)
		if (concave2_points[i].y <= -13.986 && concave2_points[i].y >= -14.493)
			glVertex3f(concave2_points[i].x, concave2_points[i].y, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(-2, -13.986, 10);					//-14.493
	glVertex3f(-2, -13.986, 9.6);					//-13.986
	glVertex3f(max_cross.x, max_cross.y, 10);
	glVertex3f(max_cross.x, max_cross.y, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(-2, -13.986, 10);
	glVertex3f(-2, -14.493, 9.6);
	glVertex3f(-2, -13.986, 9.6);
	glVertex3f(-2, -14.493, 10);
	glEnd();


	glBegin(GL_QUAD_STRIP);							//саббортик рядом с клавиатурой (левый)  4,107933
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(-2, -0.507, 10);
	glVertex3f(-2, -1.014, 10);
	glVertex3f(4.1, -0.507, 10);
	glVertex3f(4.1, -1.014, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(-2, -0.507, 9.6);
	glVertex3f(-2, -1.014, 9.6);
	glVertex3f(4.1, -0.507, 9.6);
	glVertex3f(4.1, -1.014, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(-2, -1.014, 10);
	glVertex3f(4.1, -1.014, 10);
	glVertex3f(-2, -1.014, 9.6);
	glVertex3f(4.1, -1.014, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(4.1, -0.507, 9.6);
	glVertex3f(4.1, -1.014, 9.6);
	glVertex3f(4.1, -0.507, 10);
	glVertex3f(4.1, -1.014, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(-2, -0.507, 9.6);
	glVertex3f(-2, -1.014, 9.6);
	glVertex3f(-2, -0.507, 10);
	glVertex3f(-2, -1.014, 10);
	glEnd();


	free(arched_points);
	free(concave_points);

	//Дека

	arched = ArcCustom(1, -14.493, 10.7, -5, 40);									//основание
	arched_points = NULL;
	arched.GenerateArc(arched_points, count_of_elems_arched, false);

	concave = ArcCustom(10.7, -5, 15.493, -0.507, 20);
	concave_points = NULL;
	concave.GenerateArc(concave_points, count_of_elems_concave);

	Point* all_deck_points = GetDeckPointsArray(arched_points, count_of_elems_arched, concave_points, count_of_elems_concave);
	DrawStringsArray(all_deck_points, count_of_elems_arched + count_of_elems_concave);
	free(all_deck_points);

	for (i = 0; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 8.5);
		glVertex3f(concave_points[i].x, concave_points[i].y, 8.9);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 8.5);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 8.9);
		glEnd();


	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	glVertex3f(concave_points[0].x, concave_points[0].y, 8.5);
	glVertex3f(concave_points[0].x, concave_points[0].y, 8.9);
	glVertex3f(arched_points[0].x, arched_points[0].y, 8.5);
	glVertex3f(arched_points[0].x, arched_points[0].y, 8.9);
	glEnd();

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 8.5);
		glVertex3f(arched_points[i].x, arched_points[i].y, 8.9);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 8.5);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 8.9);
		glEnd();


	}





	glaStencilWindow(1);

	center_of_circle.x = 10.7;										//окружности в деке
	center_of_circle.y = -7;
	GetCircle(circle_points_deck[0], center_of_circle, 0.5);

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[0][i].x, circle_points_deck[0][i].y, 8.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
		glVertex3f(circle_points_deck[0][i + 1].x, circle_points_deck[0][i + 1].y, 8.9);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[0][0].x, circle_points_deck[0][0].y, 8.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
	glVertex3f(circle_points_deck[0][359].x, circle_points_deck[0][359].y, 8.9);
	glEnd();


	center_of_circle.x = 7;
	center_of_circle.y = -9.5;
	GetCircle(circle_points_deck[1], center_of_circle, 0.5);
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[1][i].x, circle_points_deck[1][i].y, 8.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
		glVertex3f(circle_points_deck[1][i + 1].x, circle_points_deck[1][i + 1].y, 8.9);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[1][0].x, circle_points_deck[1][0].y, 8.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
	glVertex3f(circle_points_deck[1][359].x, circle_points_deck[1][359].y, 8.9);
	glEnd();

	center_of_circle.x = 4.2;
	center_of_circle.y = -12;
	GetCircle(circle_points_deck[2], center_of_circle, 0.5);
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[2][i].x, circle_points_deck[2][i].y, 8.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
		glVertex3f(circle_points_deck[2][i + 1].x, circle_points_deck[2][i + 1].y, 8.9);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[2][0].x, circle_points_deck[2][0].y, 8.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
	glVertex3f(circle_points_deck[2][359].x, circle_points_deck[2][359].y, 8.9);
	glEnd();

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave2; i++)
	{
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 8.9);
	}
	glEnd();
	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{

		glVertex3f(concave_points[i].x, concave_points[i].y, 8.9);

	}
	glEnd();



	glaStencilWall(1);




	for (i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(3.793, -14.493, 8.9);
		glVertex3f(arched_points[i].x, arched_points[i].y, 8.9);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 8.9);
		glEnd();
	}
	for (i = 0; i < count_of_elems_arched2 - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(3.793, -14.493, 8.9);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 8.9);
		glVertex3f(arched2_points[i + 1].x, arched2_points[i + 1].y, 8.9);
		glEnd();
	}
	for (i = 0; i < count_of_elems_concave - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(3.793, -14.493, 8.9);
		glVertex3f(concave_points[i].x, concave_points[i].y, 8.9);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 8.9);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	glVertex3f(3.793, -14.493, 8.9);
	glVertex3f(10.7, -5, 8.9);
	glVertex3f(arched_points[0].x, arched_points[0].y, 8.9);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	glVertex3f(3.793, -14.493, 8.9);
	glVertex3f(10.7, -5, 8.9);
	glVertex3f(concave_points[0].x, concave_points[0].y, 8.9);
	glEnd();


	glaStencilEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[0][i].x, circle_points_deck[0][i].y, 8.5);
		glVertex3f(circle_points_deck[0][i].x, circle_points_deck[0][i].y, 8.9);
		glVertex3f(circle_points_deck[0][i + 1].x, circle_points_deck[0][i + 1].y, 8.5);
		glVertex3f(circle_points_deck[0][i + 1].x, circle_points_deck[0][i + 1].y, 8.9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[0][0].x, circle_points_deck[0][0].y, 8.5);
	glVertex3f(circle_points_deck[0][0].x, circle_points_deck[0][0].y, 8.9);
	glVertex3f(circle_points_deck[0][359].x, circle_points_deck[0][359].y, 8.5);
	glVertex3f(circle_points_deck[0][359].x, circle_points_deck[0][359].y, 8.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[1][i].x, circle_points_deck[1][i].y, 8.5);
		glVertex3f(circle_points_deck[1][i].x, circle_points_deck[1][i].y, 8.9);
		glVertex3f(circle_points_deck[1][i + 1].x, circle_points_deck[1][i + 1].y, 8.5);
		glVertex3f(circle_points_deck[1][i + 1].x, circle_points_deck[1][i + 1].y, 8.9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[1][0].x, circle_points_deck[1][0].y, 8.5);
	glVertex3f(circle_points_deck[1][0].x, circle_points_deck[1][0].y, 8.9);
	glVertex3f(circle_points_deck[1][359].x, circle_points_deck[1][359].y, 8.5);
	glVertex3f(circle_points_deck[1][359].x, circle_points_deck[1][359].y, 8.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[2][i].x, circle_points_deck[2][i].y, 8.5);
		glVertex3f(circle_points_deck[2][i].x, circle_points_deck[2][i].y, 8.9);
		glVertex3f(circle_points_deck[2][i + 1].x, circle_points_deck[2][i + 1].y, 8.5);
		glVertex3f(circle_points_deck[2][i + 1].x, circle_points_deck[2][i + 1].y, 8.9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[2][0].x, circle_points_deck[2][0].y, 8.5);
	glVertex3f(circle_points_deck[2][0].x, circle_points_deck[2][0].y, 8.9);
	glVertex3f(circle_points_deck[2][359].x, circle_points_deck[2][359].y, 8.5);
	glVertex3f(circle_points_deck[2][359].x, circle_points_deck[2][359].y, 8.9);
	glEnd();

	glaStencilWindow(1);

	center_of_circle.x = 10.7;										//окружности в деке(2)
	center_of_circle.y = -7;
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[0][i].x, circle_points_deck[0][i].y, 8.5);
		glVertex3f(center_of_circle.x, center_of_circle.y, 8.5);
		glVertex3f(circle_points_deck[0][i + 1].x, circle_points_deck[0][i + 1].y, 8.5);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[0][0].x, circle_points_deck[0][0].y, 8.5);
	glVertex3f(center_of_circle.x, center_of_circle.y, 8.5);
	glVertex3f(circle_points_deck[0][359].x, circle_points_deck[0][359].y, 8.5);
	glEnd();


	center_of_circle.x = 7;
	center_of_circle.y = -9.5;
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[1][i].x, circle_points_deck[1][i].y, 8.5);
		glVertex3f(center_of_circle.x, center_of_circle.y, 8.5);
		glVertex3f(circle_points_deck[1][i + 1].x, circle_points_deck[1][i + 1].y, 8.5);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[1][0].x, circle_points_deck[1][0].y, 8.5);
	glVertex3f(center_of_circle.x, center_of_circle.y, 8.5);
	glVertex3f(circle_points_deck[1][359].x, circle_points_deck[1][359].y, 8.5);
	glEnd();

	center_of_circle.x = 4.2;
	center_of_circle.y = -12;
	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points_deck[2][i].x, circle_points_deck[2][i].y, 8.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
		glVertex3f(circle_points_deck[2][i + 1].x, circle_points_deck[2][i + 1].y, 8.9);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points_deck[2][0].x, circle_points_deck[2][0].y, 8.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 8.9);
	glVertex3f(circle_points_deck[2][359].x, circle_points_deck[2][359].y, 8.9);
	glEnd();

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave2; i++)
	{
		glVertex3f(concave2_points[i].x, concave2_points[i].y, 8.5);
	}
	glEnd();
	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{

		glVertex3f(concave_points[i].x, concave_points[i].y, 8.5);

	}
	glEnd();

	glaStencilWall(1);

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(3.793, -14.493, 8.5);
		glVertex3f(arched_points[i].x, arched_points[i].y, 8.5);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 8.5);
		glEnd();
	}
	for (i = 0; i < count_of_elems_arched2 - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(3.793, -14.493, 8.5);
		glVertex3f(arched2_points[i].x, arched2_points[i].y, 8.5);
		glVertex3f(arched2_points[i + 1].x, arched2_points[i + 1].y, 8.5);
		glEnd();
	}
	for (i = 0; i < count_of_elems_concave - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(0, 0, 1);
		glVertex3f(3.793, -14.493, 8.5);
		glVertex3f(concave_points[i].x, concave_points[i].y, 8.5);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 8.5);
		glEnd();
	}
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	glVertex3f(3.793, -14.493, 8.5);
	glVertex3f(10.7, -5, 8.5);
	glVertex3f(arched_points[0].x, arched_points[0].y, 8.5);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(0, 0, 1);
	glVertex3f(3.793, -14.493, 8.5);
	glVertex3f(10.7, -5, 8.5);
	glVertex3f(concave_points[0].x, concave_points[0].y, 8.5);
	glEnd();


	glaStencilEnd();

	min_cross.y = 1;
	max_cross.x = -20;
	glBegin(GL_QUAD_STRIP);							//доски деки (как они там называются?)
	glColor3f(0.0f, 1.0f, 10.0f);					//1-2
	glNormal3f(0, 0, 1);
	glVertex3f(0, -0.507, 9.5);
	glVertex3f(0, -0.707, 9.5);
	/*for(int i=0;i<count_of_elems_arched;i++)
		if (arched2_points[i].y <= -3.547 && arched2_points[i].y >= -3.747)
		{
			glVertex3f(arched2_points[i].x, arched2_points[i].y, 9.6);
			if (max_cross.y < arched2_points[i].y)
			{
				max_cross.y = arched2_points[i].y;
				max_cross.x = arched2_points[i].x;
			}
			else
			{
				min_cross.y = arched2_points[i].y;
				min_cross.x = arched2_points[i].x;
			}
		}*/
	glVertex3f(15.493, -3.747, 9.5);
	glVertex3f(15.493, -3.947, 9.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.493, -3.747, 9.5);
	glVertex3f(15.493, -3.947, 9.5);
	glVertex3f(15.893, -3.817, 9.25);
	glVertex3f(15.893, -4.026, 9.25);
	glEnd();

	min_cross.y = 1;
	max_cross.x = -20;
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -0.707, 9.25);
	glVertex3f(0, -0.707, 9.5);
	glVertex3f(15.893, -4.026, 9.25);
	glVertex3f(15.493, -3.947, 9.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -0.507, 9.5);
	glVertex3f(0, -0.507, 9.25);
	glVertex3f(15.493, -3.747, 9.5);
	glVertex3f(15.893, -3.817, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -0.507, 9.25);
	glVertex3f(0, -0.707, 9.25);
	glVertex3f(15.893, -3.817, 9.25);
	glVertex3f(15.893, -4.026, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.893, -3.817, 8.9);
	glVertex3f(15.893, -4.026, 8.9);
	glVertex3f(15.893, -3.817, 9.25);
	glVertex3f(15.893, -4.026, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.493, -3.747, 8.9);
	glVertex3f(15.493, -3.947, 8.9);
	glVertex3f(15.493, -3.747, 9.25);
	glVertex3f(15.493, -3.947, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.493, -3.747, 9.25);
	glVertex3f(15.893, -3.817, 9.25);
	glVertex3f(15.493, -3.747, 8.9);
	glVertex3f(15.893, -3.817, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.493, -3.947, 9.25);
	glVertex3f(15.893, -4.026, 9.25);
	glVertex3f(15.493, -3.947, 8.9);
	glVertex3f(15.893, -4.026, 8.9);
	glEnd();



	glBegin(GL_QUAD_STRIP);						//9-10
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.55, 9.25);
	glVertex3f(0.4, -3.75, 9.25);
	glVertex3f(15.093, -0.787, 9.25);
	glVertex3f(15.093, -0.987, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.093, -0.787, 9.25);
	glVertex3f(15.093, -0.987, 9.25);
	glVertex3f(15.493, -0.707, 8.9);
	glVertex3f(15.493, -0.907, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.75, 9.25);
	glVertex3f(0.4, -3.75, 8.9);
	glVertex3f(15.093, -0.987, 9.25);
	glVertex3f(15.493, -0.907, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.55, 9.25);
	glVertex3f(0.4, -3.55, 8.9);
	glVertex3f(15.493, -0.707, 8.9);
	glVertex3f(15.093, -0.787, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.55, 8.9);
	glVertex3f(0.4, -3.75, 8.9);
	glVertex3f(15.493, -0.707, 8.9);
	glVertex3f(15.493, -0.907, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//13-14			
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -3.55, 9.5);
	glVertex3f(0, -3.75, 9.5);
	glVertex3f(15.093, -6.50, 9.5);
	glVertex3f(15.093, -6.70, 9.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.093, -6.50, 9.5);
	glVertex3f(15.093, -6.70, 9.5);
	glVertex3f(15.493, -6.59, 9.25);
	glVertex3f(15.493, -6.79, 9.25);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -3.55, 9.5);
	glVertex3f(0, -3.55, 9.25);
	glVertex3f(15.093, -6.50, 9.5);
	glVertex3f(15.493, -6.59, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -3.75, 9.5);
	glVertex3f(0, -3.75, 9.25);
	glVertex3f(15.093, -6.70, 9.5);
	glVertex3f(15.493, -6.79, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -3.55, 9.25);
	glVertex3f(0, -3.75, 9.25);
	glVertex3f(15.093, -6.50, 9.25);
	glVertex3f(15.093, -6.70, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.093, -6.50, 8.9);
	glVertex3f(15.093, -6.70, 8.9);
	glVertex3f(15.093, -6.50, 9.25);
	glVertex3f(15.093, -6.70, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.493, -6.59, 9.25);
	glVertex3f(15.493, -6.79, 9.25);
	glVertex3f(15.493, -6.59, 8.9);
	glVertex3f(15.493, -6.79, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.493, -6.59, 9.25);
	glVertex3f(15.093, -6.5, 9.25);
	glVertex3f(15.493, -6.59, 8.9);
	glVertex3f(15.093, -6.5, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15.093, -6.70, 9.25);
	glVertex3f(15.493, -6.79, 9.25);
	glVertex3f(15.093, -6.70, 8.9);
	glVertex3f(15.493, -6.79, 8.9);
	glEnd();



	glBegin(GL_QUAD_STRIP);						//9-14 прямая
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.55, 9.25);
	glVertex3f(0.4, -3.75, 9.25);
	glVertex3f(0, -3.55, 9.25);
	glVertex3f(0, -3.75, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.55, 9.25);
	glVertex3f(0, -3.55, 9.25);
	glVertex3f(0.4, -3.55, 8.9);
	glVertex3f(0, -3.55, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.75, 9.25);
	glVertex3f(0, -3.75, 9.25);
	glVertex3f(0.4, -3.75, 8.9);
	glVertex3f(0, -3.75, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.4, -3.55, 8.9);
	glVertex3f(0.4, -3.75, 8.9);
	glVertex3f(0, -3.55, 8.9);
	glVertex3f(0, -3.75, 8.9);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//15-16
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -8.11, 9.25);
	glVertex3f(0, -8.31, 9.25);
	glVertex3f(9, -8.11, 9.25);
	glVertex3f(9, -8.31, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(9.4, -8.11, 8.9);
	glVertex3f(9.4, -8.31, 8.9);
	glVertex3f(9, -8.11, 9.25);
	glVertex3f(9, -8.31, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -8.11, 9.25);
	glVertex3f(9, -8.11, 9.25);
	glVertex3f(0, -8.11, 8.9);
	glVertex3f(9.4, -8.11, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -8.31, 9.25);
	glVertex3f(9, -8.31, 9.25);
	glVertex3f(0, -8.31, 8.9);
	glVertex3f(9.4, -8.31, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -8.11, 8.9);
	glVertex3f(0, -8.31, 8.9);
	glVertex3f(9.4, -8.11, 8.9);
	glVertex3f(9.4, -8.31, 8.9);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//17-18
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -10.7, 9.25);
	glVertex3f(0, -10.8, 9.25);
	glVertex3f(4.6, -10.7, 9.25);
	glVertex3f(4.6, -10.8, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(4.6, -10.7, 9.25);
	glVertex3f(4.6, -10.8, 9.25);
	glVertex3f(5, -10.7, 8.9);
	glVertex3f(5, -10.8, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -10.8, 8.9);
	glVertex3f(5, -10.8, 8.9);
	glVertex3f(0, -10.8, 9.25);
	glVertex3f(4.6, -10.8, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -10.7, 8.9);
	glVertex3f(5, -10.7, 8.9);
	glVertex3f(0, -10.7, 9.25);
	glVertex3f(4.6, -10.7, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -10.7, 8.9);
	glVertex3f(0, -10.8, 8.9);
	glVertex3f(5, -10.7, 8.9);
	glVertex3f(5, -10.8, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -10.7, 8.9);
	glVertex3f(0, -10.8, 8.9);
	glVertex3f(5, -10.7, 8.9);
	glVertex3f(5, -10.8, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//горизонтальная доска
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -0.707, 9.6);
	glVertex3f(0, -0.707, 9.5);
	glVertex3f(0, -3.55, 9.6);
	glVertex3f(0, -3.55, 9.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.2, -0.707, 9.6);
	glVertex3f(0.2, -0.707, 9.5);
	glVertex3f(0.2, -3.55, 9.6);
	glVertex3f(0.2, -3.55, 9.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.2, -0.707, 9.5);
	glVertex3f(0.2, -3.55, 9.5);
	glVertex3f(0, -0.707, 9.5);
	glVertex3f(0, -3.55, 9.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.2, -0.707, 9.6);
	glVertex3f(0.2, -3.55, 9.6);
	glVertex3f(0, -0.707, 9.6);
	glVertex3f(0, -3.55, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -3.55, 9.6);
	glVertex3f(0, -13.986, 9.6);
	glVertex3f(0, -3.55, 9.25);
	glVertex3f(0, -13.986, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.2, -3.55, 9.6);
	glVertex3f(0.2, -13.986, 9.6);
	glVertex3f(0.2, -3.55, 9.25);
	glVertex3f(0.2, -13.986, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0.2, -3.55, 9.6);
	glVertex3f(0.2, -13.986, 9.6);
	glVertex3f(0, -3.55, 9.6);
	glVertex3f(0, -13.986, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 1.0f, 10.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -3.55, 9.25);
	glVertex3f(0, -13.986, 9.25);
	glVertex3f(0.2, -3.55, 9.25);
	glVertex3f(0.2, -13.986, 9.25);
	glEnd();

	free(arched_points);
	free(concave_points);
	free(arched2_points);
	free(concave2_points);


	//Педали

	glVertex3f(0, -8, 6.3);			//ножки
	glVertex3f(0.5, -8, 6.3);
	glVertex3f(0.5, -8.5, 6.3);
	glVertex3f(0, -8.5, 6.3);

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -8, 6.3);
	glVertex3f(0.5, -8, 6.3);
	glVertex3f(0, -8, 2);
	glVertex3f(0.5, -8, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -8, 6.3);
	glVertex3f(0.5, -8.5, 6.3);
	glVertex3f(0.5, -8, 2);
	glVertex3f(0.5, -8.5, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -8.5, 6.3);
	glVertex3f(0, -8.5, 6.3);
	glVertex3f(0.5, -8.5, 2);
	glVertex3f(0, -8.5, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -8, 6.3);
	glVertex3f(0, -8.5, 6.3);
	glVertex3f(0, -8, 2);
	glVertex3f(0, -8.5, 2);
	glEnd();



	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -6.5, 6.3);
	glVertex3f(0.5, -6.5, 6.3);
	glVertex3f(0, -6.5, 2);
	glVertex3f(0.5, -6.5, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0.5, -6.5, 6.3);
	glVertex3f(0.5, -7.0, 6.3);
	glVertex3f(0.5, -6.5, 2);
	glVertex3f(0.5, -7.0, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.5, -7.0, 6.3);
	glVertex3f(0, -7.0, 6.3);
	glVertex3f(0.5, -7.0, 2);
	glVertex3f(0, -7.0, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -7.0, 6.3);
	glVertex3f(0, -6.5, 6.3);
	glVertex3f(0, -7.0, 2);
	glVertex3f(0, -6.5, 2);
	glEnd();


	center_of_circle.x = 0;
	center_of_circle.y = -7.5;
	GetCircle(circle_points, center_of_circle, 0.05);			//передающий стержень
	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 6.3);
		glVertex3f(circle_points[i].x, circle_points[i].y, 2);
		glEnd();
	}

	center_of_circle.x = 0;
	center_of_circle.y = -7.3;
	GetCircle(circle_points, center_of_circle, 0.05);			//передающий стержень
	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 6.3);
		glVertex3f(circle_points[i].x, circle_points[i].y, 2);
		glEnd();
	}

	center_of_circle.x = 0;
	center_of_circle.y = -7.7;
	GetCircle(circle_points, center_of_circle, 0.05);			//передающий стержень
	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 6.3);
		glVertex3f(circle_points[i].x, circle_points[i].y, 2);
		glEnd();
	}


	glBegin(GL_QUAD_STRIP);					//коробка
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.5, -6, 2);
	glVertex3f(-0.5, -9, 2);
	glVertex3f(1, -6, 2);
	glVertex3f(1, -9, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.5, -6, 1);
	glVertex3f(-0.5, -9, 1);
	glVertex3f(1, -6, 1);
	glVertex3f(1, -9, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.5, -6, 2);
	glVertex3f(-0.5, -9, 2);
	glVertex3f(-0.5, -6, 1);
	glVertex3f(-0.5, -9, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.5, -9, 2);
	glVertex3f(1, -6, 2);
	glVertex3f(-0.5, -9, 1);
	glVertex3f(1, -6, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(1, -6, 2);
	glVertex3f(1, -9, 2);
	glVertex3f(1, -6, 1);
	glVertex3f(1, -9, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.5, -6, 2);
	glVertex3f(1, -9, 2);
	glVertex3f(-0.5, -6, 1);
	glVertex3f(1, -9, 1);
	glEnd();

	arched = ArcCustom(-1.5, -6, -1.8, -6.6, 1);							//левая педаль
	arched_points = NULL;
	count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	concave = ArcCustom(-0.5, -6.3, -1.5, -6, 2);
	concave_points = NULL;
	count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave, false);

	for (i = 1; i < count_of_elems_concave - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.3);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.5);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 1.3);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 1.5);
		glEnd();


	}

	for (i = 0; i < count_of_elems_arched - 1; i++)
	{

		glBegin(GL_QUAD_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.3);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.5);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1.3);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1.5);
		glEnd();
	}

	/*glaStencilWindow(1);

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.3);
	}
	glEnd();

	glaStencilWall(1);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(-0.5, -6.9, 1.3);
	glVertex3f(-0.5, -6.3, 1.3);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.3);
	}
	glEnd();

	glaStencilEnd();

	glaStencilWindow(1);

	glBegin(GL_POLYGON);
	for (i = 0; i < count_of_elems_concave; i++)
	{
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.5);
	}
	glEnd();

	glaStencilWall(1);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(-0.5, -6.9, 1.5);
	glVertex3f(-0.5, -6.3, 1.5);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.5);
	}
	glEnd();

	glaStencilEnd();*/

	for (int i = 1; i < count_of_elems_concave - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -6.6, 1.3);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.3);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 1.3);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -6.6, 1.3);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.3);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1.3);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glVertex3f(-1.8, -6.6, 1.3);
	glVertex3f(-0.5, -6.9, 1.3);
	glVertex3f(-0.5, -6.3, 1.3);
	glEnd();

	for (int i = 1; i < count_of_elems_concave - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -6.6, 1.5);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.5);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 1.5);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -6.6, 1.5);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.5);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1.5);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glVertex3f(-1.8, -6.6, 1.5);
	glVertex3f(-0.5, -6.9, 1.5);
	glVertex3f(-0.5, -6.3, 1.5);
	glEnd();

	free(arched_points);
	free(concave_points);


	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.8, -6.6, 1.3);
	glVertex3f(-0.5, -6.9, 1.3);
	glVertex3f(-1.8, -6.6, 1.5);
	glVertex3f(-0.5, -6.9, 1.5);
	glEnd();

	arched = ArcCustom(-1.5, -9, -1.8, -8.4, 1);							//правая педаль
	arched_points = NULL;
	count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched, false);

	concave = ArcCustom(-0.5, -8.7, -1.5, -9, 2);
	concave_points = NULL;
	count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave);

	for (i = 0; i < count_of_elems_concave; i++)
	{

		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.3);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.5);
		glEnd();


	}

	for (i = 0; i < count_of_elems_arched; i++)
	{

		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.3);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.5);
		glEnd();


	}

	//glaStencilWindow(1);

	//glBegin(GL_POLYGON);
	//for (i = 0; i < count_of_elems_concave; i++)
	//{
	//	glVertex3f(concave_points[i].x, concave_points[i].y, 1.3);
	//}
	//glEnd();

	///*glBegin(GL_POINT);
	//glVertex3f(-1.5, -9, 1.3);
	//glEnd();*/

	//glaStencilWall(1);

	//glBegin(GL_POLYGON);
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glNormal3f(0, 0, 1);
	//glVertex3f(-0.5, -8.1, 1.3);
	//for (i = 0; i < count_of_elems_arched; i++)
	//{
	//	glVertex3f(arched_points[i].x, arched_points[i].y, 1.3);
	//}
	//glVertex3f(-0.5, -8.7, 1.3);
	//glEnd();

	//glaStencilEnd();

	//glaStencilWindow(1);

	//glBegin(GL_POLYGON);
	//for (i = 0; i < count_of_elems_concave; i++)
	//{
	//	glVertex3f(concave_points[i].x, concave_points[i].y, 1.5);
	//}
	//glEnd();

	//glaStencilWall(1);

	//glBegin(GL_POLYGON);
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glNormal3f(0, 0, 1);
	//glVertex3f(-0.5, -8.1, 1.5);
	//for (i = 0; i < count_of_elems_arched; i++)
	//{
	//	glVertex3f(arched_points[i].x, arched_points[i].y, 1.5);
	//}
	//glVertex3f(-0.5, -8.7, 1.5);
	//glEnd();

	//glaStencilEnd();

	for (int i = 0; i < count_of_elems_concave - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -8.4, 1.3);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.3);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 1.3);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -8.4, 1.3);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.3);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1.3);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glVertex3f(-1.8, -8.4, 1.3);
	glVertex3f(-0.5, -8.1, 1.3);
	glVertex3f(-0.5, -8.7, 1.3);
	glEnd();

	for (int i = 1; i < count_of_elems_concave - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -8.4, 1.5);
		glVertex3f(concave_points[i].x, concave_points[i].y, 1.5);
		glVertex3f(concave_points[i + 1].x, concave_points[i + 1].y, 1.5);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1, 1, 1);
		glVertex3f(-1.8, -8.4, 1.5);
		glVertex3f(arched_points[i].x, arched_points[i].y, 1.5);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 1.5);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glVertex3f(-1.8, -8.4, 1.5);
	glVertex3f(-0.5, -8.1, 1.5);
	glVertex3f(-0.5, -8.7, 1.5);
	glEnd();

	free(arched_points);
	free(concave_points);


	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.8, -8.4, 1.3);
	glVertex3f(-0.5, -8.1, 1.3);
	glVertex3f(-1.8, -8.4, 1.5);
	glVertex3f(-0.5, -8.1, 1.5);
	glEnd();


	glBegin(GL_QUAD_STRIP);							//центральная педаль
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.5, -7.2, 1.5);
	glVertex3f(-0.5, -7.2, 1.3);
	glVertex3f(-1.5, -7.2, 1.3);
	glVertex3f(-1.5, -7.2, 1.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.5, -7.2, 1.3);
	glVertex3f(-1.5, -7.2, 1.5);
	glVertex3f(-1.8, -7.5, 1.5);
	glVertex3f(-1.8, -7.5, 1.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.5, -7.8, 1.5);
	glVertex3f(-0.5, -7.8, 1.3);
	glVertex3f(-1.5, -7.8, 1.3);
	glVertex3f(-1.5, -7.8, 1.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.5, -7.8, 1.3);
	glVertex3f(-1.5, -7.8, 1.5);
	glVertex3f(-1.8, -7.5, 1.5);
	glVertex3f(-1.8, -7.5, 1.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(-0.5, -7.2, 1.5);
	glVertex3f(-1.5, -7.2, 1.5);
	glVertex3f(-1.8, -7.5, 1.5);
	glVertex3f(-1.5, -7.8, 1.5);
	glVertex3f(-0.5, -7.8, 1.5);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(-0.5, -7.2, 1.3);
	glVertex3f(-1.5, -7.2, 1.3);
	glVertex3f(-1.8, -7.5, 1.3);
	glVertex3f(-1.5, -7.8, 1.3);
	glVertex3f(-0.5, -7.8, 1.3);
	glEnd();

	//Ножки

	glBegin(GL_POLYGON);					//правая ножка
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(2, -14, 6.3);
	glVertex3f(1, -14, 5);
	glVertex3f(0, -14, 5);
	glVertex3f(-1, -14, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(2, -12, 6.3);
	glVertex3f(1, -12, 5);
	glVertex3f(0, -12, 5);
	glVertex3f(-1, -12, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(2, -12, 6.3);
	glVertex3f(1, -12, 5);
	glVertex3f(1, -14, 5);
	glVertex3f(2, -14, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, -12, 6.3);
	glVertex3f(0, -12, 5);
	glVertex3f(0, -14, 5);
	glVertex3f(-1, -14, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -12, 5);
	glVertex3f(0, -12, 5);
	glVertex3f(0, -14, 5);
	glVertex3f(1, -14, 5);
	glEnd();

	glBegin(GL_POLYGON);					//основная ножка
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -12.5, 5);
	glVertex3f(0, -12.5, 5);
	glVertex3f(0, -12.5, 2);
	glVertex3f(1, -12.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -13.5, 5);
	glVertex3f(0, -13.5, 5);
	glVertex3f(0, -13.5, 2);
	glVertex3f(1, -13.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -13.5, 5);
	glVertex3f(1, -13.5, 2);
	glVertex3f(1, -12.5, 2);
	glVertex3f(1, -12.5, 5);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -13.5, 5);
	glVertex3f(0, -13.5, 2);
	glVertex3f(0, -12.5, 2);
	glVertex3f(0, -12.5, 5);
	glEnd();

	glBegin(GL_POLYGON);						//поддержка колеса
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -12.5, 2);
	glVertex3f(0, -12.5, 2);
	glVertex3f(0, -12.5, 1);
	glVertex3f(1, -12.5, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -12.8, 2);
	glVertex3f(0, -12.8, 2);
	glVertex3f(0, -12.8, 1);
	glVertex3f(1, -12.8, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -12.8, 2);
	glVertex3f(1, -12.8, 1);
	glVertex3f(1, -12.5, 1);
	glVertex3f(1, -12.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -12.8, 2);
	glVertex3f(0, -12.8, 1);
	glVertex3f(0, -12.5, 1);
	glVertex3f(0, -12.5, 2);
	glEnd();

	glBegin(GL_POLYGON);						//поддержка колеса
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -13.2, 2);
	glVertex3f(0, -13.2, 2);
	glVertex3f(0, -13.2, 1);
	glVertex3f(1, -13.2, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -13.5, 2);
	glVertex3f(0, -13.5, 2);
	glVertex3f(0, -13.5, 1);
	glVertex3f(1, -13.5, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -13.5, 2);
	glVertex3f(1, -13.5, 1);
	glVertex3f(1, -13.5, 1);
	glVertex3f(1, -13.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -13.5, 2);
	glVertex3f(0, -13.5, 1);
	glVertex3f(0, -13.2, 1);
	glVertex3f(0, -13.2, 2);
	glEnd();

	center_of_circle_3D.x = 0.5;
	center_of_circle_3D.y = -12.8;
	center_of_circle_3D.z = 1;
	GetCircleXZ(circle_points_3D, center_of_circle_3D, 0.8);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.4, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.4, circle_points_3D[i].z);
		glEnd();
	}

	glBegin(GL_POLYGON);					//левая ножка
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(2, -1, 6.3);
	glVertex3f(1, -1, 5);
	glVertex3f(0, -1, 5);
	glVertex3f(-1, -1, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(2, -3, 6.3);
	glVertex3f(1, -3, 5);
	glVertex3f(0, -3, 5);
	glVertex3f(-1, -3, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(2, -1, 6.3);
	glVertex3f(1, -1, 5);
	glVertex3f(1, -3, 5);
	glVertex3f(2, -3, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, -3, 6.3);
	glVertex3f(0, -3, 5);
	glVertex3f(0, -1, 5);
	glVertex3f(-1, -1, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -3, 5);
	glVertex3f(0, -3, 5);
	glVertex3f(0, -1, 5);
	glVertex3f(1, -1, 5);
	glEnd();

	glBegin(GL_POLYGON);					//основная ножка
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -2.5, 5);
	glVertex3f(0, -2.5, 5);
	glVertex3f(0, -2.5, 2);
	glVertex3f(1, -2.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -1.5, 5);
	glVertex3f(0, -1.5, 5);
	glVertex3f(0, -1.5, 2);
	glVertex3f(1, -1.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -1.5, 5);
	glVertex3f(1, -1.5, 2);
	glVertex3f(1, -2.5, 2);
	glVertex3f(1, -2.5, 5);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -1.5, 5);
	glVertex3f(0, -1.5, 2);
	glVertex3f(0, -2.5, 2);
	glVertex3f(0, -2.5, 5);
	glEnd();

	glBegin(GL_POLYGON);						//поддержка колеса
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -1.5, 2);
	glVertex3f(0, -1.5, 2);
	glVertex3f(0, -1.5, 1);
	glVertex3f(1, -1.5, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -1.8, 2);
	glVertex3f(0, -1.8, 2);
	glVertex3f(0, -1.8, 1);
	glVertex3f(1, -1.8, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -1.8, 2);
	glVertex3f(1, -1.8, 1);
	glVertex3f(1, -1.5, 1);
	glVertex3f(1, -1.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -1.8, 2);
	glVertex3f(0, -1.8, 1);
	glVertex3f(0, -1.5, 1);
	glVertex3f(0, -1.5, 2);
	glEnd();

	glBegin(GL_POLYGON);						//поддержка колеса
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -2.2, 2);
	glVertex3f(0, -2.2, 2);
	glVertex3f(0, -2.2, 1);
	glVertex3f(1, -2.2, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -2.5, 2);
	glVertex3f(0, -2.5, 2);
	glVertex3f(0, -2.5, 1);
	glVertex3f(1, -2.5, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(1, -2.5, 2);
	glVertex3f(1, -2.5, 1);
	glVertex3f(1, -2.5, 1);
	glVertex3f(1, -2.5, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -2.5, 2);
	glVertex3f(0, -2.5, 1);
	glVertex3f(0, -2.2, 1);
	glVertex3f(0, -2.2, 2);
	glEnd();

	center_of_circle_3D.x = 0.5;
	center_of_circle_3D.y = -1.8;
	center_of_circle_3D.z = 1;
	GetCircleXZ(circle_points_3D, center_of_circle_3D, 0.8);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.4, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.4, circle_points_3D[i].z);
		glEnd();
	}

	glBegin(GL_POLYGON);					//задняя ножка
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15, -2, 6.3);
	glVertex3f(15, -3, 5);
	glVertex3f(15, -4, 5);
	glVertex3f(15, -5, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(13, -2, 6.3);
	glVertex3f(13, -3, 5);
	glVertex3f(13, -4, 5);
	glVertex3f(13, -5, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15, -2, 6.3);
	glVertex3f(15, -3, 5);
	glVertex3f(13, -3, 5);
	glVertex3f(13, -2, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15, -4, 5);
	glVertex3f(15, -5, 6.3);
	glVertex3f(13, -5, 6.3);
	glVertex3f(13, -4, 5);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(15, -3, 5);
	glVertex3f(15, -4, 5);
	glVertex3f(13, -4, 5);
	glVertex3f(13, -3, 5);
	glEnd();

	glBegin(GL_POLYGON);					//основная ножка
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(14.5, -3, 5);
	glVertex3f(13.5, -3, 5);
	glVertex3f(13.5, -3, 2);
	glVertex3f(14.5, -3, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(14.5, -3, 5);
	glVertex3f(14.5, -4, 5);
	glVertex3f(14.5, -4, 2);
	glVertex3f(14.5, -3, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(13.5, -3, 5);
	glVertex3f(13.5, -4, 5);
	glVertex3f(13.5, -4, 2);
	glVertex3f(13.5, -3, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(14.5, -4, 5);
	glVertex3f(13.5, -4, 5);
	glVertex3f(13.5, -4, 2);
	glVertex3f(14.5, -4, 2);
	glEnd();

	glBegin(GL_POLYGON);						//поддержка колеса
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(14.5, -3, 2);
	glVertex3f(14.2, -3, 2);
	glVertex3f(14.2, -3, 1);
	glVertex3f(14.5, -3, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(14.5, -4, 2);
	glVertex3f(14.2, -4, 2);
	glVertex3f(14.2, -4, 1);
	glVertex3f(14.5, -4, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(14.5, -3, 2);
	glVertex3f(14.5, -3, 1);
	glVertex3f(14.5, -4, 1);
	glVertex3f(14.5, -4, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(14.2, -3, 2);
	glVertex3f(14.2, -3, 1);
	glVertex3f(14.2, -4, 1);
	glVertex3f(14.2, -4, 2);
	glEnd();

	glBegin(GL_POLYGON);						//поддержка колеса
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(13.8, -3, 2);
	glVertex3f(13.5, -3, 2);
	glVertex3f(13.5, -3, 1);
	glVertex3f(13.8, -3, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(13.8, -4, 2);
	glVertex3f(13.5, -4, 2);
	glVertex3f(13.5, -4, 1);
	glVertex3f(13.8, -4, 1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(13.8, -3, 2);
	glVertex3f(13.8, -3, 1);
	glVertex3f(13.8, -4, 1);
	glVertex3f(13.8, -4, 2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(13.5, -3, 2);
	glVertex3f(13.5, -3, 1);
	glVertex3f(13.5, -4, 1);
	glVertex3f(13.5, -4, 2);
	glEnd();

	center_of_circle_3D.x = 14.2;
	center_of_circle_3D.y = -3.5;
	center_of_circle_3D.z = 1;
	GetCircleYZ(circle_points_3D, center_of_circle_3D, 0.8);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x - 0.4, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x - 0.4, circle_points_3D[i].y, circle_points_3D[i].z);
		glEnd();
	}

	//Клавиатура (часть корпуса)

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 9.6);
	glVertex3f(-2, 0, 9.6);
	glVertex3f(-2, -15.0, 9.6);
	glVertex3f(0, -15.0, 9.6);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 6.3);
	glVertex3f(-2, 0, 6.3);
	glVertex3f(-2, -15.0, 6.3);
	glVertex3f(0, -15.0, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 6.3);
	glVertex3f(0, 0, 9.6);
	glVertex3f(-2, 0, 9.6);
	glVertex3f(-2, 0, 6.3);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, -15, 6.3);
	glVertex3f(0, -15, 9.6);
	glVertex3f(-2, -15, 9.6);
	glVertex3f(-2, -15, 6.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, 0, 6.3);
	glVertex3f(-2, -15.0, 6.3);
	glVertex3f(-2, 0, 9.6);
	glVertex3f(-2, -15.0, 9.6);
	glEnd();


	TextureStart("piano.bmp");

	glBegin(GL_QUAD_STRIP);								//производитель базз-рояля (текстуру сюда)
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2.01, 4, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-2.01, -6, 8.5);
	glTexCoord2f(1, 0);
	glVertex3f(-2.01, -9.0, 8.5);
	glTexCoord2f(0, 1);
	glVertex3f(-2.01, -6, 9.5);
	glTexCoord2f(1, 1);
	glVertex3f(-2.01, -9.0, 9.5);
	glEnd();

	TextureEnd();

	glBegin(GL_QUAD_STRIP);				//бортики (левый)
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.507, 10.2);
	glVertex3f(0, 0, 10.2);
	glVertex3f(-2, -0.507, 10.2);
	glVertex3f(-2, 0, 10.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 10.2);
	glVertex3f(-2, -0.507, 10.2);
	glVertex3f(0, 0, 9.6);
	glVertex3f(-2, -0.507, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -0.507, 10.2);
	glVertex3f(-2, 0, 10.2);
	glVertex3f(-2, -0.507, 9.6);
	glVertex3f(-2, 0, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -0.507, 10.2);
	glVertex3f(-2, 0, 10.2);
	glVertex3f(0, -0.507, 9.6);
	glVertex3f(-2, 0, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//подставка сверху (левая)
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -1.014, 9.8);
	glVertex3f(0, -4, 9.8);
	glVertex3f(-2, -1.014, 9.8);
	glVertex3f(-2, -4, 9.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -1.014, 9.8);
	glVertex3f(0, -4, 9.8);
	glVertex3f(0, -1.014, 9.6);
	glVertex3f(0, -4, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -4, 9.8);
	glVertex3f(-2, -4, 9.8);
	glVertex3f(0, -4, 9.6);
	glVertex3f(-2, -4, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -1.014, 9.8);
	glVertex3f(-2, -4, 9.8);
	glVertex3f(-2, -1.014, 9.6);
	glVertex3f(-2, -4, 9.6);
	glEnd();


	glBegin(GL_QUAD_STRIP);				//бортики (правый)
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -14.493, 10.2);
	glVertex3f(0, -15, 10.2);
	glVertex3f(-2, -14.493, 10.2);
	glVertex3f(-2, -15, 10.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -15, 10.2);
	glVertex3f(-2, -14.493, 10.2);
	glVertex3f(0, -15, 9.6);
	glVertex3f(-2, -14.493, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -14.493, 10.2);
	glVertex3f(-2, -15, 10.2);
	glVertex3f(-2, -14.493, 9.6);
	glVertex3f(-2, -15, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -15, 10.2);
	glVertex3f(-2, -14.493, 10.2);
	glVertex3f(0, -15, 9.6);
	glVertex3f(-2, -14.493, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//подставка сверху (правая)
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -13.986, 9.8);
	glVertex3f(0, -11, 9.8);
	glVertex3f(-2, -13.986, 9.8);
	glVertex3f(-2, -11, 9.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -13.986, 9.8);
	glVertex3f(0, -11, 9.8);
	glVertex3f(0, -13.986, 9.6);
	glVertex3f(0, -11, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -13.986, 9.8);
	glVertex3f(-2, -11, 9.8);
	glVertex3f(-2, -13.986, 9.6);
	glVertex3f(-2, -11, 9.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -11, 9.8);
	glVertex3f(-2, -11, 9.8);
	glVertex3f(0, -11, 9.6);
	glVertex3f(-2, -11, 9.6);
	glEnd();





	glBegin(GL_QUAD_STRIP);				//основание клавиатуры
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-3.6, 0, 6.3);
	glVertex3f(-3.6, -15.0, 6.3);
	glVertex3f(-2, 0, 6.3);
	glVertex3f(-2, -15, 6.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-3.6, 0, 7.2);
	glVertex3f(-3.6, -15.0, 7.2);
	glVertex3f(-2, 0, 7.2);
	glVertex3f(-2, -15.0, 7.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-3.6, 0, 7.2);
	glVertex3f(-3.6, -15.0, 7.2);
	glVertex3f(-3.6, 0, 6.3);
	glVertex3f(-3.6, -15.0, 6.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-3.6, 0, 7.2);
	glVertex3f(-2, 0, 7.2);
	glVertex3f(-3.6, 0, 6.3);
	glVertex3f(-2, 0, 6.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-3.6, -15.0, 7.2);
	glVertex3f(-2, -15.0, 7.2);
	glVertex3f(-3.6, -15.0, 6.3);
	glVertex3f(-2, -15.0, 6.3);
	glEnd();

	glPushMatrix();								//правый бок
	glTranslatef(-2, -15, 7.2);
	glRotatef(-90, 1, 0, 0);


	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 0.507);
	glVertex3f(0, -2.6, 0);
	glVertex3f(0, -2.6, 0.507);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 0.507);
	glVertex3f(-1.7, 0, 0);
	glVertex3f(-1.7, 0, 0.507);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.7, -0.65, 0);
	glVertex3f(-1.7, -0.65, 0.507);
	glVertex3f(-1.7, 0, 0);
	glVertex3f(-1.7, 0, 0.507);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.2, -1.3, 0);
	glVertex3f(-1.2, -1.3, 0.507);
	glVertex3f(-0.7, -1.3, 0);
	glVertex3f(-0.7, -1.3, 0.507);
	glEnd();

	arched = ArcCustom(-1.7, -0.65, -1.2, -1.3, 0.9);
	arched_points = NULL;
	count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	concave = ArcCustom(-0.7, -1.3, 0, -2.6, 1.5);
	concave_points = NULL;
	count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave, false);

	for (i = 0; i < count_of_elems_concave; i++)
	{

		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.507);
		glEnd();


	}

	for (i = 0; i < count_of_elems_arched; i++)
	{

		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.507);
		glEnd();


	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(-1.7, 0, 0);
	glVertex3f(-1.7, -0.65, 0);
	glVertex3f(-1.2, -1.3, 0);
	glVertex3f(-0.7, -1.3, 0);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
	}
	glEnd();


	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 0.507);
	glVertex3f(-1.7, 0, 0.507);
	glVertex3f(-1.7, -0.65, 0.507);
	glVertex3f(-1.2, -1.3, 0.507);
	glVertex3f(-0.7, -1.3, 0.507);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.507);
	}
	glEnd();



	for (i = 0; i < count_of_elems_concave; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.507);
		glVertex3f(0, 0, 0.507);
		glVertex3f(-0.1, 0, 0.507);
		glEnd();
	}

	for (i = 0; i < count_of_elems_concave; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.1, 0, 0);
		glEnd();
	}

	free(arched_points);
	free(concave_points);

	glPopMatrix();

	glPushMatrix();								//левый бок
	glTranslatef(-2, -0.507, 7.2);
	glRotatef(-90, 1, 0, 0);


	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 0.507);
	glVertex3f(0, -2.6, 0);
	glVertex3f(0, -2.6, 0.507);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 0.507);
	glVertex3f(-1.7, 0, 0);
	glVertex3f(-1.7, 0, 0.507);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.7, -0.65, 0);
	glVertex3f(-1.7, -0.65, 0.507);
	glVertex3f(-1.7, 0, 0);
	glVertex3f(-1.7, 0, 0.507);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.2, -1.3, 0);
	glVertex3f(-1.2, -1.3, 0.507);
	glVertex3f(-0.7, -1.3, 0);
	glVertex3f(-0.7, -1.3, 0.507);
	glEnd();

	arched = ArcCustom(-1.7, -0.65, -1.2, -1.3, 0.9);
	arched_points = NULL;
	count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	concave = ArcCustom(-0.7, -1.3, 0, -2.6, 1.5);
	concave_points = NULL;
	count_of_elems_concave;
	concave.GenerateArc(concave_points, count_of_elems_concave, false);

	for (i = 0; i < count_of_elems_concave; i++)
	{

		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.507);
		glEnd();


	}

	for (i = 0; i < count_of_elems_arched; i++)
	{

		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.507);
		glEnd();


	}

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(-1.7, 0, 0);
	glVertex3f(-1.7, -0.65, 0);
	glVertex3f(-1.2, -1.3, 0);
	glVertex3f(-0.7, -1.3, 0);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
	}
	glEnd();


	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 0.507);
	glVertex3f(-1.7, 0, 0.507);
	glVertex3f(-1.7, -0.65, 0.507);
	glVertex3f(-1.2, -1.3, 0.507);
	glVertex3f(-0.7, -1.3, 0.507);
	for (i = 0; i < count_of_elems_arched; i++)
	{
		glVertex3f(arched_points[i].x, arched_points[i].y, 0.507);
	}
	glEnd();



	for (i = 0; i < count_of_elems_concave; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0.507);
		glVertex3f(0, 0, 0.507);
		glVertex3f(-0.1, 0, 0.507);
		glEnd();
	}

	for (i = 0; i < count_of_elems_concave; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(concave_points[i].x, concave_points[i].y, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.1, 0, 0);
		glEnd();
	}

	free(arched_points);
	free(concave_points);

	glPopMatrix();

	//подставки на клавиатуре
	glBegin(GL_QUAD_STRIP);					//левая						
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -0.507, 7.8);
	glVertex3f(-3.45, -0.507, 7.8);
	glVertex3f(-2, -1.52, 7.8);
	glVertex3f(-3.45, -1.52, 7.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-3.45, -0.507, 7.8);
	glVertex3f(-3.45, -1.52, 7.8);
	glVertex3f(-3.45, -0.507, 7.2);
	glVertex3f(-3.45, -1.52, 7.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -1.52, 7.8);
	glVertex3f(-3.45, -1.52, 7.8);
	glVertex3f(-2, -1.52, 7.2);
	glVertex3f(-3.45, -1.52, 7.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//правая					
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -15, 7.8);
	glVertex3f(-3.45, -15, 7.8);
	glVertex3f(-2, -13.48, 7.8);
	glVertex3f(-3.45, -13.48, 7.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-3.45, -15, 7.8);
	glVertex3f(-3.45, -13.48, 7.8);
	glVertex3f(-3.45, -15, 7.2);
	glVertex3f(-3.45, -13.48, 7.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-2, -13.48, 7.8);
	glVertex3f(-3.45, -13.48, 7.8);
	glVertex3f(-2, -13.48, 7.2);
	glVertex3f(-3.45, -13.48, 7.2);
	glEnd();

	glPushMatrix();								//центральная
	glTranslatef(-3.45, -0.507, 7.4);
	glRotatef(90, 1, 0, 0);

	arched = ArcCustom(0, 0, -0.25, -0.25, 0.3);
	arched_points = NULL;
	arched.GenerateArc(arched_points, count_of_elems_arched);

	for (i = 0; i < count_of_elems_concave; i++)
	{

		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 13.986);
		glEnd();


	}

	free(arched_points);


	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2, -1.52, 7.4);
	DrawPianoKeyboard();
	glPopMatrix();

	//пюпитир

	glPushMatrix();
	glTranslatef(-1.7, -4, 9.6);
	glRotatef(120, 0, 1, 0);


	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-1.8, -7, 0.1);
	glVertex3f(-1.6, -7, 0.1);
	glVertex3f(-1.6, 0, 0.1);
	glVertex3f(-1.8, 0, 0.1);

	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-0.2, -7, 0.1);
	glVertex3f(0, -7, 0.1);
	glVertex3f(0, 0, 0.1);
	glVertex3f(-0.2, 0, 0.1);

	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-1.5, -6.7, 0.1);
	glVertex3f(-0.3, -6.7, 0.1);
	glVertex3f(-0.3, -0.3, 0.1);
	glVertex3f(-1.5, -0.3, 0.1);

	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-0.3, -6.7, 0.1);
	glVertex3f(-0.2, -7, 0.1);
	glVertex3f(-0.2, 0, 0.1);
	glVertex3f(-0.3, -0.3, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.6, -7, 0.1);
	glVertex3f(-1.5, -6.7, 0.1);
	glVertex3f(-1.5, -0.3, 0.1);
	glVertex3f(-1.6, 0, 0.1);
	glEnd();


	glBegin(GL_POLYGON);				//низ
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-1.8, -7, 0);
	glVertex3f(-1.6, -7, 0);
	glVertex3f(-1.6, 0, 0);
	glVertex3f(-1.8, 0, 0);

	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-0.2, -7, 0);
	glVertex3f(0, -7, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.2, 0, 0);

	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-1.5, -6.7, 0);
	glVertex3f(-0.3, -6.7, 0);
	glVertex3f(-0.3, -0.3, 0);
	glVertex3f(-1.5, -0.3, 0);

	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);

	glVertex3f(-0.3, -6.7, 0);
	glVertex3f(-0.2, -7, 0);
	glVertex3f(-0.2, 0, 0);
	glVertex3f(-0.3, -0.3, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.6, -7, 0);
	glVertex3f(-1.5, -6.7, 0);
	glVertex3f(-1.5, -0.3, 0);
	glVertex3f(-1.6, 0, 0);
	glEnd();

	glBegin(GL_POLYGON);				//бока
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.8, -7, 0);
	glVertex3f(-1.6, -7, 0);
	glVertex3f(-1.6, -7, 0.1);
	glVertex3f(-1.8, -7, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.6, 0, 0);
	glVertex3f(-1.8, 0, 0);
	glVertex3f(-1.8, 0, 0.1);
	glVertex3f(-1.6, 0, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.2, -7, 0);
	glVertex3f(0, -7, 0);
	glVertex3f(0, -7, 0.1);
	glVertex3f(-0.2, -7, 0.1);
	glEnd();


	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.2, 0, 0);
	glVertex3f(-0.2, 0, 0.1);
	glVertex3f(0, 0, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.5, -6.7, 0);
	glVertex3f(-0.3, -6.7, 0);
	glVertex3f(-0.3, -6.7, 0.1);
	glVertex3f(-1.5, -6.7, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.3, -0.3, 0);
	glVertex3f(-1.5, -0.3, 0);
	glVertex3f(-1.5, -0.3, 0.1);
	glVertex3f(-0.3, -0.3, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.3, -6.7, 0);
	glVertex3f(-0.2, -7, 0);
	glVertex3f(-0.2, -7, 0.1);
	glVertex3f(-0.3, -6.7, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-0.2, 0, 0);
	glVertex3f(-0.3, -0.3, 0);
	glVertex3f(-0.3, -0.3, 0.1);
	glVertex3f(-0.2, 0, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.6, -7, 0);
	glVertex3f(-1.5, -6.7, 0);
	glVertex3f(-1.5, -6.7, 0.1);
	glVertex3f(-1.6, -7, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.5, -0.3, 0);
	glVertex3f(-1.6, 0, 0);
	glVertex3f(-1.6, 0, 0.1);
	glVertex3f(-1.5, -0.3, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(-1.8, -7, 0);
	glVertex3f(-1.8, 0, 0);
	glVertex3f(-1.8, 0, 0.1);
	glVertex3f(-1.8, -7, 0.1);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -7, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 0.1);
	glVertex3f(0, -7, 0.1);
	glEnd();

	glPopMatrix();

	DrawDamperArray();

	glPushMatrix();
	glTranslatef(3, -14.24, 10.1);
	glRotatef(270, 0, 0, 1);
	glRotatef(50, 0, 1, 0);

	DrawSupport();

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 10.2);
	glRotatef(360 - 31.866, 1, 0, 0);

	DrawCover();


	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.5, -14.493, 10);
	DrawHolders();
	glPopMatrix();

}

void DrawJukebox()
{
	//ДЖУКБОКС---------------------------------------------------------------------------------

	glBegin(GL_QUAD_STRIP);								//основание
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, 0, 9.9);
	glVertex3f(6.4, 0, 0.3);
	glVertex3f(6.4, -9.6, 9.9);
	glVertex3f(6.4, -9.6, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, 0, 9.9);
	glVertex3f(6.4, 0, 0.3);
	glVertex3f(0, 0, 9.9);
	glVertex3f(0, 0, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -9.6, 9.9);
	glVertex3f(6.4, -9.6, 0.3);
	glVertex3f(0, -9.6, 9.9);
	glVertex3f(0, -9.6, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -9.6, 0.3);
	glVertex3f(0, -9.6, 0.3);
	glVertex3f(6.4, 0, 0.3);
	glVertex3f(0, 0, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//ножка (левая)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -1, 0.3);
	glVertex3f(0, -1, 0.3);
	glVertex3f(6.4, 0, 0.3);
	glVertex3f(0, 0, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -1, 0);
	glVertex3f(0, -1, 0);
	glVertex3f(6.4, 0, 0);
	glVertex3f(0, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -1, 0.3);
	glVertex3f(6.4, 0, 0.3);
	glVertex3f(6.4, -1, 0);
	glVertex3f(6.4, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -1, 0.3);
	glVertex3f(0, 0, 0.3);
	glVertex3f(0, -1, 0);
	glVertex3f(0, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -1, 0.3);
	glVertex3f(0, -1, 0.3);
	glVertex3f(6.4, -1, 0);
	glVertex3f(0, -1, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, 0, 0.3);
	glVertex3f(0, 0, 0.3);
	glVertex3f(6.4, 0, 0);
	glVertex3f(0, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);										//ножка (правая)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -9.6, 0.3);
	glVertex3f(0, -9.6, 0.3);
	glVertex3f(6.4, -8.6, 0.3);
	glVertex3f(0, -8.6, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -9.6, 0);
	glVertex3f(0, -9.6, 0);
	glVertex3f(6.4, -8.6, 0);
	glVertex3f(0, -8.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -9.6, 0.3);
	glVertex3f(6.4, -8.6, 0.3);
	glVertex3f(6.4, -9.6, 0);
	glVertex3f(6.4, -8.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(0, -9.6, 0.3);
	glVertex3f(0, -8.6, 0.3);
	glVertex3f(0, -9.6, 0);
	glVertex3f(0, -8.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -8.6, 0.3);
	glVertex3f(0, -8.6, 0.3);
	glVertex3f(6.4, -8.6, 0);
	glVertex3f(0, -8.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(6.4, -9.6, 0.3);
	glVertex3f(0, -9.6, 0.3);
	glVertex3f(6.4, -9.6, 0);
	glVertex3f(0, -9.6, 0);
	glEnd();

	//ободок
	center_of_circle.x = 0;										//насадки
	center_of_circle.y = -0.7;
	GetCircle(circle_points, center_of_circle, 1.15);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.3);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0.3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.3);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.3);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0.3);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.3);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.3);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.3);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.3);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glEnd();

	center_of_circle.x = 0;
	center_of_circle.y = -8.9;
	GetCircle(circle_points, center_of_circle, 1.15);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.3);
		glVertex3f(center_of_circle.x, center_of_circle.y, 0.3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.3);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.3);
	glVertex3f(center_of_circle.x, center_of_circle.y, 0.3);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.3);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0.3);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0.3);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 0.3);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 0.3);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glEnd();

	glPushMatrix();
	glTranslatef(0, 0, 0);
	gacha_index++;				//генерация капель
	if (gacha_index == 100)
		gacha_index = 0;
	if (gacha_var[gacha_index] < 66)
	{
		count_of_drops++;
		array_of_drops = (drop*)realloc(array_of_drops, count_of_drops * sizeof(drop));
		float radius_x = 0.1 + (std::rand() % 100) / 1000.;
		float radius_y = 0.1 + (std::rand() % 100) / 1000.;
		float x_position = (rand() % 100 / 100. * 0.7 - radius_x - 0.4);
		float y_position = -(8.6 + 0.8 * (rand() % 100 / 100.));
		array_of_drops[count_of_drops - 1] = drop(x_position, y_position, radius_x, radius_y, 9.9, -4.8);
	}

	i_dr = 0;
	while (i_dr < count_of_drops)
	{
		glColor3b(248 / 255., 180 / 255., 148 / 255.);
		array_of_drops[i_dr].Move();
		i_dr++;
	}
	for (int i = 0; i < count_of_drops; i++)
	{
		if (array_of_drops[i].IsDeletable())
		{
			for (int j = i; j < count_of_drops - 1; j++)
				array_of_drops[j] = array_of_drops[j + 1];
			i--;
			count_of_drops--;
		}
	}
	glPopMatrix();

	center_of_circle.x = 0;										//ободок - параллельная часть
	center_of_circle.y = -0.7;
	GetCircle(circle_points, center_of_circle, 1);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		if (i < 210)
			glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
		else
			glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		if (i < 210)
			glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
		else
			glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 9.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 9.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 9.9);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 9.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 9.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 9.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		if (i < 210)
			glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
		else
			glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(circle_points[i].x, circle_points[i].y, 9.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 9.9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(circle_points[0].x, circle_points[0].y, 9.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 9.9);
	glEnd();

	center_of_circle.x = 0;
	center_of_circle.y = -8.9;
	GetCircle(circle_points, center_of_circle, 1);


	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		if (i > 150)
			glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
		else
			glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(-2, 4, 0);
		if (i > 150)
			glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
		else
			glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(circle_points[i].x, circle_points[i].y, 9.9);
		glVertex3f(center_of_circle.x, center_of_circle.y, 9.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 9.9);
		glEnd();
	}

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-2, 4, 0);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(circle_points[0].x, circle_points[0].y, 9.9);
	glVertex3f(center_of_circle.x, center_of_circle.y, 9.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 9.9);
	glEnd();

	for (int i = 0; i < 359; i++)
	{
		glBegin(GL_QUAD_STRIP);
		if (i > 150)
			glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
		else
			glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glNormal3f(-2, 4, 0);
		glVertex3f(circle_points[i].x, circle_points[i].y, 1.9);
		glVertex3f(circle_points[i].x, circle_points[i].y, 9.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 1.9);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 9.9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glNormal3f(-2, 4, 0);
	glVertex3f(circle_points[0].x, circle_points[0].y, 1.9);
	glVertex3f(circle_points[0].x, circle_points[0].y, 9.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 1.9);
	glVertex3f(circle_points[359].x, circle_points[359].y, 9.9);
	glEnd();

	

	glPushMatrix();
	glTranslatef(0, -0.7, 9.9);
	center_of_circle.x = 0;										//ободок - дуга
	center_of_circle.y = -8.2;
	GetCircle(circle_points, center_of_circle, 1);
	for (int i = 0; i < 359; i++)
	{
		for (int j = 0; j < 179; j++)
		{
			glBegin(GL_QUAD_STRIP);
			if (i < 150)
				glColor3d(78 / 255., 67 / 255., 71 / 255.);
			else
				glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
			glVertex3f(arched_points_jukebox[i][j].x, arched_points_jukebox[i][j].y, arched_points_jukebox[i][j].z);
			glVertex3f(arched_points_jukebox[i + 1][j].x, arched_points_jukebox[i + 1][j].y, arched_points_jukebox[i + 1][j].z);
			glVertex3f(arched_points_jukebox[i][j + 1].x, arched_points_jukebox[i][j + 1].y, arched_points_jukebox[i][j + 1].z);
			glVertex3f(arched_points_jukebox[i + 1][j + 1].x, arched_points_jukebox[i + 1][j + 1].y, arched_points_jukebox[i + 1][j + 1].z);
			glEnd();
		}
	}

	for (int i = 0; i < 359; i++)										//4.1 - центр
	{
		glBegin(GL_QUAD_STRIP);
		if (i < 150)
			glColor3f(78 / 255., 67 / 255., 71 / 255.);
		else
			glColor4d(234 / 255., 128 / 255., 107 / 255., 0.8);
		glVertex3f(circle_points[i].x, circle_points[i].y, 0);
		glVertex3f(arched_points_jukebox[i][179].x, arched_points_jukebox[i][179].y, arched_points_jukebox[i][179].z);
		glVertex3f(circle_points[i + 1].x, circle_points[i + 1].y, 0);
		glVertex3f(arched_points_jukebox[i + 1][179].x, arched_points_jukebox[i + 1][179].y, arched_points_jukebox[i + 1][179].z);
		glEnd();
	}


	glPopMatrix();

	Point3D arc_of_jukebox[180];					//дуга на корпусе
	Point3D center;
	center.y = -4.8;
	center.x = -1;
	center.z = 9.9;
	GetSubCircleYZ(arc_of_jukebox, center, 4.8);

	for (int j = 0; j < 179; j++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(0, arc_of_jukebox[j].y, arc_of_jukebox[j].z);
		glVertex3f(6.4, arc_of_jukebox[j].y, arc_of_jukebox[j].z);
		glVertex3f(0, arc_of_jukebox[j + 1].y, arc_of_jukebox[j + 1].z);
		glVertex3f(6.4, arc_of_jukebox[j + 1].y, arc_of_jukebox[j + 1].z);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, arc_of_jukebox[179].y, arc_of_jukebox[179].z);
	glVertex3f(6.4, arc_of_jukebox[179].y, arc_of_jukebox[179].z);
	glVertex3f(0, -9.6, 9.9);
	glVertex3f(6.4, -9.6, 9.9);
	glEnd();

	for (int j = 0; j < 179; j++)
	{
		glBegin(GL_TRIANGLES);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(6.4, arc_of_jukebox[j].y, arc_of_jukebox[j].z);
		glVertex3f(6.4, arc_of_jukebox[j + 1].y, arc_of_jukebox[j + 1].z);
		glVertex3f(6.4, center.y, center.z);
		glEnd();
	}

	glBegin(GL_TRIANGLES);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(6.4, arc_of_jukebox[179].y, arc_of_jukebox[179].z);
	glVertex3f(6.4, center.y, center.z);
	glVertex3f(6.4, -9.6, 9.9);
	glEnd();

	Point3D arc_of_jukebox_inner[180];
	center.y = -4.8;								//отступ внутри
	center.x = 0.3;
	center.z = 9.9;
	GetSubCircleYZ(arc_of_jukebox, center, 3.2);
	GetSubCircleYZ(arc_of_jukebox_inner, center, 2.9);

	for (int j = 0; j < 179; j++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0.3, arc_of_jukebox[j].y, arc_of_jukebox[j].z);
		glVertex3f(0.3, arc_of_jukebox[j + 1].y, arc_of_jukebox[j + 1].z);
		glVertex3f(0.3, arc_of_jukebox_inner[j].y, arc_of_jukebox_inner[j].z);
		glVertex3f(0.3, arc_of_jukebox_inner[j + 1].y, arc_of_jukebox_inner[j + 1].z);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(0.3, arc_of_jukebox[179].y, arc_of_jukebox[179].z);
	glVertex3f(0.3, arc_of_jukebox_inner[179].y, arc_of_jukebox_inner[179].z);
	glVertex3f(0.3, -8, 9.9);
	glVertex3f(0.3, -7.7, 9.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(0.3, -8, 9.9);
	glVertex3f(0.3, -7.7, 9.9);
	glVertex3f(0.3, -8, 2.2);
	glVertex3f(0.3, -7.7, 2.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(0.3, -1.6, 9.9);
	glVertex3f(0.3, -1.9, 9.9);
	glVertex3f(0.3, -1.6, 2.2);
	glVertex3f(0.3, -1.9, 2.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(0.3, -6, 1.2);
	glVertex3f(0.3, -5.7, 1.2);
	glVertex3f(0.3, -8, 2.2);
	glVertex3f(0.3, -7.7, 2.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(0.3, -3.6, 1.2);
	glVertex3f(0.3, -3.9, 1.2);
	glVertex3f(0.3, -1.6, 2.2);
	glVertex3f(0.3, -1.9, 2.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(234 / 255., 128 / 255., 107 / 255.);
	glVertex3f(0.3, -3.6, 1.3);
	glVertex3f(0.3, -3.9, 1.2);
	glVertex3f(0.3, -6, 1.3);
	glVertex3f(0.3, -5.7, 1.2);
	glEnd();


	glBegin(GL_QUAD_STRIP);							//порог
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.6, 1.2);
	glVertex3f(0, -1.6, 2.2);
	glVertex3f(0, -3.6, 0.3);
	glVertex3f(0, -1.6, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.6, 1.2);
	glVertex3f(0, -1.6, 2.2);
	glVertex3f(0.3, -3.6, 1.2);
	glVertex3f(0.3, -1.6, 2.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -6, 1.2);
	glVertex3f(0, -8, 2.2);
	glVertex3f(0, -6, 0.3);
	glVertex3f(0, -8, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -6, 1.2);
	glVertex3f(0, -8, 2.2);
	glVertex3f(0.3, -6, 1.2);
	glVertex3f(0.3, -8, 2.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.6, 1.2);
	glVertex3f(0, -6, 1.2);
	glVertex3f(0, -3.6, 0.3);
	glVertex3f(0, -6, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.6, 1.2);
	glVertex3f(0, -6, 1.2);
	glVertex3f(0.3, -3.6, 1.2);
	glVertex3f(0.3, -6, 1.2);
	glEnd();

	for (float i = -1.7; i > -7.7; i -= 0.29)				//решётка на динамике
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(1, 0, 0);
		glVertex3f(0.31, i - 0.29, 1.2);
		glVertex3f(0.31, i - 0.39, 1.2);
		glVertex3f(0.31, i - 0.29, 8);
		glVertex3f(0.31, i - 0.39, 8);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);									//панель для ввода монет и декоративного элемента
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -1.9, 8);
	glVertex3f(0.3, -7.7, 8);
	glVertex3f(0.3, -1.9, 9);
	glVertex3f(0.3, -7.7, 9);
	glEnd();

	TextureStart("song_list.bmp");

	glBegin(GL_QUAD_STRIP);								//список треков
	glColor3f(0.0f, 0.0f, 0.0f);
	glNormal3f(0.29, 4, 0);
	glTexCoord2f(0, 0);
	glVertex3f(0.29, -2, 8);
	glTexCoord2f(1, 0);
	glVertex3f(0.29, -3, 8);
	glTexCoord2f(0, 1);
	glVertex3f(0.29, -2, 9);
	glTexCoord2f(1, 1);
	glVertex3f(0.29, -3, 9);
	glEnd();

	TextureEnd();

	glBegin(GL_QUAD_STRIP);									//монетоприёмник
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -6.5, 8.25);
	glVertex3f(0.2, -6.6, 8.25);
	glVertex3f(0.3, -6.5, 8.75);
	glVertex3f(0.2, -6.6, 8.75);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -6.5, 8.25);
	glVertex3f(0.2, -6.6, 8.25);
	glVertex3f(0.3, -6.6, 8.25);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -6.5, 8.75);
	glVertex3f(0.2, -6.6, 8.75);
	glVertex3f(0.3, -6.6, 8.75);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -7.2, 8.25);
	glVertex3f(0.2, -7.1, 8.25);
	glVertex3f(0.3, -7.2, 8.75);
	glVertex3f(0.2, -7.1, 8.75);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -7.2, 8.25);
	glVertex3f(0.2, -7.1, 8.25);
	glVertex3f(0.3, -7.1, 8.25);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -7.2, 8.75);
	glVertex3f(0.2, -7.1, 8.75);
	glVertex3f(0.3, -7.1, 8.75);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -7.1, 8.85);
	glVertex3f(0.3, -6.6, 8.85);
	glVertex3f(0.2, -7.1, 8.75);
	glVertex3f(0.2, -6.6, 8.75);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -7.1, 8.85);
	glVertex3f(0.2, -7.1, 8.75);
	glVertex3f(0.3, -7.1, 8.75);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -6.6, 8.85);
	glVertex3f(0.2, -6.6, 8.75);
	glVertex3f(0.3, -6.6, 8.75);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -7.1, 8.15);
	glVertex3f(0.3, -6.6, 8.15);
	glVertex3f(0.2, -7.1, 8.25);
	glVertex3f(0.2, -6.6, 8.25);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -7.1, 8.15);
	glVertex3f(0.2, -7.1, 8.25);
	glVertex3f(0.3, -7.1, 8.25);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.3, -6.6, 8.15);
	glVertex3f(0.2, -6.6, 8.25);
	glVertex3f(0.3, -6.6, 8.25);
	glEnd();


	glaStencilWindow(1);


	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 1);
	glVertex3f(0.2, -6.84, 8.375);
	glVertex3f(0.2, -6.86, 8.325);
	glVertex3f(0.2, -6.84, 8.675);
	glVertex3f(0.2, -6.86, 8.625);
	glEnd();


	glaStencilWall(1);

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0.2, -7.1, 8.75);
	glVertex3f(0.2, -6.6, 8.75);
	glVertex3f(0.2, -7.1, 8.25);
	glVertex3f(0.2, -6.6, 8.25);
	glEnd();

	glaStencilEnd();

	for (int i = 1; i < 9; i++)								//кнопки
	{
		glPushMatrix();
		glTranslatef(0, -3.8 - (i - 1)*0.2, 9);

		glBegin(GL_QUAD_STRIP);
		if (current_disc == i && !isFirstTimeChangingDiscs && !isJukeboxStopped)
			glColor3f(234 / 255., 128 / 255., 107 / 255.);
		else if (isFirstTimeChangingDiscs || isJukeboxStopped)
			glColor3f(1, 1, 1);
		else
			glColor3f(1, 1, 1);
		glVertex3f(0, 0, 0.1);
		glVertex3f(0, -0.1, 0.1);
		glVertex3f(0.3, 0, 0.1);
		glVertex3f(0.3, -0.1, 0.1);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		if (current_disc == i && !isFirstTimeChangingDiscs && !isJukeboxStopped)
			glColor3f(234 / 255., 128 / 255., 107 / 255.);
		else if (isFirstTimeChangingDiscs || isJukeboxStopped)
			glColor3f(1, 1, 1);
		else
			glColor3f(1, 1, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, -0.1, 0);
		glVertex3f(0.3, 0, 0);
		glVertex3f(0.3, -0.1, 0);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		if (current_disc == i && !isFirstTimeChangingDiscs && !isJukeboxStopped)
			glColor3f(234 / 255., 128 / 255., 107 / 255.);
		else if (isFirstTimeChangingDiscs || isJukeboxStopped)
			glColor3f(1, 1, 1);
		else
			glColor3f(1, 1, 1);
		glVertex3f(0, 0, 0.1);
		glVertex3f(0, -0.1, 0.1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, -0.1, 0);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		if (current_disc == i && !isFirstTimeChangingDiscs && !isJukeboxStopped)
			glColor3f(234 / 255., 128 / 255., 107 / 255.);
		else if (isFirstTimeChangingDiscs || isJukeboxStopped)
			glColor3f(1, 1, 1);
		else
			glColor3f(1, 1, 1);
		glVertex3f(0.3, 0, 0.1);
		glVertex3f(0.3, -0.1, 0.1);
		glVertex3f(0.3, 0, 0);
		glVertex3f(0.3, -0.1, 0);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		if (current_disc == i && !isFirstTimeChangingDiscs && !isJukeboxStopped)
			glColor3f(234 / 255., 128 / 255., 107 / 255.);
		else if (isFirstTimeChangingDiscs || isJukeboxStopped)
			glColor3f(1, 1, 1);
		else
			glColor3f(1, 1, 1);
		glVertex3f(0, -0.1, 0.1);
		glVertex3f(0.3, -0.1, 0.1);
		glVertex3f(0, -0.1, 0);
		glVertex3f(0.3, -0.1, 0);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		if (current_disc == i && !isFirstTimeChangingDiscs && !isJukeboxStopped)
			glColor3f(234 / 255., 128 / 255., 107 / 255.);
		else if (isFirstTimeChangingDiscs || isJukeboxStopped)
			glColor3f(1, 1, 1);
		else
			glColor3f(1, 1, 1);
		glVertex3f(0, 0, 0.1);
		glVertex3f(0.3, 0, 0.1);
		glVertex3f(0, 0, 0);
		glVertex3f(0.3, 0, 0);
		glEnd();

		glPopMatrix();
	}

	glBegin(GL_QUAD_STRIP);				//украшение для кнопок
	glColor3f(1, 0, 0);
	glVertex3f(0, -3.8, 9);
	glVertex3f(0, -5.8, 9);
	glVertex3f(0.3, -3.8, 9);
	glVertex3f(0.3, -5.8, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -4.1, 8.8);
	glVertex3f(0, -5.5, 8.8);
	glVertex3f(0.3, -4.1, 8.8);
	glVertex3f(0.3, -5.5, 8.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -3.8, 9);
	glVertex3f(0, -5.8, 9);
	glVertex3f(0, -4.1, 8.8);
	glVertex3f(0, -5.5, 8.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -3.8, 9);
	glVertex3f(0, -3.3, 8.7);
	glVertex3f(0.3, -3.8, 9);
	glVertex3f(0.3, -3.3, 8.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -4.1, 8.8);
	glVertex3f(0, -3.8, 8.7);
	glVertex3f(0.3, -4.1, 8.8);
	glVertex3f(0.3, -3.8, 8.7);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1, 0, 0);
	glVertex3f(0, -3.8, 9);
	glVertex3f(0, -4.1, 8.8);
	glVertex3f(0, -3.8, 8.7);
	glVertex3f(0, -3.3, 8.7);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -6.3, 8.7);
	glVertex3f(0, -5.8, 9);
	glVertex3f(0.3, -6.3, 8.7);
	glVertex3f(0.3, -5.8, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -5.5, 8.8);
	glVertex3f(0.3, -5.5, 8.8);
	glVertex3f(0, -5.8, 8.7);
	glVertex3f(0.3, -5.8, 8.7);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1, 0, 0);
	glVertex3f(0, -5.8, 9);
	glVertex3f(0, -5.5, 8.8);
	glVertex3f(0, -5.8, 8.7);
	glVertex3f(0, -6.3, 8.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -3.3, 8.7);
	glVertex3f(0, -4.3, 7);
	glVertex3f(0.3, -3.3, 8.7);
	glVertex3f(0.3, -4.3, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -3.8, 8.7);
	glVertex3f(0, -4.6, 7.2);
	glVertex3f(0.3, -3.8, 8.7);
	glVertex3f(0.3, -4.6, 7.2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1, 0, 0);
	glVertex3f(0, -3.8, 8.7);
	glVertex3f(0, -3.3, 8.7);
	glVertex3f(0, -4.3, 7);
	glVertex3f(0, -4.6, 7.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//бок
	glColor3f(1, 0, 0);
	glVertex3f(0, -6.3, 8.7);
	glVertex3f(0, -5.3, 7);
	glVertex3f(0.3, -6.3, 8.7);
	glVertex3f(0.3, -5.3, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -5.8, 8.7);
	glVertex3f(0, -5, 7.2);
	glVertex3f(0.3, -5.8, 8.7);
	glVertex3f(0.3, -5, 7.2);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1, 0, 0);
	glVertex3f(0, -5.8, 8.7);
	glVertex3f(0, -6.3, 8.7);
	glVertex3f(0, -5.3, 7);
	glVertex3f(0, -5, 7.2);
	glEnd();


	glBegin(GL_QUAD_STRIP);				//низ
	glColor3f(1, 0, 0);
	glVertex3f(0, -4.3, 7);
	glVertex3f(0, -5.3, 7);
	glVertex3f(0.3, -4.3, 7);
	glVertex3f(0.3, -5.3, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -5, 7.2);
	glVertex3f(0, -4.6, 7.2);
	glVertex3f(0.3, -5, 7.2);
	glVertex3f(0.3, -4.6, 7.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 0, 0);
	glVertex3f(0, -5, 7.2);
	glVertex3f(0, -5.3, 7);
	glVertex3f(0, -4.6, 7.2);
	glVertex3f(0, -4.3, 7);
	glEnd();

	TextureStart("key.bmp");

	glBegin(GL_POLYGON);				//поле для изображения
	glColor3f(1, 1, 0);
	glTexCoord2d(0.6, 0);
	glVertex3f(0.1, -5, 7.2);
	glTexCoord2d(0.3, 0);
	glVertex3f(0.1, -4.6, 7.2);
	glTexCoord2d(0, 0.8);
	glVertex3f(0.1, -3.8, 8.7);
	glTexCoord2d(0.2, 1);
	glVertex3f(0.1, -4.1, 8.8);
	glTexCoord2d(0.8, 1);
	glVertex3f(0.1, -5.5, 8.8);
	glTexCoord2d(1, 0.8);
	glVertex3f(0.1, -5.8, 8.7);
	glEnd();

	TextureEnd();

	glBegin(GL_QUAD_STRIP);				//поле для списка песен и бортик до стекла
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -1.9, 9);
	glVertex3f(1.3, -1.9, 9);
	glVertex3f(0.3, -7.7, 9);
	glVertex3f(1.3, -7.7, 9);
	glEnd();

	for (int j = 0; j < 179; j++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(78 / 255., 67 / 255., 71 / 255.);
		glVertex3f(0.3, arc_of_jukebox_inner[j].y, arc_of_jukebox_inner[j].z);
		glVertex3f(0.3, arc_of_jukebox_inner[j + 1].y, arc_of_jukebox_inner[j + 1].z);
		glVertex3f(1.3, arc_of_jukebox_inner[j].y, arc_of_jukebox_inner[j].z);
		glVertex3f(1.3, arc_of_jukebox_inner[j + 1].y, arc_of_jukebox_inner[j + 1].z);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -7.7, 9.9);
	glVertex3f(1.3, -7.7, 9.9);
	glVertex3f(0.3, arc_of_jukebox_inner[179].y, arc_of_jukebox_inner[179].z);
	glVertex3f(1.3, arc_of_jukebox_inner[179].y, arc_of_jukebox_inner[179].z);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -1.9, 9.9);
	glVertex3f(1.3, -1.9, 9.9);
	glVertex3f(0.3, -1.9, 9);
	glVertex3f(1.3, -1.9, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -7.7, 9.9);
	glVertex3f(1.3, -7.7, 9.9);
	glVertex3f(0.3, -7.7, 9);
	glVertex3f(1.3, -7.7, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//пол внутренней поверхности
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(6.4, -1.9, 9);
	glVertex3f(1.3, -1.9, 9);
	glVertex3f(6.4, -7.7, 9);
	glVertex3f(1.3, -7.7, 9);
	glEnd();

	glPushMatrix();					//считыватель
	glTranslatef(3.5, -4.2, 9.3);
	glPushMatrix();
	glTranslatef(-0.3, -2.3, -0.3);
	DrawDiscReader();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();						//устройство хранения пластинок
	glTranslatef(3.5, -4.2, 9.3);
	if (new_angle != current_angle_of_disc_automat && !unplug)
	{
		isJukeboxStopped = false;
		current_angle_of_disc_automat = current_angle_of_disc_automat + direction_of_rotation_of_disc_automat * speed_of_spin;
		if (current_angle_of_disc_automat < 0)
			current_angle_of_disc_automat = 360;
		if (current_angle_of_disc_automat > 360)
			current_angle_of_disc_automat = 0;

		glRotatef(-current_angle_of_disc_automat, 0, 0, 1);
		printf("%f\n", current_angle_of_disc_automat);
		if (new_angle == current_angle_of_disc_automat)
		{
			printf("PLUG_ZONE\n");
			plug = true;
			current_disc = new_disc;
		}
	}
	else
	{
		glRotatef(-current_angle_of_disc_automat, 0, 0, 1);
	}

	if (isSameDiscPicked)
	{
		printf("PLUG_ZONE_SAME\n");
		plug = true;
		isSameDiscPicked = false;
	}

	/*glPushMatrix();
	glTranslatef(0.15, 1.5, -0.3);*/
	DrawDiscPlatform();
	/*glPopMatrix();*/


	/*for (int i = 0; i < 8; i++)
		disc_holders[i].DrawDiscHolder(current_disc, plug, unplug);*/
	glPopMatrix();

	glBegin(GL_POLYGON);			//стекло перед внутренним устройством
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(1.3, -1.9, 9);
	glVertex3f(1.3, -1.9, 9.9);

	for (int j = 0; j < 180; j++)
	{
		glVertex3f(1.3, arc_of_jukebox_inner[j].y, arc_of_jukebox_inner[j].z);
	}

	glVertex3f(1.3, -7.7, 9.9);
	glVertex3f(1.3, -7.7, 9);


	glEnd();


}

void DrawBarChair()
{
	//БАРНЫЙ СТУЛ--------------------------------------------------------------------------------------

	glBegin(GL_QUAD_STRIP);						//передняя ножка (левая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0.3, -0.3, 0);
	glVertex3f(0, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 7);
	glVertex3f(0.3, 0, 7);
	glVertex3f(0.3, -0.3, 7);
	glVertex3f(0, -0.3, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 7);
	glVertex3f(0.3, -0.3, 7);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0.3, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 7);
	glVertex3f(0, -0.3, 7);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -0.3, 0);
	glVertex3f(0, -0.3, 0);
	glVertex3f(0.3, -0.3, 7);
	glVertex3f(0, -0.3, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0, 0, 7);
	glVertex3f(0.3, 0, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//передняя ножка (правая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.45, 0);
	glVertex3f(0.3, -3.45, 0);
	glVertex3f(0.3, -3.15, 0);
	glVertex3f(0, -3.15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.45, 7);
	glVertex3f(0.3, -3.45, 7);
	glVertex3f(0.3, -3.15, 7);
	glVertex3f(0, -3.15, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -3.45, 7);
	glVertex3f(0.3, -3.15, 7);
	glVertex3f(0.3, -3.45, 0);
	glVertex3f(0.3, -3.15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.45, 7);
	glVertex3f(0, -3.15, 7);
	glVertex3f(0, -3.45, 0);
	glVertex3f(0, -3.15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -3.15, 0);
	glVertex3f(0, -3.15, 0);
	glVertex3f(0.3, -3.15, 7);
	glVertex3f(0, -3.15, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.45, 0);
	glVertex3f(0.3, -3.45, 0);
	glVertex3f(0, -3.45, 7);
	glVertex3f(0.3, -3.45, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//передняя перекладина
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.15, 2);
	glVertex3f(0, -3.15, 2.3);
	glVertex3f(0, -0.3, 2);
	glVertex3f(0, -0.3, 2.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -3.15, 2);
	glVertex3f(0.3, -3.15, 2.3);
	glVertex3f(0.3, -0.3, 2);
	glVertex3f(0.3, -0.3, 2.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.15, 2);
	glVertex3f(0, -0.3, 2);
	glVertex3f(0.3, -3.15, 2);
	glVertex3f(0.3, -0.3, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.15, 2.3);
	glVertex3f(0, -0.3, 2.3);
	glVertex3f(0.3, -3.15, 2.3);
	glVertex3f(0.3, -0.3, 2.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//задняя ножка (левая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 0);
	glVertex3f(3.45, 0, 0);
	glVertex3f(3.45, -0.3, 0);
	glVertex3f(3.15, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 7);
	glVertex3f(3.45, 0, 7);
	glVertex3f(3.45, -0.3, 7);
	glVertex3f(3.15, -0.3, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 7);
	glVertex3f(3.45, -0.3, 7);
	glVertex3f(3.45, 0, 0);
	glVertex3f(3.45, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 7);
	glVertex3f(3.15, -0.3, 7);
	glVertex3f(3.15, 0, 0);
	glVertex3f(3.15, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, -0.3, 0);
	glVertex3f(3.15, -0.3, 0);
	glVertex3f(3.45, -0.3, 7);
	glVertex3f(3.15, -0.3, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 0);
	glVertex3f(3.45, 0, 0);
	glVertex3f(3.15, 0, 7);
	glVertex3f(3.45, 0, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//задняя ножка (правая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.45, 0);
	glVertex3f(3.45, -3.45, 0);
	glVertex3f(3.45, -3.15, 0);
	glVertex3f(3.15, -3.15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.45, 7);
	glVertex3f(3.45, -3.45, 7);
	glVertex3f(3.45, -3.15, 7);
	glVertex3f(3.15, -3.15, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, -3.45, 7);
	glVertex3f(3.45, -3.15, 7);
	glVertex3f(3.45, -3.45, 0);
	glVertex3f(3.45, -3.15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.45, 7);
	glVertex3f(3.15, -3.15, 7);
	glVertex3f(3.15, -3.45, 0);
	glVertex3f(3.15, -3.15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, -3.15, 0);
	glVertex3f(3.15, -3.15, 0);
	glVertex3f(3.45, -3.15, 7);
	glVertex3f(3.15, -3.15, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.45, 0);
	glVertex3f(3.45, -3.45, 0);
	glVertex3f(3.15, -3.45, 7);
	glVertex3f(3.45, -3.45, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//задняя перекладина
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.15, 2);
	glVertex3f(3.15, -3.15, 2.3);
	glVertex3f(3.15, -0.3, 2);
	glVertex3f(3.15, -0.3, 2.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, -3.15, 2);
	glVertex3f(3.45, -3.15, 2.3);
	glVertex3f(3.45, -0.3, 2);
	glVertex3f(3.45, -0.3, 2.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.15, 2);
	glVertex3f(3.15, -0.3, 2);
	glVertex3f(3.45, -3.15, 2);
	glVertex3f(3.45, -0.3, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.15, 2.3);
	glVertex3f(3.15, -0.3, 2.3);
	glVertex3f(3.45, -3.15, 2.3);
	glVertex3f(3.45, -0.3, 2.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//нижняя перекладина (левая)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(3.15, 0, 0.3);
	glVertex3f(0.3, 0, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -0.3, 0);
	glVertex3f(0.3, -0.3, 0);
	glVertex3f(3.15, -0.3, 0.3);
	glVertex3f(0.3, -0.3, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 0.3);
	glVertex3f(0.3, 0, 0.3);
	glVertex3f(3.15, -0.3, 0.3);
	glVertex3f(0.3, -0.3, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(3.15, -0.3, 0);
	glVertex3f(0.3, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//нижняя перекладина (правая)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.15, 0);
	glVertex3f(0.3, -3.15, 0);
	glVertex3f(3.15, -3.15, 0.3);
	glVertex3f(0.3, -3.15, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.45, 0);
	glVertex3f(0.3, -3.45, 0);
	glVertex3f(3.15, -3.45, 0.3);
	glVertex3f(0.3, -3.45, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.15, 0.3);
	glVertex3f(0.3, -3.15, 0.3);
	glVertex3f(3.15, -3.45, 0.3);
	glVertex3f(0.3, -3.45, 0.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -3.15, 0);
	glVertex3f(0.3, -3.15, 0);
	glVertex3f(3.15, -3.45, 0);
	glVertex3f(0.3, -3.45, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//железная часть - зона посадки
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 7);
	glVertex3f(0, -3.45, 7);
	glVertex3f(3.45, 0, 7);
	glVertex3f(3.45, -3.45, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 7.3);
	glVertex3f(0, -3.45, 7.3);
	glVertex3f(3.45, 0, 7.3);
	glVertex3f(3.45, -3.45, 7.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 7.3);
	glVertex3f(3.45, -3.45, 7.3);
	glVertex3f(3.45, 0, 7);
	glVertex3f(3.45, -3.45, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 7.3);
	glVertex3f(0, -3.45, 7.3);
	glVertex3f(0, 0, 7);
	glVertex3f(0, -3.45, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 7.3);
	glVertex3f(3.45, 0, 7.3);
	glVertex3f(0, 0, 7);
	glVertex3f(3.45, 0, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -3.45, 7.3);
	glVertex3f(3.45, -3.45, 7.3);
	glVertex3f(0, -3.45, 7);
	glVertex3f(3.45, -3.45, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//мягкая часть - зона посадки
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 7.3);
	glVertex3f(0, -3.45, 7.3);
	glVertex3f(3.45, 0, 7.3);
	glVertex3f(3.45, -3.45, 7.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 7.7);
	glVertex3f(0, -3.45, 7.7);
	glVertex3f(3.45, 0, 7.7);
	glVertex3f(3.45, -3.45, 7.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(3.45, 0, 7.7);
	glVertex3f(3.45, -3.45, 7.7);
	glVertex3f(3.45, 0, 7.3);
	glVertex3f(3.45, -3.45, 7.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 7.7);
	glVertex3f(0, -3.45, 7.7);
	glVertex3f(0, 0, 7.3);
	glVertex3f(0, -3.45, 7.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 7.7);
	glVertex3f(3.45, 0, 7.7);
	glVertex3f(0, 0, 7.3);
	glVertex3f(3.45, 0, 7.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, -3.45, 7.7);
	glVertex3f(3.45, -3.45, 7.7);
	glVertex3f(0, -3.45, 7.3);
	glVertex3f(3.45, -3.45, 7.3);
	glEnd();

}

void DrawPianoChair()
{
	//БАНКЕТКА

	glBegin(GL_QUAD_STRIP);						//передняя ножка (левая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0.3, -0.3, 0);
	glVertex3f(0, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4);
	glVertex3f(0.3, 0, 4);
	glVertex3f(0.3, -0.3, 4);
	glVertex3f(0, -0.3, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 4);
	glVertex3f(0.3, -0.3, 4);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0.3, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4);
	glVertex3f(0, -0.3, 4);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -0.3, 0);
	glVertex3f(0, -0.3, 0);
	glVertex3f(0.3, -0.3, 4);
	glVertex3f(0, -0.3, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 0);
	glVertex3f(0.3, 0, 0);
	glVertex3f(0, 0, 4);
	glVertex3f(0.3, 0, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//передняя ножка (правая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 0);
	glVertex3f(0.3, -5.3, 0);
	glVertex3f(0.3, -5, 0);
	glVertex3f(0, -5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 4);
	glVertex3f(0.3, -5.3, 4);
	glVertex3f(0.3, -5, 4);
	glVertex3f(0, -5, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -5.3, 4);
	glVertex3f(0.3, -5, 4);
	glVertex3f(0.3, -5.3, 0);
	glVertex3f(0.3, -5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 4);
	glVertex3f(0, -5, 4);
	glVertex3f(0, -5.3, 0);
	glVertex3f(0, -5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -5, 0);
	glVertex3f(0, -5, 0);
	glVertex3f(0.3, -5, 4);
	glVertex3f(0, -5, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 0);
	glVertex3f(0.3, -5.3, 0);
	glVertex3f(0, -5.3, 4);
	glVertex3f(0.3, -5.3, 4);
	glEnd();



	glBegin(GL_QUAD_STRIP);						//задняя ножка (левая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 0);
	glVertex3f(3.45, 0, 0);
	glVertex3f(3.45, -0.3, 0);
	glVertex3f(3.15, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 4);
	glVertex3f(3.45, 0, 4);
	glVertex3f(3.45, -0.3, 4);
	glVertex3f(3.15, -0.3, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 4);
	glVertex3f(3.45, -0.3, 4);
	glVertex3f(3.45, 0, 0);
	glVertex3f(3.45, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 4);
	glVertex3f(3.15, -0.3, 4);
	glVertex3f(3.15, 0, 0);
	glVertex3f(3.15, -0.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, -0.3, 0);
	glVertex3f(3.15, -0.3, 0);
	glVertex3f(3.45, -0.3, 4);
	glVertex3f(3.15, -0.3, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 0);
	glVertex3f(3.45, 0, 0);
	glVertex3f(3.15, 0, 4);
	glVertex3f(3.45, 0, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//задняя ножка (правая)		
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -5.3, 0);
	glVertex3f(3.45, -5.3, 0);
	glVertex3f(3.45, -5, 0);
	glVertex3f(3.15, -5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -5.3, 4);
	glVertex3f(3.45, -5.3, 4);
	glVertex3f(3.45, -5, 4);
	glVertex3f(3.15, -5, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, -5.3, 4);
	glVertex3f(3.45, -5, 4);
	glVertex3f(3.45, -5.3, 0);
	glVertex3f(3.45, -5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -5.3, 4);
	glVertex3f(3.15, -5, 4);
	glVertex3f(3.15, -5.3, 0);
	glVertex3f(3.15, -5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, -5, 0);
	glVertex3f(3.15, -5, 0);
	glVertex3f(3.45, -5, 4);
	glVertex3f(3.15, -5, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, -5.3, 0);
	glVertex3f(3.45, -5.3, 0);
	glVertex3f(3.15, -5.3, 4);
	glVertex3f(3.45, -5.3, 4);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//сиденье - нижний уровень	(право)	
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5, 4);
	glVertex3f(3.45, -5, 4);
	glVertex3f(0, -5, 3.7);
	glVertex3f(3.45, -5, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 4);
	glVertex3f(3.45, -5.3, 4);
	glVertex3f(0, -5.3, 3.7);
	glVertex3f(3.45, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 4);
	glVertex3f(3.45, -5.3, 4);
	glVertex3f(0, -5, 4);
	glVertex3f(3.45, -5, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5, 3.7);
	glVertex3f(3.45, -5, 3.7);
	glVertex3f(0, -5.3, 3.7);
	glVertex3f(3.45, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//сиденье - нижний уровень	(лево)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4);
	glVertex3f(3.45, 0, 4);
	glVertex3f(0, 0, 3.7);
	glVertex3f(3.45, 0, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -0.3, 4);
	glVertex3f(3.45, -0.3, 4);
	glVertex3f(0, -0.3, 3.7);
	glVertex3f(3.45, -0.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -0.3, 4);
	glVertex3f(3.45, -0.3, 4);
	glVertex3f(0, 0, 4);
	glVertex3f(3.45, 0, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 3.7);
	glVertex3f(3.45, 0, 3.7);
	glVertex3f(0, -0.3, 3.7);
	glVertex3f(3.45, -0.3, 3.7);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//сиденье - нижний уровень	(впереди)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4);
	glVertex3f(0, -5.3, 4);
	glVertex3f(0, 0, 3.7);
	glVertex3f(0, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 4);
	glVertex3f(0.3, -5.3, 4);
	glVertex3f(0.3, 0, 3.7);
	glVertex3f(0.3, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 4);
	glVertex3f(0.3, -5.3, 4);
	glVertex3f(0, 0, 4);
	glVertex3f(0, -5.3, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 3.7);
	glVertex3f(0.3, -5.3, 3.7);
	glVertex3f(0, 0, 3.7);
	glVertex3f(0, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//сиденье - нижний уровень	
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 4);
	glVertex3f(3.15, -5.3, 4);
	glVertex3f(3.15, 0, 3.7);
	glVertex3f(3.15, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 4);
	glVertex3f(3.45, -5.3, 4);
	glVertex3f(3.45, 0, 3.7);
	glVertex3f(3.45, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 4);
	glVertex3f(3.45, -5.3, 4);
	glVertex3f(3.15, 0, 4);
	glVertex3f(3.15, -5.3, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 3.7);
	glVertex3f(3.45, -5.3, 3.7);
	glVertex3f(3.15, 0, 3.7);
	glVertex3f(3.15, -5.3, 3.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//механизм подъёма (X - левая)
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.3, 3.9);
	glVertex3f(1.4, -0.3, 3.9);
	glVertex3f(2.2, -0.3, 4.4);
	glVertex3f(2.3, -0.3, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.35, 3.9);
	glVertex3f(1.4, -0.35, 3.9);
	glVertex3f(2.2, -0.35, 4.4);
	glVertex3f(2.3, -0.35, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(2.2, -0.3, 4.4);
	glVertex3f(2.3, -0.3, 4.4);
	glVertex3f(2.2, -0.35, 4.4);
	glVertex3f(2.3, -0.35, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.35, 3.9);
	glVertex3f(1.4, -0.35, 3.9);
	glVertex3f(1.3, -0.3, 3.9);
	glVertex3f(1.4, -0.3, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.3, 3.9);
	glVertex3f(1.3, -0.35, 3.9);
	glVertex3f(2.2, -0.3, 4.4);
	glVertex3f(2.2, -0.35, 4.4);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.4, -0.3, 3.9);
	glVertex3f(1.4, -0.35, 3.9);
	glVertex3f(2.3, -0.3, 4.4);
	glVertex3f(2.3, -0.35, 4.4);
	glEnd();



	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.3, 4.4);
	glVertex3f(1.4, -0.3, 4.4);
	glVertex3f(2.2, -0.3, 3.9);
	glVertex3f(2.3, -0.3, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.35, 4.4);
	glVertex3f(1.4, -0.35, 4.4);
	glVertex3f(2.2, -0.35, 3.9);
	glVertex3f(2.3, -0.35, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(2.2, -0.3, 3.9);
	glVertex3f(2.3, -0.3, 3.9);
	glVertex3f(2.2, -0.35, 3.9);
	glVertex3f(2.3, -0.35, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.35, 3.9);
	glVertex3f(1.4, -0.35, 3.9);
	glVertex3f(1.3, -0.3, 3.9);
	glVertex3f(1.4, -0.3, 3.9);

	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -0.3, 4.4);
	glVertex3f(1.3, -0.35, 4.4);
	glVertex3f(2.2, -0.3, 3.9);
	glVertex3f(2.2, -0.35, 3.9);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.4, -0.3, 4.4);
	glVertex3f(1.4, -0.35, 4.4);
	glVertex3f(2.3, -0.3, 3.9);
	glVertex3f(2.3, -0.35, 3.9);
	glEnd();



	glBegin(GL_QUAD_STRIP);								//механизм подъёма (X - правая)
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -5, 3.9);
	glVertex3f(1.4, -5, 3.9);
	glVertex3f(2.2, -5, 4.4);
	glVertex3f(2.3, -5, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -4.95, 3.9);
	glVertex3f(1.4, -4.95, 3.9);
	glVertex3f(2.2, -4.95, 4.4);
	glVertex3f(2.3, -4.95, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(2.2, -5, 4.4);
	glVertex3f(2.3, -5, 4.4);
	glVertex3f(2.2, -4.95, 4.4);
	glVertex3f(2.3, -4.95, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -4.95, 3.9);
	glVertex3f(1.4, -4.95, 3.9);
	glVertex3f(1.3, -5, 3.9);
	glVertex3f(1.4, -5, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -5, 3.9);
	glVertex3f(1.3, -4.95, 3.9);
	glVertex3f(2.2, -5, 4.4);
	glVertex3f(2.2, -4.95, 4.4);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.4, -5, 3.9);
	glVertex3f(1.4, -4.95, 3.9);
	glVertex3f(2.3, -5, 4.4);
	glVertex3f(2.3, -4.95, 4.4);
	glEnd();



	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -5, 4.4);
	glVertex3f(1.4, -5, 4.4);
	glVertex3f(2.2, -5, 3.9);
	glVertex3f(2.3, -5, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -4.95, 4.4);
	glVertex3f(1.4, -4.95, 4.4);
	glVertex3f(2.2, -4.95, 3.9);
	glVertex3f(2.3, -4.95, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(2.2, -5, 3.9);
	glVertex3f(2.3, -5, 3.9);
	glVertex3f(2.2, -4.95, 3.9);
	glVertex3f(2.3, -4.95, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -4.95, 3.9);
	glVertex3f(1.4, -4.95, 3.9);
	glVertex3f(1.3, -5, 3.9);
	glVertex3f(1.4, -5, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.3, -5, 4.4);
	glVertex3f(1.3, -4.95, 4.4);
	glVertex3f(2.2, -5, 3.9);
	glVertex3f(2.2, -4.95, 3.9);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.4, -5, 4.4);
	glVertex3f(1.4, -4.95, 4.4);
	glVertex3f(2.3, -5, 3.9);
	glVertex3f(2.3, -4.95, 3.9);
	glEnd();


	glPushMatrix();						//движущая балка
	glTranslatef(1.65, -4.95, 3.7);
	glRotatef(90, 0, 1, 0);
	DrawChairHolders();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.65, -0.3, 4);
	glRotatef(-90, 0, 1, 0);
	glRotatef(180, 1, 0, 0);
	DrawChairHolders();
	glPopMatrix();

	glBegin(GL_QUAD_STRIP);					//левая первая
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -0.4, 4);
	glVertex3f(1.7, -0.4, 3.8);
	glVertex3f(1.74, -0.4, 4);
	glVertex3f(1.74, -0.4, 3.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -0.8, 4.1);
	glVertex3f(1.7, -0.8, 4.3);
	glVertex3f(1.74, -0.8, 4.1);
	glVertex3f(1.74, -0.8, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -0.4, 4);
	glVertex3f(1.74, -0.4, 4);
	glVertex3f(1.7, -0.8, 4.3);
	glVertex3f(1.74, -0.8, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -0.4, 3.8);
	glVertex3f(1.74, -0.4, 3.8);
	glVertex3f(1.7, -0.8, 4.1);
	glVertex3f(1.74, -0.8, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -0.4, 4);
	glVertex3f(1.7, -0.4, 3.8);
	glVertex3f(1.7, -0.8, 4.3);
	glVertex3f(1.7, -0.8, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.74, -0.4, 4);
	glVertex3f(1.74, -0.4, 3.8);
	glVertex3f(1.74, -0.8, 4.3);
	glVertex3f(1.74, -0.8, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//левая вторая
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -0.4, 4);
	glVertex3f(1.91, -0.4, 3.8);
	glVertex3f(1.95, -0.4, 4);
	glVertex3f(1.95, -0.4, 3.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -0.8, 4.1);
	glVertex3f(1.91, -0.8, 4.3);
	glVertex3f(1.95, -0.8, 4.1);
	glVertex3f(1.95, -0.8, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -0.4, 4);
	glVertex3f(1.95, -0.4, 4);
	glVertex3f(1.91, -0.8, 4.3);
	glVertex3f(1.95, -0.8, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -0.4, 3.8);
	glVertex3f(1.95, -0.4, 3.8);
	glVertex3f(1.91, -0.8, 4.1);
	glVertex3f(1.95, -0.8, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -0.4, 4);
	glVertex3f(1.91, -0.4, 3.8);
	glVertex3f(1.91, -0.8, 4.3);
	glVertex3f(1.91, -0.8, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.95, -0.4, 4);
	glVertex3f(1.95, -0.4, 3.8);
	glVertex3f(1.95, -0.8, 4.3);
	glVertex3f(1.95, -0.8, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//левый бегунок
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -0.8, 4);
	glVertex3f(1.74, -0.8, 4);
	glVertex3f(1.91, -0.6, 4);
	glVertex3f(1.74, -0.6, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -0.8, 4.4);
	glVertex3f(1.74, -0.8, 4.4);
	glVertex3f(1.91, -0.6, 4.4);
	glVertex3f(1.74, -0.6, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -0.8, 4.4);
	glVertex3f(1.91, -0.6, 4.4);
	glVertex3f(1.91, -0.8, 4);
	glVertex3f(1.91, -0.6, 4);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.74, -0.8, 4.4);
	glVertex3f(1.74, -0.6, 4.4);
	glVertex3f(1.74, -0.8, 4);
	glVertex3f(1.74, -0.6, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -0.6, 4);
	glVertex3f(1.74, -0.6, 4);
	glVertex3f(1.91, -0.6, 4.4);
	glVertex3f(1.74, -0.6, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -0.8, 4.4);
	glVertex3f(1.74, -0.8, 4.4);
	glVertex3f(1.91, -0.8, 4);
	glVertex3f(1.74, -0.8, 4);
	glEnd();


	glBegin(GL_QUAD_STRIP);					//правая первая
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -4.9, 4);
	glVertex3f(1.7, -4.9, 3.8);
	glVertex3f(1.74, -4.9, 4);
	glVertex3f(1.74, -4.9, 3.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -4.5, 4.1);
	glVertex3f(1.7, -4.5, 4.3);
	glVertex3f(1.74, -4.5, 4.1);
	glVertex3f(1.74, -4.5, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -4.9, 4);
	glVertex3f(1.74, -4.9, 4);
	glVertex3f(1.7, -4.5, 4.3);
	glVertex3f(1.74, -4.5, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -4.9, 3.8);
	glVertex3f(1.74, -4.9, 3.8);
	glVertex3f(1.7, -4.5, 4.1);
	glVertex3f(1.74, -4.5, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.7, -4.9, 4);
	glVertex3f(1.7, -4.9, 3.8);
	glVertex3f(1.7, -4.5, 4.3);
	glVertex3f(1.7, -4.5, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.74, -4.9, 4);
	glVertex3f(1.74, -4.9, 3.8);
	glVertex3f(1.74, -4.5, 4.3);
	glVertex3f(1.74, -4.5, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//правая вторая
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -4.9, 4);
	glVertex3f(1.91, -4.9, 3.8);
	glVertex3f(1.95, -4.9, 4);
	glVertex3f(1.95, -4.9, 3.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -4.5, 4.1);
	glVertex3f(1.91, -4.5, 4.3);
	glVertex3f(1.95, -4.5, 4.1);
	glVertex3f(1.95, -4.5, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -4.9, 4);
	glVertex3f(1.95, -4.9, 4);
	glVertex3f(1.91, -4.5, 4.3);
	glVertex3f(1.95, -4.5, 4.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -4.9, 3.8);
	glVertex3f(1.95, -4.9, 3.8);
	glVertex3f(1.91, -4.5, 4.1);
	glVertex3f(1.95, -4.5, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.91, -4.9, 4);
	glVertex3f(1.91, -4.9, 3.8);
	glVertex3f(1.91, -4.5, 4.3);
	glVertex3f(1.91, -4.5, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(1.95, -4.9, 4);
	glVertex3f(1.95, -4.9, 3.8);
	glVertex3f(1.95, -4.5, 4.3);
	glVertex3f(1.95, -4.5, 4.1);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//правый бегунок
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -4.5, 4);
	glVertex3f(1.74, -4.5, 4);
	glVertex3f(1.91, -4.7, 4);
	glVertex3f(1.74, -4.7, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -4.5, 4.4);
	glVertex3f(1.74, -4.5, 4.4);
	glVertex3f(1.91, -4.7, 4.4);
	glVertex3f(1.74, -4.7, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -4.5, 4.4);
	glVertex3f(1.91, -4.7, 4.4);
	glVertex3f(1.91, -4.5, 4);
	glVertex3f(1.91, -4.7, 4);
	glEnd();


	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.74, -4.5, 4.4);
	glVertex3f(1.74, -4.7, 4.4);
	glVertex3f(1.74, -4.5, 4);
	glVertex3f(1.74, -4.7, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -4.7, 4);
	glVertex3f(1.74, -4.7, 4);
	glVertex3f(1.91, -4.7, 4.4);
	glVertex3f(1.74, -4.7, 4.4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1.91, -4.5, 4.4);
	glVertex3f(1.74, -4.5, 4.4);
	glVertex3f(1.91, -4.5, 4);
	glVertex3f(1.74, -4.5, 4);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//сиденье - верхний уровень	(право)	
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5, 4.5);
	glVertex3f(3.45, -5, 4.5);
	glVertex3f(0, -5, 4.2);
	glVertex3f(3.45, -5, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 4.5);
	glVertex3f(3.45, -5.3, 4.5);
	glVertex3f(0, -5.3, 4.2);
	glVertex3f(3.45, -5.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.3, 4.5);
	glVertex3f(3.45, -5.3, 4.5);
	glVertex3f(0, -5, 4.5);
	glVertex3f(3.45, -5, 4.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5, 4.2);
	glVertex3f(3.45, -5, 4.2);
	glVertex3f(0, -5.3, 4.2);
	glVertex3f(3.45, -5.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//сиденье - нижний уровень	(лево)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4.5);
	glVertex3f(3.45, 0, 4.5);
	glVertex3f(0, 0, 4.2);
	glVertex3f(3.45, 0, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -0.3, 4.5);
	glVertex3f(3.45, -0.3, 4.5);
	glVertex3f(0, -0.3, 4.2);
	glVertex3f(3.45, -0.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -0.3, 4.5);
	glVertex3f(3.45, -0.3, 4.5);
	glVertex3f(0, 0, 4.5);
	glVertex3f(3.45, 0, 4.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4.2);
	glVertex3f(3.45, 0, 4.2);
	glVertex3f(0, -0.3, 4.2);
	glVertex3f(3.45, -0.3, 4.2);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//сиденье - нижний уровень	(впереди)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4.5);
	glVertex3f(0, -5.3, 4.5);
	glVertex3f(0, 0, 4.2);
	glVertex3f(0, -5.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 4.5);
	glVertex3f(0.3, -5.3, 4.5);
	glVertex3f(0.3, 0, 4.2);
	glVertex3f(0.3, -5.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 4.5);
	glVertex3f(0.3, -5.3, 4.5);
	glVertex3f(0, 0, 4.5);
	glVertex3f(0, -5.3, 4.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 4.2);
	glVertex3f(0.3, -5.3, 4.2);
	glVertex3f(0, 0, 4.2);
	glVertex3f(0, -5.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//сиденье - нижний уровень	
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.15, 0, 4.5);
	glVertex3f(3.15, -5.3, 4.5);
	glVertex3f(3.15, 0, 4.2);
	glVertex3f(3.15, -5.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 4.5);
	glVertex3f(3.45, -5.3, 4.5);
	glVertex3f(3.45, 0, 4.2);
	glVertex3f(3.45, -5.3, 4.2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 4.5);
	glVertex3f(3.45, -5.3, 4.5);
	glVertex3f(3.15, 0, 4.5);
	glVertex3f(3.15, -5.3, 4.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(3.45, 0, 4.2);
	glVertex3f(3.45, -5.3, 4.2);
	glVertex3f(3.15, 0, 4.2);
	glVertex3f(3.15, -5.3, 4.2);
	glEnd();

	center_of_circle_3D.x = 1.82;								//перекладина
	center_of_circle_3D.y = -0.3;
	center_of_circle_3D.z = 4.35;
	GetCircleXZ(circle_points_3D, center_of_circle_3D, 0.025);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 5.3, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 5.3, circle_points_3D[i].z);
		glEnd();
	}

	center_of_circle_3D.x = 1.82;								//контроллер высоты
	center_of_circle_3D.y = -5.6;
	center_of_circle_3D.z = 4.35;
	GetCircleXZ(circle_points_3D, center_of_circle_3D, 0.3);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.3, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 0.3, circle_points_3D[i].z);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);								//мягкая часть - зона посадки
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 4.5);
	glVertex3f(0, -5.3, 4.5);
	glVertex3f(3.45, 0, 4.5);
	glVertex3f(3.45, -5.3, 4.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 4.8);
	glVertex3f(0, -5.3, 4.8);
	glVertex3f(3.45, 0, 4.8);
	glVertex3f(3.45, -5.3, 4.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(3.45, 0, 4.5);
	glVertex3f(3.45, -5.3, 4.5);
	glVertex3f(3.45, 0, 4.8);
	glVertex3f(3.45, -5.3, 4.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 4.8);
	glVertex3f(0, -5.3, 4.8);
	glVertex3f(0, 0, 4.5);
	glVertex3f(0, -5.3, 4.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 4.8);
	glVertex3f(3.45, 0, 4.8);
	glVertex3f(0, 0, 4.5);
	glVertex3f(3.45, 0, 4.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, -5.3, 4.8);
	glVertex3f(3.45, -5.3, 4.8);
	glVertex3f(0, -5.3, 4.5);
	glVertex3f(3.45, -5.3, 4.5);
	glEnd();

}

void DrawCommonChair()
{
	//ОБЫЧНЫЙ СТУЛ --------------------------------------------------------------------------------

	glBegin(GL_QUAD_STRIP);					//сиденье
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 3.9);
	glVertex3f(0, -5.1, 3.9);
	glVertex3f(4.2, 0, 3.9);
	glVertex3f(4.2, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 4.7);
	glVertex3f(0, -5.1, 4.7);
	glVertex3f(4.2, 0, 4.7);
	glVertex3f(4.2, -5.1, 4.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.2, 0, 3.9);
	glVertex3f(4.2, -5.1, 3.9);
	glVertex3f(4.2, 0, 4.7);
	glVertex3f(4.2, -5.1, 4.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 4.7);
	glVertex3f(0, -5.1, 4.7);
	glVertex3f(0, 0, 3.9);
	glVertex3f(0, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, -5.1, 4.7);
	glVertex3f(4.2, -5.1, 4.7);
	glVertex3f(0, -5.1, 3.9);
	glVertex3f(4.2, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 4.7);
	glVertex3f(4.2, 0, 4.7);
	glVertex3f(0, 0, 3.9);
	glVertex3f(4.2, 0, 3.9);
	glEnd();

	glPushMatrix();
	glTranslatef(0, -5.1, 4.7);
	glRotatef(-90, 1, 0, 0);
	glRotatef(180, 0, 0, 1);

	ArcCustom arched = ArcCustom(0, 0, -4.2, 0, 6);
	Point* arched_points = NULL;
	int count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);
	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.0f, 1.0f, 10.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(-2.1, 0, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.0f, 1.0f, 10.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(-2.1, 0, 5.1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 5.1);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 5.1);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 1.0f, 10.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 5.1);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 5.1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0);
		glEnd();
	}

	free(arched_points);

	glPopMatrix();


	glBegin(GL_QUAD_STRIP);			//спинка, часть сиденья			
	glColor3f(0, 0, 0);
	glVertex3f(4.5, 0, 3.9);
	glVertex3f(4.5, -5.1, 3.9);
	glVertex3f(4.5, 0, 4.7);
	glVertex3f(4.5, -5.1, 4.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.5, 0, 3.9);
	glVertex3f(4.5, 0, 4.7);
	glVertex3f(4.2, 0, 3.9);
	glVertex3f(4.2, 0, 4.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.5, -5.1, 3.9);
	glVertex3f(4.5, -5.1, 4.7);
	glVertex3f(4.2, -5.1, 3.9);
	glVertex3f(4.2, -5.1, 4.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.5, 0, 3.9);
	glVertex3f(4.5, -5.1, 3.9);
	glVertex3f(4.2, 0, 3.9);
	glVertex3f(4.2, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.2, 0, 4.7);
	glVertex3f(4.2, -5.1, 4.7);
	glVertex3f(4.5, 0, 4.7);
	glVertex3f(4.5, -5.1, 4.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//cпинка
	glColor3f(0, 0, 0);
	glVertex3f(4.5, -5.1, 4.7);
	glVertex3f(4.5, 0, 4.7);
	glVertex3f(5.8, -5.1, 9.7);
	glVertex3f(5.8, 0, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.2, -5.1, 4.7);
	glVertex3f(4.2, 0, 4.7);
	glVertex3f(5.5, -5.1, 9.7);
	glVertex3f(5.5, 0, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(5.5, -5.1, 9.7);
	glVertex3f(5.5, 0, 9.7);
	glVertex3f(5.8, -5.1, 9.7);
	glVertex3f(5.8, 0, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.2, 0, 4.7);
	glVertex3f(4.5, 0, 4.7);
	glVertex3f(5.5, 0, 9.7);
	glVertex3f(5.8, 0, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(4.5, -5.1, 4.7);
	glVertex3f(4.2, -5.1, 4.7);
	glVertex3f(5.8, -5.1, 9.7);
	glVertex3f(5.5, -5.1, 9.7);
	glEnd();

	/*glPushMatrix();
	glTranslatef(4.2, -5.1, 4.7);
	glRotatef(-90, 1, 0, 0);
	glRotatef(105, 0, 0, 1);

	arched = ArcCustom(0, 0, -5.16, 0, 6);
	arched_points = NULL;
	count_of_elems_arched;
	arched.GenerateArc(arched_points, count_of_elems_arched);
	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.0f, 1.0f, 10.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(-2.1, 0, 0);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.0f, 1.0f, 10.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(-2.1, 0, 5.1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 5.1);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 5.1);
		glEnd();
	}

	for (int i = 0; i < count_of_elems_arched - 1; i++)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3f(0.0f, 1.0f, 10.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 5.1);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 5.1);
		glVertex3f(arched_points[i].x, arched_points[i].y, 0);
		glVertex3f(arched_points[i + 1].x, arched_points[i + 1].y, 0);
		glEnd();
	}

	free(arched_points);
	glPopMatrix();*/


	glBegin(GL_QUAD_STRIP);						//спинка, мягкая часть
	glColor3f(0, 10, 1);
	glVertex3f(5, -5.1, 9.7);
	glVertex3f(5, 0, 9.7);
	glVertex3f(5.5, -5.1, 9.7);
	glVertex3f(5.5, 0, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 10, 1);
	glVertex3f(5, -5.1, 9.7);
	glVertex3f(5, 0, 9.7);
	glVertex3f(3.9, -5.1, 4.7);
	glVertex3f(3.9, 0, 4.7);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0, 10, 1);
	glVertex3f(3.9, 0, 4.7);
	glVertex3f(5, 0, 9.7);
	glVertex3f(5.5, 0, 9.7);
	glVertex3f(4.2, 0, 4.7);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0, 10, 1);
	glVertex3f(3.9, -5.1, 4.7);
	glVertex3f(5, -5.1, 9.7);
	glVertex3f(5.5, -5.1, 9.7);
	glVertex3f(4.2, -5.1, 4.7);
	glEnd();


	glBegin(GL_QUAD_STRIP);						//первая левая ножка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 3.9);
	glVertex3f(0.6, 0, 3.9);
	glVertex3f(0.6, -0.6, 3.9);
	glVertex3f(0, -0.6, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.2, 0);
	glVertex3f(-0.2, 0, 0);
	glVertex3f(-0.2, 0.2, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.6, 0, 3.9);
	glVertex3f(0, 0, 3.9);
	glVertex3f(0, 0.2, 0);
	glVertex3f(-0.2, 0.2, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.6, 0, 3.9);
	glVertex3f(0.6, -0.6, 3.9);
	glVertex3f(0, 0.2, 0);
	glVertex3f(0, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 3.9);
	glVertex3f(0, -0.6, 3.9);
	glVertex3f(-0.2, 0.2, 0);
	glVertex3f(-0.2, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.6, -0.6, 3.9);
	glVertex3f(0, -0.6, 3.9);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.2, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//вторая левая ножка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, 0, 3.9);
	glVertex3f(3.9, 0, 3.9);
	glVertex3f(3.9, -0.6, 3.9);
	glVertex3f(4.5, -0.6, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, 0, 0);
	glVertex3f(4.5, 0.2, 0);
	glVertex3f(4.7, 0, 0);
	glVertex3f(4.7, 0.2, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, 0, 3.9);
	glVertex3f(4.5, -0.6, 3.9);
	glVertex3f(4.7, 0.2, 0);
	glVertex3f(4.7, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, 0, 0);
	glVertex3f(4.5, 0.2, 0);
	glVertex3f(3.9, -0.6, 3.9);
	glVertex3f(3.9, 0, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, 0.2, 0);
	glVertex3f(4.7, 0.2, 0);
	glVertex3f(3.9, 0, 3.9);
	glVertex3f(4.5, 0, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, 0, 0);
	glVertex3f(4.7, 0, 0);
	glVertex3f(3.9, -0.6, 3.9);
	glVertex3f(4.5, -0.6, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//первая правая ножка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.6, -4.5, 3.9);
	glVertex3f(0.6, -5.1, 3.9);
	glVertex3f(0, -4.5, 3.9);
	glVertex3f(0, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.1, 0);
	glVertex3f(0, -5.3, 0);
	glVertex3f(-0.2, -5.1, 0);
	glVertex3f(-0.2, -5.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -5.1, 0);
	glVertex3f(0, -5.3, 0);
	glVertex3f(0.6, -4.5, 3.9);
	glVertex3f(0.6, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(-0.2, -5.1, 0);
	glVertex3f(-0.2, -5.3, 0);
	glVertex3f(0, -4.5, 3.9);
	glVertex3f(0, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.6, -4.5, 3.9);
	glVertex3f(0, -4.5, 3.9);
	glVertex3f(0, -5.1, 0);
	glVertex3f(-0.2, -5.1, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.6, -5.1, 3.9);
	glVertex3f(0, -5.1, 3.9);
	glVertex3f(0, -5.3, 0);
	glVertex3f(-0.2, -5.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);						//вторая правая ножка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -4.5, 3.9);
	glVertex3f(4.5, -5.1, 3.9);
	glVertex3f(3.9, -4.5, 3.9);
	glVertex3f(3.9, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -5.1, 0);
	glVertex3f(4.5, -5.3, 0);
	glVertex3f(4.7, -5.1, 0);
	glVertex3f(4.7, -5.3, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -5.1, 0);
	glVertex3f(4.5, -5.3, 0);
	glVertex3f(3.9, -4.5, 3.9);
	glVertex3f(3.9, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.7, -5.1, 0);
	glVertex3f(4.7, -5.3, 0);
	glVertex3f(4.5, -4.5, 3.9);
	glVertex3f(4.5, -5.1, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -5.1, 0);
	glVertex3f(4.7, -5.1, 0);
	glVertex3f(3.9, -4.5, 3.9);
	glVertex3f(4.5, -4.5, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -5.3, 0);
	glVertex3f(4.7, -5.3, 0);
	glVertex3f(3.9, -5.1, 3.9);
	glVertex3f(4.5, -5.1, 3.9);
	glEnd();


}

void DrawTable()
{
	//СТОЛ-----------------------------------------------------------------------------

	glBegin(GL_QUAD_STRIP);							//левая балка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 5);
	glVertex3f(7.7, 0, 5);
	glVertex3f(0, 0, 5.5);
	glVertex3f(7.7, 0, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -0.3, 5);
	glVertex3f(7.7, -0.3, 5);
	glVertex3f(0, -0.3, 5.5);
	glVertex3f(7.7, -0.3, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 5);
	glVertex3f(7.7, 0, 5);
	glVertex3f(0, -0.3, 5);
	glVertex3f(7.4, -0.3, 5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -0.3, 5.5);
	glVertex3f(7.7, -0.3, 5.5);
	glVertex3f(0, 0, 5.5);
	glVertex3f(7.7, 0, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//задняя балка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 5);
	glVertex3f(7.7, -10.5, 5.5);
	glVertex3f(7.7, 0, 5);
	glVertex3f(7.7, 0, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.4, -10.5, 5);
	glVertex3f(7.4, -10.5, 5.5);
	glVertex3f(7.4, -0, 5);
	glVertex3f(7.4, -0, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 5);
	glVertex3f(7.7, 0, 5);
	glVertex3f(7.4, -10.5, 5);
	glVertex3f(7.4, -0, 5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 5.5);
	glVertex3f(7.7, 0, 5.5);
	glVertex3f(7.4, -10.5, 5.5);
	glVertex3f(7.4, 0, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//правая балка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 5);
	glVertex3f(7.7, -10.5, 5.5);
	glVertex3f(0, -10.5, 5);
	glVertex3f(0, -10.5, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.2, 5);
	glVertex3f(7.7, -10.2, 5.5);
	glVertex3f(0, -10.2, 5);
	glVertex3f(0, -10.2, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 5);
	glVertex3f(7.7, -10.2, 5);
	glVertex3f(0, -10.5, 5);
	glVertex3f(0, -10.2, 5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.2, 5.5);
	glVertex3f(7.7, -10.5, 5.5);
	glVertex3f(0, -10.2, 5.5);
	glVertex3f(0, -10.5, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//передняя балка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 5);
	glVertex3f(0, 0, 5.5);
	glVertex3f(0, -10.5, 5);
	glVertex3f(0, -10.5, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, 0, 5);
	glVertex3f(0.3, 0, 5.5);
	glVertex3f(0.3, -10.5, 5);
	glVertex3f(0.3, -10.5, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 5);
	glVertex3f(0.3, 0, 5);
	glVertex3f(0, -10.5, 5);
	glVertex3f(0.3, -10.5, 5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.3, -10.5, 5.5);
	glVertex3f(0, 0, 5.5);
	glVertex3f(0.3, 0, 5.5);
	glVertex3f(0, -10.5, 5.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//левая первая
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4.98);
	glVertex3f(1, 0, 4.98);
	glVertex3f(0, -1, 4.98);
	glVertex3f(1, -1, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.6, 0);
	glVertex3f(-0.6, 0, 0);
	glVertex3f(-0.6, 0.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.6, 0);
	glVertex3f(1, -1, 4.98);
	glVertex3f(1, 0, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4.98);
	glVertex3f(0, -1, 4.98);
	glVertex3f(-0.6, 0.6, 0);
	glVertex3f(-0.6, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, 0, 4.98);
	glVertex3f(1, 0, 4.98);
	glVertex3f(-0.6, 0.6, 0);
	glVertex3f(0, 0.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -1, 4.98);
	glVertex3f(1, -1, 4.98);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.6, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//левая вторая
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(6.7, 0, 4.98);
	glVertex3f(7.7, 0, 4.98);
	glVertex3f(6.7, -1, 4.98);
	glVertex3f(7.7, -1, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, 0, 0);
	glVertex3f(8.3, 0, 0);
	glVertex3f(7.7, 0.6, 0);
	glVertex3f(8.3, 0.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, 0, 4.98);
	glVertex3f(7.7, -1, 4.98);
	glVertex3f(8.3, 0.6, 0);
	glVertex3f(8.3, 0, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(6.7, 0, 4.98);
	glVertex3f(7.7, 0.6, 0);
	glVertex3f(7.7, 0, 0);
	glVertex3f(6.7, -1, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(6.7, 0, 4.98);
	glVertex3f(7.7, 0, 4.98);
	glVertex3f(7.7, 0.6, 0);
	glVertex3f(8.3, 0.6, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(6.7, -1, 4.98);
	glVertex3f(7.7, -1, 4.98);
	glVertex3f(7.7, 0, 0);
	glVertex3f(8.3, 0, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//правая первая
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1, -9.5, 4.98);
	glVertex3f(1, -10.5, 4.98);
	glVertex3f(0, -9.5, 4.98);
	glVertex3f(0, -10.5, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0, -10.5, 0);
	glVertex3f(0, -11.1, 0);
	glVertex3f(-0.6, -10.5, 0);
	glVertex3f(-0.6, -11.1, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1, -9.5, 4.98);
	glVertex3f(1, -10.5, 4.98);
	glVertex3f(0, -10.5, 0);
	glVertex3f(0, -11.1, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(-0.6, -10.5, 0);
	glVertex3f(-0.6, -11.1, 0);
	glVertex3f(0, -9.5, 4.98);
	glVertex3f(0, -10.5, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1, -9.5, 4.98);
	glVertex3f(0, -9.5, 4.98);
	glVertex3f(0, -10.5, 0);
	glVertex3f(-0.6, -10.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(1, -10.5, 4.98);
	glVertex3f(0, -10.5, 4.98);
	glVertex3f(0, -11.1, 0);
	glVertex3f(-0.6, -11.1, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//правая вторая
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -9.5, 4.98);
	glVertex3f(7.7, -10.5, 4.98);
	glVertex3f(6.7, -9.5, 4.98);
	glVertex3f(6.7, -10.5, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 0);
	glVertex3f(7.7, -11.1, 0);
	glVertex3f(8.3, -10.5, 0);
	glVertex3f(8.3, -11.1, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 4.98);
	glVertex3f(6.7, -10.5, 4.98);
	glVertex3f(7.7, -11.1, 0);
	glVertex3f(8.3, -11.1, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -9.5, 4.98);
	glVertex3f(6.7, -9.5, 4.98);
	glVertex3f(8.3, -10.5, 0);
	glVertex3f(7.7, -10.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -9.5, 4.98);
	glVertex3f(7.7, -10.5, 4.98);
	glVertex3f(8.3, -10.5, 0);
	glVertex3f(8.3, -11.1, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(7.7, -10.5, 0);
	glVertex3f(7.7, -11.1, 0);
	glVertex3f(6.7, -9.5, 4.98);
	glVertex3f(6.7, -10.5, 4.98);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//стёкла
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(0, 0, 5.51);
	glVertex3f(7.7, 0, 5.51);
	glVertex3f(0, -10.5, 5.51);
	glVertex3f(7.7, -10.5, 5.51);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(0, 0, 4.99);
	glVertex3f(7.7, 0, 4.99);
	glVertex3f(0, -10.5, 4.99);
	glVertex3f(7.7, -10.5, 4.99);
	glEnd();

}

void DrawBarCounter()
{
	//БАРНАЯ СТОЙКА -----------------------------------------------------------------




	//внешний корпус

	glBegin(GL_QUAD_STRIP);								//основные линии
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.5, 0, 0);
	glVertex3f(0.5, 0, 11.5);
	glVertex3f(0.5, -43.5, 0);
	glVertex3f(0.5, -43.5, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.5, 0, 0);
	glVertex3f(0.5, 0, 11.5);
	glVertex3f(18, 0, 0);
	glVertex3f(18, 0, 11.5);
	glEnd();



	glBegin(GL_QUAD_STRIP);								//передний нижний бортик
	glColor3f(0, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(0, 0.5, 0.5);
	glVertex3f(0, -43.5, 0);
	glVertex3f(0, -43.5, 0.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0.5, -43.5, 0);
	glVertex3f(0.5, -43.5, 0.5);
	glVertex3f(0, -43.5, 0);
	glVertex3f(0, -43.5, 0.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -43.5, 0.5);
	glVertex3f(0, 0.5, 0.5);
	glVertex3f(0, -43.5, 0.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//боковой нижний бортик
	glColor3f(0, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(0, 0.5, 0.5);
	glVertex3f(18, 0.5, 0);
	glVertex3f(18, 0.5, 0.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 0.5);
	glVertex3f(18, 0, 0.5);
	glVertex3f(0, 0.5, 0.5);
	glVertex3f(18, 0.5, 0.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(18, 0, 0);
	glVertex3f(18, 0, 0.5);
	glVertex3f(18, 0.5, 0);
	glVertex3f(18, 0.5, 0.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//передний верхний бортик
	glColor3f(0, 0, 0);
	glVertex3f(0, 0.5, 11);
	glVertex3f(0, 0.5, 11.5);
	glVertex3f(0, -43.5, 11);
	glVertex3f(0, -43.5, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0.5, -43.5, 11);
	glVertex3f(0.5, -43.5, 11.5);
	glVertex3f(0, -43.5, 11);
	glVertex3f(0, -43.5, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0.5, 0.5, 11.5);
	glVertex3f(0.5, -43.5, 11.5);
	glVertex3f(0, 0.5, 11.5);
	glVertex3f(0, -43.5, 11.5);
	glEnd();


	glBegin(GL_QUAD_STRIP);								//боковой верхний бортик
	glColor3f(0, 0, 0);
	glVertex3f(0, 0.5, 11);
	glVertex3f(0, 0.5, 11.5);
	glVertex3f(18, 0.5, 11);
	glVertex3f(18, 0.5, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 11.5);
	glVertex3f(18, 0, 11.5);
	glVertex3f(0, 0.5, 11.5);
	glVertex3f(18, 0.5, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0, 0, 0);
	glVertex3f(18, 0, 11);
	glVertex3f(18, 0, 11.5);
	glVertex3f(18, 0.5, 11);
	glVertex3f(18, 0.5, 11.5);
	glEnd();



	for (int i = 11; i > 0.5; i--)						//декорация, передняя, левый бок
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, 0.5, i);
		glVertex3f(0, -0.5, i);
		glVertex3f(0, 0.5, i - 0.5);
		glVertex3f(0, -0.5, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, 0.5, i - 0.5);
		glVertex3f(0, -0.5, i - 0.5);
		glVertex3f(0.5, 0.5, i - 0.5);
		glVertex3f(0.5, -0.5, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, 0.5, i);
		glVertex3f(0, -0.5, i);
		glVertex3f(0.5, 0.5, i);
		glVertex3f(0.5, -0.5, i);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0.5, -0.5, i);
		glVertex3f(0.5, -0.5, i - 0.5);
		glVertex3f(0, -0.5, i);
		glVertex3f(0, -0.5, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, 0.5, i);
		glVertex3f(0, 0, i);
		glVertex3f(1, 0.5, i);
		glVertex3f(1, 0, i);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, 0.5, i - 0.5);
		glVertex3f(0, 0, i - 0.5);
		glVertex3f(1, 0.5, i - 0.5);
		glVertex3f(1, 0, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, 0.5, i);
		glVertex3f(0, 0.5, i - 0.5);
		glVertex3f(1, 0.5, i);
		glVertex3f(1, 0.5, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(1, 0, i);
		glVertex3f(1, 0, i - 0.5);
		glVertex3f(1, 0.5, i);
		glVertex3f(1, 0.5, i - 0.5);
		glEnd();
	}

	for (int i = 11; i > 0.5; i--)						//декорация, передняя, правый бок
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, -43, i);
		glVertex3f(0, -44, i);
		glVertex3f(0, -43, i - 0.5);
		glVertex3f(0, -44, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, -43, i);
		glVertex3f(0, -43, i - 0.5);
		glVertex3f(0.5, -43, i);
		glVertex3f(0.5, -43, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, -43, i);
		glVertex3f(0, -44, i);
		glVertex3f(0.5, -43, i);
		glVertex3f(0.5, -44, i);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, -43, i - 0.5);
		glVertex3f(0, -44, i - 0.5);
		glVertex3f(0.5, -43, i - 0.5);
		glVertex3f(0.5, -44, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, -44, i);
		glVertex3f(1, -44, i);
		glVertex3f(0, -44, i - 0.5);
		glVertex3f(1, -44, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(1, -43.5, i);
		glVertex3f(1, -43.5, i - 0.5);
		glVertex3f(1, -44, i);
		glVertex3f(1, -44, i - 0.5);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, -44, i);
		glVertex3f(1, -44, i);
		glVertex3f(0, -43.5, i);
		glVertex3f(1, -43.5, i);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor3d(234 / 255., 128 / 255., 107 / 255.);
		glVertex3f(0, -44, i - 0.5);
		glVertex3f(1, -44, i - 0.5);
		glVertex3f(0, -43.5, i - 0.5);
		glVertex3f(1, -43.5, i - 0.5);
		glEnd();
	}

	bool current_seconds[5][6];
	GeneratePixelRepresentanionOfCurrentPixels(current_seconds);
	float current_color[3];
	/*for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 6; j++)
			printf("%d", current_seconds[i][j]);
		printf("\n");
	}
	printf("--------------------\n");*/

	int i_count = 0;
	int j_count = 0;
	for (int j = -3.5; j > -43; j -= 4)										//передняя панель украшений
	{
		for (int i = 11; i > 0.5; i--)
		{
			/*if ((j == -3.5 && (i == 9 && current_seconds[0][0] || i==8 && current_seconds[1][0] || i==7 && current_seconds[2][0] || i==6 && current_seconds[3][0] ||))
				)*/
			if (i_count < 6 && j_count>2 && j_count < 8 && current_seconds[j_count - 3][i_count])
			{
				current_color[0] = 0;
				current_color[1] = 1;
				current_color[2] = 1;
			}
			else
			{
				current_color[0] = 234 / 255.;
				current_color[1] = 128 / 255.;
				current_color[2] = 107 / 255.;
			}

			glBegin(GL_QUAD_STRIP);
			glColor3d(current_color[0], current_color[1], current_color[2]);
			glVertex3f(0, j, i);
			glVertex3f(0, j - 1, i);
			glVertex3f(0, j, i - 0.5);
			glVertex3f(0, j - 1, i - 0.5);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(current_color[0], current_color[1], current_color[2]);
			glVertex3f(0, j, i);
			glVertex3f(0, j, i - 0.5);
			glVertex3f(0.5, j, i);
			glVertex3f(0.5, j, i - 0.5);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(current_color[0], current_color[1], current_color[2]);
			glVertex3f(0, j - 1, i);
			glVertex3f(0, j - 1, i - 0.5);
			glVertex3f(0.5, j - 1, i);
			glVertex3f(0.5, j - 1, i - 0.5);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(current_color[0], current_color[1], current_color[2]);
			glVertex3f(0, j, i);
			glVertex3f(0, j - 1, i);
			glVertex3f(0.5, j, i);
			glVertex3f(0.5, j - 1, i);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(current_color[0], current_color[1], current_color[2]);
			glVertex3f(0, j, i - 0.5);
			glVertex3f(0, j - 1, i - 0.5);
			glVertex3f(0.5, j, i - 0.5);
			glVertex3f(0.5, j - 1, i - 0.5);
			glEnd();

			j_count++;
		}
		i_count++;
		j_count = 0;
	}

	for (int j = 5; j < 18; j += 4)										//боковая панель украшений
	{
		for (int i = 11; i > 0.5; i--)
		{

			glBegin(GL_QUAD_STRIP);
			glColor3d(234 / 255., 128 / 255., 107 / 255.);
			glVertex3f(j, 0.5, i - 0.5);
			glVertex3f(j - 1, 0.5, i - 0.5);
			glVertex3f(j, 0.5, i);
			glVertex3f(j - 1, 0.5, i);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(234 / 255., 128 / 255., 107 / 255.);
			glVertex3f(j - 1, 0, i - 0.5);
			glVertex3f(j - 1, 0, i);
			glVertex3f(j - 1, 0.5, i - 0.5);
			glVertex3f(j - 1, 0.5, i);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(234 / 255., 128 / 255., 107 / 255.);
			glVertex3f(j, 0.5, i - 0.5);
			glVertex3f(j, 0.5, i);
			glVertex3f(j, 0, i - 0.5);
			glVertex3f(j, 0, i);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(234 / 255., 128 / 255., 107 / 255.);
			glVertex3f(j, 0.5, i - 0.5);
			glVertex3f(j - 1, 0.5, i - 0.5);
			glVertex3f(j, 0, i - 0.5);
			glVertex3f(j - 1, 0, i - 0.5);
			glEnd();

			glBegin(GL_QUAD_STRIP);
			glColor3d(234 / 255., 128 / 255., 107 / 255.);
			glVertex3f(j, 0, i);
			glVertex3f(j - 1, 0, i);
			glVertex3f(j, 0.5, i);
			glVertex3f(j - 1, 0.5, i);
			glEnd();
		}
	}

	glBegin(GL_QUAD_STRIP);							//клиентская часть стола (передняя)
	glColor3d(0, 0, 0);
	glVertex3f(0.5, 0, 11);
	glVertex3f(0.5, -43.5, 11);
	glVertex3f(4.5, 0, 11);
	glVertex3f(4.5, -43.5, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.5, 0, 11.5);
	glVertex3f(0.5, -43.5, 11.5);
	glVertex3f(4.5, 0, 11.5);
	glVertex3f(4.5, -43.5, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//клиентская часть стола (боковая)
	glColor3d(0, 0, 0);
	glVertex3f(0, 0, 11);
	glVertex3f(0, -4, 11);
	glVertex3f(18, 0, 11);
	glVertex3f(18, -4, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0, 0, 11.5);
	glVertex3f(0, -4, 11.5);
	glVertex3f(18, 0, 11.5);
	glVertex3f(18, -4, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(18, 0, 11);
	glVertex3f(18, -4, 11);
	glVertex3f(18, 0, 11.5);
	glVertex3f(18, -4, 11.5);
	glEnd();

	for (float i = 4.5; i < 5.5; i += 0.2)				//спил-стоп (передняя часть)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glVertex3f(i, -43.5, 11.5);
		glVertex3f(i, -5, 11.5);
		glVertex3f(i + 0.1, -43.5, 11.5);
		glVertex3f(i + 0.1, -5, 11.5);
		glEnd();
	}

	for (float i = -43.4; i < -5; i += 0.2)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glVertex3f(4.5, i - 0.1, 11.5);
		glVertex3f(5.5, i - 0.1, 11.5);
		glVertex3f(4.5, i, 11.5);
		glVertex3f(5.5, i, 11.5);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(4.5, -43.4, 11);
	glVertex3f(5.5, -43.4, 11);
	glVertex3f(4.5, -5, 11);
	glVertex3f(5.5, -5, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(5.5, -43.4, 11);
	glVertex3f(5.5, -5, 11);
	glVertex3f(5.5, -43.4, 11.5);
	glVertex3f(5.5, -5, 11.5);
	glEnd();

	for (float i = 17.9; i > 4.5; i -= 0.2)				//спил-стоп
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glVertex3f(i, -5, 11.5);
		glVertex3f(i, -4, 11.5);
		glVertex3f(i + 0.1, -5, 11.5);
		glVertex3f(i + 0.1, -4, 11.5);
		glEnd();
	}

	for (float i = -5; i < -4; i += 0.2)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glVertex3f(4.5, i - 0.1, 11.5);
		glVertex3f(18, i - 0.1, 11.5);
		glVertex3f(4.5, i, 11.5);
		glVertex3f(18, i, 11.5);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(4.5, -5, 11);
	glVertex3f(4.5, -4, 11);
	glVertex3f(18, -5, 11);
	glVertex3f(18, -4, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(4.5, -5, 11);
	glVertex3f(18, -5, 11);
	glVertex3f(4.5, -5, 11.5);
	glVertex3f(18, -5, 11.5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(18, -5, 11.5);
	glVertex3f(18, -4, 11.5);
	glVertex3f(18, -5, 11);
	glVertex3f(18, -4, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ограждение под спил стопом (переднее)
	glColor3d(1, 1, 1);
	glVertex3f(4.5, -43.5, 11);
	glVertex3f(4.5, -43.5, 9);
	glVertex3f(4.5, -4, 11);
	glVertex3f(4.5, -4, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ограждение под спил стопом (бок)
	glColor3d(1, 1, 1);
	glVertex3f(4.5, -4, 11);
	glVertex3f(4.5, -4, 9);
	glVertex3f(18, -4, 11);
	glVertex3f(18, -4, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//секция 1 - терминал и хородильник
	glColor3d(0, 0, 0);
	glVertex3f(4.5, -43.5, 9);
	glVertex3f(8.5, -43.5, 9);
	glVertex3f(4.5, -37, 9);
	glVertex3f(8.5, -37, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//терминал
	glColor3d(0, 1, 0);					//деньгохранитель
	glVertex3f(5, -43, 10);
	glVertex3f(8, -43, 10);
	glVertex3f(5, -37.5, 10);
	glVertex3f(8, -37.5, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(5, -43, 10);
	glVertex3f(5, -37.5, 10);
	glVertex3f(5, -43, 9);
	glVertex3f(5, -37.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(8, -43, 10);
	glVertex3f(8, -37.5, 10);
	glVertex3f(8, -43, 9);
	glVertex3f(8, -37.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(5, -37.5, 10);
	glVertex3f(8, -37.5, 10);
	glVertex3f(5, -37.5, 9);
	glVertex3f(8, -37.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(5, -43, 10);
	glVertex3f(8, -43, 10);
	glVertex3f(5, -43, 9);
	glVertex3f(8, -43, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//дверка деньгохранителя
	glColor3d(0, 1, 1);
	glVertex3f(8.01, -43, 9.75);
	glVertex3f(8.01, -37.5, 9.75);
	glVertex3f(8.01, -43, 9.25);
	glVertex3f(8.01, -37.5, 9.25);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//ножка
	glColor3d(1, 1, 0);
	glVertex3f(7, -41, 11);
	glVertex3f(8, -41, 11);
	glVertex3f(7, -39.5, 11);
	glVertex3f(8, -39.5, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(8, -41, 11);
	glVertex3f(8, -39.5, 11);
	glVertex3f(8, -41, 10);
	glVertex3f(8, -39.5, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(7, -41, 11);
	glVertex3f(7, -39.5, 11);
	glVertex3f(7, -41, 10);
	glVertex3f(7, -39.5, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(7, -39.5, 11);
	glVertex3f(8, -39.5, 11);
	glVertex3f(7, -39.5, 10);
	glVertex3f(8, -39.5, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(7, -41, 11);
	glVertex3f(8, -41, 11);
	glVertex3f(7, -41, 10);
	glVertex3f(8, -41, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//экран
	glColor3d(0, 0, 0);					//дисплей (для текстуры)
	glVertex3f(8, -43, 11);
	glVertex3f(7, -43, 14);
	glVertex3f(8, -37.5, 11);
	glVertex3f(7, -37.5, 14);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(7, -43, 11);
	glVertex3f(6, -43, 14);
	glVertex3f(7, -37.5, 11);
	glVertex3f(6, -37.5, 14);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(8, -43, 11);
	glVertex3f(7, -43, 14);
	glVertex3f(7, -43, 11);
	glVertex3f(6, -43, 14);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(8, -37.5, 11);
	glVertex3f(7, -37.5, 14);
	glVertex3f(7, -37.5, 11);
	glVertex3f(6, -37.5, 14);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(8, -43, 11);
	glVertex3f(8, -37.5, 11);
	glVertex3f(7, -43, 11);
	glVertex3f(7, -37.5, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 0);
	glVertex3f(7, -43, 14);
	glVertex3f(7, -37.5, 14);
	glVertex3f(6, -43, 14);
	glVertex3f(6, -37.5, 14);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//холодильник
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -43, 8.9);
	glVertex3f(8, -43, 8.9);
	glVertex3f(0.51, -37.5, 8.9);
	glVertex3f(8, -37.5, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);		//дно
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -43, 1);
	glVertex3f(8, -43, 1);
	glVertex3f(0.51, -37.5, 1);
	glVertex3f(8, -37.5, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -43, 0);
	glVertex3f(8, -43, 0);
	glVertex3f(0.51, -37.5, 0);
	glVertex3f(8, -37.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -43, 1);
	glVertex3f(8, -43, 1);
	glVertex3f(0.51, -43, 0);
	glVertex3f(8, -43, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -37.5, 1);
	glVertex3f(8, -37.5, 1);
	glVertex3f(0.51, -37.5, 0);
	glVertex3f(8, -37.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8, -43, 1);
	glVertex3f(8, -37.5, 1);
	glVertex3f(8, -43, 0);
	glVertex3f(8, -37.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -43, 1);
	glVertex3f(0.51, -37.5, 1);
	glVertex3f(0.51, -43, 0);
	glVertex3f(0.51, -37.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//бока
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -43, 8.9);
	glVertex3f(0.51, -37.5, 8.9);
	glVertex3f(0.51, -43, 0);
	glVertex3f(0.51, -37.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -37.5, 8.9);
	glVertex3f(8, -37.5, 8.9);
	glVertex3f(0.49, -37.5, 0);
	glVertex3f(8, -37.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -43, 8.9);
	glVertex3f(8, -43, 8.9);
	glVertex3f(0.51, -43, 0);
	glVertex3f(8, -43, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//полки
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -43, 5.9);
	glVertex3f(8, -43, 5.9);
	glVertex3f(0.51, -37.5, 5.9);
	glVertex3f(8, -37.5, 5.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -43, 1.9);
	glVertex3f(8, -43, 1.9);
	glVertex3f(0.51, -37.5, 1.9);
	glVertex3f(8, -37.5, 1.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//стекло холодильника
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(8, -43, 8.9);
	glVertex3f(8, -37.5, 8.9);
	glVertex3f(8, -43, 1);
	glVertex3f(8, -37.5, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ручка
	glColor3d(1, 1, 1);
	glVertex3f(8, -42, 8);
	glVertex3f(8.3, -42, 8);
	glVertex3f(8, -42, 7);
	glVertex3f(8.3, -42, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(8.3, -42, 8);
	glVertex3f(8.3, -41.7, 8);
	glVertex3f(8.3, -42, 7);
	glVertex3f(8.3, -41.7, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ограждение между 1 и 2 секциями
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -37, 9);
	glVertex3f(8.5, -37, 9);
	glVertex3f(0.51, -37, 0);
	glVertex3f(8.5, -37, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//секция 2 - сушка стаканов и посудомоечная машина
	glColor3d(0, 0, 0);
	glVertex3f(4.5, -30.5, 9);
	glVertex3f(8.5, -30.5, 9);
	glVertex3f(4.5, -37, 9);
	glVertex3f(8.5, -37, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//холодильник
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -31, 8.9);
	glVertex3f(8, -31, 8.9);
	glVertex3f(0.51, -36.5, 8.9);
	glVertex3f(8, -36.5, 8.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);		//дно
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -31, 1);
	glVertex3f(8, -31, 1);
	glVertex3f(0.51, -36.5, 1);
	glVertex3f(8, -36.5, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -31, 0);
	glVertex3f(8, -31, 0);
	glVertex3f(0.51, -36.5, 0);
	glVertex3f(8, -36.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -31, 1);
	glVertex3f(8, -31, 1);
	glVertex3f(0.51, -31, 0);
	glVertex3f(8, -31, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -36.5, 1);
	glVertex3f(8, -36.5, 1);
	glVertex3f(0.51, -36.5, 0);
	glVertex3f(8, -36.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(8, -31, 1);
	glVertex3f(8, -36.5, 1);
	glVertex3f(8, -31, 0);
	glVertex3f(8, -36.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -31, 1);
	glVertex3f(0.51, -36.5, 1);
	glVertex3f(0.51, -31, 0);
	glVertex3f(0.51, -36.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//бока
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -31, 8.9);
	glVertex3f(0.51, -36.5, 8.9);
	glVertex3f(0.51, -31, 0);
	glVertex3f(0.51, -36.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -36.5, 8.9);
	glVertex3f(8, -36.5, 8.9);
	glVertex3f(0.49, -36.5, 0);
	glVertex3f(8, -36.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(0.51, -31, 8.9);
	glVertex3f(8, -31, 8.9);
	glVertex3f(0.51, -31, 0);
	glVertex3f(8, -31, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(8, -31, 8.9);
	glVertex3f(8, -36.5, 8.9);
	glVertex3f(8, -31, 1);
	glVertex3f(8, -36.5, 1);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ручка
	glColor3d(0, 0, 0);
	glVertex3f(8, -35.5, 7);
	glVertex3f(8.3, -35.5, 7);
	glVertex3f(8, -32, 7);
	glVertex3f(8.3, -32, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(8.3, -35.5, 7);
	glVertex3f(8.3, -32, 7);
	glVertex3f(8.3, -35.5, 6.7);
	glVertex3f(8.3, -32, 6.7);
	glEnd();

	//glBegin(GL_QUAD_STRIP);								//секция 3 - барная станция
	//glColor3d(0, 0, 0);
	//glVertex3f(4.5, -30.5, 9);
	//glVertex3f(8.5, -30.5, 9);
	//glVertex3f(4.5, -14.5, 9);
	//glVertex3f(8.5, -14.5, 9);
	//glEnd();

	glaStencilWindow(1);

	glBegin(GL_QUAD_STRIP);								//секция 3.1 - барная станция, раковина
	glVertex3f(4, -29, 9);
	glVertex3f(7, -29, 9);
	glVertex3f(4, -27, 9);
	glVertex3f(7, -27, 9);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 0);
	glVertex3f(3.5, -30.5, 9);
	glVertex3f(7.5, -30.5, 9);
	glVertex3f(3.5, -26.5, 9);
	glVertex3f(7.5, -26.5, 9);
	glEnd();

	glaStencilEnd();

	glaStencilWindow(1);

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(6, -28, 8);
	glVertex3f(6.5, -28, 8);
	glVertex3f(6, -27.5, 8);
	glVertex3f(6.5, -27.5, 8);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(3, -29, 8);
	glVertex3f(6, -29, 8);
	glVertex3f(3, -27, 8);
	glVertex3f(6, -27, 8);
	glEnd();

	glaStencilEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(7, -29, 8);
	glVertex3f(7, -27, 8);
	glVertex3f(7, -29, 9);
	glVertex3f(7, -27, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(4, -29, 8);
	glVertex3f(4, -27, 8);
	glVertex3f(4, -29, 9);
	glVertex3f(4, -27, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(4, -27, 8);
	glVertex3f(7, -27, 8);
	glVertex3f(4, -27, 9);
	glVertex3f(7, -27, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(4, -29, 8);
	glVertex3f(7, -29, 8);
	glVertex3f(4, -29, 9);
	glVertex3f(7, -29, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//кран
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -30, 10);
	glVertex3f(5, -30, 10);
	glVertex3f(4.5, -29.5, 10);
	glVertex3f(5, -29.5, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(5, -30, 10);
	glVertex3f(5, -29.5, 10);
	glVertex3f(5, -30, 9);
	glVertex3f(5, -29.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -30, 10);
	glVertex3f(4.5, -29.5, 10);
	glVertex3f(4.5, -30, 9);
	glVertex3f(4.5, -29.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -29.5, 10);
	glVertex3f(5, -29.5, 10);
	glVertex3f(4.5, -29.5, 9);
	glVertex3f(5, -29.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -30, 10);
	glVertex3f(5, -30, 10);
	glVertex3f(4.5, -30, 9);
	glVertex3f(5, -30, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);					//кран(гор)
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -29.5, 10);
	glVertex3f(5, -29.5, 10);
	glVertex3f(4.5, -28, 10);
	glVertex3f(5, -28, 10);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -29.5, 9.7);
	glVertex3f(5, -29.5, 9.7);
	glVertex3f(4.5, -28, 9.7);
	glVertex3f(5, -28, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -28, 10);
	glVertex3f(5, -28, 10);
	glVertex3f(4.5, -28, 9.7);
	glVertex3f(5, -28, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(5, -29.5, 10);
	glVertex3f(5, -28, 10);
	glVertex3f(5, -29.5, 9.7);
	glVertex3f(5, -28, 9.7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(4.5, -29.5, 10);
	glVertex3f(4.5, -28, 10);
	glVertex3f(4.5, -29.5, 9.7);
	glVertex3f(4.5, -28, 9.7);
	glEnd();


	for (float i = 7.5; i > 3.5; i -= 0.2)				//секция 3.2 - сушка специальных приборов
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glVertex3f(i, -26.5, 9);
		glVertex3f(i, -23.5, 9);
		glVertex3f(i + 0.1, -26.5, 9);
		glVertex3f(i + 0.1, -23.5, 9);
		glEnd();
	}

	for (float i = -26.3; i < -23.3; i += 0.2)
	{
		glBegin(GL_QUAD_STRIP);
		glColor3d(0, 0, 0);
		glVertex3f(7.5, i - 0.1, 9);
		glVertex3f(3.5, i - 0.1, 9);
		glVertex3f(7.5, i, 9);
		glVertex3f(3.5, i, 9);
		glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(3.5, -26.5, 8.9);
	glVertex3f(3.5, -23.5, 8.9);
	glVertex3f(7.5, -26.5, 8.9);
	glVertex3f(7.5, -23.5, 8.9);
	glEnd();



	glBegin(GL_QUAD_STRIP);					//бортики (лёдогенератор)
	glColor3d(0, 0, 0);
	glVertex3f(4, -20, 7);
	glVertex3f(7, -20, 7);
	glVertex3f(4, -20, 9);
	glVertex3f(7, -20, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(4, -22, 7);
	glVertex3f(7, -22, 7);
	glVertex3f(4, -22, 9);
	glVertex3f(7, -22, 9);
	glEnd();

	glaStencilWindow(1);

	glBegin(GL_QUAD_STRIP);								//секция 3.3 - лёдогенератор
	glVertex3f(4, -23, 9);
	glVertex3f(7, -23, 9);
	glVertex3f(4, -19, 9);
	glVertex3f(7, -19, 9);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 0);
	glVertex3f(3.5, -18.5, 9);
	glVertex3f(7.5, -18.5, 9);
	glVertex3f(3.5, -23.5, 9);
	glVertex3f(7.5, -23.5, 9);
	glEnd();

	glaStencilEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(4, -23, 7);
	glVertex3f(7, -23, 7);
	glVertex3f(4, -19, 7);
	glVertex3f(7, -19, 7);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(4, -23, 7);
	glVertex3f(7, -23, 7);
	glVertex3f(4, -23, 9);
	glVertex3f(7, -23, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(4, -19, 7);
	glVertex3f(7, -19, 7);
	glVertex3f(4, -19, 9);
	glVertex3f(7, -19, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(7, -23, 7);
	glVertex3f(7, -19, 7);
	glVertex3f(7, -23, 9);
	glVertex3f(7, -19, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(4, -23, 7);
	glVertex3f(4, -19, 7);
	glVertex3f(4, -23, 9);
	glVertex3f(4, -19, 9);
	glEnd();


	glBegin(GL_QUAD_STRIP);								//секция 3 - низ
	glColor3d(0, 0, 1);
	glVertex3f(0.5, -30.5, 6.99);
	glVertex3f(8.5, -30.5, 6.99);
	glVertex3f(0.5, -14.5, 6.99);
	glVertex3f(8.5, -14.5, 6.99);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.5, -18.5, 7);
	glVertex3f(7.5, -18.5, 7);
	glVertex3f(0.5, -18.5, 9);
	glVertex3f(7.5, -18.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(7.5, -30.5, 7);
	glVertex3f(7.5, -18.5, 7);
	glVertex3f(7.5, -30.5, 9);
	glVertex3f(7.5, -18.5, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//мусорка
	glColor3d(0, 0, 1);
	glVertex3f(0.5, -20, 5.99);
	glVertex3f(8.5, -20, 5.99);
	glVertex3f(0.5, -15, 5.99);
	glVertex3f(8.5, -15, 5.99);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.5, -20, 0);
	glVertex3f(8.5, -20, 0);
	glVertex3f(0.5, -15, 0);
	glVertex3f(8.5, -15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.5, -20, 5.99);
	glVertex3f(8.5, -20, 5.99);
	glVertex3f(0.5, -20, 0);
	glVertex3f(8.5, -20, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.5, -15, 5.99);
	glVertex3f(8.5, -15, 5.99);
	glVertex3f(0.5, -15, 0);
	glVertex3f(8.5, -15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8.5, -20, 5.99);
	glVertex3f(8.5, -15, 5.99);
	glVertex3f(8.5, -20, 0);
	glVertex3f(8.5, -15, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(8.5, -19.5, 4.99);
	glVertex3f(8.7, -19.5, 4.99);
	glVertex3f(8.5, -19.5, 3.99);
	glVertex3f(8.7, -19.5, 3.99);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(8.7, -19.5, 4.99);
	glVertex3f(8.7, -19.5, 3.99);
	glVertex3f(8.7, -19.2, 4.99);
	glVertex3f(8.7, -19.2, 3.99);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ограждение между 3 и 4 секциями
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -14.5, 9);
	glVertex3f(8.5, -14.5, 9);
	glVertex3f(0.51, -14.5, 0);
	glVertex3f(8.5, -14.5, 0);
	glEnd();

	glaStencilWindow(1);

	glBegin(GL_QUAD_STRIP);				//секция 4 - фрукты, миксер и холодильник
	glColor3d(1, 0, 0);
	glVertex3f(4, -14, 9);
	glVertex3f(7, -14, 9);
	glVertex3f(4, -13, 9);
	glVertex3f(7, -13, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 0);
	glVertex3f(4, -12, 9);
	glVertex3f(7, -12, 9);
	glVertex3f(4, -11, 9);
	glVertex3f(7, -11, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 0);
	glVertex3f(4, -10, 9);
	glVertex3f(7, -10, 9);
	glVertex3f(4, -9, 9);
	glVertex3f(7, -9, 9);
	glEnd();

	glaStencilWall(1);

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 0);
	glVertex3f(3.5, -14.5, 9);
	glVertex3f(8.5, -14.5, 9);
	glVertex3f(3.5, -8, 9);
	glVertex3f(8.5, -8, 9);
	glEnd();

	glaStencilEnd();

	glBegin(GL_QUAD_STRIP);				//1 форма под фрукты
	glColor3d(1, 1, 0);
	glVertex3f(4, -14, 8);
	glVertex3f(7, -14, 8);
	glVertex3f(4, -13, 8);
	glVertex3f(7, -13, 8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -14, 8);
	glVertex3f(7, -14, 8);
	glVertex3f(4, -14, 9);
	glVertex3f(7, -14, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -13, 8);
	glVertex3f(7, -13, 8);
	glVertex3f(4, -13, 9);
	glVertex3f(7, -13, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -14, 8);
	glVertex3f(4, -13, 8);
	glVertex3f(4, -14, 9);
	glVertex3f(4, -13, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(7, -14, 8);
	glVertex3f(7, -13, 8);
	glVertex3f(7, -14, 9);
	glVertex3f(7, -13, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);			//2 форма под фрукты
	glColor3d(1, 1, 0);
	glVertex3f(4, -12, 8);
	glVertex3f(7, -12, 8);
	glVertex3f(4, -11, 8);
	glVertex3f(7, -11, 8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -11, 9);
	glVertex3f(7, -11, 9);
	glVertex3f(4, -11, 8);
	glVertex3f(7, -11, 8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -12, 8);
	glVertex3f(7, -12, 8);
	glVertex3f(4, -12, 9);
	glVertex3f(7, -12, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -12, 8);
	glVertex3f(4, -11, 8);
	glVertex3f(4, -12, 9);
	glVertex3f(4, -11, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(7, -12, 8);
	glVertex3f(7, -11, 8);
	glVertex3f(7, -12, 9);
	glVertex3f(7, -11, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//3 форма под фрукты
	glColor3d(1, 1, 0);
	glVertex3f(4, -10, 8);
	glVertex3f(7, -10, 8);
	glVertex3f(4, -9, 8);
	glVertex3f(7, -9, 8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -9, 8);
	glVertex3f(7, -9, 8);
	glVertex3f(4, -9, 9);
	glVertex3f(7, -9, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -10, 8);
	glVertex3f(7, -10, 8);
	glVertex3f(4, -10, 9);
	glVertex3f(7, -10, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(7, -10, 8);
	glVertex3f(7, -9, 8);
	glVertex3f(7, -10, 9);
	glVertex3f(7, -9, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 0);
	glVertex3f(4, -10, 8);
	glVertex3f(4, -9, 8);
	glVertex3f(4, -10, 9);
	glVertex3f(4, -9, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//холодильник (секции 4)
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -14, 7.9);
	glVertex3f(8, -14, 7.9);
	glVertex3f(0.51, -8.5, 7.9);
	glVertex3f(8, -8.5, 7.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);		//дно
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -14, 2.9);
	glVertex3f(8, -14, 2.9);
	glVertex3f(0.51, -8.5, 2.9);
	glVertex3f(8, -8.5, 2.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -14, 0);
	glVertex3f(8, -14, 0);
	glVertex3f(0.51, -8.5, 0);
	glVertex3f(8, -8.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -14, 2.9);
	glVertex3f(8, -14, 2.9);
	glVertex3f(0.51, -14, 0);
	glVertex3f(8, -14, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -8.5, 2.9);
	glVertex3f(8, -8.5, 2.9);
	glVertex3f(0.51, -8.5, 0);
	glVertex3f(8, -8.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8, -14, 2.9);
	glVertex3f(8, -8.5, 2.9);
	glVertex3f(8, -14, 0);
	glVertex3f(8, -8.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -14, 2.9);
	glVertex3f(0.51, -8.5, 2.9);
	glVertex3f(0.51, -14, 0);
	glVertex3f(0.51, -8.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//бока
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -14, 7.9);
	glVertex3f(0.51, -8.5, 7.9);
	glVertex3f(0.51, -14, 0);
	glVertex3f(0.51, -8.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -8.5, 7.9);
	glVertex3f(8, -8.5, 7.9);
	glVertex3f(0.49, -8.5, 0);
	glVertex3f(8, -8.5, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(0.51, -14, 7.9);
	glVertex3f(8, -14, 7.9);
	glVertex3f(0.51, -14, 0);
	glVertex3f(8, -14, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//полки
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -14, 6.9);
	glVertex3f(8, -14, 6.9);
	glVertex3f(0.51, -8.5, 6.9);
	glVertex3f(8, -8.5, 6.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -14, 5.9);
	glVertex3f(8, -14, 5.9);
	glVertex3f(0.51, -8.5, 5.9);
	glVertex3f(8, -8.5, 5.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -14, 4.9);
	glVertex3f(8, -14, 4.9);
	glVertex3f(0.51, -8.5, 4.9);
	glVertex3f(8, -8.5, 4.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -14, 3.9);
	glVertex3f(8, -14, 3.9);
	glVertex3f(0.51, -8.5, 3.9);
	glVertex3f(8, -8.5, 3.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.51, -14, 2.9);
	glVertex3f(8, -14, 2.9);
	glVertex3f(0.51, -8.5, 2.9);
	glVertex3f(8, -8.5, 2.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//стекло холодильника
	glColor4d(81 / 255., 78 / 255., 111 / 255., 0.8);
	glVertex3f(8, -14, 7.9);
	glVertex3f(8, -8.5, 7.9);
	glVertex3f(8, -14, 2.9);
	glVertex3f(8, -8.5, 2.9);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ручка (вертикальная)
	glColor3d(1, 1, 1);
	glVertex3f(8, -13.5, 7);
	glVertex3f(8.3, -13.5, 7);
	glVertex3f(8, -13.5, 6);
	glVertex3f(8.3, -13.5, 6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(8.3, -13.5, 7);
	glVertex3f(8.3, -13.2, 7);
	glVertex3f(8.3, -13.5, 6);
	glVertex3f(8.3, -13.2, 6);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//ручка (горизонтальная)
	glColor3d(1, 1, 1);
	glVertex3f(8, -14, 2.6);
	glVertex3f(8.3, -14, 2.6);
	glVertex3f(8, -8.5, 2.6);
	glVertex3f(8.3, -8.5, 2.6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3f(8.3, -14, 2.6);
	glVertex3f(8.3, -14, 2.3);
	glVertex3f(8.3, -8.5, 2.6);
	glVertex3f(8.3, -8.5, 2.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//секция 5 (боковая) - контейнеры
	glColor3d(0, 0, 0);
	glVertex3f(0.5, -4, 9);
	glVertex3f(0.5, -8, 9);
	glVertex3f(18, -4, 9);
	glVertex3f(18, -8, 9);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//полки
	glColor3d(0, 0, 0);
	glVertex3f(0.5, -4, 6);
	glVertex3f(0.5, -8, 6);
	glVertex3f(18, -4, 6);
	glVertex3f(18, -8, 6);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.5, -4, 5.8);
	glVertex3f(0.5, -8, 5.8);
	glVertex3f(18, -4, 5.8);
	glVertex3f(18, -8, 5.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.5, -8, 6);
	glVertex3f(18, -8, 6);
	glVertex3f(0.5, -8, 5.8);
	glVertex3f(18, -8, 5.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.5, -4, 3);
	glVertex3f(0.5, -8, 3);
	glVertex3f(18, -4, 3);
	glVertex3f(18, -8, 3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.5, -4, 2.8);
	glVertex3f(0.5, -8, 2.8);
	glVertex3f(18, -4, 2.8);
	glVertex3f(18, -8, 2.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3f(0.5, -8, 3);
	glVertex3f(18, -8, 3);
	glVertex3f(0.5, -8, 2.8);
	glVertex3f(18, -8, 2.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);				//контейнеры для посуды
	glColor3d(0, 0, 1);
	glVertex3f(8.5, -4, 3.01);
	glVertex3f(8.5, -8, 3.01);
	glVertex3f(17, -4, 3.01);
	glVertex3f(17, -8, 3.01);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(17, -4, 5);
	glVertex3f(17, -8, 5);
	glVertex3f(17, -4, 3.01);
	glVertex3f(17, -8, 3.01);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8.5, -4, 3.01);
	glVertex3f(8.5, -8, 3.01);
	glVertex3f(8.5, -4, 5);
	glVertex3f(8.5, -8, 5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8.5, -8, 3.01);
	glVertex3f(17, -8, 3.01);
	glVertex3f(8.5, -8, 5);
	glVertex3f(17, -8, 5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 1);
	glVertex3f(8.5, -4, 3.01);
	glVertex3f(17, -4, 3.01);
	glVertex3f(8.5, -4, 5);
	glVertex3f(17, -4, 5);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8.5, -4, 6.01);
	glVertex3f(8.5, -8, 6.01);
	glVertex3f(17, -4, 6.01);
	glVertex3f(17, -8, 6.01);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(17, -4, 8);
	glVertex3f(17, -8, 8);
	glVertex3f(17, -4, 6.01);
	glVertex3f(17, -8, 6.01);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8.5, -4, 6.01);
	glVertex3f(8.5, -8, 6.01);
	glVertex3f(8.5, -4, 8);
	glVertex3f(8.5, -8, 8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 1);
	glVertex3f(8.5, -8, 6.01);
	glVertex3f(17, -8, 6.01);
	glVertex3f(8.5, -8, 8);
	glVertex3f(17, -8, 8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 1, 1);
	glVertex3f(8.5, -4, 6.01);
	glVertex3f(17, -4, 6.01);
	glVertex3f(8.5, -4, 8);
	glVertex3f(17, -4, 8);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//задние полки
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 11);
	glVertex3f(15.5, -43.55, 11);
	glVertex3f(18, -8, 11);
	glVertex3f(18, -43.5, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 11.3);
	glVertex3f(15.5, -43.55, 11.3);
	glVertex3f(18, -8, 11.3);
	glVertex3f(18, -43.5, 11.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 11.3);
	glVertex3f(15.5, -43.55, 11.3);
	glVertex3f(15.5, -8, 11);
	glVertex3f(15.5, -43.55, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//задние полки (2)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 15);
	glVertex3f(15.5, -43.55, 15);
	glVertex3f(18, -8, 15);
	glVertex3f(18, -43.5, 15);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 15.3);
	glVertex3f(15.5, -43.55, 15.3);
	glVertex3f(18, -8, 15.3);
	glVertex3f(18, -43.5, 15.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 15.3);
	glVertex3f(15.5, -43.55, 15.3);
	glVertex3f(15.5, -8, 15);
	glVertex3f(15.5, -43.55, 15);
	glEnd();


	glBegin(GL_QUAD_STRIP);							//задние полки (бока)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 11);
	glVertex3f(15.5, -8, 19.3);
	glVertex3f(18, -8, 11);
	glVertex3f(18, -8, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -7.8, 11);
	glVertex3f(15.5, -7.8, 19.3);
	glVertex3f(18, -7.8, 11);
	glVertex3f(18, -7.8, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 11);
	glVertex3f(15.5, -8, 19.3);
	glVertex3f(15.5, -7.8, 11);
	glVertex3f(15.5, -7.8, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 19.3);
	glVertex3f(18, -7.8, 19.3);
	glVertex3f(15.5, -7.8, 19.3);
	glVertex3f(18, -8, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -8, 11);
	glVertex3f(18, -7.8, 11);
	glVertex3f(15.5, -7.8, 11);
	glVertex3f(18, -8, 11);
	glEnd();

	glBegin(GL_QUAD_STRIP);							//задние полки (бока 2)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -43.5, 11);
	glVertex3f(15.5, -43.5, 19.3);
	glVertex3f(18, -43.5, 11);
	glVertex3f(18, -43.5, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -43.7, 11);
	glVertex3f(15.5, -43.7, 19.3);
	glVertex3f(18, -43.7, 11);
	glVertex3f(18, -43.7, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -43.5, 11);
	glVertex3f(15.5, -43.5, 19.3);
	glVertex3f(15.5, -43.7, 11);
	glVertex3f(15.5, -43.7, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -43.5, 19.3);
	glVertex3f(18, -43.7, 19.3);
	glVertex3f(15.5, -43.7, 19.3);
	glVertex3f(18, -43.5, 19.3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(15.5, -43.5, 11);
	glVertex3f(18, -43.7, 11);
	glVertex3f(15.5, -43.7, 11);
	glVertex3f(18, -43.5, 11);
	glEnd();

	center_of_circle_3D.x = 15.4;								//перекладина 1
	center_of_circle_3D.y = -7.9;
	center_of_circle_3D.z = 11.9;
	GetCircleXZ(circle_points_3D, center_of_circle_3D, 0.025);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 35.5, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 35.5, circle_points_3D[i].z);
		glEnd();
	}

	center_of_circle_3D.x = 15.4;								//перекладина 2
	center_of_circle_3D.y = -7.9;
	center_of_circle_3D.z = 15.9;
	GetCircleXZ(circle_points_3D, center_of_circle_3D, 0.025);

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 0, 1);
	for (int i = 0; i < 360; i++)
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 35.5, circle_points_3D[i].z);
	glEnd();

	for (int i = 0; i < 360; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glNormal3f(0, 0, 1);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y, circle_points_3D[i].z);
		glVertex3f(circle_points_3D[i].x, circle_points_3D[i].y - 35.5, circle_points_3D[i].z);
		glEnd();
	}

	float colors[3] = { 78 / 255., 67 / 255., 71 / 255. };

	glPushMatrix();								//бутылки
	glTranslatef(16.2, -7.9, 11.5);
	rows_of_bottles[0].DrawBottlesArray();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(16.2, -7.9, 15.5);
	rows_of_bottles[1].DrawBottlesArray();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.5, -14.6, 7);
	rows_of_bottles[2].DrawBottlesArray();
	glPopMatrix();

	for (float i = 4.5; i < 7.5; i += 1)
		for (float j = -17.5; j < -14.5; j += 1)
		{
			glPushMatrix();
			glTranslatef(i, j, 7);
			DrawBottleType3(1, colors);
			glPopMatrix();
		}

	for (float i = 5; i < 8; i += 1)
		for (float j = -26.5; j < -24.5; j += 1)
		{
			glPushMatrix();
			glTranslatef(i, j, 10);
			glRotatef(180, 1, 0, 0);
			DrawGlass(1);
			glPopMatrix();
		}

	glPushMatrix();
	glTranslatef(6, -23.5, 9);
	DrawShaker(1, colors);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7, -23.5, 9);
	DrawVortex(0.5, colors);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5, -23.5, 9);
	DrawMiniGlass(0.4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7, -6, 9.01);
	glRotatef(35, 0, 0, 1);
	DrawBlender(0.9, colors);
	glPopMatrix();


	glBegin(GL_QUAD_STRIP);										//саббок с дверью
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(0.5, -43.5, 0);
	glVertex3f(0.5, -43.5, 11.5);
	glVertex3f(10, -43.5, 0);
	glVertex3f(10, -43.5, 11.5);
	glEnd();

	glPushMatrix();
	glTranslatef(10, -43.5, 5);
	glRotatef(90, 1, 0, 0);
	glRotatef(90, 0, 0, 1);
	DrawHolders();
	glPopMatrix();

	glBegin(GL_QUAD_STRIP);								//держатель дверки
	glColor3f(10 / 255., 67 / 255., 71 / 255.);
	glVertex3f(9.7, -44, 4);
	glVertex3f(9.7, -44, 4.493);
	glVertex3f(11, -44, 4);
	glVertex3f(11, -44, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(9.7, -43.6, 4);
	glVertex3f(9.7, -43.6, 4.493);
	glVertex3f(11, -43.6, 4);
	glVertex3f(11, -43.6, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(9.7, -43.6, 4);
	glVertex3f(9.7, -43.6, 4.493);
	glVertex3f(9.7, -44, 4);
	glVertex3f(9.7, -44, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(11, -43.6, 4);
	glVertex3f(11, -43.6, 4.493);
	glVertex3f(11, -44, 4);
	glVertex3f(11, -44, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(9.7, -43.6, 4);
	glVertex3f(11, -43.6, 4);
	glVertex3f(9.7, -44, 4);
	glVertex3f(11, -44, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(9.7, -43.6, 4.493);
	glVertex3f(11, -43.6, 4.493);
	glVertex3f(9.7, -44, 4.493);
	glVertex3f(11, -44, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);								//дверка
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(10.5, -43.5, 0);
	glVertex3f(10.5, -43.5, 10);
	glVertex3f(18, -43.5, 0);
	glVertex3f(18, -43.5, 10);
	glEnd();


	glBegin(GL_QUAD_STRIP);								//держатель дверки (2)
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(10.5, -44, 4);
	glVertex3f(10.5, -44, 4.493);
	glVertex3f(11, -44, 4);
	glVertex3f(11, -44, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(10.5, -43.5, 4);
	glVertex3f(10.5, -43.5, 4.493);
	glVertex3f(11, -43.5, 4);
	glVertex3f(11, -43.5, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(10.5, -43.5, 4);
	glVertex3f(10.5, -43.5, 4.493);
	glVertex3f(10.5, -44, 4);
	glVertex3f(10.5, -44, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0 / 255., 67 / 255., 71 / 255.);
	glVertex3f(11, -43.5, 4);
	glVertex3f(11, -43.5, 4.493);
	glVertex3f(11, -44, 4);
	glVertex3f(11, -44, 4.493);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(10.5, -43.5, 4);
	glVertex3f(11, -43.5, 4);
	glVertex3f(10.5, -44, 4);
	glVertex3f(11, -44, 4);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(78 / 255., 67 / 255., 71 / 255.);
	glVertex3f(10.5, -43.5, 4.493);
	glVertex3f(11, -43.5, 4.493);
	glVertex3f(10.5, -44, 4.493);
	glVertex3f(11, -44, 4.493);
	glEnd();

}

void DrawCurtain()
{
	glPushMatrix();
	glTranslatef(sin(-36), -36, 0);
	glRotatef(20, 0, 0, 1);
	glTranslatef(-sin(-36), 36, 0);

	Point current_line, old_line;
	current_line.x = sin(-36);
	current_line.y = -36;
	for (float curtain_line_position = -36; curtain_line_position < -15; curtain_line_position += 0.01)
	{
		old_line = current_line;
		current_line.x = sin(curtain_line_position);
		current_line.y = curtain_line_position;
		glBegin(GL_QUAD_STRIP);
		glColor3f(228. / 255, 227. / 255, 220. / 255);
		glVertex3f(current_line.x, current_line.y, 2);
		glVertex3f(old_line.x, old_line.y, 2);
		glVertex3f(current_line.x, current_line.y, 30);
		glVertex3f(old_line.x, old_line.y, 30);
		glEnd();
	}
	glPopMatrix();
}


void DrawPicture()
{
	glPushMatrix();												//КАРТИНА
	glTranslatef(0, 0, 15);
	Point epicycloid[500];
	float big_radius = 1.25;
	float little_radius = 1.5;
	Point center_of_big_circle, old_point, new_point ,first_point;
	center_of_big_circle.x = 0;
	center_of_big_circle.y = 0;
	int count_of_cycles = 40;
	int count_of_points_in_cycle = 8;
	for (float i = 0; i < 2 * pi; i += (2 * pi) / count_of_cycles)
	{
		for (int j = 0; j < count_of_points_in_cycle; j++)
		{
			if (!(i == 0 && j == 0))
				old_point = new_point;
			new_point.x = center_of_big_circle.x + big_radius * cos(i + j * (2 * pi / count_of_points_in_cycle / count_of_cycles)) + little_radius * cos(j*(2 * pi / count_of_points_in_cycle));
			new_point.y = center_of_big_circle.y + big_radius * sin(i + j * (2 * pi / count_of_points_in_cycle / count_of_cycles)) - little_radius * sin(j*(2 * pi / count_of_points_in_cycle));
			if (!(i == 0 && j == 0))
			{
				glBegin(GL_LINES);
				glColor3d((float)j / (count_of_points_in_cycle + 120), i/(2*pi/count_of_cycles)/10, j);
				glVertex3d(0, old_point.x, old_point.y);
				glVertex3d(0, new_point.x, new_point.y);
				glEnd();
			}
			else
				first_point = new_point;
		}
	}

	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(0, first_point.x, first_point.y);
	glVertex3d(0, new_point.x, new_point.y);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 1, 1);
	glVertex3d(0.01, -3, -3);
	glVertex3d(0.01, -3, 3);
	glVertex3d(0.01, 3, -3);
	glVertex3d(0.01, 3, 3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(-0.3, -3, -3);
	glVertex3d(-0.3, -3, 3);
	glVertex3d(-0.3, -2.8, -3);
	glVertex3d(-0.3, -2.8, 3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, -2.8, -3);
	glVertex3d(0.01, -2.8, 3);
	glVertex3d(-0.3, -2.8, -3);
	glVertex3d(-0.3, -2.8, 3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, -3, -3);
	glVertex3d(0.01, -3, 3);
	glVertex3d(-0.3, -3, -3);
	glVertex3d(-0.3, -3, 3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(-0.3, 3, 3);
	glVertex3d(-0.3, 3, -3);
	glVertex3d(-0.3, 2.8, 3);
	glVertex3d(-0.3, 2.8, -3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, 2.8, 3);
	glVertex3d(0.01, 2.8, -3);
	glVertex3d(-0.3, 2.8, 3);
	glVertex3d(-0.3, 2.8, -3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, 3, 3);
	glVertex3d(0.01, 3, -3);
	glVertex3d(-0.3, 3, 3);
	glVertex3d(-0.3, 3, -3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(-0.3, 3, 3);
	glVertex3d(-0.3, 3, 2.8);
	glVertex3d(-0.3, -3, 3);
	glVertex3d(-0.3, -3, 2.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, 3, 2.8);
	glVertex3d(0.01, -3, 2.8);
	glVertex3d(-0.3, 3, 2.8);
	glVertex3d(-0.3, -3, 2.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, 3, 3);
	glVertex3d(0.01, -3, 3);
	glVertex3d(-0.3, 3, 3);
	glVertex3d(-0.3, -3, 3);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(-0.3, 3, -3);
	glVertex3d(-0.3, 3, -2.8);
	glVertex3d(-0.3, -3, -3);
	glVertex3d(-0.3, -3, -2.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, 3, -2.8);
	glVertex3d(0.01, -3, -2.8);
	glVertex3d(-0.3, 3, -2.8);
	glVertex3d(-0.3, -3, -2.8);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3d(0, 0, 0);
	glVertex3d(0.01, 3, -3);
	glVertex3d(0.01, -3, -3);
	glVertex3d(-0.3, 3, -3);
	glVertex3d(-0.3, -3, -3);
	glEnd();

	glPopMatrix();
}



void DrawModel(void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Ugol_update();
	//axes  Оси кооординат

	////OX RED
	//glBegin(GL_LINES);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//glVertex3f(30.0f, 0.0f, 0.0f);
	//glEnd();

	//// OY GREEN
	//glBegin(GL_LINES);
	//glColor3f(0.0f, 1.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//glVertex3f(0.0f, 30.0f, 0.0f);
	//glEnd();

	//// OZ BLUE
	//glBegin(GL_LINES);
	//glColor3f(0.0f, 0.0f, 1.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 30.0f);
	//glEnd();




	//Ground-------------------------------------------------------------------------------------------------------





	for (int i = -36; i < 36; i += 3)
		for (int j = -36; j < 36; j += 3)
		{
			glBegin(GL_QUADS);
			if ((abs(i) + abs(j)) % 2)
				glColor3f(228. / 255, 227. / 255, 220. / 255);
			else
				glColor3f(45 / 255., 11 / 255., 62 / 255.);
			glNormal3f(0, 0, 1);
			glVertex3d(i, j, 0);
			glVertex3d(i + 3, j, 0);
			glVertex3d(i + 3, j + 3, 0);
			glVertex3d(i, j + 3, 0);
			glEnd();
		}









	/*


  // созраняем матрицу в стек
  glPushMatrix();
  // поднимаем коондинаты на 3 по оси Z
  glTranslatef(10,5,3);

  // Рисуем бабочку
  // тельце
   glColor3f( 0.f,0.f,0.f);
   glLineWidth(5);
   glBegin(GL_LINES);
   glVertex3f(0,-1,0);
   glVertex3f(0,8,0);
	glEnd();
   glLineWidth(1);

  // повернем будущее крыло на УГОЛ вдоль вектора 0,1,0
  glRotatef(Ugol,0,1,0);


  // Левая сторона
  glNormal3f(0,0,1);
  // Вернеее крыло
   glColor3f( 0.9f,0.4f,0.0f);
   glBegin(GL_POLYGON);
   glVertex3f(0,0,0);
   glVertex3f(4,-2,0);
   glVertex3f(7,0,0);
   glVertex3f(5,4,0);
   glVertex3f(0,4,0);
   glEnd();
  // нижнее крыло
	glBegin(GL_POLYGON);
	glColor3f( 0.8f,0.5f,0.0f);
   glVertex3f(0,5,0);
   glVertex3f(4,4,0);
   glVertex3f(5,7,0);
   glVertex3f(4,9,0);
   glVertex3f(2,9,0);
   glVertex3f(0,7,0);
	glEnd();

  // Правая сторона

  // повернем координаты в два раза больше назад
  glRotatef((-Ugol)*2,0,1,0);
  // Сохраняем мактрицу в стек
   glPushMatrix();

  // применяем функцию растягивания в качестве отражения по оси Y
  glScalef(-1,1,-1);


  glColor3f( 0.9f,0.4f,0.0f);
  // верхнее крыло
	glBegin(GL_POLYGON);
   glVertex3f(0,0,0);
   glVertex3f(4,-2,0);
   glVertex3f(7,0,0);
   glVertex3f(5,4,0);
   glVertex3f(0,4,0);
   glEnd();
  // нижнее крыло
	glBegin(GL_POLYGON);
	glColor3f( 0.8f,0.5f,0.0f);
   glVertex3f(0,5,0);
   glVertex3f(4,4,0);
   glVertex3f(5,7,0);
   glVertex3f(4,9,0);
   glVertex3f(2,9,0);
   glVertex3f(0,7,0);
	glEnd();

	glPopMatrix();
	glPopMatrix();

	*/



	//НАЧАЛО ЛАБЫ
	//
	//	glPushMatrix();
	//
	//
	//        
	//glRotatef(Ugol,0,0,1);
	//glBegin(GL_QUADS);
	//glColor3f( 1.0f,0.0f,0.0f);
	//glNormal3f(0,0,1);
	//    glVertex3f(2,2,2);
	//    glVertex3f(-2,2,2);
	//    glVertex3f(-2,-2,2);
	//    glVertex3f(2,-2,2);
	//glEnd();
	//
	//glBegin(GL_TRIANGLES);
	//glColor3f( 0.0f,2.0f,0.0f);
	//glNormal3f(0,0,0);
	//    glVertex3f(2,2,2);
	//    glVertex3f(-2,2,2);
	//    glVertex3f(0,0,6);
	//glEnd();
	//
	//glBegin(GL_TRIANGLES);
	//glColor3f( 0.0f,0.0f,3.0f);
	//glNormal3f(0,0,0);
	//    glVertex3f(-2,-2,2);
	//    glVertex3f(-2,2,2);
	//    glVertex3f(0,0,6);
	//glEnd();
	//
	//glBegin(GL_TRIANGLES);
	//glColor3f( 4.0f,0.0f,3.0f);
	//glNormal3f(0,0,0);
	//    glVertex3f(-2,-2,2);
	//    glVertex3f(2,-2,2);
	//    glVertex3f(0,0,6);
	//glEnd();
	//
	//glBegin(GL_TRIANGLES);
	//glColor3f( 4.0f,5.0f,3.0f);
	//glNormal3f(0,0,0);
	//    glVertex3f(2,2,2);
	//    glVertex3f(2,-2,2);
	//    glVertex3f(0,0,6);
	//glEnd();
	//
	//
	//	glPopMatrix();
	//
	//
	////ВТОРАЯ ЧАСТЬ
	//
	//
	//
	//ArcCustom arcCD = ArcCustom(9, 4, 9, 8, 3);
	//Point* arc_pointsCD = NULL;
	//int count_of_elemsCD;
	//arcCD.GenerateArc(arc_pointsCD, count_of_elemsCD);
	//
	//ArcCustom arcBC = ArcCustom(6, 2, 9, 4, 5);
	//Point* arc_pointsBC = NULL;
	//int count_of_elemsBC;
	//arcBC.GenerateArc(arc_pointsBC, count_of_elemsBC, false);
	//
	////printf("%d %d\n", count_of_elems, count_of_elemsBC);
	//
	//
	//glaStencilWindow(1);									//закомментировано для 3 лабы
	//
	//glBegin(GL_POLYGON);
	//for (i = 0; i < count_of_elemsBC; i++)
	//{
	//	glVertex3f(arc_pointsBC[i].x, arc_pointsBC[i].y, 2);
	//}
	//glEnd();
	//
	//glaStencilWall(1);
	//
	//
	//glBegin(GL_POLYGON);
	//glColor3f( 100.0f,0.0f,0.0f);
	//glNormal3f(0,0,-1);
	//    glVertex3f(3,2,2);
	//    glVertex3f(6,2,2);
	//    glVertex3f(9,4,2);
	//	for (i = 0; i < count_of_elemsCD; i++)
	//	{
	//		glVertex3f(arc_pointsCD[i].x, arc_pointsCD[i].y, 2);
	//	}
	//	glVertex3f(9,8,2);
	//    glVertex3f(5,6,2);
	//    glVertex3f(4,9,2);
	//glEnd();
	//
	//glaStencilEnd();
	//
	//glaStencilWindow(1);
	//
	//glBegin(GL_POLYGON);
	//for (i = 0; i < count_of_elemsBC; i++)
	//{
	//	glVertex3f(arc_pointsBC[i].x, arc_pointsBC[i].y, 4);
	//}
	//glEnd();
	//
	//glaStencilWall(1);
	//
	//glBegin(GL_POLYGON);
	//glColor3f( 100.0f,0.0f,0.0f);
	//glNormal3f(0,0,1);
	//    glVertex3f(3,2,4);
	//    glVertex3f(6,2,4);
	//
	//    glVertex3f(9,4,4);
	//	for (i = 0; i < count_of_elemsCD; i++)
	//	{
	//		glVertex3f(arc_pointsCD[i].x, arc_pointsCD[i].y, 4);
	//	}
	//	glVertex3f(9,8,4);
	//    glVertex3f(5,6,4);
	//    glVertex3f(4,9,4);
	//glEnd();
	//
	//glaStencilEnd();
	//
	//
	//glBegin(GL_QUAD_STRIP);
	//glColor3f( 0.0f,1.0f,0.0f);
	//glNormal3f(0,-3,0);
	//    glVertex3f(3,2,2);
	//    glVertex3f(6,2,2);
	//    glVertex3f(3,2,4);
	//    glVertex3f(6,2,4);
	//glEnd();
	//
	//
	//for (i = 0; i < count_of_elemsBC; i++)
	//{
	//
	//	glBegin(GL_LINES);
	//	glColor3f(1.0f, 1.0f, 0.0f);
	//	Point3D normal = GetNormalVector(arc_pointsBC[i].x, arc_pointsBC[i].y, 4, arc_pointsBC[i].x, arc_pointsBC[i].y, 2, 4, 9, 4);
	//	glNormal3f(normal.x, normal.y, normal.z);
	//	glVertex3f(arc_pointsBC[i].x, arc_pointsBC[i].y, 2);
	//	glVertex3f(arc_pointsBC[i].x, arc_pointsBC[i].y, 4);
	//	glEnd();
	//
	//
	//}
	//
	//
	///*glBegin(GL_QUAD_STRIP);						//BC, впуклый rad: 5
	//glColor3f( 0.0f,0.0f,1.0f);
	//glNormal3f(0,0,1);
	//    glVertex3f(6,2,2);
	//    glVertex3f(9,4,2);
	//    glVertex3f(6,2,4);
	//    glVertex3f(9,4,4);
	//glEnd();*/
	//
	//
	//for (i = 0; i< count_of_elemsCD; i++)
	//{
	//
	//	glBegin(GL_LINES);
	//	glColor3f(0.0f, 1.0f, 1.0f);
	//	Point3D normal = GetNormalVector(arc_pointsCD[i].x, arc_pointsCD[i].y, 4, arc_pointsCD[i].x, arc_pointsCD[i].y, 2, 4, 9, 4);
	//	//printf("%f %f %f\n", normal.x, normal.y, normal.z);
	//	glNormal3f(normal.x, normal.y, normal.z);
	//	glVertex3f(arc_pointsCD[i].x, arc_pointsCD[i].y, 2);
	//	glVertex3f(arc_pointsCD[i].x, arc_pointsCD[i].y, 4);
	//	glEnd();
	//}
	//
	//
	///*glBegin(GL_QUAD_STRIP);						//CD, выпуклый rad: 3
	//glColor3f( 0.0f,1.0f,1.0f);
	//glNormal3f(0,0,1);
	//    glVertex3f(9,4,2);
	//    glVertex3f(9,8,2);
	//    glVertex3f(9,4,4);
	//    glVertex3f(9,8,4);
	//glEnd();
	//*/
	//free(arc_pointsCD);
	//free(arc_pointsBC);
	//
	//
	//glBegin(GL_QUAD_STRIP);					//закомментировано для 3 лабы
	//glColor3f( 1.0f,1.0f,1.0f);
	//glNormal3f(-2,4,0);
	//    glVertex3f(9,8,2);
	//    glVertex3f(5,6,2);
	//    glVertex3f(9,8,4);
	//    glVertex3f(5,6,4);
	//glEnd();
	//
	//
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	//
	//glBegin(GL_QUAD_STRIP);
	//glNormal3f(3,1,0);
	//	glTexCoord2f(0, 0);
	//    glVertex3f(5,6,2);
	//	glTexCoord2f(0.2, 0);
	//    glVertex3f(4,9,2);
	//	glTexCoord2f(0.2, 0.2);
	//    glVertex3f(5,6,4);
	//	glTexCoord2f(0, 0.2);
	//    glVertex3f(4,9,4);
	//glEnd();
	//
	//glDisable(GL_TEXTURE_2D);
	//
	//
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[1]);
	//
	//
	//glBegin(GL_QUAD_STRIP);
	//glNormal3f(-7,1,0);
	//	glTexCoord2f(0, 0);
	//    glVertex3f(3,2,2);
	//
	//	glTexCoord2f(0.4, 0);
	//    glVertex3f(4,9,2);
	//
	//	glTexCoord2f(0.4, 0.4);
	//    glVertex3f(3,2,4);
	//
	//	glTexCoord2f(0, 0.4);
	//    glVertex3f(4,9,4);
	//glEnd();
	//
	//glDisable(GL_TEXTURE_2D);
	//
	//
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[0]);
	//
	//
	//glBegin(GL_QUADS);
	//glNormal3f(0, 0, 1);
	//
	//
	//glTexCoord2f(0, 0);
	//glVertex3f(-30, -30, -50);
	//
	//
	//glTexCoord2f(1, 0);
	//glVertex3f(30, -30, -50);
	//
	//glTexCoord2f(1, 1);
	//glVertex3f(30, 30, -50);
	//
	//glTexCoord2f(0, 1);
	//glVertex3f(-30, 30, -50);
	//
	//glEnd();
	//
	//
	//
	//glDisable(GL_TEXTURE_2D);
	//
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	//
	//
	//glBegin(GL_QUADS);
	//glNormal3f(0, 0, 1);
	//
	//
	//glTexCoord2f(0, 0);
	//glVertex3f(-30, -30, -30);
	//
	//
	//glTexCoord2f(1, 0);
	//glVertex3f(30, -30, -30);
	//
	//glTexCoord2f(1, 1);
	//glVertex3f(30, 30, -30);
	//
	//glTexCoord2f(0, 1);
	//glVertex3f(-30, 30, -30);
	//
	//glEnd();
	//
	//
	//
	//glDisable(GL_TEXTURE_2D);

	//Курсач-------------------------------------------------------------------------------


	glPushMatrix();
	glTranslatef(18, 33, 0);
	DrawBarCounter();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(12, 26, 0);
	glRotatef(-35, 0, 0, 1);
	DrawBarChair();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(12, 12, 0);
	DrawBarChair();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(12, 6, 0);
	glRotatef(25, 0, 0, 1);
	DrawBarChair();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-11, 20, 0);
	DrawTable();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2, 17, 0);
	DrawCommonChair();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-8, 24, 0);
	glRotatef(75, 0, 0, 1);
	DrawCommonChair();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 8, 0);
	glRotatef(-90, 0, 0, 1);
	DrawCommonChair();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8, -29.5, 0);
	glRotatef(-90, 0, 0, 1);
	DrawJukebox();
	glPopMatrix();


	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(45 / 255., 11 / 255., 62 / 255.);
	glVertex3f(-6, -36, 2);
	glVertex3f(-36, 36, 2);
	glVertex3f(-36, -36, 2);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(45 / 255., 11 / 255., 62 / 255.);
	glVertex3f(-6, -36, 0);
	glVertex3f(-36, 36, 0);
	glVertex3f(-36, -36, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(45 / 255., 11 / 255., 62 / 255.);
	glVertex3f(-6, -36, 0);
	glVertex3f(-36, 36, 0);
	glVertex3f(-6, -36, 2);
	glVertex3f(-36, 36, 2);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(45 / 255., 11 / 255., 62 / 255.);
	glVertex3f(-6, -36, 2);
	glVertex3f(-36, -36, 2);
	glVertex3f(-6, -36, 0);
	glVertex3f(-36, -36, 0);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(45 / 255., 11 / 255., 62 / 255.);
	glVertex3f(-36, 36, 2);
	glVertex3f(-36, -36, 2);
	glVertex3f(-36, 36, 0);
	glVertex3f(-36, -36, 0);
	glEnd();


	glPushMatrix();
	glTranslatef(-18, -12, 2);
	glRotatef(-70, 0, 0, 1);
	DrawPiano();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-30, -10, 2);
	glRotatef(110, 0, 0, 1);
	DrawPianoChair();
	glPopMatrix();


	GenerateTheSetOfScreenshotsNumbers(screens_texture_numbers);

	glPushMatrix();										//освещение (доработать)
	glTranslatef(32, 32, 0);
	glRotatef(-130, 0, 0, 1);
	DrawLampLC(20, texture_array[screens_texture_numbers[0]]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10, -10, 0);
	glRotatef(30, 0, 0, 1);
	DrawLampCRT(23, texture_array[screens_texture_numbers[1]]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20, 20, 0);
	DrawLampLCC(18, texture_array[screens_texture_numbers[2]]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, -20, 0);
	glRotatef(-35, 0, 0, 1);
	DrawLampCRT(25, texture_array[screens_texture_numbers[3]]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-30, 15, 0);
	glRotatef(-70, 0, 0, 1);
	DrawLampLC(18, texture_array[screens_texture_numbers[4]]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-30, -30, 0);
	glRotatef(45, 0, 0, 1);
	DrawLampLCC(13, texture_array[screens_texture_numbers[5]]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-7.9, 0, 0);
	DrawCurtain();
	glPopMatrix();



	bartender.DrawPlayer();
	glDisable(GL_BLEND);

	glEnable(GL_BLEND);




	/*if (isBartenderingModeOn && isVNModeOn)
	{
		glBegin(GL_QUAD_STRIP);
		glColor4d(0, 0, 0, 0.8);
		glVertex3f(24, 0, 11);
		glVertex3f(24, 36, 11);
		glVertex3f(24, 0, 14);
		glVertex3f(24, 36, 14);
		glEnd();

		glColor3f(223 / 255., 133 / 255., 123 / 255.);
		DrawCharName(Talker);
		if (Talk[counter_of_chars] != '\0')
			counter_of_chars++;
		glColor3f(1, 1, 1);
		DrawComplexTalk(Talk, counter_of_chars);
	}*/




	if (isRightButtonReleased)
		StopChangingAngle();



	glass.DrawActiveGlass();


	glPushMatrix();
	glTranslatef(21, -35.8, 0);
	glRotatef(-90, 0, 0, 1);
	DrawPicture();
	glPopMatrix();


	/*if (isBartenderingModeOn && !isVNModeOn)
	{

		shaker.DrawActiveShaker();

		for (int i = 0; i < 5; i++)
		{
			if(i<3)
				active_bottles[i].DrawActiveBottle();
			else
			{
				glPushMatrix();
				Point3D pos = active_bottles[i].isBottleActive()? active_bottles[i].GetCurrentPosition(): active_bottles[i].GetInitialPosition();
				glTranslatef(pos.x, pos.y, pos.z);
				if(i==4)
					glRotatef(3, 1, 0, 0);
				else
					glRotatef(2, 1, 0, 0);
				glTranslatef(-pos.x, -pos.y, -pos.z);
				active_bottles[i].DrawActiveBottle();
				glPopMatrix();
			}
		}

		IceButton.DrawButton();
		AgeButton.DrawButton();
		EraseButton.DrawButton();
		if(glass.isFilled)
			ServeButton.DrawButton();

		shaker_cont.SetIce(addIce);
		shaker_cont.SetAge(addAge);

		if (resetDrink)
		{
			shaker_cont.Reset();
			shaker.Reset();
			addIce = false;
			addAge = false;
			glass.isFilled = false;
		}

		resetDrink = false;
		if (serve)
			printf("Drink index:%d\n",shaker.GetResultDrinkIndex());
		serve = false;

		glBegin(GL_QUAD_STRIP);
		glColor4d(0, 0, 0, 0.8);
		glVertex3f(22.5, 8.5, 11);
		glVertex3f(22.5, 36, 11);
		glVertex3f(20.5, 8.5, 30);
		glVertex3f(20.5, 36, 30);
		glEnd();

		glBegin(GL_QUAD_STRIP);
		glColor4d(0, 0, 0, 0);
		glVertex3f(22.5, 8.5, 11);
		glVertex3f(22.5, 0, 11);
		glVertex3f(20.5, 8.5, 30);
		glVertex3f(20.5, 0, 30);
		glEnd();

		ChangePositionOfObjectsDuringLeftMousePress(leftclickedmouse_x, leftclickedmouse_y);

		glColor3d(1, 1, 1);
		DrawBarTitles();
	}*/

	vn.DrawTextComponent();

	/*GenerateAlcoDrops();

	for (int i = 0; i < count_of_alco_drops; i++)
		stream[i].DrawAlcoDrop();*/

	if (ShowWalls)
	{

	TextureStart("bar_wall.bmp");
	glBegin(GL_QUAD_STRIP);
	glTexCoord2d(0, 0);
	glVertex3f(36.01, 36, 0);
	glTexCoord2d(0, 1);
	glVertex3f(36.01, 36, 30);
	glTexCoord2d(1, 0);
	glVertex3f(36.01, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3f(36.01, 0, 30);
	glEnd();
	TextureEnd();

	glaStencilWindow(1);

	glBegin(GL_QUAD_STRIP);
	glVertex3f(36, -30, 0);
	glVertex3f(36, -30, 20);
	glVertex3f(36, -20, 0);
	glVertex3f(36, -20, 20);
	glEnd();

	glaStencilWall(1);

	
	TextureStart("bar_wall.bmp");
	glBegin(GL_QUAD_STRIP);
	glTexCoord2d(1, 0);
	glVertex3f(36, -36, 0);
	glTexCoord2d(1, 1);
	glVertex3f(36, -36, 30);
	glTexCoord2d(0, 0);
	glVertex3f(36, 0, 0);
	glTexCoord2d(0, 1);
	glVertex3f(36, 0, 30);
	glEnd();
	TextureEnd();

	glaStencilEnd();

	/*for (int i = -36; i < 36; i += 3)
		for (int j = 0; j < 30; j += 3)
		{
			glBegin(GL_QUADS);
			if ((abs(i) + abs(j)) % 2)
				glColor3f(45. / 255, 11. / 255, 62. / 255);
			else
				glColor3f(228. / 255, 227. / 255, 220. / 255);
			glNormal3f(0, 0, 1);
			glVertex3d(i,  -36, j );
			glVertex3d(i + 3, -36,j);
			glVertex3d(i + 3, -36,j + 3);
			glVertex3d(i, -36,j + 3);
			glEnd();
		}*/

	TextureStart("common_wall.bmp");
	glBegin(GL_QUAD_STRIP);
	glTexCoord2d(0, 0);
	glVertex3f(36, -36.02, 0);
	glTexCoord2d(0, 1);
	glVertex3f(36, -36.02, 30);
	glTexCoord2d(1, 0);
	glVertex3f(0, -36.02, 0);
	glTexCoord2d(1, 1);
	glVertex3f(0, -36.02, 30);
	glEnd();
	TextureEnd();

	TextureStart("common_wall.bmp");
	glBegin(GL_QUAD_STRIP);
	glTexCoord2d(1, 0);
	glVertex3f(-36, -36.01, 0);
	glTexCoord2d(1, 1);
	glVertex3f(-36, -36.01, 30);
	glTexCoord2d(0, 0);
	glVertex3f(0, -36.01, 0);
	glTexCoord2d(0, 1);
	glVertex3f(0, -36.01, 30);
	glEnd();
	TextureEnd();

	/*for(int i=0;i< 36;i+=12)
		for (int j = 0; j < 30; j += 10)
		{
			TextureStart("common_wall.bmp");
			glBegin(GL_QUAD_STRIP);
			glTexCoord2d(1, 0);
			glVertex3f(i+12, -36, j);
			glTexCoord2d(1, 1);
			glVertex3f(i+12, -36, j+10);
			glTexCoord2d(0, 0);
			glVertex3f(i, -36, j);
			glTexCoord2d(0, 1);
			glVertex3f(i, -36, j+10);
			glEnd();
			TextureEnd();
		}

	for (int i = 0; i > -36; i -= 12)
		for (int j = 0; j < 30; j += 10)
		{
			TextureStart("common_wall.bmp");
			glBegin(GL_QUAD_STRIP);
			glTexCoord2d(1, 0);
			glVertex3f(i - 12, -36, j);
			glTexCoord2d(1, 1);
			glVertex3f(i - 12, -36, j + 10);
			glTexCoord2d(0, 0);
			glVertex3f(i, -36, j);
			glTexCoord2d(0, 1);
			glVertex3f(i, -36, j + 10);
			glEnd();
			TextureEnd();
		}*/
	}
	glDisable(GL_BLEND);
}
