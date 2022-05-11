#include<QWidget>
#include <QFrame>
#include <QGroupBox>


class QLineEdit;
class QPushButton;
class QLabel;
class QVBoxLayout;
class QFontComboBox;
class QComboBox;
class QMenuBar;

class EditGroupBox : public QFrame
{
	Q_OBJECT
public:
	explicit EditGroupBox(QWidget* parent = nullptr);
	~EditGroupBox();

	void setMyText(const QString& text);
	void addWidget(QWidget* widget);

signals:
	void wheelslip(const bool& direction);

protected:
	void wheelEvent(QWheelEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	bool			m_isAdd;
	QVBoxLayout*	m_VLayout;
	QLabel*			m_text;
	double			m_startY;
	double			m_endY;
};

//图像参数标签
class ImageParam : public QGroupBox
{
	Q_OBJECT
public:
	explicit ImageParam(QWidget* parent = nullptr);
	~ImageParam();

public slots:
	void showImageParam(const QStringList& paramList);
	void cleanParam();

private:
	void init();
	void styleSheetInit();

private:
	QVBoxLayout*	 m_hLayout;
	QVector<QLabel*> m_listShow;
};

//左侧菜单
class LeftMenu : public QWidget
{
	Q_OBJECT
public:
	explicit LeftMenu(QWidget* parent = nullptr);
	~LeftMenu();

public slots:
	void pathImage1(const QString& path);
	void pathImage2(const QString& path);
	void cleanParam();

signals:


private:
	void init();
	void styleSheetInit();

private:
	ImageParam* m_image1;
	ImageParam* m_image2;

};

//右侧菜单
class RightMenu : public QWidget
{
	Q_OBJECT
public:
	explicit RightMenu(QWidget* parent = nullptr);
	~RightMenu();

public slots:


signals:
	void evalFuncSelected(int evalType);
	void sizeSelected(int sizeType);
	void effSizeSelected(int sizeType);
	void effPercentChanged(double percent);
	void cleanClicked();
	void saveClicked();
	void fusionClicked();
	void weakenEffectClicked();

private:
	void init();
	void connectionsInit();
	void styleSheetInit();

private:
	QPushButton* m_fusionBtn; //合成按钮
	QPushButton* m_cleanBtn;  //清理按钮
	QPushButton* m_saveBtn;	  //保存按钮
	QPushButton* m_effectBtn;	  //削弱方块效应按钮
	EditGroupBox* m_evalFuncBox;  //选择融合方式
	EditGroupBox* m_sizeBox;  //选择扫描窗口大小
	EditGroupBox* m_effSizeBox;	//削弱方块效应的扫描窗口
	EditGroupBox* m_effPerBox;  //削弱方块效应无效点所占百分比
	QComboBox*	 m_evalSelect;//评价函数
	QComboBox*	 m_sizeSelect;//选择扫描窗口的大小
	QComboBox*	 m_effSize;		//方块效应的扫描窗口大小
	QLineEdit*   m_effEdit;		//方块效应无效点百分比
};

//顶部菜单
class TopMenu : public QWidget
{
	Q_OBJECT
public:
	explicit TopMenu(QWidget* parent = nullptr);
	~TopMenu();

	enum MenuClickedType {
		New,
		Open1,
		Open2,
		Save,
		//...
	};

signals:
	void menuClicked(const MenuClickedType& typeClicked);

public slots:


private:
	void init();
	void styleSheetInit();

private:
	QMenuBar* m_menuBar;

};