# inpaint
### Implementation of inpaint algorithms.

1. 程序会默认读取当前路径下的`pictures.txt`
其中`pictures.txt`中为输入图像的路径，用换行分隔多个路径

2. 程序首先会生成4类损坏掩图，其中Point为随机画点，Line为随机画直线，Curve为随机画曲线，Text为随机英文文本。前三种方法，每种均会由强到若生成9张掩图，并将掩图的损坏信息输出至`output/data.txt`中。随后，会对每一张输入图像，针对每一种损坏进行以下算法的修复：

- #### nearestNeighborBfs
使用广度优先搜索实现的最近邻插值算法

- #### nearestNeighborRbfs
使用反向广度优先搜索实现的近似最近邻插值算法

- #### nearestNeighborKd
使用kd-tree实现的最近邻插值算法

- #### bilinearEquation
使用最小二乘共轭梯度法求解方程组的最优解进行双线性插值算法

- #### bilinearKd
使用kd-tree实现的近似双线性插值算法

- #### RBF
使用径向基函数的插值算法，使用Gaussian和Inverse Quadratic两种基函数

- #### fastMarching
使用Opencv中的inpaint函数，该函数的原理：
Alexandru Telea. An image inpainting technique based on the fast marching method. Journal of graphics tools, 9(1):23–34, 2004.

3. 程序会对每一张输入图像，每一种损坏，每一种修复算法的修复结果与原图像进行相似度的计算量化计算，分别计算MSE, PSNR, MSSIM值。这些值以及每一次修复消耗的CPU时间均会被输出至`output/data.txt`
