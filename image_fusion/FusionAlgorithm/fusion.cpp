#include "fusion.h"
#include <math.h>

GS_Pyramid::GS_Pyramid() :
	m_level(3)
	, m_size(5, 5)
{
}

GS_Pyramid::GS_Pyramid(const int& num, const Size& size) :
	m_level(num)
	, m_size(size)
{
	if (1 >= num)
		m_level = 3;
}

GS_Pyramid::GS_Pyramid(const int& num, const Mat& img, const Size& size) :
	m_level(num)
	, m_size(size)
{
	if (1 >= num)
		m_level = 3;
	transformation(img);
}

GS_Pyramid::~GS_Pyramid()
{
}

void GS_Pyramid::transformation(const Mat& img)
{
	Mat imgIn, imgOut;
	m_pyramid.push_back(img);
	imgIn = img;
	for (int i = 1; i < m_level; ++i)
	{
		//先进行高斯卷积
		//GaussianBlur(imgIn, imgOut, m_size, 15, 0, BORDER_DEFAULT);
		//imgIn = imgOut;
		//再对高斯卷积后的图像进行下采样
		pyrDown(imgIn, imgOut, Size(imgIn.cols / 2, imgIn.rows / 2));
		m_pyramid.push_back(imgOut);
		imgIn = imgOut;
	}
}

void GS_Pyramid::setLevels(const int& num)
{
	m_level = num;
}

void GS_Pyramid::setSize(const int& width, const int& height)
{
	m_size = Size(width, height);
}

vector<Mat> GS_Pyramid::getPyramid() const
{
	return m_pyramid;
}

LP_Pyramid::LP_Pyramid()
{
}

LP_Pyramid::LP_Pyramid(const GS_Pyramid& gpyramid)
{
	transformation(gpyramid);
}

LP_Pyramid::~LP_Pyramid()
{
}

void LP_Pyramid::transformation(const GS_Pyramid& gpyramid)
{
	vector<Mat> pyramid = gpyramid.getPyramid();
	if (1 >= pyramid.size())
		return;
	int level = pyramid.size();
	Mat imgIn, imgOut;
	for (int i = 1; i < level; ++i)
	{
		imgIn = pyramid[i];
		pyrUp(imgIn, imgOut, Size(imgIn.cols * 2, imgIn.rows * 2));
		imgOut = pyramid[i - 1] - imgOut;
		m_pyramid.push_back(imgOut);
	}
}

vector<Mat> LP_Pyramid::getPyramid() const
{
	return m_pyramid;
}

ImageFusion::ImageFusion() :
	m_imgWd(0)
	, m_imgHt(0)
	, m_mat1()
	, m_mat2()
{
}

ImageFusion::ImageFusion(const Mat& mat1, const Mat& mat2):
	m_mat1(mat1)
	, m_mat2(mat2)
{
	m_imgWd = mat1.cols;
	m_imgHt = mat1.rows;
}

ImageFusion::~ImageFusion()
{
}

void ImageFusion::fusionImage(const Mat& mat1, const Mat& mat2)
{
	m_mat1 = mat1;
	m_mat2 = mat2;
	m_imgWd = mat1.cols;
	m_imgHt = mat1.rows;
}

Mat ImageFusion::imageFusion(const EvalFunc& type, const Size& size)
{
	Mat fmat;
	switch (type)
	{
	case Var:
		fmat = variance(size);
		break;
	case SF:
		fmat = sf(size);
		break;
	case EOG:
		fmat = eog(size);
		break;
	case SML:
		fmat = sml(size);
		break;
	default:
		break;
	}
	return fmat;
}

void ImageFusion::effectWeaken(Mat& fmat, const Size& size, const double& percent)
{
	Mat diffMat1;
	absdiff(fmat, m_mat1, diffMat1);
	isFocus(fmat, m_mat1, Deal::binaryzation( m_mat1 - fmat), size, percent);
	Mat diffMat2;
	absdiff(fmat, m_mat2, diffMat2);
	isFocus(fmat, m_mat2, Deal::binaryzation(m_mat2 - fmat), size, percent);
	//isFocus(fmat, m_mat1, m_mat2, Deal::binaryzation(diffMat1), Deal::binaryzation(diffMat2), size, percent);
}

