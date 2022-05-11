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

//ͼ�������ǩ
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

//���˵�
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

//�Ҳ�˵�
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
	QPushButton* m_fusionBtn; //�ϳɰ�ť
	QPushButton* m_cleanBtn;  //����ť
	QPushButton* m_saveBtn;	  //���水ť
	QPushButton* m_effectBtn;	  //��������ЧӦ��ť
	EditGroupBox* m_evalFuncBox;  //ѡ���ںϷ�ʽ
	EditGroupBox* m_sizeBox;  //ѡ��ɨ�贰�ڴ�С
	EditGroupBox* m_effSizeBox;	//��������ЧӦ��ɨ�贰��
	EditGroupBox* m_effPerBox;  //��������ЧӦ��Ч����ռ�ٷֱ�
	QComboBox*	 m_evalSelect;//���ۺ���
	QComboBox*	 m_sizeSelect;//ѡ��ɨ�贰�ڵĴ�С
	QComboBox*	 m_effSize;		//����ЧӦ��ɨ�贰�ڴ�С
	QLineEdit*   m_effEdit;		//����ЧӦ��Ч��ٷֱ�
};

//�����˵�
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