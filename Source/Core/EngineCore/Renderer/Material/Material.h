﻿#pragma once

#include "../../EngineCoreHeader.h"

#include "../RenderObject.h"
#include "../../Handler.h"
#include "RenderAPI/Interface/ShaderParametersLayout.h"
#include "../../Allocator/FrameAllocator.h"

namespace cube
{
	enum class MaterialParameterType
	{
		Data,
		Texture
	};

	struct ENGINE_CORE_EXPORT MaterialParameterInfo
	{
		String name;
		MaterialParameterType type;
		size_t dataSize;
	};

	struct MaterialInitializer
	{
		FrameVector<RPtr<Shader>> shaders;
		FrameVector<MaterialParameterInfo> parameters;
	};

	class ENGINE_CORE_EXPORT Material : public RenderObject
	{
	public:
		static HMaterial Create(const MaterialInitializer& init);

	public:
		virtual ~Material();

		virtual SPtr<rt::RenderObject> CreateRenderObject() const override;
		SPtr<rt::Material> GetRenderObject() const { return DPCast(rt::Material)(mRenderObject); }

		HMaterial GetHandler() const { return mMyHandler; }

		HMaterialInstance CreateInstance();

		virtual void Destroy();

		const Vector<MaterialParameterInfo>& GetParameterInfos() const { return mParamInfos; }

	private:
		friend class RendererManager;
		friend class MaterialInstance;

		Material(const MaterialInitializer& init);

		Vector<RPtr<Shader>> mShaders;
		Vector<MaterialParameterInfo> mParamInfos;
	};

	namespace rt
	{
		class Material : public RenderObject
		{
		public:
			virtual ~Material() {}
			virtual void Initialize() override;
			virtual void Destroy() override;

			void SyncMaterial(const Vector<RPtr<Shader>>& shaders, const Vector<MaterialParameterInfo>& paramInfos);

			const Vector<MaterialParameterInfo>& GetParameterInfos() const { return mParamInfos; }

			const Vector<RPtr<Shader>>& GetShaders() const { return mShaders; }

			SPtr<render::ShaderParametersLayout> GetShaderParametersLayout() const { return mShaderParamsLayout; }

		private:
			friend class cube::Material;
			friend class RendererManager;

			Material();

			// MaterialInitializer mMaterialInit;

			int mIndex = -1; // Used in RendererManager

			Vector<MaterialParameterInfo> mParamInfos;

			Vector<RPtr<Shader>> mShaders;
			SPtr<render::ShaderParametersLayout> mShaderParamsLayout;
		};
	} // namespace rt
} // namespace cube
