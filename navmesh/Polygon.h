//====================================
// brief: Polygon�࣬����Ѱ·����ε��࣬��Ҫʵ�ֶ���ε������ʷֺ�Ѱ·
// author:sunxvming@163.com
// date:  2019-11-15
//====================================

#ifndef NAVMESH_POLYGON_H
#define NAVMESH_POLYGON_H

#include<vector>
#include<unordered_map>
#include"Point.h"
#include"Triangle.h"
#include"Edge.h"
#include"Circle.h"

using namespace std;

typedef unordered_map<int, int> Hash;
#define  MAXPOINT 100000 
#define PIndex(p1, p2) (p1>p2?(p2*MAXPOINT+p1):(p1*MAXPOINT+p2))

//====================================
//�����࣬���а������ɶ���εĶ�������ͱ�����	
//====================================
class Cell {
public:
	vector<int> points;
	vector<int> edges;
};

//====================================
// ����ζ�Ӧ������ṹ�࣬���ݺ���߷ָ��Cell�����ӣ����
//====================================
class Grid {
public:
	vector<Cell> cells;   // ���������и���
	int gride;            // ���ӳ���
	double minx;
	double miny;
	double maxx;
	double maxy;
	int xnum;
	int ynum;
};

//====================================
// �߶��࣬����Ļ�ϻ�����ʱҪ�õ�
//====================================
class Line {
public:
	Point p1;
	Point p2;
	float color[3];
};

//====================================
// ������࣬�����ʷֺ�Ѱ·�߼�����Ҫʵ���ڴ˴�
//====================================
class Polygon
{
public:
	vector<Point>     points;
	vector<Triangle>  triangles;
	vector<Edge>      edges;
	Grid              grid;
private:
	inline void CreateTriangle(Hash* eindex, int p1, int p2, int p3);
	inline int CreateEdge(Hash* eindexs, int triangle, int p1, int p2);
	inline int FindDT(Grid* grid, int p1, int p2);
	// Polygon�������ʷ�
	void Delaunay();
	// ���ɶ��������
	void GenExtData();
public:
	Point GetPoint(int p);
	bool IsIntersect(Grid* grid, int edgepos, int pa1, int p1);
	bool JudgeIsVisible(int pa1, int p1, Grid* grid);
	Polygon(double* pos, int size);
	vector<Line> GetLines();
	vector<Line> GetGrideLines();
	vector<Point> Polygon::GetCenters();
	int IsFrist(int p) { return p == 4; }
	inline int Polygon::FindTriangle(Point p);
	vector<Point> Polygon::FindPath(Point from, Point to);
	virtual ~Polygon();
};

#endif // NAVMESH_POLYGON_H