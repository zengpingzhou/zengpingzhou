#include "menu.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "FileOperation/fileoperation.h"
#include <QPushButton>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QMenuBar>
#include <QFontComboBox>
#include <QWheelEvent>
#include <QLayout>
#include <QDebug>

//编辑框
EditGroupBox::EditGroupBox(QWidget* parent) :
	QFrame(parent)
	, m_isAdd(false)
{
	m_VLayout = new QVBoxLayout(this);
	m_text = new QLabel(this);
	m_VLayout->addWidget(m_text);
	setMaximumHeight(85);
	setMaximumWidth(85);
}

EditGroupBox::~EditGroupBox()
{
}

void EditGroupBox::setMyText(const QString& text)
{
	m_text->setText(text);
}

void EditGroupBox::addWidget(QWidget* widget)
{
	if (false == m_isAdd)
	{
		widget->setMinimumWidth(55);
		widget->setMinimumHeight(35);
		m_VLayout->addWidget(widget);
		m_isAdd = true;
	}
}

void EditGroupBox::wheelEvent(QWheelEvent* event)
{
	if (0 < event->delta())
		emit wheelslip(true);
	else
		emit wheelslip(false);
}

void EditGroupBox::mousePressEvent(QMouseEvent* event)
{
	m_startY = event->localPos().y();
	m_endY = event->localPos().y();
}

void EditGroupBox::mouseReleaseEvent(QMouseEvent* event)
{
	this->setCursor(Qt::ArrowCursor);
}

void EditGroupBox::mouseMoveEvent(QMouseEvent* event)
{
	m_startY = m_endY;
	m_endY = event->localPos().y();
	this->setCursor(Qt::ClosedHandCursor);
	if (m_endY < m_startY)
		emit wheelslip(true);
	else if (m_endY > m_startY)
		emit wheelslip(false);
}

//图像标签
ImageParam::ImageParam(QWidget* parent) :
	QGroupBox(parent)
	, m_hLayout(nullptr)
{
	init();
	styleSheetInit();
}

ImageParam::~ImageParam()
{
}

void ImageParam::init()
{
	setMaximumWidth(140);
	setMinimumWidth(80);
	m_hLayout = new QVBoxLayout(this);
}

void ImageParam::styleSheetInit()
{
	cleanParam();
}

void ImageParam::showImageParam(const QStringList& paramList)
{
	if (nullptr != m_hLayout)
	{
		delete m_hLayout;
		m_hLayout = nullptr;
	}
	m_hLayout = new QVBoxLayout(this);
	if (!m_listShow.isEmpty())
	{
		for (int i = 0; i < m_listShow.size(); ++i)
		{
			delete m_listShow[i];
		}
		m_listShow.clear();
	}
	for (QString str : paramList)
	{
		QLabel* lab = new QLabel();
		lab->setText(str);
		lab->setMinimumWidth(80);
		//lab->setMaximumWidth(100);
		lab->setMaximumHeight(40);
		lab->setMinimumHeight(30);
		m_listShow.push_back(lab);
	}
	for(int i=0; i<m_listShow.size(); ++i)
	{
		m_hLayout->addWidget(m_listShow[i]);
	}
	m_hLayout->addStretch();
}

void ImageParam::cleanParam()
{
	if (!m_listShow.isEmpty())
	{
		for (int i = 0; i < m_listShow.size(); ++i)
		{
			delete m_listShow[i];
		}
	}
	m_listShow.clear();
}

//左侧菜单
LeftMenu::LeftMenu(QWidget* parent):
	QWidget(parent)
{
	init();
	styleSheetInit();
}

LeftMenu::~LeftMenu()
{
}

void LeftMenu::pathImage1(const QString& path)
{
	cv::Mat image = FileRW::readImage(path);
	int cols = image.cols;
	int rows = image.rows;
	QFileInfo file(path);
	QStringList textList;
	textList.push_back(file.fileName());
	textList.push_back(tr("width: ") + QString::number(cols));
	textList.push_back(tr("height: ") + QString::number(rows));
	m_image1->showImageParam(textList);
}

void LeftMenu::pathImage2(const QString& path)
{
	cv::Mat image = FileRW::readImage(path);
	int cols = image.cols;
	int rows = image.rows;
	QFileInfo file(path);
	QStringList textList;
	textList.push_back(file.fileName());
	textList.push_back(tr("width: ") + QString::number(cols));
	textList.push_back(tr("height: ") + QString::number(rows));
	m_image2->showImageParam(textList);
}

void LeftMenu::cleanParam()
{
	m_image1->cleanParam();
	m_image2->cleanParam();
}

void LeftMenu::init()
{
	setMaximumWidth(150);
	setMinimumWidth(100);
	m_image1 = new ImageParam(this);
	m_image2 = new ImageParam(this);
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->addWidget(m_image1);
	vLayout->addWidget(m_image2);
}

