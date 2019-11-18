//====================================
// brief: �����main����ļ�������glut���������������ʷֺ�Ķ���������Ѱ·��·��
// author:sunxvming@163.com
// date:  2019-11-16
//====================================

#include <stdio.h>
#include <glut.h>
#include "Polygon.h"


int fromx = -1;
int fromy = -1;
int tox = -1;
int toy  = -1;
int clicknum = 0;


// ��������ĵ㴴������Σ�����������ĵ���Ϊ������յ�������Ѱ·�������������·��ͼ��
static void DrawLines() {
	double pos[] = { 50,50,275,100,425,60,450,160,350,230, 600, 325, 750,200,750,500, 525,460,575,550,425,550,480,450,325,425,300,300,30,400,85,275 };
	//double pos[] = { 0, 126 ,119,118,158,0 ,200,122, 317,120 ,224,201, 260, 317, 163, 245, 65, 316, 99, 196 };
	//double pos[] = { 150, 100, 300, 100, 350, 150, 400, 100, 550, 100, 600, 150, 450, 450, 200, 500, 200, 350, 50, 250 };
	//double pos[] = {50, 150,150,100,200,100,250,200, 300, 150,250,250,200,250,150,200,150,150,50,250};
	vector<Point> ways;
	Polygon* p = new Polygon(pos, sizeof(pos)/sizeof(double));
	//ǰһ��Point����㣬��һ��Point���յ㡣
	//��0��0�������Ͻ�
	if (fromx > 0 && fromy > 0 && tox > 0 && toy > 0)
	{
		ways = p->FindPath(Point(fromx, fromy), Point(tox, toy));
	}

	glEnable(GL_LINE_STIPPLE);
	glLineWidth(2);//�����߶ο��
	glBegin(GL_LINES);

	//�����ĸ�����
	vector<Line> gridelines = p->GetGrideLines();
	for (unsigned i = 0; i < gridelines.size(); i++)
	{
		Line grideline = gridelines[i];
		glColor3fv(grideline.color);
		glVertex2f((GLfloat)grideline.p1.x, (GLfloat)grideline.p1.y);
		glVertex2f((GLfloat)grideline.p2.x, (GLfloat)grideline.p2.y);
	}

	vector<Line> lines = p->GetLines();
	for (unsigned i = 0; i < lines.size(); i++)
	{
		Line line = lines[i];
		glColor3fv(line.color);
		glVertex2f((GLfloat)line.p1.x, (GLfloat)line.p1.y);
		glVertex2f((GLfloat)line.p2.x, (GLfloat)line.p2.y);
	}

	

	for (unsigned i = 1; i < ways.size(); i++)
	{
		glColor3f(0.0f, 0.0, 1.0);
		glVertex2f((GLfloat)ways[i-1].x, (GLfloat)ways[i-1].y);
		glVertex2f((GLfloat)ways[i].x, (GLfloat)ways[i].y);
	}
	glEnd();

	glPointSize(3);
	glBegin(GL_POINTS);
	auto centers = p->GetCenters();
	for (unsigned i = 0; i < centers.size(); i++)
	{
		glVertex3f(centers[i].x, centers[i].y, 0.0);
	}
	glEnd();

	glDisable(GL_LINE_STIPPLE);
	
}
 
void myDisplay(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
    DrawLines();
	glFlush();
}


// ������Ļص�������Ŀ��Ϊ��ȡ�û����������룬����ΪѰ·�������յ�������Ѱ·
// param ��
//	buttonΪGLUT_LEFT_BUTTON��GLUT_RIGHT_BUTTON�ֱ��ʾ���Ұ�����
//	stateΪ������״̬����Ϊ������ΪGLUT_DOWN
void myClick(int button, int state, int x, int y)
{
	if (state == 1) //̧����1
	{
		clicknum += 1;
		int t = clicknum % 3;
		if (t == 0)
		{
			fromx = fromy = tox = toy = -1;
		
		}
		else if (t == 1)
		{
			fromx = x;
			fromy = y;
		}
		else if (t == 2)
		{
			tox = x;
			toy = y;
		}
		printf("click---state=%d,clicknum=%d,t=%d, x=%d,y=%d\n", state,clicknum,t, x, y);
		myDisplay();
	}
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(30, 30);  //���崰��λ��
	glutInitWindowSize(1000, 800);  //���崰�ڴ�С
	glutCreateWindow("Ѱ·����");
	//gluOrtho2D(x_mix, x_max, y_mix, y_max)
	//���ڽ�ȡ��������ϵ��Ӧ��������������ϵ���Դ�(x_mix, x_max)��(y_mix, y_max)��ֱ��Ϊ�Խ��ߵ�
	//���β��ֽ�ȡͼ�񣬸ý�ͼ���Ա�������ʾ��
	gluOrtho2D(0, 1000, 800, 0);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myClick);
	glutMainLoop();
	return 0;
}