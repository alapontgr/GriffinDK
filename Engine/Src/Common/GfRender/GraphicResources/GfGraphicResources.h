////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/17
//	File: 	GfGraphicResources.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFGRAPHICRESOURCES_H__
#define __GFGRAPHICRESOURCES_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfRender/GraphicResources/GfGraphicResourceBase.h"
#include "Common/GfRender/GraphicResources/GfConstantBuffer.h"
#include "Common/GfRender/GraphicResources/GfTexture.h"
#include "Common/GfRender/GraphicResources/GfTextureView.h"
#include "Common/GfRender/GraphicResources/GfSamplerState.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

// Use this helper to do reference counting of resources
template <typename T, typename Id>
class ResourceFactory
{
public:

	struct ResourceEntry 
	{
		u32 m_references;
		GfUniquePtr<T> m_resource;
	};

	static Id create() 
	{
		Id index = -1;
		if (m_avalIndices.size() == 0) 
		{
			index = static_cast<Id>(m_resources.size());
			size_t newSize = GfMax<size_t>(m_resources.size() * 2, 1);
			m_resources.resize(newSize);
			for (Id i = index + 1; i < static_cast<Id>(m_resources.size()); ++i) 
			{
				m_resources[i].m_references = 0;
				m_resources[i].m_resource = nullptr;
				m_avalIndices.push(i);
			}
		}
		else 
		{
			index = m_avalIndices.front();
			m_avalIndices.pop();
		}

		m_resources[index].m_references = 0;
		m_resources[index].m_resource = std::make_unique<T>();

		return index;
	}

	static T* get(Id id) 
	{
		if (id >= 0 && id < static_cast<Id>(m_resources.size())) 
		{
			return m_resources[id].m_resource.get();
		}
		return nullptr;
	}

	static void addRef(Id id, u32 count = 1) 
	{
		if (id >= 0)
		{
			GF_ASSERT(id < static_cast<Id>(m_resources.size()), "Invalid ID");
			GF_ASSERT(m_resources[id].m_resource != nullptr, "Something went wrong");
			m_resources[id].m_references += count;
		}
	}

	static void removeRef(Id id, u32 count = 1) 
	{
		if (id >= 0) 
		{
			GF_ASSERT(id < static_cast<Id>(m_resources.size()), "Invalid ID");
			GF_ASSERT(m_resources[id].m_resource != nullptr, "Something went wrong");
			GF_ASSERT(m_resources[id].m_references >= count, "Has the resource been marked to remove? trying to remove too many references?");
			m_resources[id].m_references -= count;
			if (!m_resources[id].m_references) 
			{
				m_toRemove[m_currFrameIdx].push_back(id);
			}
		}
	}

	// Call it once at the end of the frame (after Present(). One frame of delay until the destruction actually happens
	static void removePending(const GfRenderContext& ctxt) 
	{
		flip();
		for (Id id : m_toRemove[m_currFrameIdx]) 
		{
			m_resources[id].m_resource->destroy(ctxt);
			m_resources[id].m_resource = nullptr;
			m_resources[id].m_references = 0;
			m_avalIndices.push(id);
		}
		m_toRemove[m_currFrameIdx].clear();
	}

	static void removeAll(const GfRenderContext& ctxt) 
	{
		for (const ResourceEntry& entry : m_resources) 
		{
			if (entry.m_resource) 
			{
				entry.m_resource->destroy(ctxt);
				entry.m_resource = nullptr;
				entry.m_references = 0;
			}
		}
		while (!m_avalIndices.empty()) 
		{
			m_avalIndices.pop();
		}
		m_toRemove[0].clear();
		m_toRemove[1].clear();
	}

private:

	static void flip() 
	{
		m_currFrameIdx = (m_currFrameIdx + 1) & 0x1;
	}

	static GfVector<ResourceEntry> m_resources;
	static GfQueue<Id> m_avalIndices;
	static GfVector<Id> m_toRemove [2];
	static u32 m_currFrameIdx;
};
template <typename T, typename Id>
GfVector<struct ResourceFactory<T, Id>::ResourceEntry> ResourceFactory<T, Id>::m_resources;
template <typename T, typename Id>
GfQueue<Id> ResourceFactory<T, Id>::m_avalIndices;
template <typename T, typename Id>
GfVector<Id> ResourceFactory<T, Id>::m_toRemove[];
template <typename T, typename Id>
u32 ResourceFactory<T, Id>::m_currFrameIdx = 0;


using GfBufferFactory = ResourceFactory<GfBuffer, GfBuffer::Id>;
using GfTexture2DFactory = ResourceFactory<GfTexture2D, GfTexture2D::Id>;

////////////////////////////////////////////////////////////////////////////////
#endif // __GFGRAPHICRESOURCES_H__