void LeftMenu::styleSheetInit()
{
	setStyleSheet(R"(
		QGroupBox{
			font: 12pt 'Microsoft YaHei';
			color: #ffffff;
			border: none;
			background-color: #3b3b3b;
		}
		QLabel{
			font: 8pt 'Microsoft YaHei';
			color: #ffff00;
			background-color: #3b3b3b;
		}
	)");
}

//右侧菜单
RightMenu::RightMenu(QWidget* parent) :
	QWidget(parent)
	, m_fusionBtn(nullptr)
	, m_cleanBtn(nullptr)
	, m_saveBtn(nullptr)
	, m_effectBtn(nullptr)
	, m_evalFuncBox(nullptr)
	, m_sizeBox(nullptr)
	, m_effSizeBox(nullptr)
	, m_effPerBox(nullptr)
	, m_evalSelect(nullptr)
	, m_sizeSelect(nullptr)
	, m_effSize(nullptr)
	, m_effEdit(nullptr)
{
	init();
	connectionsInit();
	styleSheetInit();
}

RightMenu::~RightMenu()
{
}

void RightMenu::init()
{
	setMaximumWidth(200);
	setMinimumWidth(180);
	QVBoxLayout* Vlayout = new QVBoxLayout(this);

	//第一行
	QHBoxLayout* Hlayout1 = new QHBoxLayout();
	//选择评价函数
	m_evalFuncBox = new EditGroupBox(this);
	m_evalSelect = new QComboBox(this);
	QStringList evalList = { "Var", "SF", "EOG","SML" };
	m_evalSelect->addItems(evalList);
	m_evalSelect->setCurrentIndex(2);
	m_evalFuncBox->addWidget(m_evalSelect);
	m_evalFuncBox->setMyText(tr("Evalfunc"));
	Hlayout1->addWidget(m_evalFuncBox);

	//选择窗口大小
	m_sizeBox = new EditGroupBox(this);
	m_sizeSelect = new QComboBox(this);
	QStringList sizeList = { "3*3", "5*5", "7*7", "9*9" };
	m_sizeSelect->addItems(sizeList);
	m_sizeSelect->setCurrentIndex(1);
	m_sizeBox->addWidget(m_sizeSelect);
	m_sizeBox->setMyText(tr("Size"));
	Hlayout1->addWidget(m_sizeBox);
	Vlayout->addLayout(Hlayout1);

	//第二行
	QHBoxLayout* Hlayout2 = new QHBoxLayout();
	//选择削弱方块效应窗口大小
	m_effSizeBox = new EditGroupBox(this);
	m_effSize = new QComboBox(this);
	QStringList sizeList2 = {"5*5", "7*7", "9*9", "11*11"};
	m_effSize->addItems(sizeList2);
	m_effSize->setCurrentIndex(2);
	m_effSizeBox->addWidget(m_effSize);
	m_effSizeBox->setMyText(tr("Eff size"));
	Hlayout2->addWidget(m_effSizeBox);

	//无效点百分比
	m_effPerBox = new EditGroupBox(this);
	m_effEdit = new QLineEdit(this);
	m_effEdit->setAlignment(Qt::AlignHCenter);
	m_effEdit->setValidator(new QDoubleValidator(0, 1, 1, this));
	m_effEdit->setText(QString::number(0.2, 'f', 1));
	m_effEdit->setEnabled(false);
	m_effPerBox->addWidget(m_effEdit);
	m_effPerBox->setMyText(tr("Eff ratio"));
	Hlayout2->addWidget(m_effPerBox);
	Vlayout->addLayout(Hlayout2);

	Vlayout->addStretch();
	//--------------
	QHBoxLayout* Hlayout3 = new QHBoxLayout();
	const QSize size(80,50);
	//融合按钮
	m_fusionBtn = new QPushButton(this);
	m_fusionBtn->setText(tr("fusion"));
	m_fusionBtn->setMinimumSize(size);
	m_fusionBtn->setMaximumSize(size);
	Hlayout3->addWidget(m_fusionBtn);

	//清除图片
	m_cleanBtn  = new QPushButton(this);
	m_cleanBtn->setText(tr("clean"));
	m_cleanBtn->setMinimumSize(size);
	m_cleanBtn->setMaximumSize(size);
	Hlayout3->addWidget(m_cleanBtn);
	Vlayout->addLayout(Hlayout3);

	//-------------
	QHBoxLayout* Hlayout4 = new QHBoxLayout();
	
	//削弱方块效应
	m_effectBtn = new QPushButton(this);
	m_effectBtn->setText(tr("clear"));
	m_effectBtn->setMinimumSize(size);
	m_effectBtn->setMaximumSize(size);
	Hlayout4->addWidget(m_effectBtn);

	//保存融合后的图片
	m_saveBtn = new QPushButton(this);
	m_saveBtn->setText(tr("save")); 
	m_saveBtn->setMinimumSize(size);
	m_saveBtn->setMaximumSize(size);
	Hlayout4->addWidget(m_saveBtn);
	Vlayout->addLayout(Hlayout4);

	//Vlayout->addStretch();
}

