#include "ui.h"
#include "menu.h"
#include "FileOperation/fileoperation.h"
#include <iostream>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QLayout>
#include <QFileDialog>
#include <ctime>
#include <QDebug>

const QString PATH_IMAGE = "border-image: url(";

MyGroupBox::MyGroupBox(QWidget* parent) :
	QFrame(parent)
	, m_insertImage(nullptr)
	, m_showImage(nullptr)
	, m_path("")
	, m_imgW(250)
	, m_imgH(250)
{
	init();
	connectionsInit();
	styleSheetInit();
}

MyGroupBox::~MyGroupBox()
{

}

void MyGroupBox::cleanImage()
{
	imageIsShow(false);
	m_path = "";
}

void MyGroupBox::showImage(const QString& path)
{
	QImage img;
	if (!img.load(path))
		return ;
	m_showImage->setPixmap(QPixmap::fromImage(img));
	m_imgW = img.width();
	m_imgH = img.height();
	imageAdapt();
	imageIsShow(true);
}

void MyGroupBox::imageIsShow(const bool& show)
{
	if (show)
	{
		m_insertImage->hide();
		m_showImage->show();
	}
	else
	{
		m_insertImage->show();
		m_showImage->hide();
	}
}

QString MyGroupBox::picturePath()
{
	return m_path;
}

QSize MyGroupBox::pictureSize()
{
	return m_imageSize;
}

void MyGroupBox::insertImage()
{
	QString fname = QFileDialog::getOpenFileName(this,
		tr("Select image"),
		"",
		tr("Images (*.png *.jpg *.bmp)"));
	if (fname.isEmpty())
	{
		return;
	}
	showImage(fname);
	m_path = fname;
	emit sendImagePath(fname);
}

void MyGroupBox::resizeEvent(QResizeEvent* event)
{
	emit sizeChanged();
}

void MyGroupBox::imageAdapt()
{
	int wd = this->width();
	int ht = this->height();
	double scale = 0.9;
	if (m_imgW <= wd && m_imgH <= ht)
	{
		m_showImage->setMinimumSize(m_imgW * scale, m_imgH * scale);
		m_showImage->setMaximumSize(m_imgW * scale, m_imgH * scale);
	}
	else
	{
		double scaleW = double(wd) / m_imgW;
		double scaleH = double(ht) / m_imgH;
		//图像容器的宽高与图像的宽高比例，比例小的作为自适应因子
		if (scaleW > scaleH)
		{
			m_showImage->setMinimumSize(m_imgW * scaleH * scale, m_imgH * scaleH * scale);
			m_showImage->setMaximumSize(m_imgW * scaleH * scale, m_imgH * scaleH * scale);
		}
		else
		{
			m_showImage->setMinimumSize(m_imgW * scaleW * scale, m_imgH * scaleW * scale);
			m_showImage->setMaximumSize(m_imgW * scaleW * scale, m_imgH * scaleW * scale);
		}
	}
}

void MyGroupBox::init()
{
	QHBoxLayout* layout_image = new QHBoxLayout(this);
	m_insertImage = new QPushButton(this);
	m_showImage   = new QLabel(this);
	m_showImage->setScaledContents(true);
	m_insertImage->setMaximumSize(200, 200);
	m_insertImage->setMinimumSize(200, 200);
	layout_image->addWidget(m_insertImage);
	layout_image->addWidget(m_showImage);
	imageIsShow(false);
}

void MyGroupBox::connectionsInit()
{
	//connect(m_insertImage, &QPushButton::clicked, [=]() {
	//	QString fname = QFileDialog::getOpenFileName(this,
	//		tr("Select image"),
	//		"",
	//		tr("Images (*.png *.jpg *.bmp)"));
	//	if (fname.isEmpty())
	//	{
	//		return;
	//	}
	//	showImage(fname);
	//	m_path = fname;
	//	emit sendImagePath(fname);
	//});
	connect(m_insertImage, &QPushButton::clicked, this, &MyGroupBox::insertImage);
	connect(this, &MyGroupBox::sizeChanged, this, &MyGroupBox::imageAdapt);

}

