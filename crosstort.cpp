/* 
Антипин Егор, antipin91@gmail.com



	Ниже вы найдете код решения(не совсем) задачи про разрезание
многоугольника двумя перпендикулярными прямыми.
	Код очень сырой, некоторые методы работают криво для 
каких-то значений(я так и не выяснил, каких именно).
	Если в во всем файле заменить "//debug <<" на "debug <<", то
в файл "debug.txt" будут выводиться некоторые полезные(и не очень)
данные о работе программы. 

*/
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace std;

//========определение структур============

struct Point;
struct Line;
typedef vector <Point> Polygon;

ofstream debug("debug.txt", ios::out | ios::trunc);
struct Point{
	double x;
	double y;
};

struct Line{ //ax + by + c = 0
	double a;
	double b;
	double c;
};

const double PI = 3.14159265;
//=======список методов===============

int dividePolygonByCross(Polygon pol, Point& pt_cross_main, double &angle); //основной метод, вычисляет точку пересечения прямых pt
// и угол поворота angle(возвращает 0 если решение не найдено)

int findCrossingPoint(Line l1, Line l2, Point& pt_cross); //нахождение точки пересечения pt_cross прямых l1 и l2
//(возвращает 0 если прямые параллельны)
int findCrossingPoint(Line l1, Point p1, Point p2, Point& pt_cross); //нахождение точки пересечения прямой l1 и отрезка p1-p2 (возвращает 0 если они не пересекаются)

Line dividePolygonByLine(Polygon pol, double angle); //возвращает прямую, которая проходит под углом angle к оси x и делит pol пополам

double S(Polygon pol); //возвращает площадь многоугольника pol

Line getLine(Point p1, Point p2); //возвращает прямую, проходящую через p1 и p2

Line getLine(double angle, Point pt_start); //возвращает прямую, проходящую под углом angle к оси x  (ax + by + c = 0;   a = tan(angle), b = -1, c = 0)

Point getPolygonStartingPoint(Polygon pol, double angle); //стартовая точка для метода dividePolygonByline

int getPolygonSlice(Polygon pol, Line l, Polygon& slice); //slice - многоугольник, полученный в результате пересечения pol и line(метод возвращает 0 если они не пересекаются

int getPolygonSlice(Polygon pol, Line l1, Line l2, Polygon& slice); //делит многоугольник на 4 части прямыми l1 и l2, slice - одна из частей

//=========реализация методов=========

void l_out(Line& l)
{
	debug<< l.a << " " << l.b << " " << l.c << endl;
}

//-----------------------------------

void pt_out(Point& pt)
{
	debug<< pt.x << " " << pt.y << endl;
}

//------------------------------------

void pol_out(Polygon& pol)
{
	for (int i = 0; i < pol.size(); i++)
		debug<< pol[i].x << ":" << pol[i].y <<"  ";
	debug<< "\n";
}

//------------------------------------

bool operator ==(Point& pt1, Point& pt2)
{
	 if ((fabs(pt1.x - pt2.x) < 0.00001) && (fabs(pt1.y - pt2.y) < 0.00001)) {return true; }
	else {return false;}
}
//----------------------------------

bool operator !=(Point& pt1, Point& pt2)
{
	if (!(pt1 == pt2)) return true;
	else return false;
}

//----------------------------------

Line getLine(Point p1, Point p2)
{
	Line l;
	l.a = p1.y - p2.y;
	l.b = p2.x - p1.x;
	l.c = (p1.x * p2.y) - (p2.x * p1.y);
	return l;
}

//------------------------------------

Line getLine(double angle, Point pt_start)
{
	//y = kx + c
	//kx - y + c = 0
	//a = k (k = tan(angle)); b = -1; c = -ax + by 
	Line l;
	if (angle != 90) //tan(90) = inf
	{
		l.a = tan(angle * PI / 180);
		l.b = -1;
	}
	else //если angle = 90, прямая параллельна оси y
	{
		l.a = 1;
		l.b = 0;
	}
	l.c = -(l.a * pt_start.x) - (l.b * pt_start.y);
	return l;
}

