#include "renderer.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "includes/stb_image_write.h"



Renderer::Renderer(
	IndView* view, 
	std::vector<DirLightDesc>* lights
):view_(view), lights_(lights){
	m = 0.3;
	world_scale = 50.;
	ssr_flag = 1;
	phony_flag = 0;
	s_alpha = 0.16;
	s_beta = 0.18;
	albedo = 0.7;

	lht_color[0] = 0.9;
	lht_color[1] = 0.9;
	lht_color[2] = 0.9;
	obj_color[0] = 0.7;
	obj_color[1] = 0.7;
	obj_color[2] = 0.7;
	amb_color[0] = 0.34;
	amb_color[1] = 0.34;
	amb_color[2] = 0.34;
	spec_color[0] = 0.24;
	spec_color[1] = 0.24;
	spec_color[2] = 0.24;

	Kd = 0.85f;
}


void Renderer::renderBackmann(
	const QuadDesc &quad
) {

	// render a quad aligned to screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, quad.render_w, quad.render_h);
	glUseProgram(shad_backmann_.Get());
	glBindFramebuffer(GL_FRAMEBUFFER, quad.fbo);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(quad.mesh.handle);
	quad.mesh.Draw();
	glBindVertexArray(0);
	// key: missing of this line would get a white texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}




void Renderer::render_hm(const std::vector<MeshDesc> &meshes) {


	// pass1: render a shadow map
	float r = glm::length(view_->SceneBox().Lengths()) / 2.f;
	lights_->at(0).pos = view_->SceneBox().Center() + glm::normalize(lights_->at(0).dir) * r;
	lights_->at(0).p = glm::ortho(-r, r, -r, r, 0.f, 2 * r);
	lights_->at(0).v = glm::lookAt(lights_->at(0).pos, view_->SceneBox().Center(), glm::vec3(0.0, 1.0, 0.0));


	
	// render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, view_->Width(), view_->Height());
	glClearColor(0.164f, 0.164f, 0.164f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shad_hm_.Get());
	for (auto &mesh : meshes) {
		AssignUniform(shad_hm_.Get(), "projection", view_->Proj());
		AssignUniform(shad_hm_.Get(), "cam_pos", view_->Pos());
		AssignUniform(shad_hm_.Get(), "view", mesh.Model);
		AssignUniform(shad_hm_.Get(), "model", view_->View());
		AssignUniform(shad_hm_.Get(), "lht_pos", lights_->at(0).pos);
		AssignUniform(shad_hm_.Get(), "albedo", albedo);
		AssignUniform(shad_hm_.Get(), "lht_color", glm::make_vec3(lht_color));

		AssignUniform(shad_hm_.Get(), "hm", 0);

		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.nrml_dstrb_txtr);
		mesh.Draw();
		glBindVertexArray(0);
	}

}





void Renderer::render_lattice(const std::vector<MeshDesc> &meshes) {


	// pass1: render a shadow map
	float r = glm::length(view_->SceneBox().Lengths()) / 2.f;
	lights_->at(0).pos = view_->SceneBox().Center() + glm::normalize(lights_->at(0).dir) * r;
	lights_->at(0).p = glm::ortho(-r, r, -r, r, 0.f, 2 * r);
	lights_->at(0).v = glm::lookAt(lights_->at(0).pos, view_->SceneBox().Center(), glm::vec3(0.0, 1.0, 0.0));



	// render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, view_->Width(), view_->Height());
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shad_lattice_.Get());
	for (auto &mesh : meshes) {
		AssignUniform(shad_lattice_.Get(), "projection", view_->Proj());
		AssignUniform(shad_lattice_.Get(), "view", view_->View());
		AssignUniform(shad_lattice_.Get(), "model", mesh.Model);

		AssignUniform(shad_lattice_.Get(), "LightPosition", lights_->at(0).pos);
		AssignUniform(shad_lattice_.Get(), "LightColor", glm::make_vec3(lht_color));
		AssignUniform(shad_lattice_.Get(), "EyePosition", view_->Pos());
		AssignUniform(shad_lattice_.Get(), "Specular", glm::make_vec3(spec_color));
		AssignUniform(shad_lattice_.Get(), "Ambient", glm::make_vec3(amb_color));
		AssignUniform(shad_lattice_.Get(), "Kd", 0.8f);
		AssignUniform(shad_lattice_.Get(), "SurfaceColor", glm::make_vec3(obj_color));

		AssignUniform(shad_lattice_.Get(), "hm", 0);

		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.nrml_dstrb_txtr);
		mesh.Draw();
		glBindVertexArray(0);
	}

}








