//====================================
// brief: Polygon�࣬����Ѱ·����ε��࣬��Ҫʵ�ֶ���ε������ʷֺ�Ѱ·
// author:sunxvming@163.com
// date:  2019-11-15
//====================================

#include "Polygon.h"

Polygon::Polygon(double* pos, int size)
{
	_ASSERT(size > 10);
	_ASSERT(size < MAXPOINT * 2 - 8);
	for (int ii = 0; ii < size; ii += 2)
	{
		points.push_back(Point(pos[ii], pos[ii + 1]));
	}
	//DT���ǻ�
	Delaunay();
	GenExtData();
}

void Polygon::CreateTriangle(Hash* eindexs, int p1, int p2, int p3)
{
	triangles.push_back(Triangle(p1, p2, p3));
	int triangle = triangles.size() - 1;
	triangles[triangle].edges[0] = CreateEdge(eindexs, triangle, p1, p2);  //���˳��Ҫ���ַ��������ʱ�룬�ڲ�˳ʱ��
	triangles[triangle].edges[1] = CreateEdge(eindexs, triangle, p3, p2);
	triangles[triangle].edges[2] = CreateEdge(eindexs, triangle, p1, p3);
}

int Polygon::CreateEdge(Hash* eindexs, int triangle, int p1, int p2)
{
	int k = PIndex(p1, p2);
	if (eindexs->find(k) == eindexs->end())
	{
		int v = edges.size();
		edges.push_back(Edge(triangle, -1, p1, p2));
		eindexs->insert(make_pair(k, v));
		return v;
	}
	int v = (*eindexs)[k];
	int t2 = edges[v].triangles[1];
	_ASSERT(t2 < 0);
	edges[v].triangles[1] = triangle;
	return v;
}


//��� ac��bc 
double mult(Point a, Point b, Point c)
{
	return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}

#define max( a, b ) ((a)>(b)?(a):(b))
#define min( a, b ) ((a)>(b)?(b):(a))
//aa, bbΪһ���߶����˵� cc, ddΪ��һ���߶ε����˵� �ཻ����true, ���ཻ����false  
bool intersect(Point aa, Point bb, Point cc, Point dd)
{
	if (max(aa.x, bb.x) < min(cc.x, dd.x))
	{
		return false;
	}
	if (max(aa.y, bb.y) < min(cc.y, dd.y))
	{
		return false;
	}
	if (max(cc.x, dd.x) < min(aa.x, bb.x))
	{
		return false;
	}
	if (max(cc.y, dd.y) < min(aa.y, bb.y))
	{
		return false;
	}
	// cd��abͬ��
	if (mult(cc, bb, aa) * mult(dd, bb, aa) > 0)
	{
		return false;
	}
	// ab��cdͬ��
	if (mult(aa, dd, cc) * mult(bb, dd, cc) > 0)
	{
		return false;
	}
	return true;
}


//p1λ��p2���Ҳࣨλ��p2��˳ʱ�뷽��
//p2λ��p1��ࣨλ��p1����ʱ�뷽��
static inline int isclockwise(Point p1, Point p2)
{
	return p1.x * p2.y > p2.x* p1.y;
}


// ���ݵ�˹�ʽ����first-center-second�ĽǶȣ������ƣ�
static  inline double Angle(Point cen, Point first, Point second)
{
	double dx1, dx2, dy1, dy2;

	dx1 = first.x - cen.x;
	dy1 = first.y - cen.y;
	dx2 = second.x - cen.x;
	dy2 = second.y - cen.y;
	//��cen �� first �ľ��룩*��cen �� second �ľ��룩
	double c = (double)sqrt(dx1 * dx1 + dy1 * dy1) * (double)sqrt(dx2 * dx2 + dy2 * dy2);
	//���� cen��  first  ���� second  �ص�������
	if (c == 0) return 0;

	return (double)acos((dx1 * dx2 + dy1 * dy2) / c);
}

static inline int lerp(int v, int min, int max)
{
	if (v < min)
		v = min;
	if (v > max)
		v = max;
	return v;
}