//------------------------------------

double S(Polygon pol)
{
	if (pol.size())
		 pol.push_back(pol.front());
	else 
	{
		//debug "empty polygon -> S=0\n";
		return 0;
	}
	double s = 0;
	for (int i = 0; i < pol.size() - 1; i++)
	{
		s += (pol[i].x + pol[i+1].x) * (pol[i].y - pol[i+1].y);
	}
	return fabs(s/2);	
}

//------------------------------------

int findCrossingPoint(Line l1, Line l2, Point& pt_cross)
{
	if (fabs((l1.a * l2.b) - (l2.a * l1.b)) < 0.0000001) return 0; //если прямые параллельны или совпадают, return 0
	pt_cross.x = ((l2.c * l1.b) - (l1.c * l2.b))/((l1.a * l2.b) - (l2.a * l1.b));
	pt_cross.y = ((l1.c * l2.a) - (l2.c * l1.a))/((l1.a * l2.b) - (l2.a * l1.b)); 
	if (l1.a == 0) pt_cross.y = -l1.c/l1.b;
	if (l2.a == 0) pt_cross.y = -l2.c/l2.b;
	if (l1.b == 0) pt_cross.x = -l1.c/l1.a;
	if (l2.b == 0) pt_cross.x = -l2.c/l2.a;
	return 1;
}

//------------------------------------

int findCrossingPoint(Line l1, Point p1, Point p2, Point& pt_cross)
{
	Line l2 = getLine(p1, p2);
	if (findCrossingPoint(l1, l2, pt_cross))
	{
		//debug "\t\tfindCrossingPoint(l, pt1, pt2, pt_cross):\n";
		//debug "\t\tline1: "; l_out(l1);
		//debug "\t\tline2: "; l_out(l2);
		//debug "\t\tp1 "; pt_out(p1);
		//debug "\t\tp2 "; pt_out(p2);
		//debug "\t\tpt_cross_l1_l2: "; pt_out(pt_cross);
		if ((pt_cross == p1) || (pt_cross == p2)) {
			//debug "\t\tpt_cross == p1 || p2 : "; pt_out(pt_cross);
			return 1;
		}
		if ( 	(((pt_cross.x <= p1.x) && (pt_cross.x >= p2.x)) ||
			 		((pt_cross.x >= p1.x) && (pt_cross.x <= p2.x))) && 
			 	(((pt_cross.y <= p1.y) && (pt_cross.y >= p2.y)) ||
				 	((pt_cross.y >= p1.y) && (pt_cross.y <= p2.y))) 	)
		{
			//debug "\t\tgood pt_cross: "; pt_out(pt_cross);
			return 1;
		}
		else 
		{
			//debug "\t\tbad fcp:\n";
			//debug "\t\tbad pt_cross: "; pt_out(pt_cross);
			return 0;
		}
	}
	else return 0;
}

//------------------------------------

Point getPolygonStartingPoint(Polygon pol)
{
	//Возвращает стартовую точку для dividePolygonByline - max_x:min_y
	//(если угол наклона прямой больше 90 градусов - min_x:min_y)
	Point pt_start = {0, 0};
	for (int i = 0; i < pol.size(); i++)
	{
		pt_start.x += pol[i].x;
		pt_start.y += pol[i].y;	
	}
	pt_start.x = pt_start.x / pol.size();
	pt_start.y = pt_start.y / pol.size();
/*	if (angle <= 90)
	{
		for (int i = 0; i < pol.size(); i++)
		{
			if (pol[i].x > pt_start.x)
				pt_start.x = pol[i].x;
			if (pol[i].y < pt_start.y)
				pt_start.y = pol[i].y;
		}
	}
	else 
	{
		for (int i = 0; i < pol.size(); i++)
		{
			if (pol[i].x < pt_start.x)
				pt_start.x = pol[i].x;
			if (pol[i].y < pt_start.y)
				pt_start.y = pol[i].y;
		}
	}*/
	return pt_start;
}

