#ifndef IND_LOADERS
#define IND_LOADERS


#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "./effects/congo.h"


void ReadObjFile(
	std::string objpath,
	MeshDesc *poly
);


void ReadIndHairFile(
	std::string path,
	MeshDesc *desc,
	unsigned sample_rate_denom
);


#endif // !IND_LOADERS