int Polygon::FindDT(Grid* grid, int p1, int p2)
{
	double x1 = points[p1].x, y1 = points[p1].y;
	double x2 = points[p2].x, y2 = points[p2].y;
	double x = (x1 + x2) / 2, y = (y1 + y2) / 2;

	double minx = grid->minx, miny = grid->miny;
	double gride = grid->gride;
	int gx = (int)((x - minx) / gride);
	int gy = (int)((y - miny) / gride);
	int xnum = grid->xnum, ynum = grid->ynum;
	int d = 0;
	int p3 = -1;
	double angle3 = -1;
	Point point1 = GetPoint(p1), point2 = GetPoint(p2);
	while (1)
	{
		for (int i = -d; i <= d; i++)
		{
			for (int j = -d; j <= d; j++)
			{
				int pos = lerp(gx + i, 0, xnum - 1) + lerp(gy + j, 0, ynum - 1) * xnum;
				//printf("  Polygon::FindDT ������  ִ�е������� ---pos = %d-------\n", pos);
				if (pos >= 0 && pos < (int)grid->cells.size())
				{
					Cell c = grid->cells[pos];
					for (unsigned k = 0; k < c.points.size(); k++)
					{
						int p = c.points[k];
						Point point = GetPoint(p);
						if (p1 != p && p2 != p && isclockwise(point2 - point1, point - point1))  // p Ҫ��p1p2�����
						{
							bool flag = JudgeIsVisible(p1, p, grid) && JudgeIsVisible(p2, p, grid);

							if (flag) {
								double angle = Angle(point, point1, point2);
								if (p3 == -1 || angle > angle3)
								{
									angle3 = angle;
									p3 = p;
								}
							}
						}
					}
				}
			}
		}
		//�ж��Ƿ�Ӧ�ý�����ǰ��
		if (p3 != -1)
		{
			Circle c(point1, point2, GetPoint(p3));
			Point cc = c.GetCenter();
			double radius = c.GetR();
			double l = cc.x - radius, r = cc.x + radius, t = cc.y - radius, b = cc.y + radius;
			int lx = lerp((int)((l - minx) / gride), 0, xnum - 1);
			int rx = lerp((int)((r - minx) / gride), 0, xnum - 1);
			int ty = lerp((int)((t - miny) / gride), 0, ynum - 1);
			int by = lerp((int)((b - miny) / gride), 0, ynum - 1);
			if ((gx - d) <= lx && (gx + d) >= rx && (gy - d) <= ty && (gy + d) >= by)
				break;
		}
		d++;
	}
	_ASSERT(p3 != -1);
	return p3;
}

Point Polygon::GetPoint(int p)
{
	return points[p];
}


bool Polygon::IsIntersect(Grid* grid, int edgepos, int pa1, int p1) {
	Point pa = GetPoint(pa1);
	Point p = GetPoint(p1);

	Cell c = grid->cells[edgepos];
	for (unsigned m = 0; m < c.edges.size(); m++) {
		int eposId = c.edges[m];
		int next_eposId = (eposId + 1) % points.size();

		bool flag = intersect(GetPoint(next_eposId), GetPoint(eposId), pa, p);
		if ((eposId == pa1) || (eposId == p1) || (next_eposId == pa1) || (next_eposId == p1))
		{
			flag = false;
		}
		if (flag)
			return true;
	}
	return false;
}


