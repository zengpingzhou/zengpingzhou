具体流程（需要有vs2019，Qt，OPencv）

1、创建名为build的文件夹（其他文件名也行）

2、在项目根目录git bash

3、cd build进入build文件

4、对build上层根目录进行cmake，注意看一下CMakeLists.txt中cmake的版本以及Qt和Opencv的目录及版本，Qt和Opencv的具体配置方式自行百度

5、cmake结束后，进入build文件夹，点击.sln打开项目解决方案，进行编译运行。



图像融合的模块设计

UI：

​	1、图片放置

​	2、工具栏

​	3、选择融合方式



功能模块：

​	1、不同融合算法

​	聚焦区域检测算法见FusionAlgorithm/fusion.h声明

​	var：方差

​	sf：空间频率

​	eog：能量梯度

​	sml：拉普拉斯绝对分量和

​	2、其他图片处理算法

​	