Mat ImageFusion::multiFocus(const Size& size)
{
	//Mat fmat = m_mat1.clone();
	Mat fmat;
	fmat.create(m_mat1.size(), m_mat1.type());
	int x = 0, y = 0;
	multiFocus(size, fmat, 0, 0);

	//剩余未处理的边界
	int wd = m_imgWd % size.width;
	int ht = m_imgHt % size.height;
	if (0 != wd) //右侧边界
		multiFocus(Size(wd,size.height), fmat, m_imgWd - wd - 1, 0);
	if (0 != ht) //下侧边界
		multiFocus(Size(size.width, ht), fmat, 0, m_imgHt - ht -1);
	if(0 != wd && 0 != ht) //右下角方块
		multiFocus(Size(size.width, size.height), fmat, m_imgWd - wd - 1, m_imgHt - ht - 1);
	return fmat;
}

Mat ImageFusion::multiFocus(const Mat& mat1, const Mat& mat2)
{
	vector<Mat> fmats;
	vector<Mat> planes1;
	vector<Mat> planes2;
	split(mat1, planes1);
	split(mat2, planes2);
	int channels = mat1.channels();
	Mat mat;
	for (int i = 0; i < channels; ++i)
	{
		mat = multiFocus_channel(planes1[i], planes2[i]);
		fmats.push_back(mat);
	}
	merge(fmats, mat);
	return mat;
}

Mat ImageFusion::multiFocusPoint(const Size& size)
{
	Mat fmat;
	fmat.create(m_mat1.size(), m_mat1.type());
	multiFocusPoint(size, fmat);
	return fmat;
}

void ImageFusion::multiFocus(const Size& size, Mat& mat_out, int sx, int sy)
{
	//扫描块的大小
	int piece_wd = size.width;
	int piece_ht = size.height;
	int x = sx, y = sy;
	vector<double> in1, in2;
	double var1, var2;

	while (y + piece_ht <= m_imgHt)
	{
		while (x + piece_wd <= m_imgWd)
		{
			//清理上一个扫描块
			in1.clear(); in2.clear();
			//扫描下一个
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					in1.push_back(m_mat1.at<Vec3b>(i + y, j + x)[0]);
					in2.push_back(m_mat2.at<Vec3b>(i + y, j + x)[0]);
				}
			}
			//计算扫描块方差
			var1 = Deal::variance(in1);
			var2 = Deal::variance(in2);
			//根据扫描块方差将对应方块赋给融合后的图像
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					mat_out.at<Vec3b>(i + y, j + x) = var1 > var2
						? m_mat1.at<Vec3b>(i + y, j + x) : m_mat2.at<Vec3b>(i + y, j + x);
				}
			}
			//x += piece_wd;
			x += 1;
		}
		x = sx;
		//y += piece_ht;
		y += 1;
	}
}

Mat ImageFusion::multiFocus_channel(const Mat& mat1, const Mat& mat2)
{
	Mat fmat;
	fmat.create(mat1.size(), mat1.type());
	int x = 0, y = 0;
	multiFocus(mat1, mat2, fmat, Size(5,5), 0, 0);

	//剩余未处理的边界
	Size size(5,5);
	int imgWd = mat1.cols;
	int imgHt = mat1.rows;
	int wd = imgWd % size.width;
	int ht = imgHt % size.height;

	if (0 != wd) //右侧边界
		multiFocus(mat1, mat2, fmat, Size(wd, size.height), imgWd - wd - 1, 0);
	if (0 != ht) //下侧边界
		multiFocus(mat1, mat2, fmat, Size(size.width, ht), 0, imgHt - ht - 1);
	if (0 != wd && 0 != ht) //右下角方块
		multiFocus(mat1, mat2, fmat, Size(size.width, size.height), imgWd - wd - 1, imgHt - ht - 1);
	return fmat;
}

