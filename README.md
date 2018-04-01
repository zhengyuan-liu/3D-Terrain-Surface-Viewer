# 3D-Terrain-Surface-Viewer
A 3D terrain surface renderer and viewer based on Qt and OpenGL which can read digital elevation model (DEM) data, extract topographic feature points, build triangulation network DEM model, draw and render 3D terrain surface based on grid DEM model or triangulation network DEM model; viewpoint can roam on the terrain surface

本程序的主要功能是读入地形图像，提取地形特征点，建立三角网DEM（数字高程模型），并依据格网DEM和三角网DEM绘制三维地形表面，并可在三维地形表面上进行视点移动/漫游。

直接采用格网的方式完成DEM绘制实际上是对每个点都进行绘制，当DEM图像较大时动态绘制三维地形表面需要消耗大量的资源；而先对DEM图像进行地形特征点的提取并建立TIN三角网之后，再基于三角网进行实时的地形绘制，可以在尽可能地保留地形特征（山顶、山谷、山脊、沟谷等）的基础上大大减小资源的消耗。在本程序运行时，也可以明显地发现在使用格网的方式绘制时在漫游时会有卡顿的感觉，而使用三角网进行地形绘制漫游时非常流畅。
本程序三角网的生成采用了渐点插入算法（Bowyer-Watson算法），可以在特征点数量级的线性的时间内完成三角网的构建，即如果特征点个数增加一倍，耗时近似增加一倍。此算法相对于传统的三角网生成算法大大提升了效率。

本程序使用Qt开源库实现了图形化用户界面，使用OpenGL和Qt配合的库实现了三维地形表面的绘制。
程序的界面如图1所示。

![Alt text](https://raw.githubusercontent.com/zhengyuan-liu/Triangulation-Network-DEM-Viewer/master/demo/1.png)

<p align = "center">图1 程序的界面</p>

程序共有三个视图
1.	DEM Data Viewer，用于显示图像格式的DEM数据
2.	DEM Triangulation Viewer，用于显示建立的DEM三角网
3.	DEM 3D Viewer，用于显示

![Alt text](https://raw.githubusercontent.com/zhengyuan-liu/Triangulation-Network-DEM-Viewer/master/demo/2.png)

<p align = "center">图2 DEM Data Viewer视图</p>

![Alt text](https://raw.githubusercontent.com/zhengyuan-liu/Triangulation-Network-DEM-Viewer/master/demo/3.png)

<p align = "center">图3 DEM Triangulation Viewer视图</p>

![Alt text](https://raw.githubusercontent.com/zhengyuan-liu/Triangulation-Network-DEM-Viewer/master/demo/4.png)

<p align = "center">图4 DEM 3D Viewer视图</p>

程序主要功能包括：

Open：打开一张DEM图像

Exit：退出程序

Extract Topographic Features：在打开DEM图像后，可以提取地形特征点

Create Triangulation：在提取地形特征点之后，可以构建三角网

Show 3D DEM：显示三维地形高程表面

在DEM 3D Viewer视图中键盘按键的功能：

↑：向前漫游

↓：向后漫游

←：向左转向

→：向右转向

W: 向上移动视点

S: 向下移动视点

Space: 旋转/停止旋转（默认）间切换

L: 在绘制填充多边形（默认）和绘制多边形线之间切换

G: 在绘制格网DEM（默认）和绘制三角网DEM之间切换（注：在三角网未生成前使用切换键无效）

在data文件夹中有3幅DEM图像，可用于程序测试用。

