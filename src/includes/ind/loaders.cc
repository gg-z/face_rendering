#include "./loaders.h"

#include <fstream>

#include "./utils.h"
#include "./effects/congo.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

void GenNormal(tinyobj::attrib_t &shape, tinyobj::mesh_t &mesh, float angle) {

	const float pi = 3.14159;
	/* calculate the cosine of the angle (in degrees) */
	float cos_angle = cos(angle * pi / 180.0);


	/* allocate space for new normals */
	shape.normals.resize(shape.vertices.size());
	for (size_t i = 0; i < shape.vertices.size(); i++)
		shape.normals[i] = 0;

	std::vector<int> count(shape.vertices.size()/3,0);
	glm::vec3 u, v, n;
	for (size_t i = 0; i < mesh.indices.size(); i += 3) {

		unsigned int indexX = 3 * mesh.indices[i].vertex_index;
		unsigned int indexY = 3 * mesh.indices[i+1].vertex_index;
		unsigned int indexZ = 3 * mesh.indices[i+2].vertex_index;

		mesh.indices[i].normal_index = mesh.indices[i].vertex_index;
		mesh.indices[i+1].normal_index = mesh.indices[i+1].vertex_index;
		mesh.indices[i+2].normal_index = mesh.indices[i+2].vertex_index;

		u[0] = shape.vertices[indexY + 0] -
			shape.vertices[indexX + 0];
		u[1] = shape.vertices[indexY + 1] -
			shape.vertices[indexX + 1];
		u[2] = shape.vertices[indexY + 2] -
			shape.vertices[indexX + 2];

		v[0] = shape.vertices[indexZ + 0] -
			shape.vertices[indexX + 0];
		v[1] = shape.vertices[indexZ + 1] -
			shape.vertices[indexX + 1];
		v[2] = shape.vertices[indexZ + 2] -
			shape.vertices[indexX + 2];

		n = glm::cross(u, v); n = glm::normalize(n); count[int(indexX / 3)] += 1;
		shape.normals[indexX] += n.x; shape.normals[indexX + 1] += n.y; shape.normals[indexX + 2] += n.z;
		shape.normals[indexY] += n.x; shape.normals[indexY + 1] += n.y; shape.normals[indexY + 2] += n.z;
		shape.normals[indexZ] += n.x; shape.normals[indexZ + 1] += n.y; shape.normals[indexZ + 2] += n.z;
	}

	for (size_t i = 0; i < shape.vertices.size(); i++)
		shape.normals[i] /= count[int(i / 3)];
}

