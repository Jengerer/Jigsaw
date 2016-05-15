#pragma once

#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

// Common shorthand type names.
using U32 = uint32_t;
using F32 = float;
using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Matrix4 = glm::mat4;
using Vertices2 = std::vector<Vector2>;
using Vertices3 = std::vector<Vector3>;
using Indices = std::vector<U32>;

namespace Math
{
	static constexpr float Pi = 3.14159265f;
	static constexpr U32 VerticesPerTriangle = 3U;
	static constexpr U32 TriangleToVerticesOffset = 2U;

	// Get maximum value between two.
	inline float Maximum(float a, float b)
	{
		return (a > b) ? a : b;
	}

	// Clamp a value between a minimum and maximum.
	inline float Clamp(float value, float minimum, float maximum)
	{
		if (value < minimum) {
			return minimum;
		}
		if (value > maximum) {
			return maximum;
		}
		return value;
	}

	// Linear interpolate between two values.
	inline float Lerp(float left, float right, float blend)
	{
		const float leftFactor = 1.f - blend;
		return (left * leftFactor) + (right *  blend);
	}

	// Sine function.
	inline F32 Sine(F32 value)
	{
		return sinf(value);
	}

	// Cosine function.
	inline F32 Cosine(F32 value)
	{
		return cosf(value);
	}

	// Arc-cosine function.
	inline F32 ArcCosine(F32 value)
	{
		return acosf(value);
	}

	// Normalize 2D vector.
	inline Vector2 Normalize2(const Vector2& vector)
	{
		return glm::normalize(vector);
	}

	// Normalize 3D vector.
	inline Vector2 Normalize3(const Vector3& vector)
	{
		return glm::normalize(vector);
	}

	// 2D vector dot product.
	inline F32 Dot2(const Vector2& a, const Vector2& b)
	{
		return glm::dot(a, b);
	}

	// 3D vector dot product.
	inline F32 Dot3(const Vector3& a, const Vector3& b)
	{
		return glm::dot(a, b);
	}

	// Useful constant vectors.
	static constexpr Vector2 Zero2(0.f);
	static constexpr Vector3 Zero3(0.f);
}

// Unused variables macro.
inline void Unused(...)
{
}
