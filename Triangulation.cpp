#include"Triangulation.h"

/****************三角形类********************/
//获取三角形的面积
double Triangle::getArea(const Point3D& pt0, const Point3D &pt1, const Point3D &pt2)
{
	Point3D vector1, vector2;
	vector1.x = pt1.x - pt0.x;
	vector1.y = pt1.y - pt0.y;
	vector2.x = pt2.x - pt1.x;
	vector2.y = pt2.y - pt1.y;
	return fabs((vector1.x * vector2.y - vector1.y * vector2.x)) / 2.0f;  //利用向量叉乘计算三角形的面积
}
#define REAL_EPSILON 1.192092896e-07f  //最小的浮点数
//计算三角形外接圆,获得三角形外接圆圆心和半径等参数
void Triangle::calculateCircumcircle()
{
	double x0 = mVertex[0].x;
	double y0 = mVertex[0].y;

	double x1 = mVertex[1].x;
	double y1 = mVertex[1].y;

	double x2 = mVertex[2].x;
	double y2 = mVertex[2].y;

	double y10 = y1 - y0;
	double y21 = y2 - y1;
	double y20 = y2 - y0;

	bool b21zero = y21 > -REAL_EPSILON && y21 < REAL_EPSILON;
	bool b10zero = y10 > -REAL_EPSILON && y10 < REAL_EPSILON;
	bool b20zero = y20 > -REAL_EPSILON && y20 < REAL_EPSILON;

	if (b20zero)  //mVertex[0]和mVertex[2]在同一水平线上
	{
		mVertex[0].y -= 0.001;
		mVertex[2].y += 0.001;
	}
	if (b10zero)  //mVertex[0]和mVertex[1]在同一水平线上
	{
		if (b21zero)	//三个顶点都在同一水平线上
		{
			if (x1 > x0)  { if (x2 > x1) x1 = x2; }
			else		  { if (x2 < x0) x0 = x2; }
			mCenter.x = (x0 + x1) * 0.5F;
			mCenter.y = y0;
		}
		else
		{
			double m1 = -(x2 - x1) / y21;
			double mx1 = (x1 + x2) * 0.5F;
			double my1 = (y1 + y2) * 0.5F;

			mCenter.x = (x0 + x1) * 0.5F;
			mCenter.y = m1 * (mCenter.x - mx1) + my1;
		}
	}
	else if (b21zero)	//mVertex[1]和mVertex[2]在同一水平线上
	{
		double m0 = -(x1 - x0) / y10;
		double mx0 = (x0 + x1) * .5F;
		double my0 = (y0 + y1) * .5F;

		mCenter.x = (x1 + x2) * .5F;
		mCenter.y = m0 * (mCenter.x - mx0) + my0;
	}
	else
	{
		double m0 = -(x1 - x0) / y10;
		double m1 = -(x2 - x1) / y21;
		double mx0 = (x0 + x1) * 0.5F;
		double my0 = (y0 + y1) * 0.5F;
		double mx1 = (x1 + x2) * 0.5F;
		double my1 = (y1 + y2) * 0.5F;
		mCenter.x = (m0 * mx0 - m1 * mx1 + my1 - my0) / (m0 - m1);  //三角形外接圆圆心x坐标
		mCenter.y = m0 * (mCenter.x - mx0) + my0;                   //三角形外接圆圆心y坐标
	}
	double dx = x0 - mCenter.x;
	double dy = y0 - mCenter.y;
	r2 = dx * dx + dy * dy; 	//三角形外接圆半径的平方d
	radius = (double)sqrt(r2);	//三角形外接圆半径
	r2 *= 1.000001f;
}
//判断点p是否在三角形内
bool Triangle::inTriangle(const Point3D &p) const
{
	double S_ABC, S_ADB, S_BDC, S_ADC;
	S_ABC = getArea(mVertex[0], mVertex[1], mVertex[2]);
	S_ADB = getArea(mVertex[0], p, mVertex[2]);
	S_BDC = getArea(p, mVertex[1], mVertex[2]);
	S_ADC = getArea(mVertex[0], mVertex[1], p);
	double S_Sum = S_ADB + S_BDC + S_ADC;
	if (fabs(S_ABC - S_Sum) < 1e-8)  //如果S_ADB + S_BDC + S_ADC==S_ABC，则D在三角形ABC中
		return true;
	else
		return false;
}

