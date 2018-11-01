#include "basepropertywidget.h"
#include <QPushButton>
#include <QDebug>
#include <QPainter>


BasePropertyWidget::BasePropertyWidget(QWidget * parent) : QWidget(parent)
{
	QFont font;
	font.setPointSizeF(font.pointSize() * devicePixelRatioF());
	setFont(font);


	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	displayName = new QLineEdit;
	displayName->setText("Display");
	displayName->setFont(font);
//	displayName->setAlignment(Qt::AlignHCenter);

	button = new QPushButton;
	button->setMaximumSize(14, 14);
	button->setIcon(QIcon(":/images/delete-26.png"));
	button->setIconSize(button->maximumSize());
	button->setCursor(Qt::PointingHandCursor);

	auto minimize = new QPushButton;
	minimize->setMaximumSize(button->maximumSize());
	minimize->setIcon(QIcon(":/icons/contract.png"));
	minimize->setIconSize(button->maximumSize());
	minimize->setCursor(Qt::PointingHandCursor);


	auto btn = new QPushButton;
	btn->setIcon(QIcon(":/icons/up.png"));
	btn->setIconSize(QSize(14, 14));

	displayWidget = new HeaderObject;
	displayWidget->setStyleSheet("background: rgba(0,0,0,0);");
	auto displayLayout = new QHBoxLayout;
	displayWidget->setLayout(displayLayout);
	/*displayLayout->addStretch();*/ 
//	displayLayout->addWidget(btn);
	displayLayout->addWidget(displayName);
	displayLayout->addStretch();
	displayLayout->addWidget(minimize);
	displayLayout->addSpacing(2);
	displayLayout->addWidget(button);
	displayLayout->addSpacing(4);
	displayLayout->setContentsMargins(5, 1, 2, 1);
	displayWidget->setCursor(Qt::OpenHandCursor);




	layout = new QVBoxLayout;
	auto mainLayout = new QVBoxLayout;
	mainLayout->addWidget(displayWidget);
	mainLayout->addLayout(layout);
	mainLayout->setContentsMargins(5, 0, 5, 0);

	setLayout(mainLayout);

	connect(button, &QPushButton::clicked, [=]() {
		emit buttonPressed();
	});
	connect(minimize, &QPushButton::clicked, [=]() {
		if (minimized) {
			// maximize
			emit shouldSetVisible(minimized);
			minimize->setIcon(QIcon(":/icons/contract.png"));
			minimized = !minimized;

		}
		else {
			// minimize
			emit shouldSetVisible(minimized);
			minimize->setIcon(QIcon(":/icons/expand.png"));

			minimized = !minimized;
		}
	});
	connect(btn, &QPushButton::clicked, [=]() {
		if (minimized) {
			// maximize
			emit shouldSetVisible(minimized);
			minimize->setIcon(QIcon(":/icons/up.png"));
			minimized = !minimized;

		}
		else {
			// minimize
			emit shouldSetVisible(minimized);
			minimize->setIcon(QIcon(":/icons/down.png"));

			minimized = !minimized;
		}
	});

	setStyleSheet("QMenu{	background: rgba(26,26,26,.9); color: rgba(250,250, 250,.9);}"
		"QMenu::item{padding: 2px 5px 2px 20px;	}"
		"QMenu::item:hover{	background: rgba(40,128, 185,.9);}"
		"QMenu::item:selected{	background: rgba(40,128, 185,.9);}"
		"QCheckBox {   spacing: 2px 5px;}"
		"QCheckBox::indicator {   width: 28px;   height: 28px; }"
		"QCheckBox::indicator::unchecked {	image: url(:/icons/check-unchecked.png);}"
		"QCheckBox::indicator::checked {		image: url(:/icons/check-checked.png);}"
		"QLineEdit {	border: 0;	background: #292929;	padding: 6px;	margin: 0;}"
		"QToolButton {	background: #1E1E1E;	border: 0;	padding: 6px;}"
		"QToolButton:pressed {	background: #111;}"
		"QToolButton:hover {	background: #404040;}"
		"QDoubleSpinBox, QSpinBox {	border-radius: 1px;	padding: 6px;	background: #292929;}"
		"QDoubleSpinBox::up-arrow, QSpinBox::up-arrow {	width:0;}"
		"QDoubleSpinBox::up-button, QSpinBox::up-button, QDoubleSpinBox::down-button, QSpinBox::down-button {	width:0;}"
		"QComboBox:editable {}"
		"QComboBox QAbstractItemView::item {    show-decoration-selected: 1;}"
		"QComboBox QAbstractItemView::item {    padding: 6px;}"
		"QListView::item:selected {    background: #404040;}"
		"QComboBox:!editable, QComboBox::drop-down:editable {     background: #1A1A1A;}"
		"QComboBox:!editable:on, QComboBox::drop-down:editable:on {    background: #1A1A1A;}"
		"QComboBox QAbstractItemView {    background-color: #1A1A1A;    selection-background-color: #404040;    border: 0;    outline: none;}"
		"QComboBox QAbstractItemView::item {    border: none;    padding-left: 5px;}"
		"QComboBox QAbstractItemView::item:selected {    background: #404040;    padding-left: 5px;}"
		"QComboBox::drop-down {    subcontrol-origin: padding;    subcontrol-position: top right;    width: 18px;    border-left-width: 1px;}"
		"QComboBox::down-arrow {    image: url(:/icons/down_arrow_check.png);	width: 18px;	height: 14px;} "
		"QComboBox::down-arrow:!enabled {    image: url(:/icons/down_arrow_check_disabled.png);    width: 18px;    height: 14px;}"
		"QLabel{}"
		"QPushButton{padding : 3px; }"
	);

	auto visibilityBtn = new QPushButton;
	visibilityBtn->setText(tr("minimize"));
	visibilityBtn->setCursor(Qt::PointingHandCursor);
	connect(visibilityBtn, &QPushButton::clicked, [=]() {
		if (minimized) {
			// maximize
		//	emit shouldSetVisible(minimized);
			visibilityBtn->setText(tr("minimize"));
			minimized = !minimized;
			animateMaximize();
		}
		else {
			 // minimize
		//	emit shouldSetVisible(minimized);
			visibilityBtn->setText(tr("maximize"));
			minimized = !minimized;
	

		}
	});

//	mainLayout->addWidget(visibilityBtn);

	visibilityBtn->setStyleSheet(""
		"QPushButton{ background: rgba(23,23,23,1); border: .5px solid rgba(0,0,0,1); }"
		"QPushButton:hover{  border: .5px solid rgba(50,150,250,.2); }"
	);

	
}


