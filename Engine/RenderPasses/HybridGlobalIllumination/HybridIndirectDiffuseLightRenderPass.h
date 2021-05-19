#ifndef HYBRIDINDIRECTDIFFUSELIGHTRENDERPASS_H
#define HYBRIDINDIRECTDIFFUSELIGHTRENDERPASS_H

#include "RenderPasses/PostProcess/PostProcessRenderPass.h"

class ENGINE_API HybridIndirectDiffuseLightRenderPass : public PostProcessRenderPass
{
	DECLARE_RENDER_PASS(HybridIndirectDiffuseLightRenderPass)

public:
	bool IsAvailable (const RenderScene* renderScene, const Camera* camera,
		const RenderSettings& settings, const RenderVolumeCollection* rvc) const;
protected:
	std::string GetPostProcessFragmentShaderPath () const;
	std::string GetPostProcessVolumeName () const;
	glm::ivec2 GetPostProcessVolumeResolution (const RenderSettings& settings) const;
	FramebufferRenderVolume* CreatePostProcessVolume (const RenderSettings& settings) const;
};

#endif