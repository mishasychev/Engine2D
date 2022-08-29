#pragma once

#include <cmath>

namespace e2d
{
	class Vector2D
	{
	public:
		float x{ 0.0f };
		float y{ 0.0f };

		constexpr Vector2D() = default;
		constexpr ~Vector2D() = default;

		constexpr Vector2D(const float Quad) : x(Quad), y(Quad) {}
		constexpr Vector2D(const float X, const float Y) : x(X), y(Y) {}
		constexpr Vector2D(const int32_t Quad) : x(static_cast<float>(Quad)), y(x) {}
		constexpr Vector2D(const int32_t X, const int32_t Y) : x(static_cast<float>(X)), y(static_cast<float>(Y)) {}

		constexpr Vector2D(const Vector2D& V) = default;
		constexpr Vector2D& operator=(const Vector2D& V) = default;
		constexpr Vector2D(Vector2D&& V) = default;
		constexpr Vector2D& operator=(Vector2D&& V) = default;

		constexpr Vector2D operator+(const Vector2D& V) const { return { x + V.x, y + V.y }; }
		constexpr Vector2D operator-(const Vector2D& V) const { return { x - V.x, y - V.y }; }
		constexpr Vector2D operator*(const Vector2D& V) const { return { x * V.x, y * V.y }; }
		constexpr Vector2D operator/(const Vector2D& V) const { return { x / V.x, y / V.y }; }

		constexpr Vector2D operator+(const float value) const { return { x + value, y + value }; }
		constexpr Vector2D operator-(const float value) const { return { x - value, y - value }; }
		constexpr Vector2D operator*(const float value) const { return { x * value, y * value }; }
		constexpr Vector2D operator/(const float value) const { return { x / value, y / value }; }


		__forceinline Vector2D& operator+=(const Vector2D& V) { x += V.x; y += V.y; return *this; }
		__forceinline Vector2D& operator-=(const Vector2D& V) { x -= V.x; y -= V.y; return *this; }
		__forceinline Vector2D& operator*=(const Vector2D& V) { x *= V.x; y *= V.y; return *this; }
		__forceinline Vector2D& operator/=(const Vector2D& V) { x /= V.x; y /= V.y; return *this; }

		__forceinline Vector2D& operator+=(const float value) { x += value; y += value; return *this; }
		__forceinline Vector2D& operator-=(const float value) { x -= value; y -= value; return *this; }
		__forceinline Vector2D& operator*=(const float value) { x *= value; y *= value; return *this; }
		__forceinline Vector2D& operator/=(const float value) { x /= value; y /= value; return *this; }


		constexpr bool operator==(const Vector2D& V) const { return x == V.x && y == V.y; }
		constexpr bool operator!=(const Vector2D& V) const { return !(x == V.x && y == V.y); }

		[[nodiscard]] constexpr Vector2D operator-() const;

		[[nodiscard]] Vector2D Normalize() const;

		[[nodiscard]] constexpr float Max() const;

		[[nodiscard]] float Length() const;
		[[nodiscard]] constexpr float LengthSquared() const;

		[[nodiscard]] Vector2D Rotate(float angle) const;

		[[nodiscard]] Vector2D Abs() const;

		[[nodiscard]] static float Distance(const Vector2D& V1, const Vector2D& V2);
		[[nodiscard]] static constexpr float DistanceSquared(const Vector2D& V1, const Vector2D& V2);

		[[nodiscard]] static constexpr float DotProduct(const Vector2D& V1, const Vector2D& V2);

		[[nodiscard]] static float Angle(const Vector2D& V1, const Vector2D& V2);
	};

	__forceinline Vector2D Vector2D::Rotate(const float angle) const
	{
		return { x * std::cos(angle) - y * std::sin(angle),
			x * std::sin(angle) + y * std::cos(angle) };
	}

	__forceinline Vector2D Vector2D::Abs() const
	{
		return { std::abs(x), std::abs(y) };
	}

	__forceinline float Vector2D::Distance(const Vector2D& V1, const Vector2D& V2)
	{
		return std::sqrt(DistanceSquared(V1, V2));
	}

	constexpr float Vector2D::DistanceSquared(const Vector2D& V1, const Vector2D& V2)
	{
		return (V2.x - V1.x) * (V2.x - V1.x) + (V2.y - V1.y) * (V2.y - V1.y);
	}

	__forceinline Vector2D Vector2D::Normalize() const
	{
		const auto length = Length();
		return { x / length, y / length };
	}

	__forceinline float Vector2D::Length() const
	{
		return std::sqrt(x * x + y * y);
	}

	constexpr Vector2D Vector2D::operator-() const
	{
		return { -x, -y };
	}

	constexpr float Vector2D::Max() const
	{
		return (x > y) ? x : y;
	}

	constexpr float Vector2D::LengthSquared() const
	{
		return x * x + y * y;
	}

	__forceinline constexpr float Vector2D::DotProduct(const Vector2D& V1, const Vector2D& V2)
	{
		return V1.x * V2.x + V1.y * V2.y;
	}

	__forceinline float Vector2D::Angle(const Vector2D& V1, const Vector2D& V2)
	{
		return std::acos(DotProduct(V1, V2) / (V1.Length() * V2.Length()));
	}
}
