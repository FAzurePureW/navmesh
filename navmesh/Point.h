//====================================
// brief: Point�࣬��ı�ʾ��
// author:sunxvming@163.com
// date:  2019-11-15
//====================================

#ifndef NAVMESH_POINT_H
#define NAVMESH_POINT_H

class Point
{
public:
	double x;
	double y;
public:
	Point(double x,double y);
	Point() { this->x = 0; this -> y = 0; }
	Point(const Point& p) { this->x = p.x; this->y = p.y; }
	double Dot(Point p){ return this->x*p.x+this->y*p.y; };
	bool operator == (const Point p) { return x == p.x&&y == p.y; };//����ط�������==����Ϊû�о��ȶ�ʧ
	Point operator - (const Point p) { return Point(this->x-p.x, this->y-p.y); };
	~Point();
};


#endif // NAVMESH_POINT_H
