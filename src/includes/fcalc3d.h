#ifndef TY_FCALC3D
#define TY_FCALC3D

#define TY_FCALC3D_NPURE

#include <string>
#include <numeric>
#ifndef TY_FCALC3D_PURE
#include <xmmintrin.h>
#endif
#include <cassert>
#include <vector>
#include <cmath>


namespace ty
{


template<typename T>
constexpr T pi = static_cast<T>(3.1415926535897932384626);

template<typename T>
constexpr T big_eps = 16 * std::numeric_limits<T>::epsilon();

template<typename T>
constexpr T almost_zero = std::numeric_limits<T>::epsilon() / 2;

template<typename T>
constexpr T almost_one = static_cast<T>(1) - almost_zero<T>;

template<typename T, typename V>
inline T Lerp(T v0, T v1, V t)
{
	return (1 - t)*v0 + t * v1;
}

template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
Clamp(T v, T vmin, T vmax)
{
	return v > vmin ? (v < vmax ? v : vmax) : vmin;
}

template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
SmoothStep(T edge0, T edge1, T v)
{
	auto t = Clamp((v - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1));
	return t * t * (static_cast<T>(3) - static_cast<T>(2) * t);
}

template<typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
Fract(T v)
{
	T integral_part;
	return std::modf(v, &integral_part);
}

inline float Perlin(float x, float y, float z, int repeat = 0)
{
	auto hash = [](int xi, int yi, int zi) {
		static int perm[] = {
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
		};

		return perm[perm[perm[xi] + yi] + zi];
	};

	auto grad = [](int hash, float x, float y, float z)->float {
		switch (hash & 0xF) {
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		default: return 0;
		}
	};

	auto xi = static_cast<int>(x) & 255;
	auto xf = Fract(x);
	auto yi = static_cast<int>(y) & 255;
	auto yf = Fract(y);
	auto zi = static_cast<int>(z) & 255;
	auto zf = Fract(z);

	if (repeat > 0) {
		xi = xi % repeat;
		yi = yi % repeat;
		zi = zi % repeat;
	}

	auto fade = [](float t) { return t * t * t * (t * (t * 6.f - 15.f) + 10.f); };

	float u = fade(xf);
	float v = fade(yf);
	float w = fade(zf);

	float l1 = Lerp(grad(hash(xi, yi, zi), xf, yf, zf), grad(hash(xi + 1, yi, zi), xf - 1, yf, zf), u);
	float l2 = Lerp(grad(hash(xi, yi + 1, zi), xf, yf - 1, zf), grad(hash(xi + 1, yi + 1, zi), xf - 1, yf - 1, zf), u);
	float l3 = Lerp(grad(hash(xi, yi + 1, zi + 1), xf, yf - 1, zf - 1), grad(hash(xi + 1, yi + 1, zi + 1), xf - 1, yf - 1, zf - 1), u);
	float l4 = Lerp(grad(hash(xi, yi, zi + 1), xf, yf, zf - 1), grad(hash(xi + 1, yi, zi + 1), xf - 1, yf, zf - 1), u);

	float l12 = Lerp(l1, l2, v);
	float l43 = Lerp(l4, l3, v);
	return (Lerp(l12, l43, w) + 1.f) / 2.f;
}

class RandomEngine {
public:
	RandomEngine(uint64_t seed)
		: state_{ seed }
	{
		Advance();
	}

	uint32_t max()
	{
		return std::numeric_limits<uint32_t>::max();
	}

	uint32_t min()
	{
		return std::numeric_limits<uint32_t>::min();
	}

	uint32_t operator()()
	{
		auto old_state = state_;
		Advance();

		auto xorshift = static_cast<uint32_t>((old_state ^ (old_state >> 18u)) >> 27u);

#ifndef TY_FCALC3D_PURE
		return _rotr(xorshift, old_state >> 59u);
#else
		auto shift = old_state >> 59u;
		auto tmp = xorshift >> shift;
		tmp |= xorshift << (32 - shift);
		return tmp;
#endif
	}

private:
	void Advance()
	{
		state_ = state_ * 0x853c49e6748fea9bull + 0xda3e39cb94b95bdbull;
	}