//--------------------------------------

int getPolygonSlice(Polygon pol, Line l, Polygon& slice)
{
	//ищем точки пересечения pol и line
	//1я точка - начало slice, 2я - конец
	Point pt_cross_lp;
	pol.push_back(pol.front());//это нужно чтобы пройти последний отрезок
	bool flag_begin_pol = false, flag_end_pol = false; 
	for (	int i = 0								;
		 	!flag_end_pol && (i < pol.size() - 1)	;
		 	i++										)
	{
		if (!flag_begin_pol) //ищем 1ю точку
		{
			//debug "i: " << i <<";   pol[i]:  "; pt_out(pol[i]);
			if (findCrossingPoint(l, pol[i], pol[i+1], pt_cross_lp))
			{				
				flag_begin_pol = true;
				slice.push_back(pt_cross_lp);
				//debug "started slice with: ";  pt_out(pt_cross_lp);
			}
		}
		//как только нашли 1ю точку, заносим вершины pol
		// в slice в порядке обхода, пока не найдем 2ю
		else 
		{
			//debug "\tentered else\n";
			if (slice.back() != pol[i])
			{
				slice.push_back(pol[i]);
				//debug "added: "; pt_out(pol[i]);
			}
			if (findCrossingPoint(l, pol[i], pol[i+1], pt_cross_lp))
			{
				if (slice.back() != pt_cross_lp)
				{
					flag_end_pol = true;
					slice.push_back(pt_cross_lp);
					//debug "finished slice with: ";  pt_out(pt_cross_lp);
					return 1;
				}
			}
		}	
	}
	//debug"\tgetPolygonSlice(pol, l, &pol): bad slice\n";
	return 0;
}

//--------------------------------------

int getPolygonSlice(Polygon pol, Line l1, Line l2, Polygon& slice)
{
	//обход начинается на 1й найденной точке пересечения для l1
	//и заканчивается 1й найденной после этого точкой пересечения l2
	//в итоге получаем координаты одной из четырех частей pol
	Point pt_cross_lp;
	if (findCrossingPoint(l1, l2, pt_cross_lp)){
		slice.push_back(pt_cross_lp);
	}
	else return 0; 
	pol.push_back(pol.front());
	bool flag_begin_pol = false, flag_end_pol = false; 
	for (	int i = 0								;
		 	!flag_end_pol && (i < pol.size() - 1)	;
		 	i++										)
	{
		if (!flag_begin_pol)
		{
			if (findCrossingPoint(l1, pol[i], pol[i+1], pt_cross_lp))
			{
				flag_begin_pol = true;
				slice.push_back(pt_cross_lp);
				//если обе прямые пересекают одну сторону
				if (findCrossingPoint(l2, pol[i], pol[i+1], pt_cross_lp)) 
				{
					flag_end_pol = true;
					slice.push_back(pt_cross_lp);
					return 1;
				}
			}
			
		}
		else
		{
			slice.push_back(pol[i]);				
			if (findCrossingPoint(l2, pol[i], pol[i+1], pt_cross_lp))
			{
				flag_end_pol = true;
				slice.push_back(pt_cross_lp);
				return 1;
			}
		}	
	}
	slice.clear();
	return 0;
}

//--------------------------------------


