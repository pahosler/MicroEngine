#include "Collider.h"

namespace ME
{
namespace
{
struct CircleLocus
{
	CircleData topLeft;
	CircleData topRight;
	CircleData bottomLeft;
	CircleData bottomRight;
	AABBData horizontal;
	AABBData vertical;
	CircleLocus() = default;
	CircleLocus(CircleLocus&&) = default;
	CircleLocus& operator=(CircleLocus&&) = default;

	bool IsPointIn(Vector2 point)
	{
		return topLeft.IsPointInCircle(point) || topRight.IsPointInCircle(point) || bottomLeft.IsPointInCircle(point)
			   || bottomRight.IsPointInCircle(point) || horizontal.IsPointInRect(point) || vertical.IsPointInRect(point);
	}
};

CircleLocus Intersect(const AABBData& bounds, const CircleData& circle)
{
	CircleLocus ret;
	ret.topLeft.centre = Vector2(bounds.lowerBound.x, bounds.upperBound.y);
	ret.topRight.centre = Vector2(bounds.upperBound.x, bounds.upperBound.y);
	ret.bottomLeft.centre = Vector2(bounds.lowerBound.x, bounds.lowerBound.y);
	ret.bottomRight.centre = Vector2(bounds.upperBound.x, bounds.lowerBound.y);
	ret.topLeft.radius = ret.topRight.radius = ret.bottomLeft.radius = ret.bottomRight.radius = circle.radius;
	ret.horizontal = AABBData(Vector2(bounds.lowerBound.x - circle.radius, bounds.lowerBound.y),
							  Vector2(bounds.upperBound.x + circle.radius, bounds.upperBound.y));
	ret.vertical = AABBData(Vector2(bounds.lowerBound.x, bounds.lowerBound.y - circle.radius),
							Vector2(bounds.upperBound.x, bounds.upperBound.y + circle.radius));
	return ret;
}
} // namespace

#if defined(DEBUGGING)
u16 Collider::s_debugShapeWidth = 2;
bool Collider::s_bShowDebugShape = false;
#endif

Collider::~Collider() = default;

void Collider::OnHit(const Collider& other)
{
	if (m_callback)
	{
		m_callback(*this, other);
	}
}

CircleCollider::CircleCollider(std::string ownerName)
{
	static const std::string NAME = "CircleCollider";
	m_name.reserve(ownerName.size() + NAME.size() + 1);
	if (!ownerName.empty())
	{
		m_name += std::move(ownerName);
		m_name += "-";
	}
	m_name += NAME;
}

void CircleCollider::SetCircle(Fixed diameter)
{
	m_circle.radius = diameter * Fixed::OneHalf;
}

CircleData CircleCollider::WorldCircle() const
{
	return CircleData(m_circle.radius, m_position);
}

bool CircleCollider::IsIntersecting(const Collider& other) const
{
	return other.IsIntersectCircle(*this);
}

bool CircleCollider::IsIntersectAABB(const AABBCollider& other) const
{
	CircleData self = WorldCircle();
	CircleLocus locus = Intersect(other.WorldAABB(), self);
	return locus.IsPointIn(self.centre);
}

bool CircleCollider::IsIntersectCircle(const CircleCollider& other) const
{
	CircleData self = WorldCircle();
	CircleData rhs = other.WorldCircle();
	return self.IsIntersecting(rhs);
}

AABBCollider::AABBCollider(std::string ownerName)
{
	static const std::string NAME = "AABBCollider";
	m_name.reserve(ownerName.size() + NAME.size() + 1);
	if (!ownerName.empty())
	{
		m_name += std::move(ownerName);
		m_name += "-";
	}
	m_name += NAME;
}

void AABBCollider::SetAABB(const AABBData& aabb)
{
	m_aabb = aabb;
}

AABBData AABBCollider::WorldAABB() const
{
	return AABBData(m_aabb.lowerBound + m_position, m_aabb.upperBound + m_position);
}

bool AABBCollider::IsIntersecting(const Collider& other) const
{
	return other.IsIntersectAABB(*this);
}

bool AABBCollider::IsIntersectAABB(const AABBCollider& other) const
{
	AABBData self = WorldAABB();
	AABBData rhs = other.WorldAABB();
	return self.Intersecting(rhs);
}

bool AABBCollider::IsIntersectCircle(const CircleCollider& other) const
{
	return other.IsIntersectAABB(*this);
}

} // namespace ME
