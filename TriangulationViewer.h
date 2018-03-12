#ifndef TRIANGULATIONVIEWER_H
#define TRIANGULATIONVIEWER_H

#include<QtWidgets\qlabel.h>
#include<QtGui\qpen.h>
#include<QtGui\qimagereader.h>
#include<QtGui\qguiapplication.h>
#include<QtGui\qpainter.h>
#include<QtCore\qdir.h>
#include <QMessageBox>
#include"Triangulation.h"

//三角网视图控件
class TriangulationViewer : public QLabel
{
	Q_OBJECT
private:
	QPen pen;  //画笔
	bool hasConstructed;   //三角网是否构建完成

public:
	Triangulation *pTriangulation;  //三角网

	//构造函数
	TriangulationViewer(QLabel *parent = 0);
	//构建三角网
	void doTriangulation();
	//设置三角网(目的是传入已经选好特征点的三角网)
	void setTriangulation(Triangulation* p) { pTriangulation = p; }

public:
	//绘图事件
	void paintEvent(QPaintEvent *event);

};

#endif