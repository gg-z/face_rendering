#ifndef IND_EFFECTS_CONGO
#define IND_EFFECTS_CONGO


#include <vector>
#include <functional>
#include <memory>

#include <glm/glm.hpp>

#include <glad/glad.h>

#include "../bounds.h"
#include "../shader.h"


enum class MeshType {
	Tri,Fiber
};

struct MeshDesc {

	MeshDesc();

	std::vector<glm::vec3> ps;
	std::vector<glm::vec3> ns;
	std::vector<glm::vec2> ts;
	std::vector<glm::vec3> tgts;
	std::vector<glm::vec3> dpdus;
	std::vector<glm::vec3> dpdvs;
	std::vector<unsigned> pcnts;
	std::string dds_path;
	std::string txtr_path;
	std::string nrml_dstrb_path;

	GLuint handle, buf, buf_ps, buf_ns, buf_ts, buf_tgts, elem, txtr, dds_txtr, nrml_dstrb_txtr;

	float velo = 0,visco = 1.f;
	glm::mat4 Model = glm::mat4{1.f};

	Bounds box{};

	MeshType type;

	void Draw() const;
};

struct DirLightDesc {
	glm::vec3 dir;
	glm::vec3 color;
	glm::vec3 pos;
	int shadow_w;
	int shadow_h;
	glm::mat4 p;
	glm::mat4 v;
	DirLightDesc(glm::vec3 dir, glm::vec3 color, int shadow_w, int shadow_h);
	DirLightDesc();
	GLuint fbo, txture;  // fbo with render buffer object

	MeshDesc mesh;
};


struct QuadDesc {

	int render_w;
	int render_h;

	MeshDesc mesh; // with vertices, normal , txture
	glm::mat4 mvp;
	GLuint fbo, txture;  // fbo with render buffer object
};


struct FboDesc {

	int render_w;
	int render_h;

	GLuint fbo, txture;  // fbo with render buffer object
};



#endif  // !IND_EFFECTS_CONGO
