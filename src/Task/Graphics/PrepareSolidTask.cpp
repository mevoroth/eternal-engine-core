#include "Task/Graphics/PrepareSolidTask.hpp"

#include <vector>
#include "Task/Graphics/SolidGBufferTask.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/ShaderFactory.hpp"
#include "GraphicData/GraphicObjects.hpp"
#include "GraphicData/Material.hpp"
#include "GraphicData/ShaderMaterialProperty.hpp"
#include "GraphicData/CameraMaterialProperty.hpp"
#include "GraphicData/TopologyMaterialProperty.hpp"
#include "GraphicData/ViewportMaterialProperty.hpp"
#include "GraphicData/SamplerMaterialProperty.hpp"

#include "Camera/PerspectiveCamera.hpp"

using namespace std;
using namespace Eternal::GraphicData;
using namespace Eternal::Graphics;
using namespace Eternal::Task;

namespace Eternal
{
	namespace Task
	{
		class PrepareSolidTaskData
		{
		public:
			PrepareSolidTaskData()
			{
				InputLayout::VertexDataType DataTypes[] = {
					InputLayout::POSITION_T,
					InputLayout::NORMAL_T,
					InputLayout::UV_T
				};

				_VertexShader = ShaderFactory::Get()->CreateVertexShader("Opaque", "opaque.vs.hlsl", DataTypes, ETERNAL_ARRAYSIZE(DataTypes));
				_VertexProperty.SetShader(_VertexShader);
				_MaterialProperties.push_back((MaterialProperty*)&_VertexProperty);

				_PixelShader = ShaderFactory::Get()->CreatePixelShader("Opaque", "opaque.ps.hlsl");
				_PixelProperty.SetShader(_PixelShader);
				_MaterialProperties.push_back((MaterialProperty*)&_PixelProperty);

				_Camera = new PerspectiveCamera(0.001f, 5000.0f, 45.0f);
				_CameraProperty.SetCamera(_Camera);
				_MaterialProperties.push_back((MaterialProperty*)&_CameraProperty);

				_MaterialProperties.push_back((MaterialProperty*)&_TopologyProperty);

				_MaterialProperties.push_back((MaterialProperty*)&_ViewportProperty);

				_MaterialProperties.push_back((MaterialProperty*)&_SamplerProperty);

				_SolidMaterial.SetProperties(&_MaterialProperties);
				_SolidMaterial.SetMaterialName("Opaque Material");

				_GraphicObjects.SetMaterial(&_SolidMaterial);
			}

			GraphicObjects& GetGraphicObjects()
			{
				return _GraphicObjects;
			}

			CameraMaterialProperty& GetCameraProperty()
			{
				return _CameraProperty;
			}

			ViewportMaterialProperty& GetViewportProperty()
			{
				return _ViewportProperty;
			}

			SamplerMaterialProperty& GetSamplerProperty()
			{
				return _SamplerProperty;
			}

		private:
			GraphicObjects _GraphicObjects;

			Material _SolidMaterial;
			vector<MaterialProperty*> _MaterialProperties;

			Shader* _VertexShader = nullptr;
			ShaderMaterialProperty<Context::VERTEX> _VertexProperty;

			Shader* _PixelShader = nullptr;
			ShaderMaterialProperty<Context::PIXEL> _PixelProperty;

			Camera* _Camera = nullptr;
			CameraMaterialProperty _CameraProperty;

			TopologyMaterialProperty _TopologyProperty;

			ViewportMaterialProperty _ViewportProperty;

			SamplerMaterialProperty _SamplerProperty;
		};
	}
}

#include "Core/GraphicGameObject.hpp"
#include "Core/MeshComponent.hpp"
#include "Mesh/MeshFactory.hpp"

using namespace Eternal::Core;
using namespace Eternal::Components;

PrepareSolidTask::PrepareSolidTask(_In_ SolidGBufferTask& SolidGBufferTaskObj)
	: _SolidGBufferTask(SolidGBufferTaskObj)
	, _SolidTaskData(new PrepareSolidTaskData())
{
	GraphicGameObject* gfxGO = new GraphicGameObject();
	MeshComponent* MeshComp = new MeshComponent();
	MeshComp->SetMesh(CreateMesh("Liya.fbx"));
	gfxGO->SetMeshComponent(MeshComp);

	vector<GraphicGameObject*>* gfxGOs = new vector<GraphicGameObject*>();
	gfxGOs->push_back(gfxGO);

	_SolidTaskData->GetGraphicObjects().SetGraphicGameObjects(gfxGOs);
}

void PrepareSolidTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void PrepareSolidTask::Execute()
{
	ETERNAL_ASSERT(GetState() == SETUP);
	SetState(EXECUTING);

	_SolidTaskData->GetCameraProperty().SetConstant(_SolidGBufferTask.GetCommonConstant());
	_SolidTaskData->GetViewportProperty().SetViewport(_SolidGBufferTask.GetViewport());
	_SolidTaskData->GetSamplerProperty().SetSampler(_SolidGBufferTask.GetSampler());
	_SolidGBufferTask.SetGraphicObjects(_SolidTaskData->GetGraphicObjects());

	SetState(DONE);
}

void PrepareSolidTask::Reset()
{
	ETERNAL_ASSERT(GetState() == DONE);
	SetState(IDLE);
}
