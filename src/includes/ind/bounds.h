#ifndef IND_BOUNDS
#define IND_BOUNDS


#include <vector>

#include <glm/glm.hpp>


// Axis-aligned bounding box.
class Bounds {
public:
	Bounds();
	Bounds(glm::vec3 corner0, glm::vec3 corner1);
	Bounds(const std::vector<glm::vec3> &ps);

	void Extend(const glm::vec3 p);
	void Extend(const Bounds &aabb);

	glm::vec3 Center() const { return (inf_ + sup_) / 2.f; }
	glm::vec3 Lengths() const { return sup_ - inf_; }

private:
	glm::vec3 inf_;
	glm::vec3 sup_;
};


#endif // !IND_BOUNDS
