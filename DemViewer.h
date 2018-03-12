#ifndef DEMVIEWER
#define DEMVIEWER

#include<set>
#include<QtWidgets\qlabel.h>
#include<QtGui\qpen.h>
#include<QtGui\qimagereader.h>
#include<QtGui\qguiapplication.h>
#include<QtGui\qpainter.h>
#include<QtCore\qdir.h>
#include <QMessageBox>
#include"Triangulation.h"
using namespace std;

//DEM图像视图控件
class DemViewer : public QLabel
{
	Q_OBJECT  //只有加入了Q_OBJECT，才能使用QT中的signal和slot机制
private:
	QPen pen;  //画笔
	bool hasData;  //是否已经读入DEM图像
	
public:	
	double scale;  //缩放比
	QImage demImage;  //DEM图像
	set<Point3D> pointSet;  //地形特征点集合

public:
	//构造函数
	DemViewer(QLabel *parent = 0);
	//是否已经读入完成DEM图像
	bool HasData()
	{
		return hasData;
	}

	//载入DEM数据
	void loadDemData(const QString &fileName);
	//缩放
	void zoom();
	//提取地形特征点
	void extractFeatures();
	
	//绘图事件
	void paintEvent(QPaintEvent *event);
};

#endif