void MyGroupBox::styleSheetInit()
{
	setStyleSheet(R"(
		QPushButton{
			background-color: #6b6b6b;
			border: none;
			border-image: url(:/res/image/picture1.svg)
		}
	)");
}

ImageBox::ImageBox(QWidget* parent) :
	QFrame(parent)
	, m_imageLab(nullptr)
	, m_imgH(250)
	, m_imgW(250)
	, m_isShow(false)
{
	init();
}

ImageBox::~ImageBox()
{
}

void ImageBox::showImage(const QImage& img)
{
	m_imgW = img.width();
	m_imgH = img.height();
	imageAdapt();
	m_imageLab->setPixmap(QPixmap::fromImage(img));
	m_isShow = true;
}

void ImageBox::cleanImage()
{
	m_imageLab->clear();
	m_isShow = false;
}

bool ImageBox::isShow()
{
	return m_isShow;
}

void ImageBox::resizeEvent(QResizeEvent* event)
{
	emit sizeChanged();
}

void ImageBox::init()
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	m_imageLab = new QLabel(this);
	m_imageLab->setScaledContents(true);
	layout->addWidget(m_imageLab);

	connect(this, &ImageBox::sizeChanged, this, &ImageBox::imageAdapt);
}

void ImageBox::imageAdapt()
{
	int wd = this->width();
	int ht = this->height();
	double scale = 0.9;
	if (m_imgW <= wd && m_imgH <= ht)
	{
		m_imageLab->setMinimumSize(m_imgW * scale, m_imgH * scale);
		m_imageLab->setMaximumSize(m_imgW * scale, m_imgH * scale);
	}
	else
	{
		double scaleW = double(wd) / m_imgW;
		double scaleH = double(ht) / m_imgH;
		//图像容器的宽高与图像的宽高比例，比例小的作为自适应因子
		if (scaleW > scaleH)
		{
			m_imageLab->setMinimumSize(m_imgW * scaleH * scale, m_imgH * scaleH * scale);
			m_imageLab->setMaximumSize(m_imgW * scaleH * scale, m_imgH * scaleH * scale);
		}
		else
		{
			m_imageLab->setMinimumSize(m_imgW * scaleW * scale, m_imgH * scaleW * scale);
			m_imageLab->setMaximumSize(m_imgW * scaleW * scale, m_imgH * scaleW * scale);
		}
	}
}

MyUI::MyUI(QWidget* parent):
	QWidget(parent)
	, m_leftMenu(nullptr)
	, m_rightMenu(nullptr)
	, m_topMenu(nullptr)
	, m_pictureOne(nullptr)
	, m_pictureTwo(nullptr)
	, m_pictureFUSN(nullptr)
	, m_gPyramid(nullptr)
	, m_lPyramid(nullptr)
	, m_imageFusion(nullptr)
	, m_weight1(0.5)
	, m_weight2(0.5)
	, m_evalFunc(ImageFusion::EvalFunc::EOG)
	, m_boxSize(5,5)
	, m_effSize(9,9)
	, m_effPer(0.2)
{
	init();
	setWindowTitle(tr("Multi-focus image fusion "));
	connectionsInit();
	styleSheetInit();
}

MyUI::~MyUI()
{
	if (m_gPyramid != nullptr)
		delete m_gPyramid;
	if (m_lPyramid != nullptr)
		delete m_lPyramid;
	if (m_imageFusion != nullptr)
		delete m_imageFusion;
}

bool MyUI::save(int saveType)
{
	if (!m_pictureFUSN->isShow())
		return false;
	QString str;
	switch (saveType)
	{
	case 0:
		str = "Images (*.jpg)";
		break;
	case 1:
		str = "Images (*.png)";
		break;
	case 2:
		str = "Images (*.bmp)";
		break;
	default:
		str = "Images (*.jpg)";
		break;
	}
	QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save as"), "", str);
	if (fileName.isEmpty())
		return false;
	return FileRW::writeImage(fileName, m_fmat);
}

