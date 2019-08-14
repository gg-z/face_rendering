#ifndef IND_VIEW
#define IND_VIEW


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "./bounds.h"

//
// Only scene-box and distance can be altered.
//
class IndView {
public:
	IndView(
		unsigned width,
		unsigned height,
		float fov_y,
		Bounds scene_box
	);

	//
	// Use positive value for zooming out.
	// Negtive value for zooming in. 
	//
	void Zoom(float dist);
	glm::vec3 Pos() const;
	Bounds SceneBox() const;
	float Aspect() const;
	unsigned Width() const;
	unsigned Height() const;
	float SceneDistance() const;
	glm::mat4 Proj() const;
	glm::mat4 View() const;

	void Track(float mousex, float mousey);
	bool Track();
	void UnTrack();
	void Reset();

private:
	void NearFar();
	bool ISect(float mousex, float mousey, glm::vec3 *nhit) const;

	Bounds scene_box_;
	float near_, far_;
	unsigned width_, height_;
	float fov_y_;

	bool arcball_track_{ false };
	glm::quat rot_{ 1.f,0.f,0.f,0.f }, rot_prev_;
	glm::vec3 nhit_op_;
};


#endif // !IND_VIEW