BasePropertyWidget::~BasePropertyWidget()
{
}


void BasePropertyWidget::setWidget(QWidget * widget)
{
	contentWidget = widget;
	anim = new QPropertyAnimation(contentWidget, "size");
	currentSize = contentWidget->size();

	anim->setDuration(300);
	anim->setEasingCurve(QEasingCurve::BezierSpline);
	connect(anim, &QPropertyAnimation::valueChanged, [=]() {
		update();
		contentWidget->update();
	});
}


void BasePropertyWidget::animateMinimize()
{
	currentSize = contentWidget->size();
	anim->setStartValue(currentSize);
	anim->setEndValue(QSize(currentSize.width(), 0));
	anim->start();
}

void BasePropertyWidget::animateMaximize()
{
	anim->setEndValue(currentSize);
	anim->setStartValue(QSize(currentSize.width(), 0));
	anim->start();
}

void BasePropertyWidget::paintEvent(QPaintEvent * event)
{
	QWidget::paintEvent(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// draw title border
	QPainterPath path;
	path.addRoundedRect(0, 0, width(), displayWidget->height(), 5, 5);
	painter.fillPath(path, QColor(22, 22, 22));

	//draw outline
	painter.setPen(QPen(QColor(70, 70, 70), 2));
	painter.drawRoundedRect(0, 0, width(), height(),5,5);

}

void BasePropertyWidget::mouseMoveEvent(QMouseEvent * event)
{
	pressed = true;
	QWidget::mouseMoveEvent(event);
}

void BasePropertyWidget::mousePressEvent(QMouseEvent * event)
{
	emit currentWidget(this);
	QWidget::mousePressEvent(event);
}

void BasePropertyWidget::mouseReleaseEvent(QMouseEvent * event)
{
	pressed = false;
	QWidget::mouseReleaseEvent(event);
}

HeaderObject::HeaderObject() : QWidget()
{
	setCursor(Qt::OpenHandCursor);
}

void HeaderObject::mousePressEvent(QMouseEvent * event)
{
	QWidget::mousePressEvent(event);
	setCursor(Qt::ClosedHandCursor);
}

void HeaderObject::mouseReleaseEvent(QMouseEvent * event)
{
	QWidget::mouseReleaseEvent(event);
	setCursor(Qt::OpenHandCursor);
}

void HeaderObject::enterEvent(QEvent * event)
{
	QWidget::enterEvent(event);
	setCursor(Qt::OpenHandCursor);
}