void ImageFusion::multiFocusPoint(const Size& size, Mat& mat_out)
{
	int piece_wd = size.width;
	int piece_ht = size.height;
	int imgWd = m_mat1.cols;
	int imgHt = m_mat1.rows;
	vector<double> in1, in2;
	double var1, var2;
	int x = 0, y = 0;
	while (y < imgHt)
	{
		while (x < imgWd)
		{
			in1.clear(); in2.clear();
			for (int i = -(piece_ht/2); i <= piece_ht/2; ++i)
			{
				if (i < 0 || i + y >= imgHt)
					continue;
				for (int j = -(piece_wd/2); j <= piece_wd/2; ++j)
				{
					if (j < 0 || j + x >= imgWd)
						continue;
					in1.push_back(m_mat1.at<Vec3b>(i + y, j + x)[0]);
					in2.push_back(m_mat2.at<Vec3b>(i + y, j + x)[0]);
				}
			}
			var1 = Deal::variance(in1);
			var2 = Deal::variance(in2);
			mat_out.at<Vec3b>(y, x) = var1 >= var2
				? m_mat1.at<Vec3b>(y, x) : m_mat2.at<Vec3b>(y, x);
			++x;
		}
		x = 0;
		++y;
	}
}

void ImageFusion::multiFocus(const Mat& mat1, const Mat& mat2, Mat& mat_out, const Size& size, int sx, int sy)
{
	//扫描块的大小
	int piece_wd = size.width;
	int piece_ht = size.height;
	int imgWd = mat1.cols;
	int imgHt = mat1.rows;
	int x = sx, y = sy;
	vector<double> in1, in2;
	double var1, var2;

	while (y + piece_ht <= imgHt)
	{
		while (x + piece_wd <= imgWd)
		{
			//清理上一个扫描块
			in1.clear(); in2.clear();
			//扫描下一个
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					in1.push_back(m_mat1.at<Vec3b>(i + y, j + x)[0]);
					in2.push_back(m_mat2.at<Vec3b>(i + y, j + x)[0]);
				}
			}
			//计算扫描块方差
			var1 = Deal::variance(in1);
			var2 = Deal::variance(in2);
			//根据扫描块方差将对应方块赋给融合后的图像
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					mat_out.at<uchar>(i + y, j + x) = var1 > var2
						? mat1.at<uchar>(i + y, j + x) : mat2.at<uchar>(i + y, j + x);
				}
			}
			x += piece_wd;
		}
		x = sx;
		y += piece_ht;
	}
}

Mat ImageFusion::variance(const Size& size)
{
	Mat mat_out;
	mat_out.create(m_mat1.size(), m_mat1.type()); 
	int piece_wd = size.width;
	int piece_ht = size.height;
	vector<double> in1, in2;
	double var1, var2;
	int x = 0, y = 0;
	while (y < m_imgHt)
	{
		while (x < m_imgWd)
		{
			in1.clear(); in2.clear();
			for (int i = -(piece_ht / 2); i <= (piece_ht/2) && (i+y) < m_imgHt; ++i)
			{
				if (i < 0)
					continue;
				for (int j = -(piece_wd / 2); j <= (piece_wd/2) && (j+x) < m_imgWd; ++j)
				{
					if (j < 0)
						continue;
					in1.push_back(m_mat1.at<Vec3b>(i + y, j + x)[0]);
					in2.push_back(m_mat2.at<Vec3b>(i + y, j + x)[0]);
				}
			}
			//计算扫描块方差
			var1 = Deal::variance(in1);
			var2 = Deal::variance(in2);
			mat_out.at<Vec3b>(y, x) = var1 >= var2
				? m_mat1.at<Vec3b>(y, x) : m_mat2.at<Vec3b>(y, x);
			++x;
		}
		x = 0;
		++y;
	}
	return mat_out;
}

Mat ImageFusion::sf(const Size& size)
{
	Mat mat_out;
	mat_out.create(m_mat1.size(), m_mat1.type());
	int piece_wd = size.width;
	int piece_ht = size.height;
	double var1, var2;
	int x = 0, y = 0;
	while (y < m_imgHt)
	{
		while (x < m_imgWd)
		{
			//计算扫描块SF
			var1 = Deal::spatialFrequency(m_mat1, size, x, y);
			var2 = Deal::spatialFrequency(m_mat2, size, x, y);
			mat_out.at<Vec3b>(y, x) = var1 >= var2
				? m_mat1.at<Vec3b>(y, x) : m_mat2.at<Vec3b>(y, x);
			++x;
		}
		x = 0;
		++y;
	}
	return mat_out;
}

