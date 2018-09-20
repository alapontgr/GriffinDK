////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/17
//	File: 	GfRenderPass.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERPASS_H__
#define __GFRENDERPASS_H__
////////////////////////////////////////////////////////////////////////////////
// Include

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfMaths.h"
#include GF_SOLVE_PLATFORM_HEADER(GfRenderPass)

////////////////////////////////////////////////////////////////////////////////

namespace GfRenderPassClear 
{
	enum Type : u32 
	{
		Clear,
		// TODO
	};
}

////////////////////////////////////////////////////////////////////////////////

class GfRenderPass : public GfRenderPass_Platform 
{
public:

	GfRenderPass();

	void AddInputSlot();

	void AddOutputSlot();

	void SetOutputClearMode(GfRenderPassClear::Type eMode, v4 vClearColor = v4(0.0f));

	void Compile(const GfRenderContext& kCtx);

	void Resize(const GfRenderContext& kCtx);

private:
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERPASS_H__