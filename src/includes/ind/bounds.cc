#include "./bounds.h"

#include <numeric>

#include <glm/glm.hpp>


// Bounds
Bounds::Bounds() {
	inf_ = glm::vec3(std::numeric_limits<float>::max());
	sup_ = glm::vec3(std::numeric_limits<float>::lowest());
}

Bounds::Bounds(glm::vec3 corner0, glm::vec3 corner1) {
	inf_.x = glm::min(corner0.x, corner1.x);
	inf_.y = glm::min(corner0.y, corner1.y);
	inf_.z = glm::min(corner0.z, corner1.z);

	sup_.x = glm::max(corner0.x, corner1.x);
	sup_.y = glm::max(corner0.y, corner1.y);
	sup_.z = glm::max(corner0.z, corner1.z);
}

Bounds::Bounds(const std::vector<glm::vec3> &ps)
	:Bounds{} {
	for (auto p : ps)
		Extend(p);
}

void Bounds::Extend(const glm::vec3 p) {
	inf_.x = glm::min(inf_.x, p.x);
	inf_.y = glm::min(inf_.y, p.y);
	inf_.z = glm::min(inf_.z, p.z);

	sup_.x = glm::max(sup_.x, p.x);
	sup_.y = glm::max(sup_.y, p.y);
	sup_.z = glm::max(sup_.z, p.z);
}

void Bounds::Extend(const Bounds &aabb) {
	inf_.x = glm::min(aabb.inf_.x, inf_.x);
	inf_.y = glm::min(aabb.inf_.y, inf_.y);
	inf_.z = glm::min(aabb.inf_.z, inf_.z);

	sup_.x = glm::max(aabb.sup_.x, sup_.x);
	sup_.y = glm::max(aabb.sup_.y, sup_.y);
	sup_.z = glm::max(aabb.sup_.z, sup_.z);
}