Mat ImageFusion::eog(const Size& size)
{
	Mat mat_out;
	mat_out.create(m_mat1.size(), m_mat1.type());
	int piece_wd = size.width;
	int piece_ht = size.height;
	double var1, var2;
	int x = 0, y = 0;
	while (y < m_imgHt)
	{
		while (x < m_imgWd)
		{
			//计算扫描块EOG
			var1 = Deal::energyGradient(m_mat1, size, x, y);
			var2 = Deal::energyGradient(m_mat2, size, x, y);
			mat_out.at<Vec3b>(y, x) = var1 >= var2
				? m_mat1.at<Vec3b>(y, x) : m_mat2.at<Vec3b>(y, x);
			++x;
		}
		x = 0;
		++y;
	}
	return mat_out;
}

Mat ImageFusion::sml(const Size& size)
{
	Mat mat_out;
	mat_out.create(m_mat1.size(), m_mat1.type());
	int piece_wd = size.width;
	int piece_ht = size.height;
	double var1, var2;
	int x = 0, y = 0;
	while (y < m_imgHt)
	{
		while (x < m_imgWd)
		{
			//计算扫描块SML
			var1 = Deal::sumOfModifiedLaplacian(m_mat1, size, x, y);
			var2 = Deal::sumOfModifiedLaplacian(m_mat2, size, x, y);
			mat_out.at<Vec3b>(y, x) = var1 >= var2
				? m_mat1.at<Vec3b>(y, x) : m_mat2.at<Vec3b>(y, x);
			++x;
		}
		x = 0;
		++y;
	}
	return mat_out;
}

void ImageFusion::isFocus(Mat& fmat, const Mat& mat1, const Mat& binMat1, const Size& size, double percent)
{
	int piece_wd = size.width;
	int piece_ht = size.height;
	int x = 0, y = 0;
	while (y < m_imgHt)
	{
		while (x < m_imgWd)
		{
			if (!Deal::isPerfectFusion(binMat1, size, x, y, percent))
				fmat.at<Vec3b>(y, x) = mat1.at<Vec3b>(y, x);
			++x;
		}
		x = 0;
		++y;
	}
}

vector<Mat> Deal::subchannel(const Mat& mat)
{
	vector<Mat> planes;
	split(mat, planes);
	return planes;
}

void Deal::autoaddBoundary(const Mat& mat1_IN, Mat& mat1_OUT, const Mat& mat2_IN, Mat& mat2_OUT)
{
	int add_rows = mat1_IN.rows > mat2_IN.rows ? mat1_IN.rows : mat2_IN.rows;
	int add_cols = mat1_IN.cols > mat2_IN.cols ? mat1_IN.cols : mat2_IN.cols;
	copyMakeBorder(mat1_IN, mat1_OUT, 0, add_rows - mat1_IN.rows, 0, add_cols - mat1_IN.cols, BORDER_CONSTANT, Scalar::all(255));
	copyMakeBorder(mat1_IN, mat1_OUT, 0, add_rows - mat2_IN.rows, 0, add_cols - mat2_IN.cols, BORDER_CONSTANT, Scalar::all(255));
}

double Deal::variance(const vector<double>& inputArray)
{
	double average = 0;
	int size = inputArray.size();
	for (int i = 0; i < size; ++i)
		average += inputArray[i];
	average /= size;
	double sum = 0;
	for (int i = 0; i < size; ++i)
		sum += pow(inputArray[i] - average, 2);
	return sqrt(sum);
}

double Deal::spatialFrequency(const Mat& mat, const Size& size, int x, int y)
{
	//行和列方向上的单位长度
	int M = size.height;
	int N = size.width;
	//图像的大小
	int imageHt = mat.rows;
	int imageWd = mat.cols;
	//行频率RF和列频率CF
	double RF = 0, CF = 0;
	double val;
	for (int i = y; i < y+M && i < imageHt; ++i)
	{
		for (int j = x+1; j < x+N && j < imageWd; ++j)
		{
			val = mat.at<Vec3b>(i, j)[0] - mat.at<Vec3b>(i, j - 1)[0];
			RF += pow(val, 2);
		}
	}
	for (int i = y+1; i < y+M && i < imageHt; ++i)
	{
		for (int j = x; j < x+N && j < imageWd; ++j)
		{
			val = mat.at<Vec3b>(i, j)[0] - mat.at<Vec3b>(i - 1, j)[0];
			CF += pow(val, 2);
		}
	}
	return sqrt((RF + CF) / double(M * N));
}

