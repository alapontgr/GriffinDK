#pragma once
#include "Utilities\types.h"

namespace fdk
{
namespace Rendering
{

	struct Viewport
	{
		v2 m_offset;
		v2 m_dimensions;
		f32 m_minDepth;
		f32 m_maxDepth;
	};

	struct Scissor
	{
		s32 m_offsetX;
		s32 m_offsetY;
    u32 m_width;
    u32 m_height;
	};

	struct RasterState
	{
		enum EPolygonMode : u32
		{
			Fill = 0,
			Lines,
			Points
		};
		enum ECullMode : u32
		{
			None = 0,
			Front,
			Back,
			FrontAndBack
		};

		RasterState()
				: m_polygonMode{Fill}
				, m_cullMode{Back}
				, m_enableRasterDiscard{false}
		{
		}

		EPolygonMode m_polygonMode;
		ECullMode m_cullMode;
		bool m_enableRasterDiscard;
	};

	struct MultiSampleState
	{
		enum ESamplesCount : u32
		{
			x1,
			x2,
			x4,
			x8,
			x16
		};

		MultiSampleState()
				: m_enabled{false}
		{
		}

		ESamplesCount m_samplesCount;
		bool m_enabled;
	};

	struct BlendState
	{
		enum EBlendFactor : u32
		{
			Zero = 0,
			One,
			Src_Color,
			One_Minus_Src_Color,
			Dst_Color,
			One_Minus_Dst_Color,
			Src_Alpha,
			One_Minus_Src_Alpha,
			Dst_Alpha,
			One_Minus_Dst_Alpha,
			Constant_Color,
			One_Minus_Constant_Color,
			Constant_Alpha,
			One_Minus_Contant_Alpha,
		};

		enum EBlendOP : u32
		{
			Add = 0,
			Substract,
			Reverse_Substract,
			Min,
			Max
		};

		enum EBlendLogicOperation : u32
		{
			Clear = 0,
			And,
			And_Reverse,
			Copy,
			And_Inverted,
			No_OP,
			Xor,
			Or,
			Nor,
			Equivalent,
			Invert,
			Reverse,
			Copy_Inverted,
			Or_Inverted,
			Nand,
			Set
		};

		EBlendFactor m_srcColorBlendFactor;
		EBlendFactor m_dstColorBlendFactor;
		EBlendOP m_colorBlendOP;
		EBlendFactor m_srcAlphaBlendFactor;
		EBlendFactor m_dstAlphaBlendFactor;
		EBlendOP m_alphaBlendOP;
		bool m_enabledBlending;

		EBlendLogicOperation m_logicOP;
		bool m_enableLogicOperations;
		v4 m_blendConstants;
	};
}
}