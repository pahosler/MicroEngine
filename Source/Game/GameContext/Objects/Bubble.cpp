#include "Bubble.h"
#include "Rocket.h"

namespace ME
{
void Bubble::OnCreate()
{
	// These are now member functions of `GameObject`!
	Instantiate(Primitive::Type::Circle);
	ShapeData data;
	data.oBorder = Fixed::One;
	SetShape(data);
}

void Bubble::OnRespawn()
{
	m_elapsed = Time::Zero;
	m_colour.a = 255;
}

void Bubble::Tick(Time dt)
{
	m_elapsed += dt;
	if (m_elapsed >= m_ttl)
	{
		// This bubble is ded
		Destroy();
		return;
	}

	// Push the object above by some amount (playtest and find out)
	// Scale it by `dt` so its independent of frame-rate (is larger
	// when more time elapses between frames, so overall movement
	// over time remains identical over varying framerates)
	m_ySpeed += Fixed(dt.AsSeconds() / 50);
	const Fixed dy = m_ySpeed * Fixed(dt.AsMilliseconds() / 5);
	const Vector2 newPos = m_transform.Position() + Vector2(0, dy);
	m_transform.SetPosition(newPos);

	// Properties might have changed, update the underlying shape to match
	ShapeData data;
	data.oSize = {m_diameter * Fixed::OneHalf, m_diameter * Fixed::OneHalf};
	data.oFill = m_colour;
	data.oOutline = m_outline;
	SetShape(data);

	// Finally let the base class update the actual primitive's transform
	GameObject::Tick(dt);
}

bool Bubble::OnHit(Collision::Info info)
{
	if (auto pRocket = dynamic_cast<Rocket*>(info.pInstigator))
	{
		LOG_D("[%s] Collided with %s, destroying all objects", m_name.data(), pRocket->Name().data());
		pRocket->Destroy();
		Destroy();
		return true;
	}
	return false;
}

void Bubble::NextTTL()
{
	// Decrement the time to live of the bubble by one second
	if (m_ttl > Time::Zero)
	{
		m_ttl -= Time::Seconds(1);
	}
}
} // namespace ME
