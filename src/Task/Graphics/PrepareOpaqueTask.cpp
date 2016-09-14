#include "Task/Graphics/PrepareOpaqueTask.hpp"

#include <vector>
#include "Task/Graphics/OpaqueTask.hpp"
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
		class PrepareOpaqueTaskData
		{
		public:
			PrepareOpaqueTaskData()
			{
				_Camera = new PerspectiveCamera(0.001f, 5000.0f, 45.0f);
				_CameraProperty.SetCamera(_Camera);
				_MaterialProperties.push_back((MaterialProperty*)&_CameraProperty);

				_OpaqueMaterial.SetProperties(&_MaterialProperties);
				_OpaqueMaterial.SetMaterialName("Opaque Material");

				_GraphicObjects.SetMaterial(&_OpaqueMaterial);
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

			Material _OpaqueMaterial;
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

PrepareOpaqueTask::PrepareOpaqueTask(_In_ OpaqueTask& OpaqueTaskObj)
	: _OpaqueTask(OpaqueTaskObj)
	, _PrepareOpaqueTaskData(new PrepareOpaqueTaskData())
{
	GraphicGameObject* gfxGO = new GraphicGameObject();
	MeshComponent* MeshComp = new MeshComponent();
	MeshComp->SetMesh(CreateMesh("Sponza.fbx"));
	gfxGO->SetMeshComponent(MeshComp);

	vector<GraphicGameObject*>* gfxGOs = new vector<GraphicGameObject*>();
	gfxGOs->push_back(gfxGO);

	_PrepareOpaqueTaskData->GetGraphicObjects().SetGraphicGameObjects(gfxGOs);
}

void PrepareOpaqueTask::Setup()
{
	ETERNAL_ASSERT(GetState() == SCHEDULED);
	SetState(SETUP);
}

void PrepareOpaqueTask::Execute()
{
	ETERNAL_ASSERT(GetState() == SETUP);
	SetState(EXECUTING);

	_PrepareOpaqueTaskData->GetCameraProperty().SetConstant(_OpaqueTask.GetCommonConstant());
	_PrepareOpaqueTaskData->GetViewportProperty().SetViewport(_OpaqueTask.GetViewport());
	_PrepareOpaqueTaskData->GetSamplerProperty().SetSampler(_OpaqueTask.GetSampler());
	_OpaqueTask.SetGraphicObjects(_PrepareOpaqueTaskData->GetGraphicObjects());

	SetState(DONE);
}

void PrepareOpaqueTask::Reset()
{
	ETERNAL_ASSERT(GetState() == DONE);
	SetState(IDLE);
}
