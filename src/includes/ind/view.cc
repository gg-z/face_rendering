#include "./view.h"

#include <numeric>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include "./bounds.h"


IndView::IndView(unsigned width, unsigned height, float FoVy, Bounds scene_box)
	: width_{ width }, height_{ height }, fov_y_{ FoVy }, scene_box_{ scene_box } {
	NearFar();
}

void IndView::Zoom(float dist) {
	if (near_ + dist <= 0.f) return;
	near_ += dist;
	far_ += dist;
}

glm::vec3 IndView::Pos() const {
	auto pos = scene_box_.Center();
	pos.z += SceneDistance();
	return pos;
}

Bounds IndView::SceneBox() const {
	return scene_box_;
}

float IndView::Aspect() const {
	return static_cast<float>(width_) / height_;
}

unsigned IndView::Width() const
{
	return width_;
}

unsigned IndView::Height() const
{
	return height_;
}

void IndView::NearFar() {
	float tmp1 = 2.f*glm::tan(fov_y_ / 2.f);
	float maxlen = glm::length(scene_box_.Lengths());
	near_ = maxlen / tmp1;
	far_ = near_ + maxlen;
}

float IndView::SceneDistance() const {
	return .5f*(near_ + far_);
}

glm::mat4 IndView::Proj() const {
	auto persp = glm::perspective(fov_y_, Aspect(), near_, far_);
	return persp;
}

glm::mat4 IndView::View() const {
	glm::mat4 trans{ 1.f };
	trans[3][0] = -scene_box_.Center().x;
	trans[3][1] = -scene_box_.Center().y;
	trans[3][2] = -scene_box_.Center().z;

	glm::mat4 trans_rev{ 1.f };
	trans_rev[3][0] = scene_box_.Center().x;
	trans_rev[3][1] = scene_box_.Center().y;
	trans_rev[3][2] = scene_box_.Center().z;

	auto view = glm::lookAt(Pos(), scene_box_.Center(), glm::vec3{ .0f,1.f,.0f });
	return view * trans_rev*glm::toMat4(rot_) *trans;
};

void IndView::Track(float mousex, float mousey) {
	glm::vec3 nhit;
	if (!ISect(mousex, mousey, &nhit))
		return;

	if (!arcball_track_) {
		nhit_op_ = nhit;
		arcball_track_ = true;
		return;
	}

	if (glm::length(nhit - nhit_op_) < 1e-3f)
		return;

	auto qrot_axis = glm::cross(nhit_op_, nhit);
	qrot_axis = glm::conjugate(rot_prev_) * qrot_axis;

	float cos_ = glm::dot(nhit_op_, nhit);
	cos_ = glm::clamp(cos_, -1.f, 1.f);
	float dw = glm::acos(cos_);

	auto qrot = glm::angleAxis(dw, glm::normalize(qrot_axis));
	rot_ = rot_prev_ * qrot;
}

bool IndView::Track()
{
	return arcball_track_;
}

void IndView::UnTrack() {
	rot_prev_ = rot_;
	arcball_track_ = false;
}

void IndView::Reset() {
	rot_ = { 1.f,0.f,0.f,0.f };
	rot_prev_ = { 1.f,0.f,0.f,0.f };
}

bool IndView::ISect(float mousex, float mousey, glm::vec3 *nhit) const {
	float y = height_ - 1 - mousey, x = mousex;
	glm::vec3 mouse_xyz{ x,y,0.f };
	glm::vec4 view_port{ 0,0,width_,height_ };

	glm::vec3 ro = Pos();

	auto view = glm::lookAt(
		Pos(),
		scene_box_.Center(),
		glm::vec3{ .0f,1.f,.0f }
	);

	auto tmp11 = glm::unProject(
		mouse_xyz,
		view,
		Proj(),
		view_port
	);
	glm::vec3 rd = glm::normalize(tmp11 - ro);
	ro -= scene_box_.Center();

	float minlen = glm::length(scene_box_.Lengths());
	float r = minlen / 2.f;  // Arc ball radius.

	float a = 1.f;  // dot(rd,rd);
	float b = 2.f*glm::dot(ro, rd);
	float c = glm::dot(ro, ro) - r * r;

	float delta = b * b - 4 * a*c;
	if (delta < -1e-3)
		return false;

	float tmp1 = -b / (2.f*a);
	float tmp2 = glm::sqrt(delta) / (2.f*a);

	float ans1 = tmp1 + tmp2, ans2 = tmp1 - tmp2;
	float t = ans1 < ans2 ? ans1 : ans2;
	*nhit = glm::normalize(ro + t * rd);
	return true;
}
