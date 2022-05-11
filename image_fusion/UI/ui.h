#include <QWidget>
#include <QFrame>
#include "FusionAlgorithm/fusion.h"

class QPushButton;
class QLabel;
class QGroupBox;
class LeftMenu;
class RightMenu;
class TopMenu;
class GS_Pyramid;
class LP_Pyramid;
class ImageFusion;

using namespace std;

const string pathOUT = "C:/Users/admin/Desktop/Image_output/";
const string pathIN = "C:/Users/admin/Desktop/image_dataset/Multi-focusd image fusion/";

//´ýÈÚºÏÍ¼Æ¬ÈÝÆ÷
class MyGroupBox : public QFrame
{
	Q_OBJECT
public:
	explicit MyGroupBox(QWidget* parent = nullptr);
	~MyGroupBox();

	void cleanImage();
	void showImage(const QString& path);
	void imageIsShow(const bool& show);
	//void setMyText(const QString& text);
	//void addWidget(QWidget* widget);

	QString picturePath();
	QSize   pictureSize();

	void insertImage();

signals:
	//void wheelslip(const bool& direction);
	void insertClicked();
	void sendImagePath(const QString& path);
	void sizeChanged();

protected:
	void resizeEvent(QResizeEvent* event) override;
	//void wheelEvent(QWheelEvent* event) override;
	//void mousePressEvent(QMouseEvent* event) override;
	//void mouseReleaseEvent(QMouseEvent* event) override;
	//void mouseMoveEvent(QMouseEvent* event) override;
private:
	void imageAdapt();

private:
	void init();
	void connectionsInit();
	void styleSheetInit();


private:
	int			 m_imgW;
	int			 m_imgH;
	QString		 m_path;
	QSize		 m_imageSize;
	//QLabel*		 m_text;
	QLabel*		 m_showImage;
	QPushButton* m_insertImage;
};

//ÈÚºÏºóÍ¼ÏñÈÝÆ÷
class ImageBox : public QFrame
{
	Q_OBJECT
public:
	explicit ImageBox(QWidget* parent = nullptr);
	~ImageBox();

	void showImage(const QImage& img);
	void cleanImage();
	bool isShow();

signals:
	void sizeChanged();

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	void init();

private:
	void imageAdapt();

private:
	bool m_isShow;
	int m_imgW;
	int m_imgH;
	QLabel* m_imageLab;
};

//Ö÷½çÃæ
class MyUI : public QWidget
{
	Q_OBJECT
public:
	explicit MyUI(QWidget* parent = nullptr);
	~MyUI();

public: //slots:
	bool save(int saveType = 0);
	void weakenTetrisEffect();

signals:
	void sendImageMsg(int sign = 0);
	void sendFusionTime(double time);

private:
	void init();
	void styleSheetInit();
	void connectionsInit();

	void showImageMsg(int sign = 0);

private:
	ImageFusion::EvalFunc m_evalFunc;
	Size	   m_boxSize;
	Size	   m_effSize;
	double	   m_effPer;

	LeftMenu*  m_leftMenu;
	RightMenu* m_rightMenu;
	TopMenu*   m_topMenu;

	MyGroupBox* m_pictureOne;
	MyGroupBox* m_pictureTwo;
	ImageBox*  m_pictureFUSN;

	GS_Pyramid* m_gPyramid;
	LP_Pyramid* m_lPyramid;
	ImageFusion* m_imageFusion;

	Mat	   m_image1;
	Mat	   m_image2;
	QImage m_fImage;
	Mat    m_fmat;
	double m_weight1;
	double m_weight2;

};