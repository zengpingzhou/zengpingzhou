#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

//类型转换
namespace TypeCast {

	QImage cvMat2QImage(const cv::Mat& mat);

	cv::Mat qimage2Mat(const QImage& image, bool inCloneImageData = true);

}

//读取文件
namespace FileRW {

	enum ReadType {
		GRAY = 0,
		Color = 1,

	};

	cv::Mat readImage(const QString& path, int flags = 1);

	bool writeImage(const QString& path, const cv::Mat& mat);

}