#pragma once
#include <QUndoStack>
#include <QPoint>
#include "../graphnode.h"
#include "../graphnodescene.h"

class UndoRedo : public QUndoCommand
{
public:
	UndoRedo();
	~UndoRedo();

	void undo() override;
	void redo() override;
	static int count;
};

class AddNodeCommand : public UndoRedo
{
public:
	AddNodeCommand(NodeModel * nodeModel, GraphNodeScene *);

	void undo() override;
	void redo() override;

private :
	QPointF initialPosition;
	GraphNodeScene* scene;
	NodeModel * nodeModel;
	GraphNode *node;
};

class AddConnectionCommand : public UndoRedo
{
public:
	AddConnectionCommand(const QString &,const QString &, GraphNodeScene *, int leftSocket, int rightSocket);

	void undo() override;
	void redo() override;
	QString connectionID;
private:
	int left, right;
	GraphNodeScene* scene;
	GraphNode* node;
	SocketConnection* connection;
	QString leftNodeId;
	QString rightNodeId;
};

class MoveNodeCommand : public UndoRedo
{
public:
	MoveNodeCommand(GraphNode *node, GraphNodeScene *,  QPointF oldPos, QPointF newPos);

	void undo() override;
	void redo() override;
private:
	QPointF oldPos;
	QPointF newPos;
	GraphNode* node;
	GraphNodeScene* scene;
};

class MoveMultipleCommand : public UndoRedo
{
public:
	MoveMultipleCommand(QList<GraphNode*> &, GraphNodeScene*);

	void undo() override;
	void redo() override;
private:
	QList<GraphNode*> nodes;
	QMap<QString, QPair<QPointF, QPointF>> map;
	GraphNodeScene* scene;

};

class DeleteNodeCommand : public UndoRedo
{
public:
	DeleteNodeCommand(QList<GraphNode*> &, GraphNodeScene *);

	void undo() override;
	void redo() override;
private:
	GraphNodeScene* scene;
	QList<GraphNode*> list;
	GraphNode* node;
	QVector<ConnectionModel*> connections;
};