//false����pa ��p���ɼ���true����ɼ�
bool Polygon::JudgeIsVisible(int pindex1, int pindex2, Grid* grid) {
	Point p1 = GetPoint(pindex1);
	Point p2 = GetPoint(pindex2);
	//��������Сֵ�Ϸ���
	Point p0 = points[0];
	double minx = p0.x, miny = p0.y, maxx = p0.x, maxy = p0.y;
	for (auto it = points.cbegin() + 1; it != points.cend(); it++)
	{
		double x = it->x;
		double y = it->y;
		if (x < minx) minx = x;
		if (x > maxx)maxx = x;
		if (y < miny) miny = y;
		if (y > maxy)maxy = y;
	}

	double dx = maxx - minx, dy = maxy - miny;
	int gride = (int)sqrt(dx * dy / (points.size()));
	int xnum = (int)ceil(dx / gride);
	int ynum = (int)ceil(dy / gride);

	if (p1.x > p2.x) {
		Point demo = p2;
		p2 = p1;
		p1 = demo;
	}
	int xn1 = (int)((p1.x - minx) / gride);
	int yn1 = (int)((p1.y - miny) / gride);
	int xn2 = (int)((p2.x - minx) / gride);
	int yn2 = (int)((p2.y - miny) / gride);

	if (xn1 == xn2) {
		if (yn1 > yn2) {
			yn1 = yn1 ^ yn2;
			yn2 = yn1 ^ yn2;
			yn1 = yn1 ^ yn2;
		}
		for (int j = yn1; j <= yn2; j++)
		{
			if (j > ynum) break;
			int edgepos = (xn1 >= xnum ? xnum - 1 : xn1) + (j >= ynum ? ynum - 1 : j) * xnum;
			if (IsIntersect(grid, edgepos, pindex1, pindex2))
				return false;
		}
	}
	else {
		double x = p1.x;
		double y = p1.y;
		for (int i = xn1; i <= xn2; i++)
		{
			double x3 = (i + 1) * gride + minx;
			if (x3 > p2.x) x3 = p2.x;
			// ������÷��� (y-y2)/(y1-y2)=(x-x2)/(x1-x2)����x3�������y3
			double y3 = (p2.x - x3) * (p2.y - p1.y) / (p1.x - p2.x) + p2.y;

			int cur_x = (int)((x - minx) / gride);
			int cur_y = (int)((y - miny) / gride);
			int next_y = (int)((y3 - miny) / gride);
			if (cur_y > next_y) {
				cur_y = cur_y ^ next_y;
				next_y = cur_y ^ next_y;
				cur_y = cur_y ^ next_y;
			}
			for (int j = cur_y; j <= next_y; j++)
			{
				if (j > ynum) break;
				int edgepos = (cur_x >= xnum ? xnum - 1 : cur_x) + (j >= ynum ? ynum - 1 : j) * xnum;
				if (IsIntersect(grid, edgepos, pindex1, pindex2))
					return false;
			}
			x = x3;
			y = y3;
		}
	}
	return true;
}

