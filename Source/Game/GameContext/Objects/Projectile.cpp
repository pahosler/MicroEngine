#include "Projectile.h"
#include "Bubble.h"

namespace ME
{
void Projectile::OnCreate()
{
	m_elapsed = Time::Zero;
}

void Projectile::OnRespawn()
{
	m_elapsed = Time::Zero;
}

void Projectile::Tick(Time dt)
{
	m_elapsed += dt;
	if (m_elapsed >= m_ttl)
	{
		// Self destruct
		Destroy();
		return;
	}

	// Move along orientation
	const Vector2 displacement = Fixed(dt.AsSeconds()) * m_transform.Orientation() * m_speed * 100;
	m_transform.SetPosition(m_transform.Position() + displacement);

	GameObject::Tick(dt);
}

bool Projectile::OnHit(Collision::Info info)
{
	// Destroy bubbles on contact; the other GameObject* is available via info.pInstigator
	if (auto pBubble = dynamic_cast<Bubble*>(info.pInstigator))
	{
		LOG_D("[%s] Collided with %s, destroying both", m_name.data(), pBubble->Name().data());
		pBubble->Destroy();
		Destroy();
		return true;
	}
	return false;
}
} // namespace ME