void ReadObjFile(
	std::string objpath,
	MeshDesc *poly
) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errmsg;

	//bool is_success = tinyobj::LoadObj(
	//	&attrib, &shapes, &materials, &errmsg, objpath.c_str(), mtlpath.c_str(), true);
	bool is_success = tinyobj::LoadObj(
		&attrib,
		&shapes,
		&materials,
		&errmsg,
		objpath.c_str(),
		nullptr,
		true
	);

	if (!errmsg.empty())
		IND_LOG(errmsg);

	IND_ASSERT(is_success, "");
	IND_ASSERT(shapes.size() == 1, "Only one shape at a time.");
	
	auto &mesh = shapes.front().mesh;
	auto num_verts = mesh.num_face_vertices.size() * 3;
	poly->ps.clear(); poly->ps.reserve(num_verts);
	poly->ns.clear(); poly->ns.reserve(num_verts);
	poly->ts.clear(); poly->ts.reserve(num_verts / 3 * 2);
	poly->tgts.clear(); poly->tgts.reserve(num_verts);

	if (attrib.normals.size() == 0)
		GenNormal(attrib, mesh, 90);


	for (std::size_t kthvert = 0; kthvert < size_t(num_verts/3); ++kthvert) {

		// face vtx 0 
		auto idxset = mesh.indices[3*kthvert + 0];
		float x = attrib.vertices[3 * idxset.vertex_index + 0];
		float y = attrib.vertices[3 * idxset.vertex_index + 1];
		float z = attrib.vertices[3 * idxset.vertex_index + 2];
		glm::vec3 pos0(x, y, z);

		float nx = attrib.normals[3 * idxset.normal_index + 0];
		float ny = attrib.normals[3 * idxset.normal_index + 1];
		float nz = attrib.normals[3 * idxset.normal_index + 2];
		glm::vec3 nrm0(nx, ny, nz);

		float tx = attrib.texcoords[2 * idxset.texcoord_index + 0];
		float ty = attrib.texcoords[2 * idxset.texcoord_index + 1];
		glm::vec2 tx0(tx, ty);

		// face vtx 1
		idxset = mesh.indices[3 * kthvert + 1];
		x = attrib.vertices[3 * idxset.vertex_index + 0];
		y = attrib.vertices[3 * idxset.vertex_index + 1];
		z = attrib.vertices[3 * idxset.vertex_index + 2];
		glm::vec3 pos1(x, y, z);

		nx = attrib.normals[3 * idxset.normal_index + 0];
		ny = attrib.normals[3 * idxset.normal_index + 1];
		nz = attrib.normals[3 * idxset.normal_index + 2];
		glm::vec3 nrm1(nx, ny, nz);

		tx = attrib.texcoords[2 * idxset.texcoord_index + 0];
		ty = attrib.texcoords[2 * idxset.texcoord_index + 1];
		glm::vec2 tx1(tx, ty);

		// face vtx 2
		idxset = mesh.indices[3 * kthvert + 2];
		x = attrib.vertices[3 * idxset.vertex_index + 0];
		y = attrib.vertices[3 * idxset.vertex_index + 1];
		z = attrib.vertices[3 * idxset.vertex_index + 2];
		glm::vec3 pos2(x, y, z);

		nx = attrib.normals[3 * idxset.normal_index + 0];
		ny = attrib.normals[3 * idxset.normal_index + 1];
		nz = attrib.normals[3 * idxset.normal_index + 2];
		glm::vec3 nrm2(nx, ny, nz);

		tx = attrib.texcoords[2 * idxset.texcoord_index + 0];
		ty = attrib.texcoords[2 * idxset.texcoord_index + 1];
		glm::vec2 tx2(tx, ty);


		poly->ps.emplace_back(pos0);
		poly->ps.emplace_back(pos1);
		poly->ps.emplace_back(pos2);
		poly->ns.emplace_back(nrm0);
		poly->ns.emplace_back(nrm1);
		poly->ns.emplace_back(nrm2);
		poly->ts.emplace_back(tx0);
		poly->ts.emplace_back(tx1);
		poly->ts.emplace_back(tx2);


		// compute tangen vector

		glm::vec3 edge1 = pos1 - pos0;
		glm::vec3 edge2 = pos2 - pos0;

		float DeltaU1 = tx1.x - tx0.x;
		float DeltaV1 = tx1.y - tx0.y;
		float DeltaU2 = tx2.x - tx0.x;
		float DeltaV2 = tx2.y - tx0.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * edge1.x - DeltaV1 * edge2.x);
		Tangent.y = f * (DeltaV2 * edge1.y - DeltaV1 * edge2.y);
		Tangent.z = f * (DeltaV2 * edge1.z - DeltaV1 * edge2.z);

		Bitangent.x = f * (-DeltaU2 * edge1.x - DeltaU1 * edge2.x);
		Bitangent.y = f * (-DeltaU2 * edge1.y - DeltaU1 * edge2.y);
		Bitangent.z = f * (-DeltaU2 * edge1.z - DeltaU1 * edge2.z);

		poly->tgts.emplace_back(glm::normalize(Tangent));
		poly->tgts.emplace_back(glm::normalize(Tangent));
		poly->tgts.emplace_back(glm::normalize(Tangent));

	}
}


void ReadIndHairFile(
	std::string path,
	MeshDesc *desc,
	unsigned sample_rate_denom
) {
	std::ifstream fp(path, std::ios::binary);

	char header[9];
	fp.read(header, 8);
	header[8] = '\0';
	IND_ASSERT(strcmp(header, "IND_HAIR") == 0, "Header not match!\n");

	auto read_unsigned = [&fp]()->unsigned {
		unsigned n;
		fp.read(reinterpret_cast<char*>(&n), sizeof(n));
		return n;
	};

	auto read_float = [&fp]()->float {
		float n;
		fp.read(reinterpret_cast<char*>(&n), sizeof(n));
		return n;
	};

	int num_fibs = read_unsigned();
	int num_ps = read_unsigned();

	desc->ps.clear();
	desc->ps.reserve(num_ps);
	desc->pcnts.clear();
	desc->pcnts.reserve(num_fibs);

	SimpleRandom sr{};
	float keep_ratio = 1.f / sample_rate_denom;

	for (int kthfib = 1; kthfib <= num_fibs; ++kthfib) {
		auto fibp_count = read_unsigned();
		if (sr.rand() > keep_ratio) {
			fp.ignore(3 * sizeof(float)*fibp_count);
			continue;
		}
		desc->pcnts.push_back(fibp_count);
		for (unsigned kthp = 0; kthp < fibp_count; ++kthp) {
			float x = read_float();
			float y = read_float();
			float z = read_float();
			desc->ps.push_back(glm::vec3{ x,y,z });
		}
	}

	// Calculate dpdus.
	desc->dpdus.swap(std::vector<glm::vec3>{desc->ps.size()});
	for (std::size_t kthvert = 0,
		kthfib = 0; kthfib < desc->pcnts.size(); ++kthfib) {
		for (std::size_t i = 0; i < desc->pcnts[kthfib] - 1; ++i) {
			desc->dpdus[kthvert] = glm::normalize(
				desc->ps[kthvert + 1] - desc->ps[kthvert]);
			++kthvert;
		}
		desc->dpdus[kthvert] = desc->dpdus[kthvert - 1];
		++kthvert;
	}
}
