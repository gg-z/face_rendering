#ifndef TY_EXTENSO
#define TY_EXTENSO


#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <memory>
#include <numeric>
#include <cassert>
#include <type_traits>
#include <array>

#include "fcalc3d.h"
#include "tiny_obj_loader.h"


#define TY_Die(reason)  ty::__DieImpl(reason,__FILE__,__LINE__)


namespace ty
{


inline std::string ReadFile(std::string path)
{
	std::ifstream fin(path);

	assert(!fin.fail());

	fin.ignore(std::numeric_limits<std::streamsize>::max());
	auto size = fin.gcount();
	fin.clear();

	fin.seekg(0, std::ios_base::beg);
	auto source = std::unique_ptr<char>(new char[size]);
	fin.read(source.get(), size);

	return std::string(source.get(), static_cast<std::string::size_type>(size));
}

class Catcher {
public:
	Catcher(unsigned milli)
	{
		update_interval_ = std::chrono::milliseconds(milli);
		next_update_time_ = std::chrono::system_clock::now();
	}

	template<typename FN>
	void Sync(FN fn)
	{
		while (std::chrono::system_clock::now() > next_update_time_) {
			fn();
			next_update_time_ += update_interval_;
		}
	}

	void SyncNoOp()
	{
		next_update_time_ = std::chrono::system_clock::now();
	}

private:
	std::chrono::time_point<std::chrono::system_clock> next_update_time_;
	std::chrono::milliseconds update_interval_;
};

inline void Print(std::string format)
{
	std::cout << format;
}

template<typename T>
inline void Print(std::string format, const T &value)
{
	auto op = format.find('{', 0);
	assert(op != std::string::npos);
	auto ed = format.find('}', op + 1);
	assert(ed != std::string::npos);

	std::cout << format.substr(0, op);
	if (ed - op != 1) {
		std::ios_base::fmtflags fmt_old_flags{ std::cout.flags() };

		auto info = format.substr(op + 1, ed - op - 1);
		if (info == "hex") {
			if (std::is_integral<T>::value)
				std::cout << std::hex << value;
			else if (std::is_floating_point<T>::value)
				std::cout << std::hexfloat << value;
			else
				std::cout << value;
		}

		std::cout.flags(fmt_old_flags);
	}
	else {
		std::cout << value;
	}

	std::cout << format.substr(ed + 1, format.size() - ed - 1);
}

template<typename T, typename... Args>
inline void Print(std::string format, const T &value, const Args &...args)
{
	auto pos = format.find('}', 0);
	assert(pos != std::string::npos);
	Print(format.substr(0, pos + 1), value);
	Print(format.substr(pos + 1, format.size() - pos - 1), args...);
}

inline void __DieImpl(std::string reason, std::string file_path, int line_number)
{
	Print("ERROR({})\n", reason);
	Print("File({})\nLine({})\n", file_path, line_number);
	std::exit(1);
}

template<typename FN>
inline void TimeProfile(FN fn, unsigned num_iters, std::string tag)
{
	auto op_time = std::chrono::system_clock::now();
	for (unsigned i = 0; i < num_iters; ++i)
		fn();
	auto ed_time = std::chrono::system_clock::now();

	auto elapse = std::chrono::duration_cast<std::chrono::milliseconds>(ed_time - op_time).count();

	Print("{}:{}ms/{} loops\n", tag, elapse, num_iters);
}

template<std::size_t N1, std::size_t N2, std::size_t... Idxs1, std::size_t... Idxs2>
constexpr std::array<char, N1 + N2 - 2> __ConcatImpl(
	const char(&a)[N1],
	const char(&b)[N2],
	std::index_sequence<Idxs1...>,
	std::index_sequence<Idxs2...>)
{
	return { a[Idxs1]...,b[Idxs2]... };
}

template<std::size_t N1, std::size_t N2>
constexpr auto __ConcatImpl(const char(&a)[N1], const char(&b)[N2])
{
	return __ConcatImpl(a, b, std::make_index_sequence<N1 - 1>(), std::make_index_sequence<N2 - 1>());
}

template<std::size_t N1, std::size_t N2, std::size_t... Idxs1, std::size_t... Idxs2>
constexpr std::array<char, N1 + N2 - 1> __ConcatImpl(
	const char(&a)[N1],
	const std::array<char, N2>(&b),
	std::index_sequence<Idxs1...>,
	std::index_sequence<Idxs2...>)
{
	return { a[Idxs1]...,b[Idxs2]... };
}

template<std::size_t N1, std::size_t N2>
constexpr auto __ConcatImpl(const char(&a)[N1], const std::array<char, N2>(&b))
{
	return __ConcatImpl(a, b, std::make_index_sequence<N1 - 1>(), std::make_index_sequence<N2>());
}

template<std::size_t N1, std::size_t N2, typename... Rests>
constexpr auto __ConcatImpl(const char(&a)[N1], const char(&b)[N2], const Rests&... args)
{
	return __ConcatImpl(a, __ConcatImpl(b, args...));
}

template<std::size_t N1, std::size_t N2>
inline std::string Concat(const char(&a)[N1], const std::array<char, N2>(&b))
{
	auto res = __ConcatImpl(a, b);
	return { res.data(),res.size() };
}

template<std::size_t N1, std::size_t N2, typename... Rests>
inline std::string Concat(const char(&a)[N1], const char(&b)[N2], const Rests&... args)
{
	auto res = __ConcatImpl(a, b, args...);
	return { res.data(),res.size() };
}

struct AABB {
	vec3 inf, sup;

