#include "./congo.h"

DirLightDesc::DirLightDesc(glm::vec3 dir, glm::vec3 color, int shadow_w, int shadow_h)
	:dir{ dir }, color{ color }, shadow_w{ shadow_w }, shadow_h{ shadow_w } {}

DirLightDesc::DirLightDesc() {}

MeshDesc::MeshDesc() {}

void MeshDesc::Draw() const {
	switch (type) {
	case MeshType::Tri:
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(ps.size()));
		break;
	case MeshType::Fiber:
		int accsum = 0;
		for (int i = 0; i < pcnts.size(); ++i) {
			int pcnt = pcnts[i];
			glDrawArrays(GL_LINE_STRIP, accsum, pcnt);
			accsum += pcnt;
		}
		break;
	}
}
