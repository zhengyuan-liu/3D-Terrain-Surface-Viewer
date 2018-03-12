#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mymainwindow.h"

#include<QtCore\qstandardpaths.h>
#include<QtWidgets\qscrollbar.h>
#include<QtWidgets\qscrollarea.h>
#include<QtWidgets\qfiledialog.h>
#include"DemViewer.h"
#include"TriangulationViewer.h"
#include"GLViewer.h"

//程序的主窗口
class MyMainWindow : public QMainWindow
{
	Q_OBJECT
		
public:
	MyMainWindow(QWidget *parent = 0);
	~MyMainWindow();

private:
	Ui::MyMainWindowClass ui;

//slots
private slots:
	//打开DEM图像文件
	void open();
	//放大
	void zoomIn();
	//缩小
	void zoomOut();
	//初始大小
	void normalSize();
	//提取地形特征点
	void extractDemFeatures();
	//基于提取的地形特征点进行三角网连接
	void doTriangulation();
	//显示三维地形
	void show3DViewer();
	//关于
	void about();

//控件
private:
	Triangulation triangulation;  //三角网
	double scaleFactor;  //缩放系数
	DemViewer* pDemViewer;                        //显示DEM图像的控件
	TriangulationViewer* pTriangulationViewer;    //显示三角网的控件
	GLViewer *pGLViewer;                          //显示3D地形表面的控件
	QScrollArea* pScrollAreaDemViewer;            //放置demDataViewer的ScrollArea
	QScrollArea* pScrollAreaTriangulationViewer;  //放置TriangulationViewer的ScrollArea
	QScrollArea *pScrollAreaGLViewer;             //放置GLViewer的ScrollArea
	QTabWidget* pTabWidget;                       //tab control控件

private:
	//从指定文件路径载入DEM数据
	bool loadFile(const QString &);
	//构造操作
	void createActions();
	//构造菜单
	void createMenus();
	//构造工具栏
	void createToolBars();

	//缩放图片
	void scaleImage(double factor);
	//调整滚动条
	void adjustScrollBar(QScrollBar *scrollBar, double factor);

	QAction *openAct;         //打开文件动作
	QAction *exitAct;         //退出动作
	QAction *zoomInAct;       //放大动作
	QAction *zoomOutAct;      //缩小动作
	QAction *normalSizeAct;   //恢复1:1缩放动作
	QAction *aboutAct;            //关于动作
	QAction *doTriangulationAct;  //构建三角网操作
	QAction *extractFeaturesAct;  //提取地形特征点动作
	QAction *show3DViewerAct;     //显示三维地形动作

	QMenu *fileMenu;  //文件菜单
	QMenu *viewMenu;  //视图菜单
	QMenu *helpMenu;  //帮助菜单

	QToolBar *fileToolBar;  //文件工具栏
	QToolBar *editToolBar;  //编辑工具栏
	QToolBar *viewToolBar;  //视图工具栏
};

#endif // MYMAINWINDOW_H
