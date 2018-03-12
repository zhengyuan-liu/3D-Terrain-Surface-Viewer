#include"DemViewer.h"

//构造函数
DemViewer::DemViewer(QLabel *parent) :QLabel(parent)
{
	setBackgroundRole(QPalette::Base);
	setScaledContents(true);
	hasData = false;
	scale = 1.0;
}
//载入DEM数据
void DemViewer::loadDemData(const QString &fileName)
{
	QImageReader reader(fileName);
	reader.setAutoTransform(true);
	demImage = reader.read();
	if (demImage.isNull())  //读入DEM数据失败
	{
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
		setWindowFilePath(QString());
		setPixmap(QPixmap());
		adjustSize();
		return;
	}
	setPixmap(QPixmap::fromImage(demImage));
	hasData = true;
}
//缩放
void DemViewer::zoom()
{
	resize(scale * pixmap()->size());
}
//提取地形特征点
void DemViewer::extractFeatures()
{
	if (!hasData)
		return;
	pointSet.clear();  //清空点集
	int width = demImage.width();  //图像的宽度/列数
	int height = demImage.height();  //图像的高度/行数

	int threshold = 7;  //阈值,阈值越大特征点越少
	int wsize = 21;  //窗口大小
	float wdata[21];
	//特征点提取算法
	for (int j = 3; j < height; j += 3)
	{
		for (int i = 3; i < width - wsize; i += 3)
		{
			int cve = qGray(demImage.pixel(i + wsize - 1, j));
			int cvb = qGray(demImage.pixel(i, j));
			float k = (cve - cvb) / wsize;
			float A = k;
			float B = -1.0;
			int yi = qGray(demImage.pixel(i, j));

			int x, y;
			int n = 0;

			for (int a = i + 1; a < i + wsize; a++)
			{
				int av = qGray(demImage.pixel(a, j));
				int xa = a - i;
				int ya = av - yi;

				float dis = fabs(A*xa + B*ya);
				wdata[n++] = dis;
			}

			int xm = i + wsize / 2;
			int ym = qGray(demImage.pixel(i + wsize / 2, j)) - yi;

			float mdis = fabs(A*xm + B*ym);

			float max = -1;
			for (int d = 0; d < n; d++)
				if (max < wdata[d])
					max = wdata[d];

			if (mdis == max && max > threshold)
			{
				x = i + wsize / 2;
				y = j;
				float iv = qGray(demImage.pixel(x, y));
				float ix = x + rand() % 100 * 0.00001F;  //需要在特征点的x，y上加一个小的扰动量，防止出现三角网交叉现象
				float iy = y + rand() % 100 * 0.00001F;
				Point3D p = Point3D(ix, iy, iv);
				pointSet.insert(p);
			}
		}
	}
	this->update();
}
//绘图事件
void DemViewer::paintEvent(QPaintEvent *  /*event*/)
{
	if (!hasData)
		return;
	//画DEM图像
	QPainter painter(this);
	painter.setPen(QColor(Qt::yellow));
	if (pixmap())
	{
		const QPixmap* pix = pixmap();
		QRectF target = QRectF(QPointF(0, 0), scale* pix->size());  //目标矩阵
		QRectF source = QRectF(QPointF(0, 0), pix->size());  //数据源
		painter.drawPixmap(target, *pix, source);
	}
	//画地形特征点
	int n = pointSet.size();
	if (n > 1)
	{
		auto iter = pointSet.begin();
		for (; iter != pointSet.end(); iter++)
		{
			Point3D point = *iter;
			QRect rect = QRect(scale * point.x, scale * point.y, 3, 3);
			rect.translate(-1, -1);
			painter.drawRect(rect);
		}
	}
}