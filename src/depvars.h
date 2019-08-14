#ifndef IND_DEPVARS
#define IND_DEPVARS
#include <math.h>

# define M_PI   3.14159265358979323846 

extern char* kShaderDir_;
//char kShaderDir_[] = "F:/2019_08_13_anpei_face_render/src/shaders";

const float sh_cof[27] = { 
	0.8249,
    0.0485,
    0.2303,
    0.1527,
   -0.1024,
    0.0005,
   -0.1863,
   -0.1788,
   -0.0247,
    0.7918,
    0.0118,
    0.1033,
    0.2202,
    0.1520,
    0.0976,
   -0.1899,
    0.0624,
    0.1979,
    0.7028,
   -0.1585,
    0.0991,
    0.3226,
    0.2884,
    0.4904,
   -0.2267,
    0.1284,
    0.3041
};


const float sh_const[7] = { 
	1.0 / (2 * sqrt(M_PI)), 
	sqrt(3) / (2 * sqrt(M_PI)), 
	sqrt(15) / (4 * sqrt(M_PI)),  
	sqrt(15) / (2 * sqrt(M_PI)), 
	sqrt(5) / (4 * sqrt(M_PI)), 
	sqrt(15) / (2 * sqrt(M_PI)),
	sqrt(15) / (2 * sqrt(M_PI))
};


//B1 = 1.0 / (2 * sqrt(pi));
//B2 = sqrt(3) / (2 * sqrt(pi));
//B3_1 = sqrt(15) / (4 * sqrt(pi));
//B3_2 = sqrt(15) / (2 * sqrt(pi));
//B3_3 = sqrt(5) / (4 * sqrt(pi));
//B3_4 = sqrt(15) / (2 * sqrt(pi));
//B3_5 = sqrt(15) / (2 * sqrt(pi));


#endif  // !IND_DEPVARS