	uint64_t state_;
};

template<int low, int high>
inline
typename std::enable_if<static_cast<uint32_t>(high - low) == std::numeric_limits<uint32_t>::max(), int>::type
Unif(RandomEngine &prng)
{
	return static_cast<int>(prng());
}

template<int low, int high>
inline
typename std::enable_if<static_cast<uint32_t>(high - low) != std::numeric_limits<uint32_t>::max(), int>::type
Unif(RandomEngine &prng)
{
	constexpr uint32_t num_samples = static_cast<uint32_t>(high - low) + 1;
	constexpr uint32_t num_drops = static_cast<uint32_t>(-static_cast<int>(num_samples)) % num_samples;
	constexpr uint32_t threshold = std::numeric_limits<uint32_t>::max() - num_drops;

	while (true) {
		auto rawrand = prng();
		if (rawrand <= threshold)
			return static_cast<int>(rawrand % num_samples + low);
	}
}

// Returns value inside open interval (0,1).
inline float Unif(RandomEngine &prng)
{
	float one = 1.0f;
	uint32_t skew = reinterpret_cast<uint32_t&>(one) | (prng() & 0x7fffffu);
	float result = reinterpret_cast<float&>(skew) - ty::almost_one<float>;
	return reinterpret_cast<float&>(result);
}

struct vec2 {
	union {
		struct { float x, y; };
		float data[2];
	};

	vec2() = default;

	constexpr vec2(float x, float y)
		:x{ x }, y{ y }
	{}

	constexpr vec2(float v)
		: x{ v }, y{ v }
	{}

	float &operator[](const unsigned i)
	{
		return data[i];
	}

	float operator[](const unsigned i) const
	{
		return data[i];
	}

	vec2 &operator+=(const float scalar)
	{
		x += scalar;
		y += scalar;
		return *this;
	}