void MyUI::weakenTetrisEffect()
{
	if (!m_pictureFUSN->isShow())
		return ;
	m_imageFusion->effectWeaken(m_fmat, m_effSize, m_effPer);
	m_fImage = TypeCast::cvMat2QImage(m_fmat);
	m_pictureFUSN->showImage(m_fImage);
	qDebug() << "after weakenTetris STD = " << Deal::STD(m_fmat);
	emit sendImageMsg(3);
}

void MyUI::init()
{
	m_gPyramid = new GS_Pyramid;
	m_lPyramid = new LP_Pyramid;
	m_imageFusion = new ImageFusion;

	m_leftMenu  = new LeftMenu(this);
	m_rightMenu = new RightMenu(this);
	m_topMenu   = new TopMenu(this);

	m_pictureOne  = new MyGroupBox(this);
	m_pictureTwo  = new MyGroupBox(this);
	m_pictureFUSN = new ImageBox(this);
	//限制布局宽度
	m_pictureOne->setMinimumSize(250, 250);
	m_pictureOne->setMaximumSize(600, 450);
	m_pictureTwo->setMinimumSize(250, 250);
	m_pictureTwo->setMaximumSize(600, 450);
	m_pictureFUSN->setMinimumWidth(250);

	QVBoxLayout* all_layout = new QVBoxLayout(this);
	QHBoxLayout* hlayout = new QHBoxLayout();
	QVBoxLayout* vlayout = new QVBoxLayout();

	//将左侧菜单添加到水平布局
	hlayout->addWidget(m_leftMenu);
	//将图片1添加到垂直布局
	vlayout->addWidget(m_pictureOne);
	//将图片2添加到垂直布局
	vlayout->addWidget(m_pictureTwo);
	//将垂直布局添加到水平布局
	hlayout->addLayout(vlayout);
	//将合成图片添加到水平布局
	hlayout->addWidget(m_pictureFUSN);
	//将右侧菜单添加到水平布局
	hlayout->addWidget(m_rightMenu);

	//将顶部菜单添加到整体布局
	all_layout->addWidget(m_topMenu);
	//将水平布局添加到整体布局
	all_layout->addLayout(hlayout);
}

