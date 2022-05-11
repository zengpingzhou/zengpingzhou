#include "fileoperation.h"
#include <QDebug>

QImage TypeCast::cvMat2QImage(const cv::Mat& mat)
{

    switch (mat.type())
    {
        // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);
        return image;
    }

    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }

    // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;
        // only create our color table once
        if (sColorTable.isEmpty())
        {
            for (int i = 0; i < 256; ++i)
                sColorTable.push_back(qRgb(i, i, i));
        }
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        image.setColorTable(sColorTable);
        return image;
    }

    default:
        qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
        break;
    }
    return QImage();
}

cv::Mat TypeCast::qimage2Mat(const QImage& image, bool inCloneImageData)
{
    switch (image.format())
    {
        // 8-bit, 4 channel
    case QImage::Format_RGB32:
    {
        cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        return (inCloneImageData ? mat.clone() : mat);
    }

    // 8-bit, 3 channel
    case QImage::Format_RGB888:
    {
        if (!inCloneImageData) {
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";
        }
        QImage swapped = image.rgbSwapped();
        return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
    }

    // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat  mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine());

        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        qDebug("Image format is not supported: depth=%d and %d format\n", image.depth(), image.format());
        break;
    }

    return cv::Mat();
}

cv::Mat FileRW::readImage(const QString& path, int flags)
{
    std::string std_path = path.toStdString();
    return cv::imread(std_path, flags);
}

bool FileRW::writeImage(const QString& path, const cv::Mat& mat)
{
    std::string std_path = path.toStdString();
    return cv::imwrite(std_path, mat);
}
