#include "texture.h"

/*    COMBINE NORMAL    */
CombineNormalsNode::CombineNormalsNode()
{
	setNodeType(NodeType::Math);
	title = "Combine Normals";
	typeName = "combinenormals";
	enablePreview = true;

	addInputSocket(new Vector3SocketModel("NormalA"));
	addInputSocket(new Vector3SocketModel("NormalB"));
	addOutputSocket(new Vector3SocketModel("Result"));
}

void CombineNormalsNode::process(ModelContext* context)
{
	auto ctx = (ShaderContext*)context;
	auto normalA = this->getValueFromInputSocket(0);
	auto normalB = this->getValueFromInputSocket(1);
	auto res = this->getOutputSocketVarName(0);

	auto code = res + " = normalize((" + normalA + " + " + normalB + ")*2 - 2)";
	ctx->addCodeChunk(this, code);
}

QString CombineNormalsNode::generatePreview(ModelContext* context)
{
	auto ctx = (ShaderContext*)context;
	auto normalA = this->getValueFromInputSocket(0);
	auto normalB = this->getValueFromInputSocket(1);
	auto res = this->getOutputSocketVarName(0);

	auto output = "preview.color = normalize((" + normalA + " + " + normalB + ")*2 - 2)";

	return output;
}


TexelSizeNode::TexelSizeNode()
{
	setNodeType(NodeType::Math);
	title = "Texel Size";
	typeName = "texelsize";
	enablePreview = true;

	addInputSocket(new TextureSocketModel("Texture"));
	addOutputSocket(new Vector2SocketModel("Size"));
	addOutputSocket(new FloatSocketModel("Width"));
	addOutputSocket(new FloatSocketModel("Height"));
	addOutputSocket(new FloatSocketModel("1/Width"));
	addOutputSocket(new FloatSocketModel("1/Height"));
}

void TexelSizeNode::process(ModelContext* context)
{
	auto texName = this->getValueFromInputSocket(0);
	this->outSockets[0]->setVarName("textureSize(" + texName + ", 0.0)");
	this->outSockets[1]->setVarName("textureSize(" + texName + ", 0.0).x");
	this->outSockets[2]->setVarName("textureSize(" + texName + ", 0.0).y");
	this->outSockets[3]->setVarName("(1.0 / textureSize(" + texName + ", 0.0).x)");
	this->outSockets[4]->setVarName("(1.0 / textureSize(" + texName + ", 0.0).y)");
}

/*
SampleEquirectangularTextureNode::SampleEquirectangularTextureNode()
{
	setNodeType(NodeType::Math);
	title = "Sample Texture Equirectangular";
	typeName = "texelsize";
	enablePreview = true;

	addInputSocket(new TextureSocketModel("Texture"));
	addOutputSocket(new Vector2SocketModel("Size"));
	addOutputSocket(new FloatSocketModel("Width"));
	addOutputSocket(new FloatSocketModel("Height"));
	addOutputSocket(new FloatSocketModel("1/Width"));
	addOutputSocket(new FloatSocketModel("1/Height"));
}
*/


FlipbookUVAnimationNode::FlipbookUVAnimationNode()
{
	setNodeType(NodeType::Math);
	title = "FlipBook";
	typeName = "flipbook";
	enablePreview = true;

	addInputSocket(new Vector2SocketModel("UV", "v_texCoord"));
	addInputSocket(new FloatSocketModel("Rows", "1.0"));
	addInputSocket(new FloatSocketModel("Columns", "1.0"));
	addInputSocket(new FloatSocketModel("Animation Length", "2.0"));
	addInputSocket(new FloatSocketModel("Time", "u_time"));

	addOutputSocket(new Vector2SocketModel("UV"));
}

void FlipbookUVAnimationNode::process(ModelContext* context)
{
	auto ctx = (ShaderContext*)context;

	ctx->addFunction("flipbook", R"(vec2 flipbook(float rows, float columns, float animlength, vec2 uv, float time)
	{
		float totalFrames = rows * columns;
		float frameWidth = 1.0 / float(columns);
		float frameHeight = 1.0 / float(rows);

		float timePerFrame = animlength / totalFrames;

		float currentFrame = floor(mod(time / timePerFrame, totalFrames));

		//float currentFrame = 2.0;
		float animCol = floor(currentFrame / float(columns));
		float animRow = rows - floor(mod(currentFrame, float(columns))) - 1.0;

		vec2 animUV = (vec2(animCol * frameWidth, animRow * frameHeight) + uv * vec2(frameWidth, frameHeight));

		return animUV;
	})");

	auto uv = this->getValueFromInputSocket(0);
	auto rows = this->getValueFromInputSocket(1);
	auto columns = this->getValueFromInputSocket(2);
	auto animLength = this->getValueFromInputSocket(3);
	auto time = this->getValueFromInputSocket(4);
	auto res = this->getOutputSocketVarName(0);

	auto code = res + " = flipbook("+rows+","+columns+","+animLength+","+uv+","+time+");";
	ctx->addCodeChunk(this, code);
}