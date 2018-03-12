#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include<set>
#include<vector>
#include<stack>
using namespace std;

//带有高程信息的三维空间点
struct Point3D
{
	float x, y, z;

	Point3D()
	{
		x = 0; y = 0; z = 0;
	};
	Point3D(float a, float b, float c)
	{
		x = a; y = b; z = c;
	}

	//重载<运算符
	bool operator<(const Point3D &p) const
	{
		if (x == p.x)
			return y < p.y;
		else
			return  x < p.x;
	}
	//重载==运算符
	bool operator == (const Point3D &p) const
	{
		return ((x == p.x) && (y == p.y));
	}
};

//三角形的边
class Edge
{
private:
	Point3D pBegin;
	Point3D  pEnd;

public:
	Edge(){}
	Edge(Point3D v1, Point3D v2)
	{
		if (v1 < v2)
		{
			pBegin = v1; pEnd = v2;
		}
		else
		{
			pBegin = v2;  pEnd = v1;
		}
	}
	//重载<运算符
	bool operator<(const Edge &e) const
	{
		if (pBegin == e.pBegin)
			return pEnd < e.pEnd;
		else
			return pBegin < e.pBegin;
	}
	//重载==运算符
	bool operator==(const Edge &e) const
	{
		return ((pBegin == e.pBegin) && (pEnd == e.pEnd));
	}
	friend class Triangulation;
};

//三角形
class Triangle
{
private:
	Point3D mCenter;     //三角形外接圆圆心
	double radius;       //三角形外接圆半径
	double r2;           //三角形外接圆半径的平方

public:
	Point3D mVertex[3];  //三角形的3个顶点
	Triangle(){}
	Triangle(Point3D v1, Point3D v2, Point3D v3)
	{
		mVertex[0] = v1;
		mVertex[1] = v2;
		mVertex[2] = v3;
		calculateCircumcircle();
	}

	//获取三角形的面积
	static double getArea(const Point3D &pt0, const Point3D &pt1, const Point3D &pt2);

	//计算三角形外接圆
	void calculateCircumcircle();
	//判断点p是否在三角形内
	bool inTriangle(const Point3D &p) const;

public:
	//重载<运算符
	bool operator <(const Triangle &t) const
	{
		return mCenter < t.mCenter;
	}
	//重载==运算符
	bool operator == (const Triangle &t) const
	{
		return mCenter == t.mCenter;
	}
	friend class Triangulation;
};

typedef set<Point3D>::iterator iterPoint;      //定义指向点集的迭代器类型
typedef set<Triangle>::iterator iterTriangle;  //定义指向三角形集的迭代器类型
typedef set<Edge>::iterator iterEdge;          //定义指向边集的迭代器类型

//三角网
class Triangulation
{
private:
	int width, height;

public:
	//构造函数
	Triangulation();

public:
	int getWidth()
	{
		return width;
	}
	int getHeight()
	{
		return height;
	}
	//初始化
	void initialize();
	//根据点集构建三角网
	bool doTriangulation();
	//设置需要构建的三角网的范围，并依此构造出外包所有点集的超三角形
	void setDelaunayAreaSize(int width, int height);
	//构造外包所有点集的超三角形
	Triangle createSuperTriangle();
	//该函数给出一个点，在当前三角网中搜索所有外接圆包含该点的三角形
	void findTriangleWithPoint(iterPoint iterPt);
	//使用找到的多边形（即边集ebuffer中的所有边）与当前点构造新的三角形
	void CreateTriangleWithPt(iterPoint iterPt);
	//删除超三角形
	void deleteSuperTriangle();

public:
	Triangle superTriangle;  //超三角形，构建三角网算法用
	bool bTrianglationFinished;  //三角网是否构建完成
	set<Point3D> pointSet;      //用于存贮点集，使用set容器会对点进行排序，加快构网的速度
	set<Triangle> triangleSet;  //用于管理连接完成的三角形
	vector<iterTriangle> tempTvec;    //用于存贮给出一点后，找到的所有三角形的指针，必须用指针管理，否则会删除三角形实体，影响整个三角网的连接	
	set<Edge> ebuffer;          //用于管理搜索到的三角形，形成的多边形
};

#endif