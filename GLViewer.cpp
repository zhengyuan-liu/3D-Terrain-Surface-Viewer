#include"GLViewer.h"
//构造函数
GLViewer::GLViewer(QGLWidget *parent) :QGLWidget(parent)
{
	rtri = 0.0f;
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(alwaysRotate()));
	timer->start(5);
	memset(myKey, 0, sizeof(bool) * 255);
	bwireframe = false;
}
//析构函数
GLViewer::~GLViewer()
{
	if (texture != nullptr)
		delete texture;
}
//初始化
void GLViewer::initializeGL()
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.3f, 0.8f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
}
//显示事件
void GLViewer::showEvent(QShowEvent *e)
{
	this->setFocus();  //获取焦点，这样可以响应到键盘事件
}
//绘图
void GLViewer::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	if (bGrid == true)
		DrawGLGridScene();  //画格网DEM
	else
		DrawGLTriangulationScene();  //画三角网DEM
	glPopMatrix();

	float rad = 0;

	if (bwireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //画边框
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  //画填充

	if (myKey[VK_UP])  //前进
	{
		moveZ += 1.5f*cosf(rad);
		moveX -= 1.5f*sinf(rad);
	}
	if (myKey[VK_DOWN])  //后退
	{
		moveZ -= 1.5f*cosf(rad);
		moveX += 1.5f*sinf(rad);
	}
	if (myKey[VK_LEFT])  //左转
	{
		rtri -= 0.15f;
		rad -= rtri / 180.0f * 3.1415926f;
	}
	if (myKey[VK_RIGHT])  //右转
	{
		rtri += 0.15f;
		rad += rtri / 180.0f * 3.1415926f;
	}
	if (myKey[Qt::Key_W])  //向上移动视点
		moveY -= 1.5f;
	if (myKey[Qt::Key_S])  //向下移动视点
		moveY += 1.5f;
	if (stopRotate)
		direction = 0;
	else
		direction = 0.15f;
}
//设置opengl场景透视图，在程序启动时中至少被执行一次
void GLViewer::resizeGL(int w, int h)
{
	tnormal = new Point3D[w*h];
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, w*1.0f / h, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//QImage tex = QImage("data/teximage.png");
	////QImage tex = QImage("data/gcanyon_img.png");
	//texture = new QOpenGLTexture(tex);
	//glEnable(GL_TEXTURE_2D);
}
//以格网的方式绘制三维地形
void GLViewer::DrawGLGridScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(moveX, moveY - 150.0f, moveZ - 500.0f);					// Move Left 1.5 Units And Into The Screen 6.0
	glRotatef(rtri, 0.0f, 1.0f, 0.0f);						// Rotate The Triangle On The Y axis
	float width = demImage->width();
	float height = demImage->height();
	float offsetX = width / 2;
	float offsetY = height / 2;
	//texture->bind();
	float s;
	float t;
	float z;
	int step = 5;  //步长
	for (int i = 0; i + step < demImage->width(); i += step)
		for (int j = 0; j + step < demImage->height(); j += step)
		{
			glBegin(GL_TRIANGLES);								// Start Drawing A Triangle		
			s = i / width;
			t = j / height;
			glTexCoord2f(s, t);
			z = qGray(demImage->pixel(i, j));
			setColor(z);
			glVertex3f(i - offsetX, z, j - offsetY);

			s = (i + step) / width;
			t = j / height;
			glTexCoord2f(s, t);
			z = qGray(demImage->pixel(i + step, j));
			setColor(z);
			glVertex3f(i + step - offsetX, z, j - offsetY);

			s = i / width;
			t = (j + step) / height;
			glTexCoord2f(s, t);
			z = qGray(demImage->pixel(i, j + step));
			setColor(z);
			glVertex3f(i - offsetX, z, j + step - offsetY);
			glEnd();

			glBegin(GL_TRIANGLES);								// Start Drawing A Triangle		
			s = (i + step) / width;
			t = (j + step) / height;
			glTexCoord2f(s, t);
			z = qGray(demImage->pixel(i + step, j + step));
			setColor(z);
			glVertex3f(i + step - offsetX, z, j + step - offsetY);

			s = (i + step) / width;
			t = j / height;
			glTexCoord2f(s, t);	
			z = qGray(demImage->pixel(i + step, j));
			setColor(z);
			glVertex3f(i + step - offsetX, z, j - offsetY);

			s = i / width;
			t = (j + step) / height;
			glTexCoord2f(s, t);
			z = qGray(demImage->pixel(i, j + step));
			setColor(z);
			glVertex3f(i - offsetX, z, j + step - offsetY);
			glEnd();
		}
}
//以三角网的方式绘制三维地形
void GLViewer::DrawGLTriangulationScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(moveX, moveY - 150.0f, moveZ - 500.0f);					// Move Left 1.5 Units And Into The Screen 6.0
	glRotatef(rtri, 0.0f, 1.0f, 0.0f);						//设置Y轴为旋转轴
	float offsetX = pTriangulation->getWidth() / 2;
	float offsetY = pTriangulation->getHeight() / 2;
	//texture->bind();
	float s;
	float t;
	for (auto pset = pTriangulation->triangleSet.begin(); pset != pTriangulation->triangleSet.end(); pset++)
	{
		glBegin(GL_TRIANGLES);								// Start Drawing A Triangle		
		s = pset->mVertex[0].x / pTriangulation->getWidth();
		t = pset->mVertex[0].y / pTriangulation->getHeight();
		glTexCoord2f(s, t);
		setColor(pset->mVertex[0].z);
		glVertex3f(pset->mVertex[0].x - offsetX, pset->mVertex[0].z, pset->mVertex[0].y - offsetY);
		s = pset->mVertex[1].x / pTriangulation->getWidth();
		t = pset->mVertex[1].y / pTriangulation->getHeight();
		glTexCoord2f(s, t);
		setColor(pset->mVertex[1].z);
		glVertex3f(pset->mVertex[1].x - offsetX, pset->mVertex[1].z, pset->mVertex[1].y - offsetY);	
		s = pset->mVertex[2].x / pTriangulation->getWidth();
		t = pset->mVertex[2].y / pTriangulation->getHeight();
		glTexCoord2f(s, t);
		setColor(pset->mVertex[2].z);
		glVertex3f(pset->mVertex[2].x - offsetX, pset->mVertex[2].z, pset->mVertex[2].y - offsetY);
		glEnd();
	}
	return;
}
//键盘按下事件
void GLViewer::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Up:
		myKey[VK_UP] = true; break;
	case Qt::Key_Down:
		myKey[VK_DOWN] = true; break;
	case Qt::Key_Left:
		myKey[VK_LEFT] = true; break;
	case Qt::Key_Right:
		myKey[VK_RIGHT] = true; break;
	case Qt::Key_W:
		myKey[Qt::Key_W] = true; break;
	case Qt::Key_S:
		myKey[Qt::Key_S] = true; break;
	case Qt::Key_Space:  //空格键控制旋转
		stopRotate = !stopRotate; break;
	case Qt::Key_L:
		bwireframe = !bwireframe; break;
	case Qt::Key_G:
		if (bGrid == true && pTriangulation != nullptr)
			bGrid = false;
		else
			bGrid = true;
		break;
	case Qt::Key_Escape:  //Ese为退出程序键
		close(); break;
	}
}
//键盘弹起事件
void GLViewer::keyReleaseEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Up:
		myKey[VK_UP] = false; break;
	case Qt::Key_Down:
		myKey[VK_DOWN] = false; break;
	case Qt::Key_Left:
		myKey[VK_LEFT] = false; break;
	case Qt::Key_Right:
		myKey[VK_RIGHT] = false; break;
	case Qt::Key_W:
		myKey[Qt::Key_W] = false; break;
	case Qt::Key_S:
		myKey[Qt::Key_S] = false; break;
	default:
		break;
	}
}
//旋转
void GLViewer::alwaysRotate()
{
	rtri += direction;
	updateGL();
}
//根据高程分层设色渲染
void GLViewer::setColor(float height)
{
	if (height == 0)
		glColor3f(0.0, 132.0 / 255, 1.0);
	else if (height < 4)
		glColor3f(0.0, 168.0 / 255, 35.0 / 255);
	else if (height < 10)
		glColor3f(100.0 / 255, 200.0 / 255, 70.0 / 255);
	else if (height < 27)
		glColor3f(1.0, 220.0 / 255, 166.0 / 255);
	else if (height < 50)
		glColor3f(210.0 / 255, 160.0 / 255, 115.0 / 255);
	else if (height < 95)
		glColor3f(220.0 / 255, 190.0 / 255, 170.0 / 255);
	else
		glColor3f(1.0, 1.0, 1.0);
}