	AABB()
	{
		inf = vec3{ std::numeric_limits<float>::max() };
		sup = vec3{ std::numeric_limits<float>::lowest() };
	}

	AABB(const std::vector<vec3> &ps)
		:AABB()
	{
		for (const auto &p : ps)
			Extend(p);
	}

	void Extend(const vec3 &p)
	{
		inf = CompMin(inf, p);
		sup = CompMax(sup, p);
	}

	void Extend(const std::vector<ty::vec3> &ps)
	{
		for (auto &p : ps)
			Extend(p);
	}

	bool IsInside(const vec3 &p) const
	{
		return p.x <= sup.x && p.y <= sup.y && p.z <= sup.z &&
			p.x >= inf.x && p.y >= inf.y && p.z >= inf.z;
	}

	vec3 Center() const
	{
		return (inf + sup) / 2.f;
	}

	vec3 Lengths() const
	{
		return sup - inf;
	}
};

constexpr vec3 HexToRGB(unsigned hex)
{
	constexpr float _255inv = 1.f / 255.f;
	auto r = ((hex & 0xff0000) >> 16) * _255inv;
	auto g = ((hex & 0x00ff00) >> 8)* _255inv;
	auto b = (hex & 0x0000ff)* _255inv;
	return { r,g,b };
}

constexpr vec4 HexToRGBA(unsigned hex)
{
	constexpr float _255inv = 1.f / 255.f;
	auto r = ((hex & 0xff000000) >> 24) * _255inv;
	auto g = ((hex & 0x00ff0000) >> 16)* _255inv;
	auto b = ((hex & 0x0000ff00) >> 8)* _255inv;
	auto a = (hex & 0x000000ff) * _255inv;
	return { r,g,b,a };
}

inline std::string StringView(vec2 v)
{
	return
		std::string("(") +
		std::to_string(v.x) +
		"," +
		std::to_string(v.y) +
		")";
}

inline std::string StringView(const vec3 &v)
{
	return
		std::string("(") +
		std::to_string(v.x) +
		"," +
		std::to_string(v.y) +
		"," +
		std::to_string(v.z) +
		")";
}

inline std::string StringView(const vec4 &v)
{
	return
		std::string("(") +
		std::to_string(v.x) +
		"," +
		std::to_string(v.y) +
		"," +
		std::to_string(v.z) +
		"," +
		std::to_string(v.w) +
		")";
}

inline std::string StringView(const mat3 &m)
{
	return
		"((" + std::to_string(m[0][0]) + "," + std::to_string(m[1][0]) + "," + std::to_string(m[2][0]) + ")\n" +
		" (" + std::to_string(m[0][1]) + "," + std::to_string(m[1][1]) + "," + std::to_string(m[2][1]) + ")\n" +
		" (" + std::to_string(m[0][2]) + "," + std::to_string(m[1][2]) + "," + std::to_string(m[2][2]) + "))\n";
}

inline std::string StringView(const mat4 &m)
{
	return
		"((" + std::to_string(m[0][0]) + "," + std::to_string(m[1][0]) + "," + std::to_string(m[2][0]) + "," + std::to_string(m[3][0]) + ")\n" +
		" (" + std::to_string(m[0][1]) + "," + std::to_string(m[1][1]) + "," + std::to_string(m[2][1]) + "," + std::to_string(m[3][1]) + ")\n" +
		" (" + std::to_string(m[0][2]) + "," + std::to_string(m[1][2]) + "," + std::to_string(m[2][2]) + "," + std::to_string(m[3][2]) + ")\n" +
		" (" + std::to_string(m[0][3]) + "," + std::to_string(m[1][3]) + "," + std::to_string(m[2][3]) + "," + std::to_string(m[3][3]) + "))";
}

inline std::string StringView(const quat &q)
{
	return
		std::string("(") +
		std::to_string(q.w) +
		"," +
		std::to_string(q.x) +
		"," +
		std::to_string(q.y) +
		"," +
		std::to_string(q.z) +
		")";
}

class ArcballCamera {
public:
	ArcballCamera()
		:rot_{ 1.f,0.f,0.f,0.f }, is_tracking_{ false }, rot_prev_{ 1.f,0.f,0.f,0.f }
	{}

