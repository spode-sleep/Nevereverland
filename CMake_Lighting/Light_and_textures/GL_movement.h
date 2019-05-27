#ifndef GL_movement
#define GL_movement

// меня замучали предупреждения о преобразованиях double во float так что будем игнорировать эти предупреждения
#pragma warning(disable : 4244)     // MIPS

extern GLfloat alX, alY; // угол поворота
extern int lx0,lx1,ly0,ly1,rx0,rx1,ry0,ry1,pzoom,dveX0,dveY0,dveX1,dveY1;
extern  int view,mouse_mode;
extern bool m2,m1;
extern GLdouble w,h,px,py;

class MyPoint{
public:
float x,y,z;

MyPoint ()
{x=0;
 y=0;
 z=0;
}


MyPoint (GLdouble XX,GLdouble YY,GLdouble ZZ)
{x=XX;
 y=YY;
 z=ZZ;
}

MyPoint(const MyPoint &ss) // конструктор копии
{x=ss.x;
 y=ss.y;
 z=ss.z;
}

MyPoint & operator =(const MyPoint  ss) //когда нельзя взять ссылку (напр от результитов выражений)
{x=ss.x;
 y=ss.y;
 z=ss.z;
 return *this;
}

MyPoint & Set(float XX, float YY, float ZZ)
{x=XX;
 y=YY;
 z=ZZ;
 return *this;
}

double  Dlina()                      // модуль вектора
{try{return sqrt(x*x+y*y+z*z);}
 catch(...)
 {}
}

MyPoint  operator+( const MyPoint & ss)     // сумма векторов
{return MyPoint(x+ss.x,y+ss.y,z+ss.z);
}

MyPoint  operator-( const MyPoint & ss)    // разность векторов
{return MyPoint(x-ss.x,y-ss.y,z-ss.z);
}

double  operator*( MyPoint & s) //скалярное произведение
{return (x*s.x+y*s.y+z*s.z);
}

MyPoint  operator *=(MyPoint &b) //векторное произведение
{return MyPoint(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);
}

MyPoint  operator*( float  s)  // умножение на константу
{MyPoint b(*this);
 b.x=x*s;
 b.y=y*s;
 b.z=z*s;
 return b;
}

MyPoint  operator/( float  s)  // деление на константу
{MyPoint b(*this);
 b.x=x/s;
 b.y=y/s;
 b.z=z/s;
 return b;

}

MyPoint  Normalize_Self()     // нормализовать себя и  вернуть нормализованный вектор
{try{
float sq=sqrt(x*x+y*y+z*z);
 x=x/sq;
 y=y/sq;
 z=z/sq;
 return *this;
 }
 catch(...)
 {}
}

MyPoint  Normalize()         // вернуть нормализованный вектор
{float a=this->Dlina();
 return MyPoint(x/a,y/a,z/a);
}

double Get_cos(MyPoint &ss)
{double aa=(*this)*ss;
 aa=aa/(this->Dlina()*ss.Dlina());
 return aa;
}

double Get_sin(MyPoint &ss)
{double aa= ((*this)*=ss).Dlina();
 aa=aa/(this->Dlina()*ss.Dlina());
 return aa;
}

 operator float * ()
{return &(this->x);
}

};


extern MyPoint e;
extern MyPoint c;
extern MyPoint u;


void strate(float vpered,float vlevo)
{
// перемешение вдоль прямой взгляда
float a,b; //перемешения в проскостти YOX
float L;   // длина перемещения в проскости XOY
float Lmov;  // длина перемещения в пространстве
float R3d;	 // длина вектора просмотра
float fi;    // угол в проскости XOY
float fi3d;  // наклон к плоскости XOY
Lmov=vpered;
R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
fi3d=asin((c.z-e.z) /R3d);
L=Lmov*cos(fi3d);

fi=atan((c.y-e.y)/(c.x-e.x));
if ((c.x-e.x)<0)
 fi+=(float)3.141592;

a=L*cos(fi);
b=L*sin(fi);

e.x+=a;
e.y+=b;
c.x+=a;
c.y+=b;

e.z+=Lmov*sin(fi3d);
c.z+=Lmov*sin(fi3d);
//printf("R3d=%3.3f  fi= %.f  fi3d= %f  L3d=%f\n",R3d,fi*180/3.141592,fi3d*180/3.141592,Lmov);
//printf("e %.0f %.0f %.0f   c %.0f %.0f %.0f  \n",e.x,e.y, e.z, c.x,c.y,c.z);

//перемешение в бок (стрейф)
Lmov=vlevo;
float fi_perp;  //  угл наклона прямой перпендикулярной взгляду(в плоскости XOY)
fi_perp=atan(-1/tan(fi));


	
a=Lmov*cos(fi_perp);
b=Lmov*sin(fi_perp);

if ((e.y-c.y)>0) 
{
e.x+=a;
e.y+=b;
c.x+=a;
c.y+=b;
}
else
{
e.x-=a;
e.y-=b;
c.x-=a;
c.y-=b;
}

glLoadIdentity();
gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );

}