double Deal::energyGradient(const Mat& mat, const Size& size, int x, int y)
{
	//行和列方向上的单位长度
	int M = size.height;
	int N = size.width;
	//图像的大小
	int imageHt = mat.rows;
	int imageWd = mat.cols;
	//
	double RE = 0, CE = 0;
	double val;
	for (int i = -(M-1)/2; i < (M-1)/2 && (y+i+1) < imageHt; ++i)
	{
		if ( y + i < 0)
			continue;
		for (int j = -(N-1)/2; j < (N-1)/2 && (x+j+1) < imageWd; ++j)
		{
			if (x + j < 0)
				continue;
			val = mat.at<Vec3b>(y + i + 1, x)[0] - mat.at<Vec3b>(y + i, x)[0];
			RE += pow(val, 2);
			val = mat.at<Vec3b>(y, x + j + 1)[0] - mat.at<Vec3b>(y, x + j)[0];
			RE += pow(val, 2);
		}
	}
	return RE + CE;
}

double Deal::sumOfModifiedLaplacian(const Mat& mat, const Size& size, int x, int y)
{
	int step = 1;
	//行和列方向上的单位长度
	int M = size.height;
	int N = size.width;
	//图像的大小
	int imageHt = mat.rows;
	int imageWd = mat.cols;
	//
	double SML = 0;
	for (int i = -M; i < (M+1) && (y + i + step) < imageHt; ++i)
	{
		if (y + i - step < 0)
			continue;
		for (int j = -N; j < (N+1) && (x + j + step) < imageWd; ++j)
		{
			if (x + j - step < 0)
				continue;
			SML += abs(2 * mat.at<Vec3b>(y+i, x+j)[0] - mat.at<Vec3b>(y+i - step, x+j)[0] - mat.at<Vec3b>(y+i + step, x+j)[0]);
			SML += abs(2 * mat.at<Vec3b>(y+i, x+j)[0] - mat.at<Vec3b>(y+i, x+j - step)[0] - mat.at<Vec3b>(y+i, x+j + step)[0]);
		}
	}
	return SML;
}

Mat Deal::binaryzation(const Mat& mat)
{
	Mat Tmat = mat;
	for (int i = 0; i < mat.rows; ++i)
	{
		for (int j = 0; j < mat.cols; ++j)
		{
			if (mat.at<Vec3b>(i, j)[0] < 3)
			{
				Tmat.at<Vec3b>(i, j)[0] = 0;
			}
			else
			{
				Tmat.at<Vec3b>(i, j)[0] = 255;
			}
		}
	}
	return Tmat;
}

bool Deal::isPerfectFusion(const Mat& binMat, const Size& size, int x, int y, double percent)
{
	//if (binMat.at<Vec3b>(y, x)[0] < 1)
	//	return true;
	//行和列方向上的单位长度
	int M = size.height;
	int N = size.width;
	//图像的大小
	int imageHt = binMat.rows;
	int imageWd = binMat.cols;
	int eff_nums = 0;
	for (int i = -(M - 1) / 2; i < (M - 1) / 2 && (y + i + 1) < imageHt; ++i)
	{
		if (y + i < 0)
			continue;
		for (int j = -(N - 1) / 2; j < (N - 1) / 2 && (x + j + 1) < imageWd; ++j)
		{
			if (x + j < 0)
				continue;
			binMat.at<Vec3b>(y + i, x + j)[0] >= 1 ? ++eff_nums : NULL;
		}
	}
	//if (double(eff_nums) / double(M * N) > 1-percent)
	//	return true;
	if (double(eff_nums) / double(M * N) < percent)
		return false;
	return true;
}

double Deal::STD(const Mat& mat, int channel)
{
	if (channel > mat.channels())
		channel = 0;
	//int h = mat.rows;
	//int w = mat.cols;
	double sum = 0 ; //, aver
	//for (int i = 0; i < h; ++i)
	//{
	//	for (int j = 0; j < w; ++j)
	//	{
	//		sum += double(mat.at<Vec3b>(i, j)[channel]);
	//	}
	//}
	//aver = sum / (h * w);
	//sum = 0;
	//for (int i = 0; i < h; ++i)
	//{
	//	for (int j = 0; j < w; ++j)
	//	{
	//		sum += (double(mat.at<Vec3b>(i, j)[channel]) - aver);
	//	}
	//}
	//sum = sum/(h * w);
	Mat meanMat, stdMat;
	meanStdDev(mat, meanMat,stdMat);
	sum = stdMat.at<double>(channel);
	return sum;
}