void Renderer::render_sh(
	const std::vector<MeshDesc> &meshes,
	const QuadDesc &quad_irrd,
	const std::vector<FboDesc> fbodesces,
	const std::vector<float> vec_sh_cof
) {

	// pass2: render an irridance map
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, quad_irrd.render_w, quad_irrd.render_h);
	glUseProgram(shad_lgt_irridance_sh_.Get());
	glBindFramebuffer(GL_FRAMEBUFFER, quad_irrd.fbo);
	//glClear(GL_DEPTH_BUFFER_BIT);
	for (auto &mesh : meshes) {

		AssignUniform(shad_lgt_irridance_sh_.Get(), "projection", view_->Proj());
		AssignUniform(shad_lgt_irridance_sh_.Get(), "view", view_->View());
		AssignUniform(shad_lgt_irridance_sh_.Get(), "model", mesh.Model);
		glUniform1fv(glGetUniformLocation(shad_lgt_irridance_sh_.Get(), "v"), 27, sh_cof);
		glUniform1fv(glGetUniformLocation(shad_lgt_irridance_sh_.Get(), "c_val"), 7, sh_const);


		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lights_->at(0).txture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh.txtr);
		mesh.Draw();
		glBindVertexArray(0);

	}
	// key: missing of this line would get a white texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// pass3: render a stretch map
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, fbodesces.at(fbodesces.size() - 1).render_w, fbodesces.at(fbodesces.size() - 1).render_h);
	glUseProgram(shad_stretch_.Get());
	glBindFramebuffer(GL_FRAMEBUFFER, fbodesces.at(fbodesces.size() - 1).fbo);
	//glClear(GL_DEPTH_BUFFER_BIT);
	for (auto &mesh : meshes) {

		AssignUniform(shad_stretch_.Get(), "model", mesh.Model);
		//AssignUniform(shad_stretch_.Get(), "dim", glm::vec2(fbodesces.at(fbodesces.size() - 1).render_w, 
		//fbodesces.at(fbodesces.size() - 1).render_h));
		AssignUniform(shad_stretch_.Get(), "dim", glm::vec2(1024, 1024));


		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		mesh.Draw();
		glBindVertexArray(0);

	}
	// key: missing of this line would get a white texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//pass4:  convolve irridance maps
	for (int i = 0; i < fbodesces.size() / 2; i++) {
		//convolve u direction
		glBindFramebuffer(GL_FRAMEBUFFER, fbodesces.at(i * 2).fbo);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, fbodesces.at(i * 2).render_w, fbodesces.at(i * 2).render_h);
		glUseProgram(shad_gaussian_convol_u_.Get());
		for (auto &mesh : meshes) {

			AssignUniform(shad_gaussian_convol_u_.Get(), "irridance_map", 0);
			AssignUniform(shad_gaussian_convol_u_.Get(), "stretch_map", 1);
			AssignUniform(shad_gaussian_convol_u_.Get(), "gauss_width", GAUSSIAN_WIDTH[i]);
			AssignUniform(shad_gaussian_convol_u_.Get(), "world_scale", world_scale);

			// the texture activating order is vital.
			glBindVertexArray(mesh.handle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, quad_irrd.txture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(fbodesces.size() - 1).txture);
			mesh.Draw();
			glBindVertexArray(0);

		}
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glFlush();

		// convolve v direction
		glBindFramebuffer(GL_FRAMEBUFFER, fbodesces.at(2 * i + 1).fbo);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, fbodesces.at(2 * i + 1).render_w, fbodesces.at(2 * i + 1).render_h);
		glUseProgram(shad_gaussian_convol_v_.Get());
		for (auto &mesh : meshes) {

			AssignUniform(shad_gaussian_convol_v_.Get(), "irridance_map", 0);
			AssignUniform(shad_gaussian_convol_v_.Get(), "stretch_map", 1);
			AssignUniform(shad_gaussian_convol_u_.Get(), "gauss_width", GAUSSIAN_WIDTH[i]);
			AssignUniform(shad_gaussian_convol_u_.Get(), "world_scale", world_scale);

			// the texture activating order is vital.
			glBindVertexArray(mesh.handle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(i * 2).txture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(fbodesces.size() - 1).txture);
			mesh.Draw();
			glBindVertexArray(0);

		}
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	// pass 5: render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, view_->Width(), view_->Height());
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shad_bssrdf_.Get());
	for (auto &mesh : meshes) {

		AssignUniform(shad_bssrdf_.Get(), "projection", view_->Proj());
		AssignUniform(shad_bssrdf_.Get(), "view", view_->View());
		AssignUniform(shad_bssrdf_.Get(), "model", mesh.Model);
		AssignUniform(shad_bssrdf_.Get(), "coeff0", COEFF0_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff1", COEFF1_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff2", COEFF2_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff3", COEFF3_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff4", COEFF4_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff5", COEFF5_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "diffuse_texture", 0);
		for (int i = 0; i < fbodesces.size() / 2; i++) {
			AssignUniform(shad_bssrdf_.Get(), ("tex_blur" + std::to_string(i)).c_str(), 1 + i);
		}

		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.txtr);
		for (int i = 0; i < fbodesces.size() / 2; i++) {
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(2 * i + 1).txture);
		}
		mesh.Draw();
		glBindVertexArray(0);
	}


	// pass 6: render light pos 
	//lights_->at(0).mesh.Model = glm::translate(glm::mat4(1.0), lights_->at(0).pos);
	//lights_->at(0).mesh.Model = glm::scale(lights_->at(0).mesh.Model, glm::vec3(0.8, 0.8, 0.8));
	//glUseProgram(shad_diffuse_.Get());
	//AssignUniform(shad_diffuse_.Get(), "MVP", view_->Proj()*view_->View()*lights_->at(0).mesh.Model);
	//glBindVertexArray(lights_->at(0).mesh.handle);
	//lights_->at(0).mesh.Draw();
	//glBindVertexArray(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

}