	vec2 &operator+=(const vec2 &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	vec2 &operator-=(const float scalar)
	{
		x -= scalar;
		y -= scalar;
		return *this;
	}

	vec2 &operator-=(const vec2 &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	vec2 &operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	vec2 &operator*=(const vec2 &v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	vec2 &operator/=(const float scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	vec2 &operator/=(const vec2 &v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	float Norm() const
	{
		return sqrtf(x * x + y * y);
	}

	float InfNorm() const
	{
		return x < y ? y : x;
	}
};

struct vec3 {
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
		float data[3];
	};

	vec3() = default;

	constexpr vec3(float x, float y, float z)
		: x{ x }, y{ y }, z{ z }
	{}

	constexpr vec3(float v)
		: x{ v }, y{ v }, z{ v }
	{}

	float &operator[](const unsigned i)
	{
		return data[i];
	}

	float operator[](const unsigned i) const
	{
		return data[i];
	}

	vec3 &operator+=(const float scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;
		return *this;
	}

	vec3 &operator+=(const vec3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3 &operator-=(const float scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;
		return *this;
	}

	vec3 &operator-=(const vec3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3 &operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	vec3 &operator*=(const vec3 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	vec3 &operator/=(const float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	vec3 &operator/=(const vec3 &v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	float Norm() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float InfNorm() const
	{
		return (x < y) ? (y < z ? z : y) : (x < z ? z : x);
	}
};

struct alignas(16) vec4
{
	union {
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float data[4];
	};

	vec4() = default;

#ifndef TY_FCALC3D_PURE
	vec4(__m128 v)
	{
		reinterpret_cast<__m128&>(data) = v;
	}
#endif

	constexpr vec4(float x, float y, float z, float w)
		:x{ x }, y{ y }, z{ z }, w{ w }
	{}

	constexpr vec4(float v)
		: x{ v }, y{ v }, z{ v }, w{ v }
	{}

	float &operator[](const unsigned i)
	{
		return data[i];
	}

	float operator[](const unsigned i) const
	{
		return data[i];
	}

#ifndef TY_FCALC3D_PURE
	vec4 &operator=(__m128 v)
	{
		reinterpret_cast<__m128&>(data) = v;
		return *this;
	}
#endif // !TY_FCLAC3D_PURE


	vec4 &operator+=(const float scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;
		w += scalar;
		return *this;
	}

	vec4 &operator+=(const vec4 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	vec4 &operator-=(const float scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;
		w -= scalar;
		return *this;
	}

	vec4 &operator-=(const vec4 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	vec4 &operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	vec4 &operator*=(const vec4 &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	vec4 &operator/=(const float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return *this;
	}

	vec4 &operator/=(const vec4 v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}
};

template<typename T>
constexpr T Min(const T &lhs, const T &rhs)
{
	return rhs < lhs ? rhs : lhs;
}

template<typename T>
constexpr T Max(const T &lhs, const T &rhs)
{
	return rhs > lhs ? rhs : lhs;
}

inline vec2 CompMin(vec2 a, vec2 b)
{
	vec2 tmp;
	tmp.x = Min(a.x, b.x);
	tmp.y = Min(a.y, b.y);
	return tmp;
}

inline vec3 CompMin(vec3 a, vec3 b)
{
	vec3 tmp;
	tmp.x = Min(a.x, b.x);
	tmp.y = Min(a.y, b.y);
	tmp.z = Min(a.z, b.z);
	return tmp;
}

#ifndef TY_FCALC3D_PURE
inline vec4 CompMin(vec4 a, vec4 b)
{
	return _mm_min_ps(
		reinterpret_cast<__m128&>(a.data),
		reinterpret_cast<__m128&>(b.data));
}
#else
inline vec4 CompMin(vec4 a, vec4 b)
{
	vec4 tmp;
	tmp.x = Min(a.x, b.x);
	tmp.y = Min(a.y, b.y);
	tmp.z = Min(a.z, b.z);
	tmp.w = Min(a.w, b.w);
	return tmp;
}
#endif

inline vec2 CompMax(vec2 a, vec2 b)
{
	vec2 tmp;
	tmp.x = Max(a.x, b.x);
	tmp.y = Max(a.y, b.y);
	return tmp;
}

inline vec3 CompMax(vec3 a, vec3 b)
{
	vec3 tmp;
	tmp.x = Max(a.x, b.x);
	tmp.y = Max(a.y, b.y);
	tmp.z = Max(a.z, b.z);
	return tmp;
}


#ifndef TY_FCALC3D_PURE
inline vec4 CompMax(vec4 a, vec4 b)
{
	return _mm_max_ps(
		reinterpret_cast<__m128&>(a.data),
		reinterpret_cast<__m128&>(b.data));
}
#else
inline vec4 CompMax(vec4 a, vec4 b)
{
	vec4 tmp;
	tmp.x = Max(a.x, b.x);
	tmp.y = Max(a.y, b.y);
	tmp.z = Max(a.z, b.z);
	tmp.w = Max(a.w, b.w);
	return tmp;
}
#endif

struct mat3 {
	union {
		vec3 cols[3];
		float data[9];
	};

	mat3() = default;
	mat3(float v)
		:mat3()
	{
		cols[0][0] = v;
		cols[1][1] = v;
		cols[2][2] = v;
	}

	vec3 &operator[](const unsigned i)
	{
		return cols[i];
	}

	const vec3 operator[](const unsigned i) const
	{
		return cols[i];
	}

	mat3 &operator*=(float v)
	{
		for (int i = 0; i < 3; ++i)
			cols[i] *= v;
		return *this;
	}
};

struct alignas(16) mat4
{
	union {
		vec4 cols[4];
		float data[16];
	};
	mat4() = default;
	mat4(float v)
		:mat4()
	{
		cols[0][0] = v;
		cols[1][1] = v;
		cols[2][2] = v;
		cols[3][3] = v;
	}

	mat4(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
	{
		cols[0] = v0;
		cols[1] = v1;
		cols[2] = v2;
		cols[3] = v3;
	}

	vec4 &operator[](const unsigned i)
	{
		return cols[i];
	}

	const vec4 operator[](const unsigned i) const
	{
		return cols[i];
	}

	mat4 &operator*=(float v)
	{
		for (int i = 0; i < 4; ++i)
			cols[i] *= v;
		return *this;
	}
};

struct quat {
	float w;
	union {
		struct { float x, y, z; };
		vec3 v;
	};

	quat() = default;
	quat(float w, float x, float y, float z)
		:w{ w }, x{ x }, y{ y }, z{ z }
	{}
	quat(float w, vec3 v)
		:w{ w }, v{ v }
	{}

	quat &operator*=(const quat &q)
	{
		const quat lhs{ *this };
		const quat rhs{ q };

		w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
		x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
		y = lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z;
		z = lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x;
		return *this;
	}

	quat &operator*=(const float scalar)
	{
		w *= scalar;
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	quat &operator/=(const float scalar)
	{
		w /= scalar;
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	float Norm() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	quat Normalize() const
	{
		float n = Norm();
		return quat{ w / n,x / n,y / n,z / n };
	}

	quat Conj() const
	{
		return quat{ w,-x,-y,-z };
	}
};

inline quat AngleAxisToQuat(const float angle, vec3 axis)
{
	const float s = sin(angle * .5f);
	quat q;
	axis /= axis.Norm();
	q.x = axis.x * s;
	q.y = axis.y * s;
	q.z = axis.z * s;
	q.w = cos(angle * .5f);
	return q;
}

inline mat4 QuatToMat4(quat q)
{
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float xz = q.x * q.z;
	float xy = q.x * q.y;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	auto tmp = mat4{ 1.f };

	tmp[0][0] = 1.f - 2.f * (yy + zz);
	tmp[0][1] = 2.f * (xy + wz);
	tmp[0][2] = 2.f * (xz - wy);

	tmp[1][0] = 2.f * (xy - wz);
	tmp[1][1] = 1.f - 2.f * (xx + zz);
	tmp[1][2] = 2.f * (yz + wx);

	tmp[2][0] = 2.f * (xz + wy);
	tmp[2][1] = 2.f * (yz - wx);
	tmp[2][2] = 1.f - 2.f * (xx + yy);
	return tmp;
}

inline vec2 operator+(const vec2 &v)
{
	return v;
}
inline vec2 operator+(const vec2 &v1, const vec2 &v2)
{
	vec2 tmp{ v1 };
	tmp += v2;
	return tmp;
}
inline vec2 operator+(const vec2 &v, const float scalar)
{
	vec2 tmp{ v };
	tmp += scalar;
	return tmp;
}
inline vec2 operator+(const float scalar, const vec2 &v)
{
	return v + scalar;
}
inline vec2 operator-(const vec2 &v)
{
	vec2 tmp{ -v.x,-v.y };
	return tmp;
}
inline vec2 operator-(const vec2 &v1, const vec2 &v2)
{
	vec2 tmp{ v1 };
	tmp -= v2;
	return tmp;
}
inline vec2 operator-(const vec2 &v, const float scalar)
{
	vec2 tmp{ v };
	tmp -= scalar;
	return tmp;
}
inline vec2 operator-(const float scalar, const vec2 &v)
{
	return -v + scalar;
}
inline vec2 operator*(const vec2 &v1, const vec2 &v2)
{
	vec2 tmp{ v1 };
	tmp *= v2;
	return tmp;
}
inline vec2 operator*(const vec2 &v, const float scalar)
{
	vec2 tmp{ v };
	tmp *= scalar;
	return tmp;
}
inline vec2 operator*(const float scalar, const vec2 &v)
{
	vec2 tmp{ v };
	tmp *= scalar;
	return tmp;
}
inline vec2 operator/(const vec2 &v1, const vec2 &v2)
{
	vec2 tmp{ v1 };
	tmp /= v2;
	return tmp;
}
inline vec2 operator/(const vec2 &v, const float scalar)
{
	vec2 tmp{ v };
	tmp /= scalar;
	return tmp;
}
inline vec2 operator/(const float scalar, const vec2 &v)
{
	vec2 tmp{ scalar,scalar };
	tmp /= v;
	return tmp;
}

/*
// Vec3
*/

inline vec3 operator+(const vec3 &v)
{
	return v;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2)
{
	vec3 tmp{ v1 };
	tmp += v2;
	return tmp;
}

inline vec3 operator+(const vec3 &v, const float scalar)
{
	vec3 tmp{ v };
	tmp += scalar;
	return tmp;
}

inline vec3 operator+(const float scalar, const vec3 &v)
{
	return v + scalar;
}

inline vec3 operator-(const vec3 &v)
{
	vec3 tmp{ -v.x,-v.y,-v.z };
	return tmp;
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2)
{
	vec3 tmp{ v1 };
	tmp -= v2;
	return tmp;
}

inline vec3 operator-(const vec3 &v, const float scalar)
{
	vec3 tmp{ v };
	tmp -= scalar;
	return tmp;
}

inline vec3 operator-(const float scalar, const vec3 &v)
{
	return -v + scalar;
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2)
{
	vec3 tmp{ v1 };
	tmp *= v2;
	return tmp;
}

inline vec3 operator*(const vec3 &v, const float scalar)
{
	vec3 tmp{ v };
	tmp *= scalar;
	return tmp;
}

inline vec3 operator*(const float scalar, const vec3 &v)
{
	vec3 tmp{ v };
	tmp *= scalar;
	return tmp;
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2)
{
	vec3 tmp{ v1 };
	tmp /= v2;
	return tmp;
}

inline vec3 operator/(const vec3 &v, const float scalar)
{
	vec3 tmp{ v };
	tmp /= scalar;
	return tmp;
}

inline vec3 operator/(const float scalar, const vec3 &v)
{
	vec3 tmp{ scalar,scalar,scalar };
	tmp /= v;
	return tmp;
}

/*
// Vec4
*/

inline vec4 operator+(const vec4 &v)
{
	return v;
}

inline vec4 operator+(const vec4 &v1, const vec4 &v2)
{
	vec4 tmp{ v1 };
	tmp += v2;
	return tmp;
}

inline vec4 operator+(const vec4 &v, const float scalar)
{
	vec4 tmp{ v };
	tmp += scalar;
	return tmp;
}

inline vec4 operator+(const float scalar, const vec4 &v)
{
	return v + scalar;
}

inline vec4 operator-(const vec4 &v)
{
	vec4 tmp{ -v.x,-v.y,-v.z,-v.w };
	return tmp;
}

inline vec4 operator-(const vec4 &v1, const vec4 &v2)
{
	vec4 tmp{ v1 };
	tmp -= v2;
	return tmp;
}

inline vec4 operator-(const vec4 &v, const float scalar)
{
	vec4 tmp{ v };
	tmp -= scalar;
	return tmp;
}

inline vec4 operator-(const float scalar, const vec4 &v)
{
	return -v + scalar;
}

inline vec4 operator*(const vec4 &v1, const vec4 &v2)
{
	vec4 tmp{ v1 };
	tmp *= v2;
	return tmp;
}

inline vec4 operator*(const vec4 &v, const float scalar)
{
	vec4 tmp{ v };
	tmp *= scalar;
	return tmp;
}

inline vec4 operator*(const float scalar, const vec4 &v)
{
	vec4 tmp{ v };
	tmp *= scalar;
	return tmp;
}

inline vec4 operator/(const vec4 &v1, const vec4 &v2)
{
	vec4 tmp{ v1 };
	tmp /= v2;
	return tmp;
}

inline vec4 operator/(const vec4 &v, const float scalar)
{
	vec4 tmp{ v };
	tmp /= scalar;
	return tmp;
}

inline vec4 operator/(const float scalar, const vec4 &v)
{
	vec4 tmp{ scalar,scalar,scalar,scalar };
	tmp /= v;
	return tmp;
}

inline quat operator*(const quat &p, const quat &q)
{
	quat tmp{ p };
	tmp *= q;
	return tmp;
}

inline quat operator*(const quat &p, const float scalar)
{
	quat tmp{ p };
	tmp *= scalar;
	return tmp;
}

inline quat operator/(const quat &p, const float scalar)
{
	quat tmp{ p };
	tmp /= scalar;
	return tmp;
}

inline float Dot(const vec2 &v1, const vec2 &v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

inline float Dot(const vec3 &v1, const vec3 &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

inline float Dot(const quat q1, const quat q2)
{
	return q1.w*q2.w + Dot(q1.v, q2.v);
}

inline vec3 Cross(const vec3 &v1, const vec3 &v2)
{
	return vec3{
		v1.y * v2.z - v2.y * v1.z,
		v1.z * v2.x - v2.z * v1.x,
		v1.x * v2.y - v2.x * v1.y
	};
}

inline vec3 operator*(const quat &q, vec3 v)
{
	auto tmp1 = 2.f*Dot(q.v, v)*q.v;
	auto tmp2 = (q.w*q.w - Dot(q.v, q.v)) * v;
	auto tmp3 = 2.f*q.w*Cross(q.v, v);
	return (tmp1 + tmp2 + tmp3) / Dot(q, q);
}

template<typename T>
inline T Normalize(const T &v)
{
	auto len = v.Norm();
	return v / len;
}

inline mat4 Translation(vec3 trans)
{
	auto tmp = mat4{ 1.f };
	tmp[3] = vec4{ trans.x,trans.y,trans.z,1.f };
	return tmp;
}

inline mat4 Scale(vec3 scale)
{
	mat4 tmp{};
	tmp[0][0] = scale.x;
	tmp[1][1] = scale.y;
	tmp[2][2] = scale.z;
	tmp[3][3] = 1.f;
	return tmp;
}

inline mat4 LookAt(const vec3 eye, const vec3 spot, const vec3 up)
{
	const vec3 f = Normalize(spot - eye);
	const vec3 s = Normalize(Cross(f, up));
	const vec3 u = Normalize(Cross(s, f));

	auto tmp = mat4{ 1.f };
	tmp[0][0] = s.x;
	tmp[1][0] = s.y;
	tmp[2][0] = s.z;
	tmp[0][1] = u.x;
	tmp[1][1] = u.y;
	tmp[2][1] = u.z;
	tmp[0][2] = -f.x;
	tmp[1][2] = -f.y;
	tmp[2][2] = -f.z;
	tmp[3][0] = -Dot(s, eye);
	tmp[3][1] = -Dot(u, eye);
	tmp[3][2] = Dot(f, eye);
	return tmp;
}

inline mat4 Perspective(
	const float FoVy, const float aspect, const float zNear, const float zFar)
{
	const float tan_half_fovy = tan(FoVy / 2.f);
	const float tan_half_fovx = aspect * tan_half_fovy;

	mat4 tmp{};
	tmp[0][0] = 1.f / (tan_half_fovx);
	tmp[1][1] = 1.f / (tan_half_fovy);
	tmp[2][2] = -(zFar + zNear) / (zFar - zNear);
	tmp[2][3] = -1.f;
	tmp[3][2] = -(2.f * zFar * zNear) / (zFar - zNear);
	return tmp;
}

inline mat4 Orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane)
{
	auto tmp = mat4{ 1.f };
	tmp[0][0] = 2.f / (right - left);
	tmp[1][1] = 2.f / (top - bottom);
	tmp[2][2] = -2.f / (far_plane - near_plane);
	tmp[3][0] = -(right + left) / (right - left);
	tmp[3][1] = -(top + bottom) / (top - bottom);
	tmp[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
	return tmp;
}

#ifndef TY_FCALC3D_PURE
inline mat4 operator*(const mat4 &m1, const mat4 &m2)
{
	mat4 tmp;

	auto &m1_m128_ref = reinterpret_cast<__m128 const (&)[4]>(m1.data);

	for (unsigned i = 0; i < 4; ++i) {
		__m128 a = _mm_set_ps1(m2[i][0]);
		__m128 b = _mm_set_ps1(m2[i][1]);
		__m128 c = _mm_set_ps1(m2[i][2]);
		__m128 d = _mm_set_ps1(m2[i][3]);
		__m128 tmp_ = _mm_add_ps(
			_mm_add_ps(
				_mm_mul_ps(m1_m128_ref[0], a),
				_mm_mul_ps(m1_m128_ref[1], b)),
			_mm_add_ps(
				_mm_mul_ps(m1_m128_ref[2], c),
				_mm_mul_ps(m1_m128_ref[3], d)));
		tmp[i] = tmp_;
	}
	return tmp;
}
#else
inline mat4 operator*(const mat4 &m1, const mat4 &m2)
{
	mat4 tmp;

	for (int i = 0; i < 4; ++i)
		tmp[i] = m1[0] * m2[i].x + m1[1] * m2[i].y + m1[2] * m2[i].z + m1[3] * m2[i].w;

	return tmp;
}
#endif // !TY_FCALC3D_PURE

#ifndef TY_FCALC3D_PURE
inline vec4 operator*(const mat4 &m, const vec4 &v)
{
	__m128 a = _mm_set_ps1(v[0]);
	__m128 b = _mm_set_ps1(v[1]);
	__m128 c = _mm_set_ps1(v[2]);
	__m128 d = _mm_set_ps1(v[3]);

	auto &m_m128_ref = reinterpret_cast<__m128 const (&)[4]>(m.data);

	auto tmp = _mm_add_ps(
		_mm_add_ps(
			_mm_mul_ps(m_m128_ref[0], a),
			_mm_mul_ps(m_m128_ref[1], b)),
		_mm_add_ps(
			_mm_mul_ps(m_m128_ref[2], c),
			_mm_mul_ps(m_m128_ref[3], d)));

	return vec4{ tmp };
}
#else
inline vec4 operator*(const mat4 &m, const vec4 &v)
{
	return m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w;
}
#endif // !TY_FCALC3D_PURE

inline mat4 operator*(const mat4 &m, const float &v)
{
	mat4 tmp = m;
	tmp *= v;
	return tmp;
}

inline float __ComputeDet(
	float a, float b, float c, float d,
	float e, float f, float g, float h,
	float i, float j, float k, float l,
	float m, float n, float o, float p)
{
	float det =
		d * g*j*m - c * h*j*m - d * f*k*m + b * h*k*m +
		c * f*l*m - b * g*l*m - d * g*i*n + c * h*i*n +
		d * e*k*n - a * h*k*n - c * e*l*n + a * g*l*n +
		d * f*i*o - b * h*i*o - d * e*j*o + a * h*j*o +
		b * e*l*o - a * f*l*o - c * f*i*p + b * g*i*p +
		c * e*j*p - a * g*j*p - b * e*k*p + a * f*k*p;
	return det;
}

inline mat4 __ComputeInverse(
	float a, float b, float c, float d,
	float e, float f, float g, float h,
	float i, float j, float k, float l,
	float m, float n, float o, float p)
{
	mat4 mat;

	mat[0] = vec4{
		-h * k*n + g * l*n + h * j*o - f * l*o - g * j*p + f * k*p,
		d*k*n - c * l*n - d * j*o + b * l*o + c * j*p - b * k*p,
		-d * g*n + c * h*n + d * f*o - b * h*o - c * f*p + b * g*p,
		d*g*j - c * h*j - d * f*k + b * h*k + c * f*l - b * g*l
	};

	mat[1] = vec4{
		h*k*m - g * l*m - h * i*o + e * l*o + g * i*p - e * k*p	,
		-d * k*m + c * l*m + d * i*o - a * l*o - c * i*p + a * k*p,
		d*g*m - c * h*m - d * e*o + a * h*o + c * e*p - a * g*p	,
		-d * g*i + c * h*i + d * e*k - a * h*k - c * e*l + a * g*l
	};

	mat[2] = vec4{
		-h * j*m + f * l*m + h * i*n - e * l*n - f * i*p + e * j*p,
		d*j*m - b * l*m - d * i*n + a * l*n + b * i*p - a * j*p	,
		-d * f*m + b * h*m + d * e*n - a * h*n - b * e*p + a * f*p,
		d*f*i - b * h*i - d * e*j + a * h*j + b * e*l - a * f*l
	};

	mat[3] = vec4{
		g*j*m - f * k*m - g * i*n + e * k*n + f * i*o - e * j*o,
		-c * j*m + b * k*m + c * i*n - a * k*n - b * i*o + a * j*o,
		c*f*m - b * g*m - c * e*n + a * g*n + b * e*o - a * f*o,
		-c * f*i + b * g*i + c * e*j - a * g*j - b * e*k + a * f*k
	};

	float det = __ComputeDet(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
#ifndef TY_FCALC3D_PURE
	auto det_inv = _mm_set_ps1(1.f / det);

	auto &m128_ref = reinterpret_cast<__m128(&)[4]>(mat.data);
	m128_ref[0] = _mm_mul_ps(m128_ref[0], det_inv);
	m128_ref[1] = _mm_mul_ps(m128_ref[1], det_inv);
	m128_ref[2] = _mm_mul_ps(m128_ref[2], det_inv);
	m128_ref[3] = _mm_mul_ps(m128_ref[3], det_inv);
#else
	float det_inv = 1.f / det;
	mat *= det_inv;
#endif

	return mat;
}

inline float Det(const mat4 &m)
{
	return __ComputeDet(
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]);
}

inline mat4 Inverse(const mat4 m)
{
	return __ComputeInverse(
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]);
}

inline vec3 UnProject(const vec3& winpos, const mat4& model, const mat4& proj, const vec4& viewport)
{
	auto inv = Inverse(proj * model);

	vec4 tmp{
		(winpos.x - viewport[0]) / viewport[2] * 2.f - 1.f,
		(winpos.y - viewport[1]) / viewport[3] * 2.f - 1.f,
		winpos.z * 2.f - 1.f,
		1.f };

	vec4 obj = inv * tmp;
	obj /= obj.w;

	return vec3{ obj.x,obj.y,obj.z };
}

inline std::vector<vec3> ApplyTransform(const mat4 &Trans, const std::vector<vec3> &ps)
{
	std::vector<vec3> pstrans(ps.size());
	for (std::size_t i = 0; i < ps.size(); ++i) {
		auto p = ps[i];
		auto res = Trans * vec4{ p.x,p.y,p.z,1.f };
		res /= res.w;
		pstrans[i] = { res.x,res.y,res.z };
	}
	return pstrans;
}

constexpr float DegreeToRadian(const float degree)
{
	std::numeric_limits<int>::max();
	return degree * pi<float> / 180.f;
}

inline float FresnelReflCoeff(float cos_wi, float ni, float nt)
{
	cos_wi = ty::Clamp(cos_wi, -1.f, 1.f);
	if (cos_wi < 0) {
		std::swap(ni, nt);
		cos_wi = -cos_wi;
	}
	float sin_wi = sqrt(1.f - cos_wi * cos_wi);

	float sin_wt = ni * sin_wi / nt;
	if (sin_wt >= 1.f)
		return 1.f;
	float cos_wt = sqrt(1.f - sin_wt * sin_wt);

	float Rs = (ni*cos_wi - nt * cos_wt) / (ni*cos_wi + nt * cos_wt);
	float Rp = (nt*cos_wi - ni * cos_wt) / (nt*cos_wi + ni * cos_wt);

	return .5f*(Rs*Rs + Rp * Rp);
}

inline ty::vec2 FloatArrayToVec(float(&v)[2])
{
	return ty::vec2{ v[0],v[1] };
}

inline ty::vec3 FloatArrayToVec(float(&v)[3])
{
	return ty::vec3{ v[0],v[1],v[2] };
}

inline ty::vec4 FloatArrayToVec(float(&v)[4])
{
	return ty::vec4{ v[0],v[1],v[2],v[3] };
}


}


#endif // !TY_FCALC3D