#include <vector>
#include <iostream>
#include "view.h"
#include "shader.h"
#include "depvars.h"
#include "glm/ext.hpp"
#include "ind/effects/congo.h"
#include "includes/ind/utils.h"

#define COEFF0_DEFAULT glm::vec4(0.233, 0.455, 0.649, 1.0)
#define COEFF1_DEFAULT glm::vec4(0.100, 0.336, 0.344, 1.0)
#define COEFF2_DEFAULT glm::vec4(0.118, 0.198, 0.000, 1.0)
#define COEFF3_DEFAULT glm::vec4(0.113, 0.007, 0.007, 1.0)
#define COEFF4_DEFAULT glm::vec4(0.358, 0.004, 0.000, 1.0)
#define COEFF5_DEFAULT glm::vec4(0.078, 0.000, 0.000, 1.0)
//extern "C" static const float GAUSSIAN_0[] = { 3.36826793e-38, 5.4099329e-17, 2.86195650e-04, 4.98677851e+00, 2.86195650e-04, 5.40993291e-17, 3.36826793e-38, 0 };
//extern "C" static const float GAUSSIAN_1[] = { 1.6261476911219536e-05, 0.010356923385433849, 0.4985092597014453, 1.813374001824694, 0.4985092597014453, 0.010356923385433849, 1.6261476911219536e-05, 0 };
//extern "C" static const float GAUSSIAN_2[] = { 0.04556403442039102,0.24231576292653256,0.6604460749843161,0.9225486178858417,0.6604460749843161,0.24231576292653256,0.04556403442039102, 0 };
//extern "C" static const float GAUSSIAN_3[] = { 0.19645850555301797,0.34090287948435893,0.4745113114834914,0.5298080300098846,0.4745113114834914,0.34090287948435893,0.19645850555301797, 0 };
//extern "C" static const float GAUSSIAN_4[] = { 0.213169177246658,0.2494157756623961,0.2740587203388307,0.282802684459741,0.2740587203388307,0.2494157756623961,0.213169177246658, 0 };
//extern "C" static const float GAUSSIAN_5[] = { 0.13584174859621476,0.1416930834792028,0.14532418104184555,0.1465551086705911,0.14532418104184555,0.1416930834792028,0.13584174859621476 };
extern "C" static const float GAUSSIAN_WIDTH[] = { 0.042, 0.22, 0.433, 0.753, 1.412, 2.722 };
//extern "C" static const float GAUSSIAN_WIDTH[] = { 0.0064, 0.0484, 0.187, 0.567, 1.99, 7.41 }; // value copied from 


class Renderer
{
public:
	Renderer(IndView * view, std::vector<DirLightDesc>* lights);
	void renderBSSRDF(const std::vector<MeshDesc> &meshes, const QuadDesc &quad_irrd, const QuadDesc &quad_beck, const std::vector<FboDesc> fbodesces);
	void render_sh(const std::vector<MeshDesc> &meshes, const QuadDesc &quad_irrd, const std::vector<FboDesc> fbodesces, const std::vector<float> vec_sh_cof);
	void render_hm(const std::vector<MeshDesc> &meshes);
	void render_lattice(const std::vector<MeshDesc> &meshes);
	void renderBackmann(const QuadDesc &quad);

	~Renderer();
	float m;
	float world_scale;
	int ssr_flag;
	int phony_flag;
	float s_alpha;
	float s_beta;
	float lht_color[3];
	float obj_color[3];
	float amb_color[3];
	float spec_color[3];
	float Kd;
	float albedo;



private:
	IndView * view_;
	std::vector<DirLightDesc>* lights_;

	GLuint depth_, var_, dist_, depth_fbo_;
	Shader shad_shadow_{ std::string(kShaderDir_) + "/shadow_map.vs", std::string(kShaderDir_) + "/shadow_map.frag" };
	Shader shad_bssrdf_{ std::string(kShaderDir_) + "/render_bssrdf.vs", std::string(kShaderDir_) + "/render_bssrdf.frag" };
	Shader shad_backmann_{ std::string(kShaderDir_) + "/ph_backmann.vs", std::string(kShaderDir_) + "/ph_backmann.frag" };
	Shader shad_lgt_irridance_{ std::string(kShaderDir_) + "/render_irridance.vs", std::string(kShaderDir_) + "/render_irridance.frag" };
	Shader shad_lgt_irridance_sh_{ std::string(kShaderDir_) + "/render_irridance_sh.vs", std::string(kShaderDir_) + "/render_irridance_sh.frag" };
	Shader shad_gaussian_convol_u_{ std::string(kShaderDir_) + "/render_gauss_convolv_u.vs", std::string(kShaderDir_) + "/render_gauss_convolv_u.frag" };
	Shader shad_gaussian_convol_v_{ std::string(kShaderDir_) + "/render_gauss_convolv_v.vs", std::string(kShaderDir_) + "/render_gauss_convolv_v.frag" };
	Shader shad_stretch_{ std::string(kShaderDir_) + "/stretch.vs", std::string(kShaderDir_) + "/stretch.frag" };
	Shader shad_hm_{ std::string(kShaderDir_) + "/render_hm.vs", std::string(kShaderDir_) + "/render_hm.frag" };
	Shader shad_lattice_{ std::string(kShaderDir_) + "/lattice.vs", std::string(kShaderDir_) + "/lattice.frag" };

};