/***************三角网类******************/
//构造函数
Triangulation::Triangulation()
{
	bTrianglationFinished = false;
	width = 0;
	height = 0;
}
//初始化三角网
void Triangulation::initialize()
{
	triangleSet.clear();
	tempTvec.clear();
	ebuffer.clear();
}
//设置需要构建的三角网的范围，并依此构造出外包所有点集的超三角形
void Triangulation::setDelaunayAreaSize(int w, int h)
{
	width = w;
	height = h;
}
//构造外包所有点集的超三角形
Triangle Triangulation::createSuperTriangle()
{
	Point3D midpt = Point3D(width / 2, 2 * height, 0);
	//当此处Y方向坐标值为0时，由于顶部区域会出现大量的奇异三角形，故三角网连接出现大量错误，设置为-50，改正了该错误
	Point3D vleft = Point3D(-width, -50, 0);
	Point3D vright = Point3D(width * 2, -50, 0);
	return Triangle(vleft, midpt, vright);
}
//根据点集构建三角网
bool Triangulation::doTriangulation()
{
	superTriangle = createSuperTriangle();  //构建超三角形
	triangleSet.insert(superTriangle);      //插入超三角形
	iterPoint pt = pointSet.begin();
	for (; pt != pointSet.end(); pt++)  //逐点插入
	{
		findTriangleWithPoint(pt);  //在三角网中搜索所有外接圆包含一点的三角形，存入tempTvec
		CreateTriangleWithPt(pt);   //使用找到的多边形（即边集ebuffer中的所有边）与当前点构造新的三角形
	}
	deleteSuperTriangle();  //删除与超三角形顶点连接的三角形
	bTrianglationFinished = true;
	return true;
}
//在三角网中搜索所有外接圆包含一点的三角形
void Triangulation::findTriangleWithPoint(iterPoint pt)
{
	ebuffer.clear();
	tempTvec.clear();
	iterTriangle trianglePtr = triangleSet.begin();
	for (; trianglePtr != triangleSet.end(); trianglePtr++)
	{
		Point3D cp = trianglePtr->mCenter;
		double dx = pt->x - cp.x, dy = pt->y - cp.y;
		if (dx * dx + dy * dy < trianglePtr->r2)
		{
			tempTvec.push_back(trianglePtr);  //使用vector管理临时找到的三角形迭代器(指针)
			Edge e[3];
			e[0] = Edge(trianglePtr->mVertex[0], trianglePtr->mVertex[1]);
			e[1] = Edge(trianglePtr->mVertex[1], trianglePtr->mVertex[2]);
			e[2] = Edge(trianglePtr->mVertex[2], trianglePtr->mVertex[0]);
			for (int k = 0; k < 3; k++)
				if (ebuffer.find(e[k]) != ebuffer.end())
					ebuffer.erase(e[k]);
				else
					ebuffer.insert(e[k]);
		}
	}
	int eraseTriNum = tempTvec.size();
	for (int i = 0; i < eraseTriNum; i++)
	{
		iterTriangle eraseTriPtr = tempTvec[i];
		triangleSet.erase(eraseTriPtr);
	}
}
//使用找到的多边形（即边集ebuffer中的所有边）与当前点构造新的三角形
void Triangulation::CreateTriangleWithPt(iterPoint iterPt)
{
	for (iterEdge iter = ebuffer.begin(); iter != ebuffer.end(); iter++)
	{
		Triangle nt = Triangle(*iterPt, iter->pBegin, iter->pEnd);
		triangleSet.insert(nt);
	}
}
//删除超三角形
void Triangulation::deleteSuperTriangle()
{
	for (iterTriangle iter = triangleSet.begin(); iter != triangleSet.end();)
		if (pointSet.find(iter->mVertex[0]) == pointSet.end() || pointSet.find(iter->mVertex[1]) == pointSet.end() || pointSet.find(iter->mVertex[2]) == pointSet.end())
			iter = triangleSet.erase(iter);
		else
			iter++;
}