void Polygon::Delaunay()
{
	//��������Сֵ�Ϸ���
	Point p0 = points[0];
	double minx = p0.x, miny = p0.y, maxx = p0.x, maxy = p0.y;
	for (auto it = points.cbegin() + 1; it != points.cend(); it++)
	{
		double x = it->x;
		double y = it->y;
		if (x < minx) minx = x;
		if (x > maxx)maxx = x;
		if (y < miny) miny = y;
		if (y > maxy)maxy = y;
	}

	double dx = maxx - minx, dy = maxy - miny;
	//����ı߳�
	int gride = (int)sqrt(dx * dy / (points.size()));
	//X��Ҫ�ֶ��ٸ�
	int xnum = (int)ceil(dx / gride);
	//Y��Ҫ�ֶ��ٸ�
	int ynum = (int)ceil(dy / gride);
	//�����еĸ��Ӵ����
	vector<Cell> cells(xnum * ynum);
	printf("Polygon::Delaunay�����grid�ı߳��� grid.length = %d, xnum = %d,  dy= %f,  ynum = %d \n", gride, xnum, dy, ynum);

	printf("�ܹ��ж��ٸ�����    points.size() = %d, ", points.size());

	printf("---------minx = %f, miny = %f-------", minx, miny);
	//�ѵ�ͱ߶��ŵ�������
	for (auto it = points.cbegin(); it != points.cend(); it++)
	{
		//point�ŵ�Grid��
		double x = it->x;
		double y = it->y;
		int xn = (int)((x - minx) / gride);
		int yn = (int)((y - miny) / gride);
		int pos = (xn >= xnum ? xnum - 1 : xn) + (yn >= ynum ? ynum - 1 : yn) * xnum;
		int point = it - points.cbegin();
		cells[pos].points.push_back(point);

		//edges�ŵ�Grid��
		Point p = GetPoint(point);
		Point p1 = GetPoint((point + 1) % points.size());
		if (p.x > p1.x) {
			Point demo = p;
			p = p1;
			p1 = demo;
		}
		int xn1 = (int)((p.x - minx) / gride);
		int xn2 = (int)((p1.x - minx) / gride);
		int yn1 = (int)((p.y - miny) / gride);
		int yn2 = (int)((p1.y - miny) / gride);

		if (xn1 == xn2) {
			if (yn1 > yn2) {
				yn1 = yn1 ^ yn2;
				yn2 = yn1 ^ yn2;
				yn1 = yn1 ^ yn2;
			}
			for (int j = yn1; j <= yn2; j++)
			{
				if (j > ynum) break;
				int edgepos = (xn1 >= xnum ? xnum - 1 : xn1) + (j >= ynum ? ynum - 1 : j) * xnum;
				cells[edgepos].edges.push_back(point);
			}
		}
		else {
			//��xֵƫС�� yֵ��������
			double y = p.y;
			double x = p.x;
			for (int i = xn1; i <= xn2; i++)
			{
				double x3 = (i + 1) * gride + minx;
				if (x3 > p1.x) x3 = p1.x;
				// ������÷��� (y-y2)/(y1-y2)=(x-x2)/(x1-x2)����x3�������y3
				double y3 = (p.x - x3) * (p.y - p1.y) / (p1.x - p.x) + p.y;

				int cur_x = (int)((x - minx) / gride);
				int cur_y = (int)((y - miny) / gride);
				int next_y = (int)((y3 - miny) / gride);
				if (cur_y > next_y) {
					cur_y = cur_y ^ next_y;
					next_y = cur_y ^ next_y;
					cur_y = cur_y ^ next_y;
				}
				for (int j = cur_y; j <= next_y; j++)
				{
					if (j > ynum) break;
					int edgepos = (cur_x >= xnum ? xnum - 1 : cur_x) + (j >= ynum ? ynum - 1 : j) * xnum;
					cells[edgepos].edges.push_back(point);
				}
				x = x3;
				y = y3;
			}
		}
	}

	grid.cells = cells;
	grid.gride = gride;
	grid.minx = minx;
	grid.maxx = maxx;
	grid.miny = miny;
	grid.maxy = maxy;
	grid.xnum = xnum;
	grid.ynum = ynum;

	Hash eindexs;  //k->v (Pindex->edges��index)��������Ѿ�����������������εı�  
	Hash restrains;  //��������е�Լ���ߣ��ڴ���ʼ���Ѿ�֪��
	for (unsigned i = 0; i < points.size() - 1; i++)
	{
		restrains.insert(make_pair((int)PIndex(i, i + 1), 1));
	}
	restrains.insert(make_pair((int)PIndex(0, points.size() - 1), 1));
	vector<int> es;  //������ıߵ�ջ 
	int p1 = 0, p2 = 1;
	int e = PIndex(p1, p2);
	//es.push_back(e);
	while (1)
	{

		int p3 = FindDT(&grid, p1, p2);
		if (restrains.find((int)PIndex(p1, p3)) == restrains.end())
		{
			vector<int>::iterator it = std::find(es.begin(), es.end(), PIndex(p1, p3));
			if (it == es.end())
			{
				es.push_back(PIndex(p1, p3));
			}
			else {
				es.erase(it);
			}
		}
		if (restrains.find((int)PIndex(p2, p3)) == restrains.end())
		{
			vector<int>::iterator it = std::find(es.begin(), es.end(), PIndex(p2, p3));
			if (it == es.end())
			{
				es.push_back(PIndex(p2, p3));
			}
			else {
				es.erase(it);
			}
		}
		CreateTriangle(&eindexs, p1, p2, p3);
		if (es.empty())	 break;
		//�õ��µ������㣬ѭ�����㡣���esΪ�ա���˵���Ѿ������꣬����������ֹͣ
		e = *(es.end() - 1);
		es.pop_back();
		int* points = edges[eindexs[e]].points;
		p1 = points[0], p2 = points[1];
	}
}

#define distance( p1, p2 )((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))

void Polygon::GenExtData()
{
	for (unsigned i = 0; i < triangles.size(); i++)
	{
		Triangle t = triangles[i];
		t.GenExtData(this);
		triangles[i] = t;
	}

	for (unsigned i = 0; i < edges.size(); i++)
	{
		Edge e = edges[i];
		int e0 = e.triangles[0];
		int e1 = e.triangles[1];
		if (e0 >= 0 && e1 >= 0) {   //һ�������������ε�
			Triangle t0 = triangles[e0];
			Triangle t1 = triangles[e1];
			int p0 = e.points[0], p1 = e.points[1];

			//ʹ���ڵ�һ�������У���p1λ��p0����ࣨ���λ�ù̶�������p1����㣬p0���ҵ㣩��ʹѰ·ʱ�յ��㷨�����߷�����һ�µ�
			//0��λ�÷��ҵ�
			if (!isclockwise(GetPoint(p0) - t0.icenter, GetPoint(p1) - t0.icenter))
			{
				e.points[0] = p1;
				e.points[1] = p0;
				edges[i] = e;
			}
		}
	}
}

