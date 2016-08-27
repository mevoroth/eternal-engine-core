#ifndef _DEBUG_SOLID_GBUFFER_TASK_HPP_
#define _DEBUG_SOLID_GBUFFER_TASK_HPP_

#include <string>
#include "Parallel/Task.hpp"
#include "Task/Graphics/SolidGBufferTask.hpp"
#include "GraphicData/GraphicObjects.hpp"
#include "GraphicData/Material.hpp"
#include "GraphicData/ShaderMaterialProperty.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/Context.hpp"

using namespace Eternal::Parallel;
using namespace Eternal::Task;
using namespace Eternal::GraphicData;
using namespace Eternal::Graphics;

class DebugSolidGBufferTask : public Task
{
public:
	SolidGBufferTask& _task;
	GraphicObjects _graphicObjects;

	DebugSolidGBufferTask(SolidGBufferTask& task)
		: _task(task)
	{
		InputLayout::VertexDataType VertexDataTypeObj[] = {
			InputLayout::POSITION_T,
			InputLayout::NORMAL_T,
			InputLayout::UV_T
		};

		vector<MaterialProperty&>* materialProperties = new vector<MaterialProperty&>();

		Shader* VertexShader = ShaderFactory::Get()->CreateVertexShader("Opaque", "opaque.vs.hlsl", VertexDataTypeObj, ETERNAL_ARRAYSIZE(VertexDataTypeObj));
		ShaderMaterialProperty<Context::VERTEX>* VertexShaderMaterialProperty = new ShaderMaterialProperty<Context::VERTEX>();
		VertexShaderMaterialProperty->SetShader(VertexShader);
		materialProperties->push_back((MaterialProperty&)*VertexShaderMaterialProperty);

		Shader* PixelShader = ShaderFactory::Get()->CreatePixelShader("Opaque", "opaque.ps.hlsl");
		ShaderMaterialProperty<Context::PIXEL>* PixelShaderMaterialProperty = new ShaderMaterialProperty<Context::PIXEL>();
		PixelShaderMaterialProperty->SetShader(PixelShader);
		materialProperties->push_back((MaterialProperty&)*PixelShaderMaterialProperty);

		Material* CommonMaterial = new Material();
		CommonMaterial->SetProperties(*materialProperties);

		_graphicObjects.SetMaterial(CommonMaterial);

		vector<GraphicGameObject*>* graphicGO = new vector<GraphicGameObject*>();
		//_graphicObjects.
	}

	virtual void Setup() override
	{
		SetState(Task::SETUP);
	}

	virtual void Reset() override
	{
		SetState(Task::IDLE);
	}

	virtual void Execute() override
	{
		SetState(Task::EXECUTING);

		//_task.SetGraphicObjects()

		SetState(Task::DONE);
	}

};

#endif
