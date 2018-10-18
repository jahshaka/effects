#pragma once

#include <QGraphicsPathItem>
#include <QGraphicsProxyWidget>


enum class GraphicsItemType : int
{
	Node = QGraphicsItem::UserType + 1,
	Socket = QGraphicsItem::UserType + 2,
	Connection = QGraphicsItem::UserType + 3
};

class Socket;
class SocketConnection;
class GraphNode : public QGraphicsPathItem
{
	QVector<Socket*> sockets;
	int nodeWidth;
	QGraphicsTextItem* text;
	QGraphicsProxyWidget* proxyWidget;

	int inSocketCount = 0;
	int outSocketCount = 0;
public:
	int nodeType;
	int level = 0;
	int titleHeight = 25;
	int titleRadius = 7;
	bool isHighlighted = false;
	bool currentSelectedState = false;
	bool check = false;
	QString nodeId;
	QColor titleColor;

	GraphNode(QGraphicsItem* parent);

	void setTitleColor(QColor color);
	void setTitle(QString title);
	void addInSocket(QString title);
	void addOutSocket(QString title);
	void addSocket(Socket* sock);
	void setWidget(QWidget* widget);
	void calcPath();
	int calcHeight();

	Socket* getInSocket(int index);
	Socket* getOutSocket(int index);

	virtual void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option,
		QWidget *widget = 0) override;

	virtual int type() const override;
private:
	QColor connectedColor = QColor(50, 150, 250);
	QColor disconnectedColor = QColor(90, 90, 90, 0);
	void highlightNode(bool val, int lvl);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event)override;
};

