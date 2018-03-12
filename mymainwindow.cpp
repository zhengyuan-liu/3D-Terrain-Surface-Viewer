#include "mymainwindow.h"
//构造函数
MyMainWindow::MyMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	pDemViewer = new DemViewer();
	pScrollAreaDemViewer = new QScrollArea;
	pScrollAreaDemViewer->setBackgroundRole(QPalette::Dark);
	pScrollAreaDemViewer->setWidget(pDemViewer);

	pTriangulationViewer = new TriangulationViewer();
	pScrollAreaTriangulationViewer = new QScrollArea;
	pScrollAreaTriangulationViewer->setBackgroundRole(QPalette::Dark);
	pScrollAreaTriangulationViewer->setWidget(pTriangulationViewer);
	
	pGLViewer = new GLViewer();  //OpenGL Viewer	
	pScrollAreaGLViewer = new QScrollArea;
	pScrollAreaGLViewer->setBackgroundRole(QPalette::Dark);
	pScrollAreaGLViewer->setWidget(pGLViewer);
	pScrollAreaGLViewer->setMinimumSize(100, 100);
	pScrollAreaGLViewer->setWidgetResizable(true);
	pScrollAreaGLViewer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	pTabWidget = new QTabWidget;
	pTabWidget->addTab(pScrollAreaDemViewer, "DEM Data Viewer");
	pTabWidget->addTab(pScrollAreaTriangulationViewer, "DEM Triangulation Viewer");
	pTabWidget->addTab(pScrollAreaGLViewer, "DEM 3D Viewer");
	setCentralWidget(pTabWidget);
	createActions();
	createMenus();
	createToolBars();
	//resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}
