#ifndef GLVIEWER_H
#define GLVIEWER_H

#include <QtGui>
#include <QtCore>
#include <gl\glaux.h>
#include <QtOpenGL\QtOpenGL>
#include <QWidget>
#include"Triangulation.h"

class GLViewer : public QGLWidget
{
	Q_OBJECT

private:
	//以格网的方式绘制三维地形
	void DrawGLGridScene();
	//以三角网的方式绘制三维地形
	void DrawGLTriangulationScene();

private slots:
	//旋转
	void alwaysRotate();
	//根据高程分层设色渲染
	void GLViewer::setColor(float height);

signals:
	void zRotationChanged(int angle);

public:
	explicit GLViewer(QGLWidget *parent = 0);
	~GLViewer();

	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	//键盘按下事件
	void keyPressEvent(QKeyEvent *e);
	//键盘弹起事件
	void keyReleaseEvent(QKeyEvent *e);
	void showEvent(QShowEvent *e);

	QImage* demImage = nullptr;  //格网DEM
	Triangulation* pTriangulation = nullptr;  //三角网
	QOpenGLTexture *texture = nullptr;  //纹理

private:
	float rtri = 0;  //旋转角

	bool myKey[255];

	bool bwireframe = false;  //bGrid为true时画线，为false时画面
	bool blight;  //光照
	bool stopRotate = true;  //stopRotate为true时不旋转
	bool bGrid = true;  //bGrid为true时画格网，为false时画三角网

	float moveX = 0, moveY = 0, moveZ = 0;
	float direction = 0.15f;

	Point3D *tnormal;

};

#endif