#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace std;

//========определение структур============

struct Point;
struct Line;
typedef vector <Point> Polygon;

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

int getPolygonSlice(Polygon pol, Polygon& pol_small, Line l); //pol_small - многоугольник, полученный в результате пересечения pol и line(метод возвращает 0 если они не пересекаются

int getPolygonSlice(Polygon pol, Polygon& pol_small, Line l1, Line l2); //делит многоугольник на 4 части прямыми l1 и l2, pol_small - одна из частей

//=========реализация методов=========

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
	pol.push_back(pol.front());
	double s = 0;
	for (int i = 0; i < pol.size() - 1; i++)
	{
		s += (pol[i].x + pol[i+1].x) * (pol[i].y - pol[i+1].y);
//		cout << i <<")  S_in = " << s << endl;
	}
	return fabs(s/2);	
}

//------------------------------------

int findCrossingPoint(Line l1, Line l2, Point& pt_cross)
{
	if ((l1.a * l2.b) == (l2.a * l1.b)) return 0; //если прямые параллельны или совпадают, return 0
	pt_cross.x = ((l2.c * l1.b) - (l1.c * l2.b))/((l1.a * l2.b) - (l2.a * l1.b));
	pt_cross.y = ((l1.c * l2.a) - (l2.c * l1.a))/((l1.a * l2.b) - (l2.a * l1.b)); 
	return 1;
}

//------------------------------------

int findCrossingPoint(Line l1, Point p1, Point p2, Point& pt_cross)
{
	if (findCrossingPoint(l1, getLine(p1, p2), pt_cross))
	{
		if ( 	(((pt_cross.x <= p1.x) && (pt_cross.x >= p2.x)) ||
			 		((pt_cross.x >= p1.x) && (pt_cross.x <= p2.x))) && 
			 	(((pt_cross.y <= p1.y) && (pt_cross.y >= p2.y)) ||
				 	((pt_cross.y >= p1.y) && (pt_cross.y <= p2.y))) 	)
			return 1;
		else return 0;
	}
	else return 0;
}

//------------------------------------

Point getPolygonStartingPoint(Polygon pol, double angle)
{
	//Возвращает стартовую точку для dividePolygonByline - max_x:min_y
	//(если угол наклона прямой больше 90 градусов - min_x:min_y)
	Point pt_start = pol.front();
	if (angle <= 90)
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
	}
	return pt_start;
}

//--------------------------------------

int getPolygonSlice(Polygon pol, Line l, Polygon& pol_small)
{
	//ищем точки пересечения pol и line
	//1я точка - начало pol_small, 2я - конец
	Point pt_cross_lp;
	pol.push_back(pol.front());//это нужно чтобы пройти последний отрезок
	bool flag_begin_pol = false, flag_end_pol = false; 
	for (	int i = 0								;
		 	!flag_end_pol && (i < pol.size() - 1)	;
		 	i++										)
	{
		if (!flag_begin_pol) //ищем 1ю точку
		{
			if (findCrossingPoint(l, pol[i], pol[i+1], pt_cross_lp))
			{
				flag_begin_pol = true;
				pol_small.push_back(pt_cross_lp);
			}
		}
		//как только нашли 1ю точку, заносим вершины pol
		// в pol_small в порядке обхода, пока не найдем 2ю
		else 
		{
			pol_small.push_back(pol[i]);				
			if (findCrossingPoint(l, pol[i], pol[i+1], pt_cross_lp))
			{
				flag_end_pol = true;
				pol_small.push_back(pt_cross_lp);
				return 1;
			}
		}	
	}
	pol_small.clear();
	return 0;
}

//--------------------------------------

int getPolygonSlice(Polygon pol, Line l1, Line l2, Polygon& pol_small)
{
	//обход начинается на 1й найденной точке пересечения для l1
	//и заканчивается 1й найденной после этого точкой пересечения l2
	//в итоге получаем координаты одной из четырех частей pol
	Point pt_cross_lp;
	if (findCrossingPoint(l1, l2, pt_cross_lp))
		pol_small.push_back(pt_cross_lp);
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
				pol_small.push_back(pt_cross_lp);
			}
		}
		else
		{
			pol_small.push_back(pol[i]);				
			if (findCrossingPoint(l2, pol[i], pol[i+1], pt_cross_lp))
			{
				flag_end_pol = true;
				pol_small.push_back(pt_cross_lp);
				return 1;
			}
		}	
	}
	pol_small.clear();
	return 0;
}

//--------------------------------------


Line dividePolygonByLine(Polygon pol, double angle)
{
	Line l = getLine(angle, getPolygonStartingPoint(pol, angle)); //здесь l.c всегда такое, что l проходит ниже pol
	double c_start = l.c;
	double eps = S(pol)/1000; 
	Polygon pol_small;
	bool flag = false;
	//двигаем l вверх (lc += step), 
	//пока не получим fabs(S(pol_small) - s/2) <= eps
	for (double s = S(pol), s_small = S(pol), delta_prev = S(pol), step = 0.1;
								fabs(step) >= 0.0000001;
											  l.c += step)
	{
		pol_small.clear();
		if (getPolygonSlice(pol, l, pol_small))
		{	
			s_small = S(pol_small);
			if ( fabs(s_small - s/2) <= eps )
			{
				cout << "line: " << l.a << " " << l.b << " " << l.c << endl;
				return l;
			}
			else
			{
				if (fabs(s_small - s/2) > delta_prev)
				{
					step = -(step/2);
					cout <<"d_prev: " << delta_prev << "  step: " << step <<endl;	
				}
				delta_prev = fabs(s_small - s/2);
			}
					
		}
/*		else
		{
			//если после 1го прохода решение не найдено, уменьшаем шаг
			if (flag)
			{
				if (step <= 0.0001) { cout << "eps: " <<
					eps << "new eps: "; cout << (eps = eps*1.1) << endl;}
				else {if (step > 0.0001) step = step/2;}
			//	cout <<"step check: " << step << endl;
				l.c = c_start;
				flag = false;
			}
		}*/
	}
	return l;
}

