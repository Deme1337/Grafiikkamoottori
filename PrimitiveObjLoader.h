#pragma once

#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <glew.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <glm.hpp>



class objLoader
{
public:
	objLoader();
	bool loadOBJ(
		const char * path,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals
		);
	~objLoader();
};
#endif