	void Init(vec3 position, int width, int height, float FoVy)
	{
		width_ = width;
		height_ = height;
		aspect_ = static_cast<float>(width) / height;
		FoVy_ = FoVy;
		pos_ = position;
	}

	int Width() const { return width_; }
	int Height() const { return height_; }
	float Near() const { return .1f; }
	float Far() const { return 10.0f; }
	void Zoom(float value)
	{
		auto posdir = Normalize(pos_);
		pos_ += posdir * value;
	}

	vec3 Pos() { return pos_; }

	mat4 Proj()
	{
		return Perspective(FoVy_, aspect_, Near(), Far());
	}

	mat4 View()
	{
		auto view = ty::LookAt(pos_, { .0f,.0f,.0f }, { .0f,1.f,.0f });
		return view * QuatToMat4(Normalize(rot_));
	}

	void Track(float mousex, float mousey)
	{
		if (mousex < 0.f || mousey < 0.f || mousex > Width() || mousey > Height()) {
			is_tracking_ = false;
			return;
		}

		vec3 nhit;
		if (!ISect(mousex, mousey, &nhit))
			return;

		if (!is_tracking_) {
			nhit_prev_ = nhit;
			rot_prev_ = rot_;

			is_tracking_ = true;
		}

		if ((nhit - nhit_prev_).Norm() < big_eps<float>)
			return;

		auto qrot_axis = Cross(nhit_prev_, nhit);
		qrot_axis = rot_prev_.Conj()*qrot_axis;

		float dw = acos(Clamp(Dot(nhit_prev_, nhit), -1.f, 1.f));
		auto qrot = AngleAxisToQuat(dw, Normalize(qrot_axis));
		rot_ = rot_prev_ * qrot;
	}

private:
	bool ISect(float mousex, float mousey, vec3 *nhit) const
	{
		float y = height_ - 1 - mousey, x = mousex;
		vec3 winpos{ x,y,0.f };
		vec4 view_port{ 0,0,static_cast<float>(width_),static_cast<float>(height_) };

		auto unitpos_ = Normalize(pos_);

		auto view = LookAt(
			unitpos_,
			{ 0.f,0.f,0.f },
			vec3{ .0f,1.f,.0f }
		);

		auto tmp11 = UnProject(
			winpos,
			view,
			Perspective(FoVy_, aspect_, Near(), Far()),
			view_port
		);

		vec3 ro = unitpos_;
		vec3 rd = Normalize(tmp11 - ro);

		float r = .71f;  // Arcball radius.

		float a = 1.f;  // dot(rd,rd);
		float b = 2.f*Dot(ro, rd);
		float c = Dot(ro, ro) - r * r;

		float delta = b * b - 4 * a*c;
		if (delta < big_eps<float>)
			return false;

		float tmp1 = -b / (2.f*a);
		float tmp2 = sqrt(delta) / (2.f*a);

		float ans1 = tmp1 + tmp2, ans2 = tmp1 - tmp2;
		float t = ans1 < ans2 ? ans1 : ans2;
		*nhit = Normalize(ro + t * rd);
		return true;
	}