//------------------------------------

int dividePolygonByCross(Polygon pol, Point& pt_cross_main, double &angle)
{
	double s = S(pol);
	double eps = s/100;  //т.к. в методе dividePolygonByLine достаточно высокая точность(eps = s/1000), здесь можно взять значение побольше
	Polygon pol_small;
	//поворачиваем крест (l1, l2) , пока не получим 
	//S(pol_small) ~~ s/4   (fabs(s/4 - S(pol_small)) <= eps)
	//Достаточно пройти 90 градусов
	//Если решение не найдено, return 0
	for (angle = 0; angle < 90; angle += 0.1)
	{
		cout << "angle: " << angle << "\t";
		Line l1 = dividePolygonByLine(pol, angle);
		Line l2 = dividePolygonByLine(pol, angle + 90);
		cout << "=====================" << endl;
		if (findCrossingPoint(l1, l2, pt_cross_main))
		{
			if (getPolygonSlice(pol, l1, l2, pol_small))
			{
				if (fabs(s/4 - S(pol_small)) <= eps)
				{
					cout << "S: " << S(pol_small) << " "<< S(pol) << endl;
					cout << "pol_small: "  << endl;
					for (int i = 0; i < pol_small.size(); i++)
						cout << pol_small[i].x << ":" << pol_small[i].y << "  ";
					cout << endl;
					return 1;
				}
			}
			cout << "S: " << S(pol_small) << endl;
			pol_small.clear();
		}
	}
	return 0;
}




int main()
{
/*	cout << "\n\ngetLine\n";
	Point p1 = {2, 3};
	Point p2 = {3, 5};
	Line l = getLine(p1, p2);
	cout << l.a << " " << l.b << " " << l.c << endl;
	cout << "\n\nfindCrossingPoint1\n";
	Line l1 = {1, -1, 1};
	Line l2 = {1, 1, 0};
	Point pt_cross;
	if (findCrossingPoint(l1, l2, pt_cross))
		cout << pt_cross.x << " " << pt_cross.y << endl;
	cout << "\n\nfindCrossingPoint\n";
	p1 = {1, 0};
	p2 = {1, 4};
	if (findCrossingPoint(l, p1, p2, pt_cross))
		cout << pt_cross.x << " " << pt_cross.y << endl;
	cout << "\n\nSSSS\n";
*/	ifstream input("input.txt");
	int n;
	input >> n;
	cout << "n = " << n << endl;
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
	cout << "\n\n";
	Point pt_cross_main = {0, 0};
	double angle_main = 0;
	cout << "---dividePolygonByCross---\n";
	cout << "pol: ";
	for (int i = 0; i < pol.size(); i++)
		cout << pol[i].x << " " << pol[i].y << "\n";
	double s = S(pol);	
	cout << "S: " << s << endl;
	if (dividePolygonByCross(pol, pt_cross_main, angle_main))
		cout << "Result:\n"
		<< "cross point: "<<pt_cross_main.x << " " << pt_cross_main.y << endl << "angle: " << angle_main << "\n\n\n\n\n";
	else cout << "Result: -1\n\n\n\n";

/*	cout << "size: " << pol.size() << endl;
	cout << "vector: "; 
	for (int i = 0; i < pol.size(); i++)
		cout << pol[i].x << " " << pol[i].y << " ";
	cout << endl;
	cout << "\n\ngetLine(angle, pt_start)\n";
	double angle = 80;
	Point pt_start = getPolygonStartingPoint(pol);
	cout <<"Starting point: " << pt_start.x << " " << pt_start.y << endl;
	Line l5 = getLine(angle, pt_start);
	cout << "line: " << l5.a << " " << l5.b << " " << l5.c << "\n\n";
	cout << "---getPolygonSlice---\n";
	l1 = dividePolygonByLine(pol, angle);
	l2 = dividePolygonByLine(pol, angle + 90);
	cout << "line1: " << l1.a << " " << l1.b << " " << l1.c << "\n";
	cout << "line2: " << l2.a << " " << l2.b << " " << l2.c << "\n";
	Polygon pol_small;
	if (getPolygonSlice(pol, l1, l2, pol_small))
	{
		cout << "Slice: ";
		for (int i = 0; i < pol_small.size(); i++)
			cout << pol_small[i].x << " " << pol_small[i].y << " ";
	}
	else cout << "rip in sliced peperonis";
	cout << "\n\n";
	Point pt_cross_main = {0, 0};
	double angle_main = 0;
	cout << "---dividePolygonByCross---\n";
	cout << "pol: ";
	for (int i = 0; i < pol.size(); i++)
		cout << pol[i].x << " " << pol[i].y << "\n";
	if (dividePolygonByCross(pol, pt_cross_main, angle_main))
		cout << "Result:\n"
		<< "cross point: "<<pt_cross_main.x << " " << pt_cross_main.y << endl << "angle: " << angle_main << "\n\n\n\n\n";
	else cout << "Result: -1\n\n\n\n";	
*/	return 0;
}