Mat Fusion::weightAverage(const Mat& mat1, double weight1, const Mat& mat2, double weight2)
{
	return mat1 * weight1 + mat2 * weight2;
}

Mat Fusion::multiFocus(const Mat& mat1, const Mat& mat2, const Size& size)
{
	//扫描块的宽高
	int piece_wd = size.width;
	int piece_ht = size.height;
	//图像宽高
	int img_wd = mat1.cols;
	int img_ht = mat1.rows;

	Mat fmat = mat1.clone();
	//fmat.create(mat1.size(), mat1.type());
	int x=0, y=0;
	vector<double> in1, in2;
	double var1, var2;

	while ( y < img_ht)
	{
		while (x < img_wd)
		{
			in1.clear(); in2.clear();
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					in1.push_back(mat1.at<Vec3b>(i + y, j + x)[0]);
					in2.push_back(mat2.at<Vec3b>(i + y, j + x)[0]);
				}
			}
			var1 = Deal::variance(in1);
			var2 = Deal::variance(in2);
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					fmat.at<Vec3b>(i + y, j + x) = var1 > var2
						? mat1.at<Vec3b>(i + y, j + x) : mat2.at<Vec3b>(i + y, j + x);
				}
			}
			x += piece_wd;
		}
		x = 0;
		y += piece_ht;
	}
	//剩余未处理的边界
	int wd = img_wd % piece_wd;
	int ht = img_ht % piece_ht;

	if (wd == 0)
		return Mat();
	//右侧剩余
	x = img_wd - piece_wd;
	y = 0;
	while (y < img_ht)
	{
		while (x < img_wd)
		{
			in1.clear(); in2.clear();
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < wd; ++j)
				{
					in1.push_back(mat1.at<Vec3b>(i + y, j + x)[0]);
					in2.push_back(mat2.at<Vec3b>(i + y, j + x)[0]);
				}
			}
			var1 = Deal::variance(in1);
			var2 = Deal::variance(in2);
			for (int i = 0; i < piece_ht; ++i)
			{
				for (int j = 0; j < wd; ++j)
				{
					fmat.at<Vec3b>(i + y, j + x) = var1 > var2
						? mat1.at<Vec3b>(i + y, j + x) : mat2.at<Vec3b>(i + y, j + x);
				}
			}
			x += wd;
		}
		x = img_wd - piece_wd;
		y += piece_ht;
	}
	if (ht == 0)
		return Mat();
	//下侧剩余
	x = 0;
	y = img_ht - piece_ht;
	while (y < img_ht)
	{
		while (x < img_wd)
		{
			in1.clear(); in2.clear();
			for (int i = 0; i < ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					in1.push_back(mat1.at<Vec3b>(i + y, j + x)[0]);
					in2.push_back(mat2.at<Vec3b>(i + y, j + x)[0]);
				}
			}
			var1 = Deal::variance(in1);
			var2 = Deal::variance(in2);
			for (int i = 0; i < ht; ++i)
			{
				for (int j = 0; j < piece_wd; ++j)
				{
					fmat.at<Vec3b>(i + y, j + x) = var1 > var2
						? mat1.at<Vec3b>(i + y, j + x) : mat2.at<Vec3b>(i + y, j + x);
				}
			}
			x += piece_wd;
		}
		x = 0;
		y += ht;
	}
	//右下侧
	x = img_wd - piece_wd;
	y = img_ht - piece_ht;

	in1.clear(); in2.clear();
	for (int i = 0; i < ht; ++i)
	{
		for (int j = 0; j < wd; ++j)
		{
			in1.push_back(mat1.at<Vec3b>(i + y, j + x)[0]);
			in2.push_back(mat2.at<Vec3b>(i + y, j + x)[0]);
		}
	}
	var1 = Deal::variance(in1);
	var2 = Deal::variance(in2);
	for (int i = 0; i < ht; ++i)
	{
		for (int j = 0; j < wd; ++j)
		{
			fmat.at<Vec3b>(i + y, j + x) = var1 > var2
				? mat1.at<Vec3b>(i + y, j + x) : mat2.at<Vec3b>(i + y, j + x);
		}
	}
	return fmat;
}
