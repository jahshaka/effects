#include "nodegraph.h"
#include "connectionmodel.h"
#include "../nodes/test.h"
#include "library.h"
#include "../core/guidhelper.h"
///#include "../nodes/libraryv1.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QDebug>

void NodeGraph::addProperty(Property* prop)
{
	this->properties.append(prop);
}

bool NodeGraph::deletePropertyById(const QString & id)
{
	return false;
}

Property *NodeGraph::getPropertyByName(const QString &name)
{
	for (auto prop : properties)
		if (prop->name == name)
			return prop;
	return nullptr;
}

Property *NodeGraph::getPropertyById(const QString &id)
{
	for (auto prop : properties)
		if (prop->id == id)
			return prop;
	return nullptr;
}

QVector<NodeModel*> NodeGraph::getNodesByTypeName(QString name)
{
	QVector<NodeModel *> list;
	for (auto node : nodes.values())
		if (node->typeName == name) {
			
			list.append(node);
			
		}
			

	return list;
}

/*
void NodeGraph::registerModel(QString name, std::function<NodeModel *()> factoryFunction)
{
	modelFactories.insert(name, factoryFunction);
}
*/

void NodeGraph::setNodeLibrary(NodeLibrary* lib)
{
	this->library = lib;
}


void NodeGraph::addNode(NodeModel *model)
{
	nodes.insert(model->id, model);
}

void NodeGraph::setMasterNode(NodeModel *masterNode)
{
	this->masterNode = masterNode;
}

NodeModel *NodeGraph::getMasterNode()
{
	return masterNode;
}

ConnectionModel* NodeGraph::addConnection(NodeModel *leftNode, int leftSockIndex, NodeModel *rightNode, int rightSockIndex)
{
	// todo: check if the indices are correct
	return addConnection(leftNode->id, leftSockIndex, rightNode->id, rightSockIndex);
}

ConnectionModel* NodeGraph::addConnection(QString leftNodeId, int leftSockIndex, QString rightNodeId, int rightSockIndex)
{
	// todo: check if the ids and socket indices are correct
	auto leftNode = nodes[leftNodeId];
	auto leftSock = leftNode->outSockets[leftSockIndex];

	auto rightNode = nodes[rightNodeId];
	auto rightSock = rightNode->inSockets[rightSockIndex];

	// todo: check if socket with pair already exists

	auto con = new ConnectionModel();
	con->leftSocket = leftSock;
	con->rightSocket = rightSock;

	leftSock->connection = con;
	rightSock->connection = con;
	connections.insert(con->id, con);

	return con;
}

void NodeGraph::removeConnection(QString connectionId)
{
	//if (!connections.contains(connectionId))
	//	return;

	auto con = connections[connectionId];

	// assuming it's a complete connection
	con->leftSocket->connection = nullptr;
	con->rightSocket->connection = nullptr;
	connections.remove(connectionId);
}

QJsonObject NodeGraph::serialize()
{
	QJsonObject graph;

	QJsonArray nodesJson;

	// save nodes
	for (auto node : this->nodes.values()) {
		QJsonObject nodeObj;
		nodeObj["id"] = node->id;
		nodeObj["value"] = node->serializeWidgetValue();
		nodeObj["type"] = node->typeName;
		nodeObj["x"] = node->x;
		nodeObj["y"] = node->y;
		nodesJson.append(nodeObj);
	}
	graph.insert("nodes", nodesJson);

	// save connections
	QJsonArray consJson;
	for (auto con : this->connections.values()) {
		QJsonObject conObj;
		conObj["id"] = con->id;
		conObj["leftNodeId"] = con->leftSocket->node->id;
		conObj["leftNodeSocketIndex"] = con->leftSocket->node->outSockets.indexOf(con->leftSocket);//todo: ugly, cleanup.
		conObj["rightNodeId"] = con->rightSocket->node->id;
		conObj["rightNodeSocketIndex"] = con->rightSocket->node->inSockets.indexOf(con->rightSocket);//todo: ugly, cleanup.

		consJson.append(conObj);
	}
	graph.insert("connections", consJson);
	graph.insert("masternode", this->masterNode->id);

	//todo: save settings (acceptLighting, blendstate, depthstate, etc..)

	graph["settings"] = serializeMaterialSettings();

	//todo: save parameters
	QJsonArray propJson;
	for (auto prop : this->properties) {
		QJsonObject propObj = prop->serialize();
		propJson.append(propObj);
	}
	graph["properties"] = propJson;

	return graph;
}

