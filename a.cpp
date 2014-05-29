#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

//========определение структур============

struct Point;
struct Line;
struct Result;
typedef vector <Point> Polygon;

struct Point{
	float x;
	float y;
};

struct Line{ //ax + by + c = 0
	float a;
	float b;
	float c;
};

//=======список методов===============

int dividePolygonByCross(Polygon pol, Point* pt_cross, float* angle); //основной метод, вычисляет точку пересечения прямых pt
			// и угол поворота angle(возвращает 0 если решение не найдено)

int findCrossingPoint(Line l1, Line l2, Point* pt_cross); //нахождение точки пересечения pt_cross прямых l1 и l2
			//(возвращает 0 если прямые параллельны)
int findCrossingPoint(Line l1, Point p1, Point p2, Point* pt_cross); //нахождение точки пересечения прямой l1 и отрезка p1-p2 (возвращает 0 если они не пересекаются)

Line dividePolygonByLine(Polygon pol, float angle); //возвращает прямую, которая проходит под углом angle к оси x и делит pol пополам

float S(Polygon pol); //возвращает площадь многоугольника pol

Line getLine(Point p1, Point p2); //возвращает прямую, проходящую через p1 и p2

//=========реализация методов=========

int findCrossingPoint(Line l1, Line l2, Point& pt_cross)
{
	if ((l1.a * l2.b) == (l2.a * l1.b)) return 0; //если прямые параллельны, return 0
	pt_cross.x = ((l2.c * l1.b) - (l1.c * l2.b))/((l1.a * l2.b) - (l2.a * l1.b));
	pt_cross.y = ((l1.c * l2.a) - (l2.c * l1.a))/((l1.a * l2.b) - (l2.a * l1.b)); 
	return 1;
}

//-----------------------------------

Line getLine(Point p1, Point p2)
{
	Line l;
	l.a = p1.y - p2.y;
	l.b = p2.x - p1.x;
	l.c = (p1.x * p2.y) - (p2.x * p1.y);
	return l;
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

int dividePolygonByCross(Polygon pol, Point* pt_cross, float* angle)
{
	float s = S(pol);
	float eps = s/100;  //точность зависит от общей площади
	for (*angle = 0; *angle <= 90; *angle += 0.5)
	{
		Line l1 = dividePolygonByLine(pol, angle);
		Line l2 = dividePolygonByline(pol, angle + 90);
		Polygon pol_small;
		if (findCrossingPoint(l1, l2, pt_cross))
			pol_small.push_back(*pt_cross);
		bool flag = false, flag1 = false;
		Point* pt_cross_lp;
		for (int i = 0; !flag; i++)
		{
			if (!flag1)
				if (findCrossingPoint(l1, pol[i], pol[i+1], pt_cross_lp))
				{
					flag1 = true;
					pol_small.push_back(*pt_cross_lp);
				}
			else
			{
				pol_small.push_back(pol[i]);				
				if (findCrossingPoint(l1, pol[i], pol[i+1], pt_cross_lp))
				{
					flag = true;
					pol_small.push_back(*pt_cross_lp);
				}
			}	
		}
		if (abs(s/4 - S(pol_small)) <= eps)
			return 1;
	}
	return 0;
}

//=============main==================

int main()
{
	ifstream input("input.txt");
	int n;
	input >> n;
	Polygon pol(n);
	Point buf;
	for (int i = 0; i < n; i++)
	{
		input >> buf.x;
		input >> buf.y;
		pol.push_back(buf);
	}
	input.close();
	result res;
	res = dividePolygonByCross(pol);
	ofstream output("output.txt", ios::app);
	if (res != null_res) output << n << endl << res.x << " " << res.y << "\n\n=====================\n\n";
	else output << -1;
	output << "\n\n=====================\n\n";
	ofstream.close();

}
