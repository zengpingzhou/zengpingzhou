#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>


using namespace std;
using namespace cv;

//高斯金字塔
class GS_Pyramid
{
public:
	GS_Pyramid();
	GS_Pyramid(const int& num, const Size& size = Size(5, 5));
	GS_Pyramid(const int& num, const Mat& img, const Size& size = Size(5, 5));
	~GS_Pyramid();

	//金字塔变换
	void transformation(const Mat& img);
	//设置高斯塔的层数
	void setLevels(const int& num);
	//设置高斯核的大小
	void setSize(const int& width, const int& height);

	//获取图像金字塔
	vector<Mat> getPyramid() const;

private:
	int m_level;
	Size m_size;
	vector<Mat> m_pyramid;
};

//拉普拉斯金字塔
class LP_Pyramid
{
public:
	LP_Pyramid();
	LP_Pyramid(const GS_Pyramid& gpyramid);
	~LP_Pyramid();

	//金字塔变换
	void transformation(const GS_Pyramid& gpyramid);

	//获取图像金字塔
	vector<Mat> getPyramid() const;

	//获取金字塔层数

private:
	vector<Mat> m_pyramid;
};

class ImageFusion
{
public:
	//评价函数类型
	enum EvalFunc {
		Var = 0, //方差
		SF  = 1,	 
		EOG = 2,
		SML = 3,
	};

public:
	ImageFusion();
	ImageFusion(const Mat& mat1, const Mat& mat2);
	~ImageFusion();

	void fusionImage(const Mat& mat1, const Mat& mat2);

	//图像融合
	Mat imageFusion(const EvalFunc& type, const Size& size);

	//去除方块效应
	void effectWeaken(Mat &fmat, const Size& size, const double& percent = 0.3);

public:
	//多聚焦图像融合 (size为块的大小)
	Mat multiFocus(const Size& size);

	//分通道
	Mat multiFocus(const Mat& mat1, const Mat& mat2);

	//以点为中心
	Mat multiFocusPoint(const Size& size);

private:
	//分块融合
	void multiFocus(const Size& size, Mat& mat_out, int sx, int sy);

	//分通道
	Mat multiFocus_channel(const Mat& mat1, const Mat& mat2);

	//以点位中心扩散，根据方差大小进行融合
	void multiFocusPoint(const Size& size, Mat& mat_out);

	//分通道在分块融合
	void multiFocus(const Mat& mat1, const Mat& mat2, Mat& mat_out, const Size& size, int sx, int sy);

private:
	//以方差作为评价函数
	Mat variance(const Size& size);

	//以SF作为评价函数
	Mat sf(const Size& size);

	//以EOG作为评价函数
	Mat eog(const Size& size);

	//以SML作为评价函数
	Mat sml(const Size& size);

	//判断融合后的图像在一定区域是否为清晰
	void isFocus(Mat& fmat, const Mat& mat1, const Mat& binMat1, const Size& size, double percent);

private:
	int m_imgWd;
	int m_imgHt;

	Mat m_mat1;
	Mat m_mat2;
};


namespace Deal {

	//分通道
	vector<Mat> subchannel(const Mat& mat);

	//对比两幅图像自动添加边界
	void autoaddBoundary(const Mat& mat1_IN, Mat& mat1_OUT, const Mat& mat2_IN, Mat& mat2_OUT);

	//计算方差
	double variance(const vector<double>& inputArray);

	//空间频率SF (行频率RF，列频率CF)
	double spatialFrequency(const Mat& mat, const Size& size, int x, int y);

	//EOG
	double energyGradient(const Mat& mat, const Size& size, int x, int y);

	//拉普拉斯绝对分量和SML
	double sumOfModifiedLaplacian(const Mat& mat, const Size& size, int x, int y);

	//图像二值化
	Mat binaryzation(const Mat& mat);

	//判断该点附近的区域是否接近完美融合
	bool isPerfectFusion(const Mat& mat, const Size& size, int x, int y, double percent);

	//计算标准差
	double STD(const Mat& mat, int channel = 0);

}


//图像融合
namespace Fusion {

	//加权平均法
	Mat weightAverage(const Mat& mat1, double weight1, const Mat& mat2, double weight2);
	
	//分通道根据不同在分别融合
	 

	//拉普拉斯金字塔融合


	//多聚焦图像融合，根据图像像素的平滑程度进行分块融合，单通道
	Mat multiFocus(const Mat& mat1, const Mat& mat2, const Size& size);
	

}