void RightMenu::connectionsInit()
{
	connect(m_fusionBtn, &QPushButton::clicked, this, &RightMenu::fusionClicked);
	connect(m_cleanBtn, &QPushButton::clicked, this, &RightMenu::cleanClicked);
	connect(m_saveBtn, &QPushButton::clicked, this, &RightMenu::saveClicked);
	connect(m_effectBtn, &QPushButton::clicked, this, &RightMenu::weakenEffectClicked);
	connect(m_evalSelect, &QComboBox::textActivated, [=](const QString& text) {
		if ("Var" == text)
			emit evalFuncSelected(0);
		else if ("SF" == text)
			emit evalFuncSelected(1);
		else if ("EOG" == text)
			emit evalFuncSelected(2);
		else if ("SML" == text)
			emit evalFuncSelected(3);
	});
	connect(m_sizeSelect, &QComboBox::textActivated, [=](const QString& text) {
		if ("3*3" == text)
			emit sizeSelected(0);
		else if ("5*5" == text)
			emit sizeSelected(1);
		else if ("7*7" == text)
			emit sizeSelected(2);
		else if ("9*9" == text)
			emit sizeSelected(3);
	});
	connect(m_effSize, &QComboBox::textActivated, [=](const QString& text) {
		if ("5*5" == text)
			emit effSizeSelected(0);
		else if ("7*7" == text)
			emit effSizeSelected(1);
		else if ("9*9" == text)
			emit effSizeSelected(2);
		else if ("11*11" == text)
			emit effSizeSelected(3);
	});
	connect(m_effPerBox, &EditGroupBox::wheelslip, [=](const bool& direction) {
		double per = m_effEdit->text().toDouble();
		direction ? per += 0.1 : per -= 0.1;
		0.1 > per ? per = 0.1 : NULL;
		0.9 < per ? per = 0.9 : NULL;
		m_effEdit->setText(QString::number(per, 'f', 1));
		emit effPercentChanged(per);
	});
}

void RightMenu::styleSheetInit()
{
	setStyleSheet(R"(
		QLabel{
			color: #ffffff;
			font: 9pt 'Microsoft YaHei';
			background-color: #5b5b5b;
		}
		QLineEdit{
			border: none;
			border-radius: 3px;
			color: #ffff00;
			font: 9pt 'Microsoft YaHei';
			background-color: #6b6b6b;
		}
		QPushButton{
			color: #ffffff;
			font: 9pt 'Microsoft YaHei';
			background-color: #5b5b5b;
		}
		QPushButton:hover{
			color: #ffffff;
			font: 9pt 'Microsoft YaHei';
			background-color: #6b6b6b;
		}
		QComboBox{
			border: none;
			color: #ffff00;
			font: 9pt 'Microsoft YaHei';
			background-color: #5b5b5b;
		}
	)");
}

//顶部菜单
TopMenu::TopMenu(QWidget* parent) :
	QWidget(parent)
{
	init();
	styleSheetInit();
}

TopMenu::~TopMenu()
{
}

void TopMenu::init()
{
	setMaximumHeight(30);
	setMinimumHeight(30);
	m_menuBar = new QMenuBar(this);
	//添加菜单
	QMenu* pFile = m_menuBar->addMenu(tr("File"));	//文件
	QMenu* pEdit = m_menuBar->addMenu(tr("Eidt"));	//编辑
	//添加菜单项，添加动作
	QAction* pNew = pFile->addAction(tr("New"));	//新建
	pFile->addSeparator();
	
	QMenu* pOpenM = pFile->addMenu(tr("Open"));		//打开
	QAction* pOpen1 = pOpenM->addAction(tr("Open Image1"));
	pOpenM->addSeparator();
	QAction* pOpen2 = pOpenM->addAction(tr("Open Image2"));

	pFile->addSeparator();
	QAction* pSave = pFile->addAction(tr("Save"));	//保存

	connect(pNew, &QAction::triggered, [=]() {
		emit menuClicked(MenuClickedType::New);
		});
	connect(pOpen1, &QAction::triggered, [=]() {
		emit menuClicked(MenuClickedType::Open1);
		});
	connect(pOpen2, &QAction::triggered, [=]() {
		emit menuClicked(MenuClickedType::Open2);
		});
	connect(pSave, &QAction::triggered, [=]() {
		emit menuClicked(MenuClickedType::Save);
		});
}

void TopMenu::styleSheetInit()
{
	setStyleSheet(R"(
		QWidget{
			background-color: #3b3b3b;
			font: 9pt 'Microsoft YaHei';
			color: #ffffff;
		}
		QMenu::item{
			background-color: #3b3b3b;
		}
		QMenu::item:selected{
			background-color: #5b5b5b;
		}
	)");
}