NodeGraph* NodeGraph::deserialize(QJsonObject graphObj, NodeLibrary* library)
{
	auto graph = new NodeGraph();
	graph->setNodeLibrary(library);
	//registerModels(graph);

	// read settings

	// read properties
	//auto graphObj = obj["graph"].toObject();
	auto propList = graphObj["properties"].toArray();
	for (auto propObj : propList) {
		auto prop = Property::parse(propObj.toObject());
		graph->addProperty(prop);
	}

	// read nodes
	auto nodeList = graphObj["nodes"].toArray();
	for (auto nodeVar : nodeList) {
		auto nodeObj = nodeVar.toObject();
		auto type = nodeObj["type"].toString();

		NodeModel* nodeModel = nullptr;
		if (type == "Material") {
			nodeModel = new SurfaceMasterNode();
		}
		else {
			//nodeModel = graph->modelFactories[type]();
			nodeModel = graph->library->createNode(type);
		}
		nodeModel->id = nodeObj["id"].toString();
		nodeModel->x = nodeObj["x"].toDouble();
		nodeModel->y = nodeObj["y"].toDouble();


		// special case for properties
		if (type == "property") {
			auto propId = nodeObj["value"].toString();
			auto prop = graph->getPropertyById(propId);
			((PropertyNode*)nodeModel)->setProperty(prop);
		}
		else {
			nodeModel->deserializeWidgetValue(nodeObj["value"]);
		}

		graph->addNode(nodeModel);
		if (type == "Material") {
			graph->setMasterNode(nodeModel);
		}
	}

	// read connections
	auto conList = graphObj["connections"].toArray();
	for (auto conVar : conList) {
		auto conObj = conVar.toObject();
		auto id = conObj["id"].toString();
		auto leftNodeId = conObj["leftNodeId"].toString();
		auto leftSockIndex = conObj["leftNodeSocketIndex"].toInt();
		auto rightNodeId = conObj["rightNodeId"].toString();
		auto rightSockIndex = conObj["rightNodeSocketIndex"].toInt();

		graph->addConnection(leftNodeId, leftSockIndex, rightNodeId, rightSockIndex);
	}
	
		// deserialize material settings
	graph->settings = graph->deserializeMaterialSettings(graphObj["settings"].toObject());
	
	return graph;
}


QJsonObject NodeGraph::serializeMaterialSettings()
{
	QJsonObject obj;

	QString blendType;
	switch (settings.blendMode) {
	case BlendMode::Opaque:
		blendType = "Opaque";
		break;
	case BlendMode::Blend:
		blendType = "Blend";
		break;
	case BlendMode::Additive:
		blendType = "Blend";
	}

	QString cullMode;
	switch (settings.cullMode) {
	case CullMode::Front:
		cullMode = "Front";
		break;
	case CullMode::Back:
		cullMode = "Back";
		break;
	case CullMode::None:
		cullMode = "None";
	}

	QString renderLayer;
	switch (settings.renderLayer) {
	case RenderLayer::Opaque:
		renderLayer = "Opaque";
		break;
	case RenderLayer::AlphaTested:
		renderLayer = "AlphaTested";
		break;
	case RenderLayer::Transparent:
		renderLayer = "Transparent";
		break;
	case RenderLayer::Overlay:
		renderLayer = "Overlay";
		break;
	}

	obj["name"] = settings.name;
	obj["zWrite"] = settings.zwrite;
	obj["depthTest"] = settings.depthTest;
	obj["fog"] = settings.fog;
	obj["castShadow"] = settings.castShadow;
	obj["receiveShadow"] = settings.receiveShadow;
	obj["acceptLighting"] = settings.acceptLighting;
	obj["blendMode"] = blendType;
	obj["cullMode"] = cullMode;
	obj["renderLayer"] = renderLayer;
	return obj;
}

MaterialSettings NodeGraph::deserializeMaterialSettings(QJsonObject obj)
{

	auto getBlendmode = [](QJsonObject obj) {
		if (obj["blendMode"].toString() == "Opaque") return BlendMode::Opaque;
		if (obj["blendMode"].toString() == "Blend") return BlendMode::Blend;
		if (obj["blendMode"].toString() == "Additive") return BlendMode::Additive;
		return BlendMode::Opaque;
	};
	auto getCullMode = [](QJsonObject obj) {
		if (obj["cullMode"].toString() == "Front") return CullMode::Front;
		if (obj["cullMode"].toString() == "Back") return CullMode::Back;
		if (obj["cullMode"].toString() == "None") return CullMode::None;
		return CullMode::Front;
	};
	auto getRenderLayer = [](QJsonObject obj) {
		if (obj["renderLayer"].toString() == "Opaque") return RenderLayer::Opaque;
		if (obj["renderLayer"].toString() == "AlphaTested") return RenderLayer::AlphaTested;
		if (obj["renderLayer"].toString() == "Transparent") return RenderLayer::Transparent;
		if (obj["renderLayer"].toString() == "Overlay") return RenderLayer::Overlay;
		return RenderLayer::Opaque;
	};

	MaterialSettings settings;
	settings.name = obj["name"].toString();
	settings.zwrite = obj["zWrite"].toBool();
	settings.depthTest = obj["depthTest"].toBool();
	settings.fog = obj["fog"].toBool();
	settings.castShadow = obj["castShadow"].toBool();
	settings.receiveShadow = obj["receiveShadow"].toBool();
	settings.acceptLighting = obj["acceptLighting"].toBool();
	settings.blendMode = getBlendmode(obj);
	settings.cullMode = getCullMode(obj);
	settings.renderLayer = getRenderLayer(obj);

	return settings;
}

void NodeGraph::setMaterialSettings(MaterialSettings setting)
{
	this->settings = setting;
}