void MyUI::connectionsInit()
{
	connect(m_topMenu, &TopMenu::menuClicked, [=](const TopMenu::MenuClickedType& typeClicked) {
		switch (typeClicked)
		{
		case TopMenu::New:
			m_pictureOne->cleanImage();
			m_pictureTwo->cleanImage();
			m_pictureFUSN->cleanImage();
			m_leftMenu->cleanParam();
			break;
		case TopMenu::Open1:
			m_pictureOne->insertImage();
			break;
		case TopMenu::Open2:
			m_pictureTwo->insertImage();
			break;
		case TopMenu::Save:
			if (save(0))
				qDebug() << "save successed";
			else
				qDebug() << "save failed";
			break;
		default:
			break;
		}
	});
	connect(m_rightMenu, &RightMenu::evalFuncSelected, [=](int eval) {
		switch (eval)
		{
		case 0:
			m_evalFunc = ImageFusion::EvalFunc::Var;
			break;
		case 1:
			m_evalFunc = ImageFusion::EvalFunc::SF;
			break;
		case 2:
			m_evalFunc = ImageFusion::EvalFunc::EOG;
			break;
		case 3:
			m_evalFunc = ImageFusion::EvalFunc::SML;
			break;
		default:
			break;
		}
	});
	connect(m_rightMenu, &RightMenu::sizeSelected, [=](int sizeType) {
		switch (sizeType)
		{
		case 0:
			m_boxSize = Size(3, 3);
			break;
		case 1:
			m_boxSize = Size(5, 5);
			break;
		case 2:
			m_boxSize = Size(7, 7);
			break;
		case 3:
			m_boxSize = Size(9, 9);
			break;
		default:
			break;
		}
	});
	connect(m_rightMenu, &RightMenu::effSizeSelected, [=](int sizeType) {
		switch (sizeType)
		{
		case 0:
			m_effSize = Size(5, 5);
			break;
		case 1:
			m_effSize = Size(7, 7);
			break;
		case 2:
			m_effSize = Size(9, 9);
			break;
		case 3:
			m_effSize = Size(11, 11);
			break;
		default:
			break;
		}
	});
	connect(m_rightMenu, &RightMenu::effPercentChanged, [=](double percent) {
		m_effPer = percent;
	});

	connect(m_rightMenu, &RightMenu::fusionClicked, [=]() {
		if (m_pictureOne->picturePath().isEmpty() || m_pictureTwo->picturePath().isEmpty())
			return; 
		m_image1 = FileRW::readImage(m_pictureOne->picturePath());
		m_image2 = FileRW::readImage(m_pictureTwo->picturePath());
		if (m_image1.size() != m_image2.size())
		{
			emit sendImageMsg(1);
			return;
		}
		m_imageFusion->fusionImage(m_image1, m_image2);
		clock_t startT = clock();
		Mat imageF = m_imageFusion->imageFusion(m_evalFunc, m_boxSize);
		sendFusionTime(double(clock() - startT));
		if (0 == imageF.rows)
		{
			emit sendImageMsg(2);
			return;
		}
		m_fmat = imageF;
		qDebug() << "STD = " << Deal::STD(m_fmat);
		m_fImage = TypeCast::cvMat2QImage(imageF);
		m_pictureFUSN->showImage(m_fImage);
		emit sendImageMsg(0);
	});

	connect(m_rightMenu, &RightMenu::cleanClicked, [=]() {
		m_pictureOne->cleanImage();
		m_pictureTwo->cleanImage();
		m_pictureFUSN->cleanImage();
		m_leftMenu->cleanParam();
	});
	connect(m_rightMenu, &RightMenu::weakenEffectClicked, this, &MyUI::weakenTetrisEffect);
	connect(m_rightMenu, &RightMenu::saveClicked, [=]() {
		if (save(0))
			qDebug() << "save successed";
		else
			qDebug() << "save failed";
	});
	connect(m_pictureOne, &MyGroupBox::sendImagePath, m_leftMenu, &LeftMenu::pathImage1);
	connect(m_pictureTwo, &MyGroupBox::sendImagePath, m_leftMenu, &LeftMenu::pathImage2);
	connect(this, &MyUI::sendImageMsg, this, &MyUI::showImageMsg);
	connect(this, &MyUI::sendFusionTime, [=](double time) { 
		qDebug() << "The fusion took " << time << " ms";
	});
}

void MyUI::showImageMsg(int sign)
{
	QMessageBox mBox;
	mBox.setWindowTitle(tr("Notice"));
	switch (sign)
	{
	case 1:
		mBox.setText(tr("The image is not registered"));
		break;
	case 2:
		mBox.setText(tr("Image fusion failure"));
		break;
	case 3:
		mBox.setText(tr("Block effect has been reduced"));
		break;
	default:
		mBox.setText(tr("Image fusion completed"));
		break;
	}
	mBox.setStandardButtons(QMessageBox::StandardButton::Ok);
	mBox.setButtonText(QMessageBox::StandardButton::Ok, tr("Ok"));
	mBox.exec();
}

void MyUI::styleSheetInit()
{
	setStyleSheet(R"(
		QWidget{
			background-color: #3b3b3b;
		}
		QFrame{
			background-color: #6b6b6b;
		}
		EditGroupBox{
			border: none;
			border-radius: 6px;
			background-color: #5b5b5b;
		}
	)");

}
