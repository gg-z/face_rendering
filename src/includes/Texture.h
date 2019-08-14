#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <algorithm>
#include <il.h>
#include <ilu.h>
#include <il_wrap.h>
#include <il21dec.h>
#include <Ilogobj.Hxx>
#include <ind/utils.h>

struct Texture {
	int width, height;
	GLuint gid;
	GLenum target;
	bool mipmaps;
	GLenum wrap;

	//used to make copies
	GLenum internalFormat;

	Texture(GLenum target=GL_TEXTURE_2D):target(target){
		glGenTextures(1, &gid);
	}

	//use this if not passing in any data
	Texture(int width, int height, GLenum internalFormat=GL_RGBA, bool mipmaps=false, GLenum wrap = GL_REPEAT):target(GL_TEXTURE_2D){
		glGenTextures(1, &gid);
		operator()(width, height, internalFormat, mipmaps, wrap);
	}

	//use this if not passing in any data
	void operator()(int width, int height, GLenum internalFormat=GL_RGBA, bool mipmaps=false, GLenum wrap = GL_REPEAT){
		//eliminate stupid driver bugs, will update as needed
		//even though these aren't used for anything, they still have to be compatible with internal format on nvidea
		GLenum format = GL_RGBA, type=GL_UNSIGNED_BYTE;
		switch(internalFormat){
			case GL_RGB32UI:
				format = GL_RGB_INTEGER;
				type = GL_UNSIGNED_INT;
				break;
		}

		operator()(NULL, width, height, internalFormat, format, type, mipmaps, wrap);
	}

	Texture(void* data, int width, int height, GLenum internalFormat=GL_RGBA, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE, bool mipmaps=false, GLenum wrap = GL_REPEAT):target(GL_TEXTURE_2D){
		glGenTextures(1, &gid);
		operator()(data, width, height, internalFormat, format, type, mipmaps, wrap);
	}

	void operator()(void* data, int width, int height, GLenum internalFormat=GL_RGBA, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE, bool mipmaps=false, GLenum wrap = GL_REPEAT){
		this->width = width;
		this->height = height;
		this->internalFormat = internalFormat;

		bind();
		glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
		sampleSettings(wrap, mipmaps);
	}

	void sampleSettings(GLenum wrap, bool mipmaps=true){
		this->wrap = wrap;
		this->mipmaps = mipmaps;

		if (wrap == 0 || wrap == 1)
			std::cout <<"sampleSettings warp & mipmaps pass in backwards" << std::endl;//leave this in for a while
		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri (target, GL_TEXTURE_WRAP_T, wrap);
		if (target == GL_TEXTURE_3D) glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);

		glTexParameteri (target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (mipmaps){
			glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(target);
		}
		else
			glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	//bind to unit 0 (0 is reserved for all operations)
	void bind(){
		activateUnit(0);
		static GLuint bound0 = 0;//might be more efficient if opengl is stupid
		if (bound0 != gid){
			glBindTexture(target, gid);
			bound0 = gid;
		}
	}

	void bind(GLuint unit){
		activateUnit(unit);
		glBindTexture(target, gid);
	}

	//must use instead of glActiveTexture everywhere
	static void activateUnit(GLuint unit){
		static GLuint active = -1;//might be more efficient if opengl is stupid
		if (unit != active){
			glActiveTexture(GL_TEXTURE0 + unit);
			active = unit;
		}
	}
};

struct ILTexture : public Texture {

	ILTexture(const char* file, ILenum origin = IL_ORIGIN_LOWER_LEFT):Texture(GL_TEXTURE_2D){
		//generates, binds, loads, creates mipmaps, deletes local copy
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(origin);

		ILuint id;
		ilGenImages(1, &id);
		ilBindImage(id);
		ilLoadImage(file);
		int imageWidth = ilGetInteger( IL_IMAGE_WIDTH )
			,imageHeight = ilGetInteger( IL_IMAGE_HEIGHT );

		width = ceilPow2(imageWidth);
		height = ceilPow2(imageHeight);
		width = height = std::min(4096,std::max(width, height));

		unsigned char* data = new unsigned char[4*width*height];
		iluScale(width, height, 1);
		ilCopyPixels(0, 0, 0, width, height, 1, GL_RGBA, IL_UNSIGNED_BYTE, data);
		operator()(data, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, true, GL_REPEAT);
		ilDeleteImages  (1, &id);
	}
};