void look_around(float left,float up)
{//рысканье в 3Д по оси Х- как оси наблюдений
	if(view==2)
	{
		static float R; 
		R =sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y));
		float vr01,vr1;    //фи0 - угол по которому смотрит воктор С-E
			if ((c.x-e.x)>=0) //в первой и четвертой четверти находится проэкция вектора наблюдения
		  vr01=atan((c.y-e.y)/(c.x-e.x));  
		else             // во 2-й и 3-й четверти к арктангенсу добавить Пи
		  vr01=(float)3.141592+atan((c.y-e.y)/(c.x-e.x));

		vr1 = left/R;  // относительный поворот, остносительно фи0, те относительно vr01
		//printf("\n   R=%3.3f  vr1= %.f  vr01= %f  rx1=%d\n",R,vr1*180/3.141592,vr01*180/3.141592,left);
	   	
		c.x=e.x+R*cos(vr01+vr1); //
		c.y=e.y+R*sin(vr01+vr1);
		

		//расчет рысканья завершен
		//расчет крена в 3Д по оси Y - как оси наблюдений

		static double R3d;
		GLdouble	vr02,vr2;	//фи0 - угол по которому смотрит воктор С-E по оси Z
		R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
		vr02=asin((c.z-e.z)/R3d);
		vr2=up/R3d;
		c.z=e.z+R3d*sin(vr02+vr2);
	}
	else if(view==1)
	{
		 static double R; 
		R =sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y));
		GLdouble vr01,vr1;    //фи0 - угол по которому смотрит воктор С-E
		if ((c.x-e.x)>=0) //в первой и четвертой четверти находится проэкция вектора наблюдения
		  vr01=atan((c.y-e.y)/(c.x-e.x));  
		else             // во 2-й и 3-й четверти к арктангенсу добавить Пи
		  vr01=3.141592+atan((c.y-e.y)/(c.x-e.x));

		vr1 = -left/R;  // относительный поворот, остносительно фи0, те относительно vr01
		//printf("\n   R=%3.3f  vr1= %.f  vr01= %f  rx1=%d\n",R,vr1*180/3.141592,vr01*180/3.141592,left);
	   	
		e.x=c.x-R*cos(vr01+vr1); //
		e.y=c.y-R*sin(vr01+vr1);
		

		//расчет рысканья завершен
		//расчет крена в 3Д по оси Y - как оси наблюдений

		static double R3d;
		GLdouble	vr02,vr2;	//фи0 - угол по которому смотрит воктор С-E по оси Z
		R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
		vr02=asin((c.z-e.z)/R3d);
		vr2=up/R3d;
		c.z=e.z+R3d*sin(vr02+vr2);

	}
	//printf("R3d=%3.3f  vr2= %.f  vr02= %f  ry1=%d\n",R3d,vr2*180/3.141592,vr02*180/3.141592,up);
 //	printf("e %.0f %.0f %.0f   c %.0f %.0f %.0f  \n",e.x,e.y, e.z, c.x,c.y,c.z);
    //printf("CPoint %.0f %.0f %.0f\n",(c.x-e.x)/R3d,(c.y-e.y)/R3d,(c.z-e.z)/R3d);
   // glLoadIdentity();
//gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );


    glLoadIdentity();
gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );
    }

void jamp(float up, float left)
{
if(view==1)
	{
		left=left*(double)px/w;
		up=up*(double)py/h;
	} 
else
{up=up/10.0;
 left=left/10.0;
}
	
strate(0,left); // сместились в бок

float	vr02,R3d;	//фи0 - угол по которому смотрит воктор С-E по оси Z
R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
vr02=asin(fabs(c.z-e.z)/R3d);

c.z-=up/cos(vr02);
e.z-=up/cos(vr02);
glLoadIdentity();

gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );
}

void inline zoom(int zoom)
{
 // выбор матрицы проэцирования
 glMatrixMode( GL_PROJECTION );
 // загрузка единичной матрицы
 glLoadIdentity();
 // изменение переменной  pzoom, она хранит угол зрения
 pzoom+=zoom;

 if (pzoom>200)
  pzoom=200;
if (pzoom<5)
  pzoom=5;
   if(w>=h) {px=w/h*pzoom; py=pzoom;}
   else     {py=h/w*pzoom; px=pzoom;}
   if (view==1)
 // использование ортогональной проэкции
 // где -px,px, -py,py определяют соотвветственно
 // левую, правую, нижную, и верхнюю отсекающую плоскости
 // а значения -10000 и 10000 - ближнюю и дальнюю отсекающую плоскости
   glOrtho(-px/2,px/2, -py/2,py/2, -10000, 10000);
   else
    // использование перспективной проэкции
  // pzoom - угол зрения, w/h - соотношение горизонтали и вертикали окна обзора
  // 2 - ближняя плоскость отсечения (всегда положительна)
  // 10000 - дальняя плоскость отсечения (всегда положительна)
   gluPerspective (pzoom,w/h,2,100);

 // для дальнейшей работы - вернемся к матрице модельного представления
 glMatrixMode(GL_MODELVIEW);

 // загр единичная матрица
    glLoadIdentity();
    return ;
}

#endif
 