vector<Line> Polygon::GetLines() {
	vector<Line> lines(edges.size());
	for (unsigned i = 0; i < edges.size(); i++)
	{
		Line line;
		line.p1 = GetPoint(edges[i].points[0]);
		line.p2 = GetPoint(edges[i].points[1]);
		if (edges[i].IsRestrain(this))
		{
			line.color[0] = 1.0;
			line.color[1] = 0.0;
			line.color[2] = 0.0;
		}
		else {
			line.color[0] = 0.0;
			line.color[1] = 1.0;
			line.color[2] = 0.0;
		}
		lines[i] = line;
	}
	return lines;
}


vector<Line> Polygon::GetGrideLines() {
	vector<Line> lines(grid.ynum + grid.xnum + 2);
	// �������
	for (int i = 0; i <= grid.xnum; i++)
	{
		Line line;
		Point pa1, pa2;
		double x = i * grid.gride + grid.minx;
		pa1.x = x;
		pa1.y = grid.miny;

		pa2.x = x;
		pa2.y = grid.miny + grid.ynum * grid.gride;
		line.p1 = pa1;
		line.p2 = pa2;

		line.color[0] = 0.2f;
		line.color[1] = 0.3f;
		line.color[2] = 0.3f;

		lines[i] = line;
	}
	// �������
	for (int j = 0; j <= grid.ynum; j++)
	{
		Line line;
		Point pa1, pa2;
		double y = j * grid.gride + grid.miny;
		pa1.x = grid.minx;
		pa1.y = y;
		pa2.x = grid.minx + grid.xnum * grid.gride;
		pa2.y = y;
		line.p1 = pa1;
		line.p2 = pa2;

		line.color[0] = 0.2f;
		line.color[1] = 0.3f;
		line.color[2] = 0.3f;

		lines[j + grid.xnum + 1] = line;
	}
	return lines;
}


vector<Point> Polygon::GetCenters() {
	vector<Point> centers(triangles.size());
	for (unsigned i = 0; i < triangles.size(); i++)
	{
		centers[i] = triangles[i].icenter;
	}
	return centers;
}