Line dividePolygonByLine(Polygon pol, double angle)
{
	Line l = getLine(angle, getPolygonStartingPoint(pol)); //здесь l.c всегда такое, что l проходит ниже pol
	double c_start = l.c;
	//debug "line_start: "; l_out(l);
	double eps = S(pol)/1000;
	Polygon slice;
	bool flag = false;
	//двигаем l (l.c += step), 
	//пока не получим fabs(S(slice) - s/2) <= eps
	double s = S(pol);
	double step;
	if ((angle > 45) && (angle < 135)) step = l.a;
	else step = 1;
	for (double s_small = s, delta_prev = s	;
			fabs(step) >= 0.0000001		;
					  l.c += step		)
	{
		slice.clear();
		//debug "=================\n";
		if (getPolygonSlice(pol, l, slice))
		{	
			s_small = S(slice);
			flag = true;
			if ( fabs(s_small - (s/2)) <= eps )
			{
				//debug "good line: "; l_out(l); 
				//debug "good slice: "; pol_out(slice);
				return l;
			}
			else
			{
				//debug "line: "; l_out(l);
				//debug "slice: "; pol_out(slice);
				if (fabs(s_small - (s/2)) > delta_prev)
				{
					//debug"d_prev: " << delta_prev << "  step: " << step << endl;	
					step = -step/2;
				}
				delta_prev = fabs(s_small - (s/2));
			}
		}
		else
		{
			l.c = c_start;
			step = step/2;
		/*	if (flag)
			{
				//debug"bad step: "<< step <<"\t";
				//debug"bad line: "; l_out(l);
				//debug"bad slice: "; pol_out(slice);
				return l;
			}*/
		}
	}
}


//------------------------------------

int dividePolygonByCross(Polygon pol, Point& pt_cross_main, double &angle)
{
	double s = S(pol);
	double eps = s/1000;  //т.к. в методе dividePolygonByLine достаточно высокая точность(eps = s/1000), здесь можно взять значение побольше
	Polygon slice;
	//поворачиваем крест (l1, l2) , пока не получим 
	//S(slice) ~~ s/4   (fabs(s/4 - S(slice)) <= eps)
	//Достаточно пройти 90 градусов
	//Если решение не найдено, return 0
	for (angle = 0; angle <= 89.9; angle += 0.1 )
	{
		//debug "=====================" << endl;
		//debug "angle: " << angle << "\n";
		//debug "-------line1---------" << endl;
		Line l1 = dividePolygonByLine(pol, angle);
		//debug "-------line2---------" << endl;
		Line l2 = dividePolygonByLine(pol, angle + 90);
		//debug "---------------------" << endl;
		if (findCrossingPoint(l1, l2, pt_cross_main))
		{
			if (getPolygonSlice(pol, l1, l2, slice))
			{
				if (fabs(s/4 - S(slice)) <= eps)
				{
					//debug "S(slice): " << S(slice) << "  S(pol): "<< S(pol) << endl;
					//debug "slice_cross: "; pol_out(slice);
					//debug "#####################\n";	
					return 1;
				}
			}	
			//debug "S(slice): " << S(slice) << "  S(pol): "<< S(pol) << endl;
			//debug "slice_cross: "; pol_out(slice);
			slice.clear();
		}
	}
	return 0;
}




int main()
{
	ifstream input("input.txt");
	int n;
	input >> n;
	//debug "n = " << n << endl;
	Polygon pol;
	pol.reserve(n+1);
	Point buf;
	for (int i = 0; i < n; i++)
	{
		input >> buf.x;
		input >> buf.y;
		pol.push_back(buf);
	}
	input.close();
	//debug "\n\n";
	Point pt_cross_main = {0, 0};
	double angle_main = 0;
	//debug "pol: "; pol_out(pol); //debug "\n\n\n";
	/*Line l_test = dividePolygonByLine(pol, angle_main + 90);*/
	ofstream output("output.txt", ios::out|ios::trunc);
	if (dividePolygonByCross(pol, pt_cross_main, angle_main))
	{	
		output << pt_cross_main.x << " " <<  pt_cross_main.y 
				<< endl << angle_main << endl; 
		//debug "Result:\n" << "cross point: "; pt_out(pt_cross_main);
		//debug "angle: " << angle_main << endl;
	}
	else 
	{
		//debug "Result: -1\n";
		output << "-1\n";
	}
	output.close();
	debug.close();
	return 0;
}