//析构函数
MyMainWindow::~MyMainWindow()
{
	delete openAct;         //打开文件动作
	delete exitAct;         //退出动作
	delete zoomInAct;       //放大动作
	delete zoomOutAct;      //缩小动作
	delete normalSizeAct;   //缩放比为1
	delete aboutAct;            //关于动作
	delete doTriangulationAct;  //构建三角网操作
	delete extractFeaturesAct;  //提取地形特征点动作

	delete fileMenu;  //文件菜单
	delete viewMenu;  //视图菜单
	delete helpMenu;  //帮助菜单

	delete fileToolBar;  //文件工具栏
	delete editToolBar;  //编辑工具栏
	delete viewToolBar;  //视图工具栏

	delete pDemViewer;             //显示DEM图像的控件
	delete pTriangulationViewer;   //显示三角网的控件
	delete pScrollAreaDemViewer;            //放置demDataViewer的ScrollArea
	delete pScrollAreaTriangulationViewer;  //放置TriangulationViewer的ScrollArea
	delete pTabWidget;                      //tab control控件
}
//从指定文件路径载入DEM数据
bool MyMainWindow::loadFile(const QString &fileName)
{
	pDemViewer->loadDemData(fileName);
	if (pDemViewer->HasData())  //如果已经读入DEM图像
	{
		extractFeaturesAct->setEnabled(true);  //可以提取地形特征点
		show3DViewerAct->setEnabled(true);     //可以显示3维地形
	}
	scaleFactor = 1.0;
	pDemViewer->adjustSize();
	zoomInAct->setEnabled(true);
	zoomOutAct->setEnabled(true);
	normalSizeAct->setEnabled(true);
	setWindowFilePath(fileName);
	int width = pDemViewer->demImage.width();
	int height = pDemViewer->demImage.height();
	pTriangulationViewer->resize(width + 20, height + 20);  //增大三角网视图的高和宽，使显示更清晰
	pGLViewer->demImage = &pDemViewer->demImage;
	return true;
}
//打开文件
void MyMainWindow::open()
{
	QStringList mimeTypeFilters;
	foreach(const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
		mimeTypeFilters.append(mimeTypeName);
	mimeTypeFilters.sort();
	const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

	//选择文件对话框
	QFileDialog dialog(this, tr("Open File"), picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setMimeTypeFilters(mimeTypeFilters);
	dialog.selectMimeTypeFilter("image/jpeg");
	while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first()))
		;
	pTabWidget->setCurrentIndex(0);
}
//放大
void MyMainWindow::zoomIn()
{
	scaleImage(1.25);
}
//缩小
void MyMainWindow::zoomOut()
{
	scaleImage(0.8);
}
void MyMainWindow::normalSize()
{
	scaleFactor = 1.0;
	pDemViewer->scale = scaleFactor;
	pDemViewer->adjustSize();
}
void MyMainWindow::about()
{
	QMessageBox::about(this, tr("About This Program"),
		tr("<p>By LiuZhengyuan</p><p>Student ID: 1300012430 </p><p>Institude of RS & GIS</p><p>School of Earth and Space Sciences</p>"));
}
//提取地形特征点
void MyMainWindow::extractDemFeatures()
{
	pDemViewer->extractFeatures();
	doTriangulationAct->setEnabled(true);
}
//基于提取的地形特征点构建三角网
void MyMainWindow::doTriangulation()
{
	int width = pDemViewer->demImage.width();
	int height = pDemViewer->demImage.height();
	triangulation.initialize();
	triangulation.setDelaunayAreaSize(width, height);
	triangulation.pointSet = pDemViewer->pointSet;
	pTriangulationViewer->setTriangulation(&triangulation);
	pTriangulationViewer->doTriangulation();
	pTabWidget->setCurrentIndex(1);
	pGLViewer->pTriangulation = &triangulation;
}
//显示3D DEM视图
void MyMainWindow::show3DViewer()
{
	pGLViewer->initializeGL();
	pGLViewer->resizeGL(640, 480);
	pTabWidget->setCurrentIndex(2);
}
//构造操作
void MyMainWindow::createActions()
{
	//打开文件操作
	openAct = new QAction(QIcon("icons/open16.png"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	//退出操作
	exitAct = new QAction(QIcon("icons/GenericDeleteRed16.png"), tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	//放大操作
	zoomInAct = new QAction(QIcon("icons/ZoomInTool16.png"), tr("Zoom &In (25%)"), this);
	zoomInAct->setShortcut(tr("Ctrl++"));
	zoomInAct->setEnabled(false);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
	//缩小操作
	zoomOutAct = new QAction(QIcon("icons/ZoomOutTool16.png"), tr("Zoom &Out (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+-"));
	zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
	//恢复原始大小
	normalSizeAct = new QAction(tr("&Normal Size"), this);
	normalSizeAct->setShortcut(tr("Ctrl+S"));
	normalSizeAct->setEnabled(false);
	connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));
	//提取地形特征点
	extractFeaturesAct = new QAction(QIcon("icons/EditingStopEditing16.png"), tr("&Extract Topographic Features"), this);
	extractFeaturesAct->setShortcut(tr("Ctrl+E"));
	extractFeaturesAct->setEnabled(false);
	connect(extractFeaturesAct, SIGNAL(triggered()), this, SLOT(extractDemFeatures()));
	//构建三角网
	doTriangulationAct = new QAction(QIcon("icons/EditingEditVertices16.png"), tr("Create &Triangulation"), this);
	doTriangulationAct->setShortcut(tr("Ctrl+T"));
	doTriangulationAct->setEnabled(false);
	connect(doTriangulationAct, SIGNAL(triggered()), this, SLOT(doTriangulation()));
	//显示三维地形动作
	show3DViewerAct = new QAction(QIcon("icons/DEM.png"), tr("&Show 3D DEM"), this);
	show3DViewerAct->setShortcut(tr("Ctrl+K"));
	show3DViewerAct->setEnabled(false);
	connect(show3DViewerAct, SIGNAL(triggered()), this, SLOT(show3DViewer()));
	//关于
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}
//构建菜单栏
void MyMainWindow::createMenus()
{
	//构建文件菜单
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(extractFeaturesAct);
	fileMenu->addAction(exitAct);
	//构建视图菜单
	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	//构建帮助菜单
	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);

	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(helpMenu);
}
//构建工具栏
void MyMainWindow::createToolBars()
{
	//打开工具栏
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(exitAct);
	//编辑工具栏
	editToolBar = addToolBar(tr("Edit"));
	editToolBar->addAction(extractFeaturesAct);
	editToolBar->addAction(doTriangulationAct);
	editToolBar->addAction(show3DViewerAct);
	//视图工具栏
	viewToolBar = addToolBar(tr("Edit"));
	viewToolBar->addAction(zoomInAct);
	viewToolBar->addAction(zoomOutAct);
}
//根据缩放因子进行缩放
void MyMainWindow::scaleImage(double factor)
{
	Q_ASSERT(pDemViewer->pixmap());
	scaleFactor *= factor;
	pDemViewer->scale = scaleFactor;
	pDemViewer->zoom();

	adjustScrollBar(pScrollAreaDemViewer->horizontalScrollBar(), factor);
	adjustScrollBar(pScrollAreaDemViewer->verticalScrollBar(), factor);

	zoomInAct->setEnabled(scaleFactor < 5.0);
	zoomOutAct->setEnabled(scaleFactor > 0.333);
}
//调整滚动条
void MyMainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value()	+ ((factor - 1) * scrollBar->pageStep() / 2)));
}
