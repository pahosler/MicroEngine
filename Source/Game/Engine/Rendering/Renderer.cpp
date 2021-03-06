#include "Renderer.h"
#include "Engine/Viewport/Viewport.h"

namespace ME
{
Renderer::Renderer() = default;
Renderer::~Renderer()
{
	Clear();
}

void Renderer::Clear()
{
	m_factory.Clear();
}

HPrim Renderer::New()
{
	return m_factory.New<Primitive>();
}

Primitive* Renderer::Find(HPrim handle)
{
	return m_factory.Find<Primitive>(handle);
}

bool Renderer::Destroy(HPrim handle)
{
	return m_factory.Destroy(handle);
}

void Renderer::Render(Viewport& viewport)
{
	std::vector<Primitive*> prims;
	prims.reserve(m_factory.m_instanced.size());
	for (auto& kvp : m_factory.m_instanced)
	{
		auto& prim = kvp.second;
		if (prim.m_bEnabled)
		{
			prims.push_back(&prim);
		}
	}
	std::sort(prims.begin(), prims.end(), [](Primitive* pLHS, Primitive* pRHS) { return pLHS->m_layer < pRHS->m_layer; });
	viewport.clear();
	for (auto pPrim : prims)
	{
		pPrim->Update();
		pPrim->Draw(viewport);
	}
	viewport.display();
}
} // namespace ME
