#pragma once

#include "../RenderAPIHeader.h"

#include "../Utilities/BaseRenderObject.h"

namespace cube
{
	namespace render
	{
		struct ShaderAttribute : public BaseAttribute
		{
			ShaderType type;
			ShaderLanguage language;

			const char* code;
			const char* entryPoint;
		};

		class Shader : public BaseRenderObject
		{
		public:
			Shader(const ShaderAttribute& attr) :
				BaseRenderObject(attr.debugName),
				mType(attr.type)
			{}
			virtual ~Shader() {}

			ShaderType GetType() const { return mType; }

		protected:
			ShaderType mType;
		};
	} // namespace render
} // namespace cube
