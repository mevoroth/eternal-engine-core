#pragma once

#include "GameData/GameData.hpp"
#include "Parallel/ThreadLocalStorage.hpp"
#include "rapidjson/document.h"
#include <string>

namespace Eternal
{
	namespace GameDataSystem
	{
		using namespace std;

		using JsonNodeType = rapidjson::GenericValue<rapidjson::UTF8<>>;

		struct JsonNode : public GameDataSource
		{
		public:

			JsonNode(_In_ const JsonNodeType& InNode);

		protected:
			const JsonNodeType& _Node;
		};

		template<typename JsonNodeType>
		struct JsonArrayNode : public JsonNode
		{
			static ETERNAL_THREAD_LOCAL GameDataSource* TemporarySubNode;

			using JsonNode::JsonNode;
			~JsonArrayNode();

			virtual const GameDataSource& GetSubNode(_In_ const GameDataSourceKey& InKey) const override final;

		protected:
			virtual void InternalGet(_Out_ void* OutValue) const override final;
		};

		struct JsonTransformNode : public JsonNode
		{
			using JsonNode::JsonNode;

		protected:
			virtual void InternalGet(_Out_ void* OutValue) const override final;
		};

		struct JsonTransformCollection
		{
			JsonTransformCollection(_In_ const JsonNodeType& InNode);
			~JsonTransformCollection();

			const GameDataSource& Get() const;

		private:
			JsonArrayNode<JsonTransformNode>* _TransformNodes;
		};

		struct JsonCameraNode : public JsonNode
		{
			JsonCameraNode(_In_ const JsonNodeType& InNode);

			virtual const GameDataSource& GetSubNode(_In_ const GameDataSourceKey& InKey) const override final;

		protected:
			virtual void InternalGet(_Out_ void* OutValue) const override final;

		private:
			JsonTransformCollection _TransformCollection;
		};

		struct JsonMeshNode : public JsonNode
		{
			JsonMeshNode(_In_ const JsonNodeType& InNode);

			virtual const GameDataSource& GetSubNode(_In_ const GameDataSourceKey& InKey) const override final;

		protected:
			virtual void InternalGet(_Out_ void* OutValue) const override final;

		private:
			JsonTransformCollection _TransformCollection;
		};

		struct JsonLightNode : public JsonNode
		{
			JsonLightNode(_In_ const JsonNodeType& InNode);

			virtual const GameDataSource& GetSubNode(_In_ const GameDataSourceKey& InKey) const override final;

		protected:
			virtual void InternalGet(_Out_ void* OutValue) const override final;

		private:
			JsonTransformCollection _TransformCollection;
		};

		struct JsonLevel : public GameDataSource
		{
		public:
			JsonLevel(_In_ const string& InFileContent);
			~JsonLevel();

			virtual const GameDataSource& GetSubNode(_In_ const GameDataSourceKey& InKey) const override final;

		private:
			rapidjson::Document _Document;

			JsonArrayNode<JsonCameraNode>* _CamerasNode;
			JsonArrayNode<JsonMeshNode>* _MeshesNode;
			JsonArrayNode<JsonLightNode>* _LightsNode;
		};
	}
}