	float aspect_, FoVy_;
	int width_, height_;
	bool is_tracking_;
	quat rot_, rot_prev_;
	vec3 nhit_prev_;
	vec3 pos_;
};


class WanderCamera {
public:
	WanderCamera()
		:rot_{ 1.f,0.f,0.f,0.f },
		is_tracking_{ false },
		rot_prev_{ 1.f,0.f,0.f,0.f },
		up_{ 0.f,1.f,0.f }
	{}

	void Init(vec3 position,vec3 spot, int width, int height, float FoVy)
	{
		width_ = width;
		height_ = height;
		aspect_ = static_cast<float>(width) / height;
		FoVy_ = FoVy;
		pos_ = position;
		ndir_ = Normalize(spot - position);
	}

	void Forward(float dist)
	{
		pos_ += (rot_ * ndir_) * dist;
	}

	int Width() const { return width_; }
	int Height() const { return height_; }
	vec3 Pos() { return pos_; }

	mat4 Proj() const
	{
		return Perspective(FoVy_, aspect_, Near(), Far());
	}

	mat4 View() const
	{
		auto view = ty::LookAt(pos_, pos_+ Normalize(rot_) * ndir_, up_);
		return view;
	}

	float Near() const { return .01f; }
	float Far() const { return 1000.0f; }

	void Track(float mousex, float mousey)
	{
		if (mousex < 0.f || mousey < 0.f || mousex > Width() || mousey > Height()) {
			rot_prev_ = rot_;
			is_tracking_ = false;
			return;
		}

		vec3 nhit;
		if (!ISect(mousex, mousey, &nhit))
			return;

		if (!is_tracking_) {
			nhit_prev_ = nhit;
			is_tracking_ = true;
			return;
		}

		if ((nhit - nhit_prev_).Norm() < big_eps<float>)
			return;

		auto qrot_axis = Cross(nhit, nhit_prev_);
		qrot_axis = rot_prev_.Conj()*qrot_axis;

		float dw = acos(Clamp(Dot(nhit_prev_, nhit), -1.f, 1.f));
		auto qrot = AngleAxisToQuat(dw, Normalize(qrot_axis));
		rot_ = rot_prev_ * qrot;
	}

private:

	bool ISect(float mousex, float mousey, vec3 *nhit) const
	{
		float y = height_ - 1 - mousey, x = mousex;
		vec3 winpos{ x,y,0.f };
		vec4 view_port{ 0,0,static_cast<float>(width_),static_cast<float>(height_) };

		auto unitpos_ = Normalize(pos_);

		auto view = ty::LookAt(pos_, pos_ + Normalize(rot_prev_) * ndir_, up_);
		auto tmp11 = UnProject(
			winpos,
			view,
			Proj(),
			view_port
		);

		*nhit = Normalize(tmp11 - pos_);
		return true;
	}

	float aspect_, FoVy_;
	int width_, height_;
	bool is_tracking_;
	quat rot_, rot_prev_;
	vec3 nhit_prev_;
	vec3 pos_, ndir_, up_;
};


}


#endif // !TY_EXTENSO