void Renderer::renderBSSRDF(
	const std::vector<MeshDesc> &meshes,
	const QuadDesc &quad_irrd,
	const QuadDesc &quad_beck,
	const std::vector<FboDesc> fbodesces

) {

	// pass1: render a shadow map
	float r = glm::length(view_->SceneBox().Lengths()) / 2.f;
	lights_->at(0).pos = view_->SceneBox().Center() + glm::normalize(lights_->at(0).dir) * r ;
	lights_->at(0).p = glm::ortho(-r, r, -r, r, 0.f, 2 * r);
	lights_->at(0).v = glm::lookAt(lights_->at(0).pos, view_->SceneBox().Center(), glm::vec3(0.0, 1.0, 0.0));
	// render scene from light's point of view
	glUseProgram(shad_shadow_.Get());
	AssignUniform(shad_shadow_.Get(), "light_pv", lights_->at(0).p * lights_->at(0).v);


	glViewport(0, 0, lights_->at(0).shadow_w, lights_->at(0).shadow_h);
	glBindFramebuffer(GL_FRAMEBUFFER, lights_->at(0).fbo);
	for (auto &mesh : meshes) {
		glClear(GL_DEPTH_BUFFER_BIT);
		AssignUniform(shad_shadow_.Get(), "model", mesh.Model);
		glBindVertexArray(mesh.handle);
		mesh.Draw();
		glBindVertexArray(0);
	}



	// pass2: render an irridance map
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, quad_irrd.render_w, quad_irrd.render_h);
	glUseProgram(shad_lgt_irridance_.Get());
	glBindFramebuffer(GL_FRAMEBUFFER, quad_irrd.fbo);
	//glClear(GL_DEPTH_BUFFER_BIT);
	for (auto &mesh : meshes) {

		AssignUniform(shad_lgt_irridance_.Get(), "projection", view_->Proj());
		AssignUniform(shad_lgt_irridance_.Get(), "view", view_->View());
		AssignUniform(shad_lgt_irridance_.Get(), "model", mesh.Model);
		AssignUniform(shad_lgt_irridance_.Get(), "light_pv", lights_->at(0).p*lights_->at(0).v);
		AssignUniform(shad_lgt_irridance_.Get(), "light_pos", lights_->at(0).pos);
		AssignUniform(shad_lgt_irridance_.Get(), "shadow_map", 0);
		AssignUniform(shad_lgt_irridance_.Get(), "diffuse_texture", 1);

		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lights_->at(0).txture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh.txtr);
		mesh.Draw();
		glBindVertexArray(0);

	}
	// key: missing of this line would get a white texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// pass3: render a stretch map
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, fbodesces.at(fbodesces.size() -1).render_w, fbodesces.at(fbodesces.size() - 1).render_h);
	glUseProgram(shad_stretch_.Get());
	glBindFramebuffer(GL_FRAMEBUFFER, fbodesces.at(fbodesces.size() - 1).fbo);
	//glClear(GL_DEPTH_BUFFER_BIT);
	for (auto &mesh : meshes) {

		AssignUniform(shad_stretch_.Get(), "model", mesh.Model);
		//AssignUniform(shad_stretch_.Get(), "dim", glm::vec2(fbodesces.at(fbodesces.size() - 1).render_w, 
			//fbodesces.at(fbodesces.size() - 1).render_h));
		AssignUniform(shad_stretch_.Get(), "dim", glm::vec2(1024, 1024));


		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		mesh.Draw();
		glBindVertexArray(0);

	}
	// key: missing of this line would get a white texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//pass4:  convolve irridance maps
	for (int i = 0; i < fbodesces.size()/2; i++) {
		//convolve u direction
		glBindFramebuffer(GL_FRAMEBUFFER, fbodesces.at(i*2).fbo);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, fbodesces.at(i*2).render_w, fbodesces.at(i*2).render_h);
		glUseProgram(shad_gaussian_convol_u_.Get());
		for (auto &mesh : meshes) {

			AssignUniform(shad_gaussian_convol_u_.Get(), "irridance_map", 0);
			AssignUniform(shad_gaussian_convol_u_.Get(), "stretch_map", 1);
			AssignUniform(shad_gaussian_convol_u_.Get(), "gauss_width", GAUSSIAN_WIDTH[i]);
			AssignUniform(shad_gaussian_convol_u_.Get(), "world_scale", world_scale);

			// the texture activating order is vital.
			glBindVertexArray(mesh.handle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, quad_irrd.txture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(fbodesces.size() - 1).txture);
			mesh.Draw();
			glBindVertexArray(0);

		}
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glFlush();

		// convolve v direction
		glBindFramebuffer(GL_FRAMEBUFFER, fbodesces.at(2*i +1).fbo);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, fbodesces.at(2*i+1).render_w, fbodesces.at(2*i+1).render_h);
		glUseProgram(shad_gaussian_convol_v_.Get());
		for (auto &mesh : meshes) {

			AssignUniform(shad_gaussian_convol_v_.Get(), "irridance_map", 0);
			AssignUniform(shad_gaussian_convol_v_.Get(), "stretch_map", 1);
			AssignUniform(shad_gaussian_convol_u_.Get(), "gauss_width", GAUSSIAN_WIDTH[i]);
			AssignUniform(shad_gaussian_convol_u_.Get(), "world_scale", world_scale);

			// the texture activating order is vital.
			glBindVertexArray(mesh.handle);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(i*2).txture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(fbodesces.size() - 1).txture);
			mesh.Draw();
			glBindVertexArray(0);

		}
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	// pass 5: render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, view_->Width(), view_->Height());
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shad_bssrdf_.Get());
	for (auto &mesh : meshes) {

		AssignUniform(shad_bssrdf_.Get(), "projection", view_->Proj());
		AssignUniform(shad_bssrdf_.Get(), "view", view_->View());
		AssignUniform(shad_bssrdf_.Get(), "model", mesh.Model);
		AssignUniform(shad_bssrdf_.Get(), "coeff0", COEFF0_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff1", COEFF1_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff2", COEFF2_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff3", COEFF3_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff4", COEFF4_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "coeff5", COEFF5_DEFAULT);
		AssignUniform(shad_bssrdf_.Get(), "diffuse_texture", 0);
		for (int i = 0; i < fbodesces.size() / 2; i++) {
			AssignUniform(shad_bssrdf_.Get(), ("tex_blur" + std::to_string(i)).c_str(), 1 + i);
		}

		// the texture activating order is vital.
		glBindVertexArray(mesh.handle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.txtr);
		for (int i = 0; i < fbodesces.size() / 2; i++) {
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, fbodesces.at(2 * i + 1).txture);
		}
		mesh.Draw();
		glBindVertexArray(0);
	}


	// pass 6: render light pos 
	//lights_->at(0).mesh.Model = glm::translate(glm::mat4(1.0), lights_->at(0).pos);
	//lights_->at(0).mesh.Model = glm::scale(lights_->at(0).mesh.Model, glm::vec3(0.8, 0.8, 0.8));
	//glUseProgram(shad_diffuse_.Get());
	//AssignUniform(shad_diffuse_.Get(), "MVP", view_->Proj()*view_->View()*lights_->at(0).mesh.Model);
	//glBindVertexArray(lights_->at(0).mesh.handle);
	//lights_->at(0).mesh.Draw();
	//glBindVertexArray(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

}



Renderer::~Renderer(){
}