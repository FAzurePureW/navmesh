//====================================
// brief: Edge�࣬��ʾ�����ʷֺ���߶Σ�һ���߶ο��ܰ���һ�����������ǣ�����������
// author:sunxvming@163.com
// date:  2019-11-15
//====================================

#ifndef NAVMESH_EDGE_H
#define NAVMESH_EDGE_H

class Polygon;
class Edge
{
public:
	int triangles[2];
	int points[2];
	Edge(int t1, int t2, int p1, int p2);
	int IsRestrain(Polygon* p);
	virtual ~Edge();
};

#endif // NAVMESH_EDGE_H
