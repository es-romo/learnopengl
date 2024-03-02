#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>

#include <string>
#include <iostream>

class Texture {

public:
	unsigned int ID;

	int width, height, nrChannels;

	Texture(const char* path, bool flipY) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture

		stbi_set_flip_vertically_on_load(flipY);

		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data)
		{
			if (nrChannels == 1)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			else if (nrChannels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if (nrChannels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "ERROR::TEXTURE::FAILED_TO_LOAD" << std::endl;
		}
		stbi_image_free(data);
	}
	void activate(int num) {
		glActiveTexture(GL_TEXTURE0 + num);
		glBindTexture(GL_TEXTURE_2D, ID);
	}
};

#endif