int Polygon::FindTriangle(Point p)
{
	for (unsigned i = 0; i < triangles.size(); i++)
	{
		if (triangles[i].Contain(this, p)) return i;
	}
	return -1;
}
//��·�� from����㣬to���յ�
vector<Point> Polygon::FindPath(Point from, Point to)
{
	//�ֱ��ҵ�������ڵ������κ��յ����ڵ�������
	int tfrom = FindTriangle(from);
	int tto = FindTriangle(to);

	printf("tfrom == %d, tto == %d \n", tfrom, tto);
	vector<int> ts;				//���е�������
	vector<int> es;				//���еı�
	vector<Point> ways;
	if (tfrom < 0) return ways;
	if (tto < 0) return ways;
	ways.push_back(from);  //��ŵ�
	//����������ڵ�������  �� �յ����ڵ���������ͬһ�������Σ���ôֱ�ӻ���һ���ߣ�����ways
	if (tfrom == tto)
	{
		ways.push_back(to);
		return ways;
	}
	ts.push_back(tfrom);  //ջ�����������
	//A*  F = G() + H()  ���ε�A*���߳�Ϊ̰���㷨���������
	// G() = 0   H() = ���������������εģ�����֮��ľ���
	int start = tfrom;
	Hash visited;
	visited.insert(make_pair(tfrom, 1));

	while (!ts.empty())
	{
		start = *(ts.cend() - 1);
		double weight = -1;
		int next = -1;
		int e;
		for (int i = 0; i < 3; i++)
		{
			int eindex = triangles[start].edges[i];
			Edge edge = edges[eindex];
			//�ҵ���Լ���ߵģ���ԭ�����ε�����������������
			int nextt = edge.triangles[0] == start ? edge.triangles[1] : edge.triangles[0];
			if (nextt >= 0)  // ��Լ���ߵ�
			{
				if (nextt == tto)
				{
					next = tto;
					e = eindex;
					break;
				}
				if (visited.find(nextt) == visited.end())
				{
					Point p = triangles[nextt].icenter;
					double d = distance(p, to);
					if (next<0 || weight > d) next = nextt, weight = d, e = eindex;
				}
			}
		}
		if (next == tto)
		{
			ts.push_back(next);
			es.push_back(e);
			break;
		}
		else if (next >= 0)
		{
			visited.insert(make_pair(next, 1));
			ts.push_back(next);
			es.push_back(e);
		}
		else {    // û��·����
			ts.pop_back();
			es.pop_back();
		}
	}

	if (ts.empty()) return ways;

	//�յ��㷨
	int t = 0;
	int size = es.size();
	printf("\n\n  es.size()=%d,ts.size()=%d  \n\n", size, ts.size());
	while (t < size)
	{
		Point p = *(ways.cend() - 1);  //ѭ����ʼ�������
		Edge e = edges[es[t]];
		int ep0, ep1;
		//ts[t]��ʾ��ǰҪ���������
		if (ts[t] == e.triangles[0])  //�ж��Ƿ��ǵ�0�����ǣ���0������p1����p0����
		{
			ep0 = e.points[0], ep1 = e.points[1];
		}
		else {   //�ǵ�һ�����ǵĻ���p1��p0���Ҳ࣬��Ҫ����˳��
			ep0 = e.points[1], ep1 = e.points[0];
		}

		Point p1 = GetPoint(ep0), p2 = GetPoint(ep1);   // p1���ҵ㣬p2�����
		Point line1 = p1 - p, line2 = p2 - p;           //line1���ߣ� line2����
		int i = t + 1;
		int t1 = i, t2 = i;
		//ÿһ��ѭ������Ҫ�ҵ�һ���յ㣬Ȼ����һ���ٴ�����յ�����һ���յ�
		bool haspoint = false;
		for (; i < size; i++)
		{
			Edge nxte = edges[es[i]];
			int nep0, nep1;
			if (ts[i] == nxte.triangles[0])
			{
				nep0 = nxte.points[0], nep1 = nxte.points[1];
			}
			else {
				nep0 = nxte.points[1], nep1 = nxte.points[0];
			}
			Point np1 = GetPoint(nep0), np2 = GetPoint(nep1);
			if (!isclockwise(line1, np2 - p)) {//np2����㣩����line1�����ߣ������Ļ���ֱ��ȷ��p1Ϊ�յ�
				Point pp = GetPoint(ep0);
				p = Point(pp.x + 0, pp.y - 0);
				ways.push_back(Point(pp.x + 0, pp.y - 0));
				t1++;
				t = nep0 == ep0 ? (i++, t1 + 1) : t1;
				haspoint = true;
				break;
			}
			else if (isclockwise(line1, np1 - p)) //np1���ҵ㣩��line1�����ߣ������Ļ�������line1
			{
				line1 = np1 - p;
				ep0 = nep0;
				t1 = i;
			}

			if (isclockwise(line2, np1 - p)) { //np1���ҵ㣩��line2�����ߣ������Ļ���ֱ��ȷ���յ�
				Point pp = GetPoint(ep1);
				p = Point(pp.x + 0, pp.y - 0);
				ways.push_back(Point(pp.x + 0, pp.y - 0));
				t2++;
				t = nep1 == ep1 ? (i++, t2 + 1) : t2;
				haspoint = true;
				break;
			}
			else if (!isclockwise(line2, np2 - p))  //np2����㣩����line2�����ߣ������Ļ�������line2
			{
				line2 = np2 - p;
				ep1 = nep1;
				t2 = i;
			}

		}
		if (i >= size - 1)  //���ߺ�����һֱ���¸��µ������
		{
			if (!isclockwise(line1, to - p))  //�յ���line1�����ߣ��Ҳ�
			{
				ways.push_back(GetPoint(ep0));    //���Ϊ�յ�
				t = t1;
			}
			else if (isclockwise(line2, to - p))   //�յ���line2�����ߣ����
			{
				ways.push_back(GetPoint(ep1));
				t = t2;
			}
			if (t >= size - 1)          // //�յ����м�
			{
				ways.push_back(to);
			}
			t++;
		}
	}
	return ways;
}

Polygon::~Polygon()
{

}
