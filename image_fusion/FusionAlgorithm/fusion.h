#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>


using namespace std;
using namespace cv;

//��˹������
class GS_Pyramid
{
public:
	GS_Pyramid();
	GS_Pyramid(const int& num, const Size& size = Size(5, 5));
	GS_Pyramid(const int& num, const Mat& img, const Size& size = Size(5, 5));
	~GS_Pyramid();

	//�������任
	void transformation(const Mat& img);
	//���ø�˹���Ĳ���
	void setLevels(const int& num);
	//���ø�˹�˵Ĵ�С
	void setSize(const int& width, const int& height);

	//��ȡͼ�������
	vector<Mat> getPyramid() const;

private:
	int m_level;
	Size m_size;
	vector<Mat> m_pyramid;
};

//������˹������
class LP_Pyramid
{
public:
	LP_Pyramid();
	LP_Pyramid(const GS_Pyramid& gpyramid);
	~LP_Pyramid();

	//�������任
	void transformation(const GS_Pyramid& gpyramid);

	//��ȡͼ�������
	vector<Mat> getPyramid() const;

	//��ȡ����������

private:
	vector<Mat> m_pyramid;
};

class ImageFusion
{
public:
	//���ۺ�������
	enum EvalFunc {
		Var = 0, //����
		SF  = 1,	 
		EOG = 2,
		SML = 3,
	};

public:
	ImageFusion();
	ImageFusion(const Mat& mat1, const Mat& mat2);
	~ImageFusion();

	void fusionImage(const Mat& mat1, const Mat& mat2);

	//ͼ���ں�
	Mat imageFusion(const EvalFunc& type, const Size& size);

	//ȥ������ЧӦ
	void effectWeaken(Mat &fmat, const Size& size, const double& percent = 0.3);

public:
	//��۽�ͼ���ں� (sizeΪ��Ĵ�С)
	Mat multiFocus(const Size& size);

	//��ͨ��
	Mat multiFocus(const Mat& mat1, const Mat& mat2);

	//�Ե�Ϊ����
	Mat multiFocusPoint(const Size& size);

private:
	//�ֿ��ں�
	void multiFocus(const Size& size, Mat& mat_out, int sx, int sy);

	//��ͨ��
	Mat multiFocus_channel(const Mat& mat1, const Mat& mat2);

	//�Ե�λ������ɢ�����ݷ����С�����ں�
	void multiFocusPoint(const Size& size, Mat& mat_out);

	//��ͨ���ڷֿ��ں�
	void multiFocus(const Mat& mat1, const Mat& mat2, Mat& mat_out, const Size& size, int sx, int sy);

private:
	//�Է�����Ϊ���ۺ���
	Mat variance(const Size& size);

	//��SF��Ϊ���ۺ���
	Mat sf(const Size& size);

	//��EOG��Ϊ���ۺ���
	Mat eog(const Size& size);

	//��SML��Ϊ���ۺ���
	Mat sml(const Size& size);

	//�ж��ںϺ��ͼ����һ�������Ƿ�Ϊ����
	void isFocus(Mat& fmat, const Mat& mat1, const Mat& binMat1, const Size& size, double percent);

private:
	int m_imgWd;
	int m_imgHt;

	Mat m_mat1;
	Mat m_mat2;
};


namespace Deal {

	//��ͨ��
	vector<Mat> subchannel(const Mat& mat);

	//�Ա�����ͼ���Զ���ӱ߽�
	void autoaddBoundary(const Mat& mat1_IN, Mat& mat1_OUT, const Mat& mat2_IN, Mat& mat2_OUT);

	//���㷽��
	double variance(const vector<double>& inputArray);

	//�ռ�Ƶ��SF (��Ƶ��RF����Ƶ��CF)
	double spatialFrequency(const Mat& mat, const Size& size, int x, int y);

	//EOG
	double energyGradient(const Mat& mat, const Size& size, int x, int y);

	//������˹���Է�����SML
	double sumOfModifiedLaplacian(const Mat& mat, const Size& size, int x, int y);

	//ͼ���ֵ��
	Mat binaryzation(const Mat& mat);

	//�жϸõ㸽���������Ƿ�ӽ������ں�
	bool isPerfectFusion(const Mat& mat, const Size& size, int x, int y, double percent);

	//�����׼��
	double STD(const Mat& mat, int channel = 0);

}


//ͼ���ں�
namespace Fusion {

	//��Ȩƽ����
	Mat weightAverage(const Mat& mat1, double weight1, const Mat& mat2, double weight2);
	
	//��ͨ�����ݲ�ͬ�ڷֱ��ں�
	 

	//������˹�������ں�


	//��۽�ͼ���ںϣ�����ͼ�����ص�ƽ���̶Ƚ��зֿ��ںϣ���ͨ��
	Mat multiFocus(const Mat& mat1, const Mat& mat2, const Size& size);
	

}
