#pragma once

#ifndef TEXTURES_H
#define TEXTURES_H
#include <Windows.h>
#include <glew.h>
#include <string>
enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

/********************************

Class:	CTexture

Purpose:	Wraps OpenGL texture
object and performs
their loading.

********************************/

class CTexture
{
public:
	bool loadTexture2D(std::string a_sPath, bool bGenerateMipMaps = true);
	void bindTexture(int iTextureUnit = 0);
	void CreateEmptyTexture(int a_iWidth, int a_iHeight, GLenum format);
	void setFiltering(int a_tfMagnification, int a_tfMinification);
	void setSamplerParameter(GLenum parameter, GLenum value);
	int getMinificationFilter();
	bool ReloadTexture();
	int getMagnificationFilter();
	void createFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps);
	void CreateRotationTexture(int a_iWidth, int a_iHeight);
	void CTexture::CreateDepthTexture(int a_iWidth, int a_iHeight);

	//Prototype cubemap
	bool LoadCubeMap(std::vector<std::string> tfile);
	BYTE* bDataPointer;
	void releaseTexture();
	void SetWrap();
	UINT GetTextureID();
	int isShadow = 0;
	GLenum textureHint = GL_TEXTURE_2D;
	std::string GetPath();
	BYTE* getDataPointer();
	int getWidth();
	int getHeight();
	int getBPP();
	CTexture();

private:
	int iWidth, iHeight, iBPP; // Texture width, height, and bytes per pixel
	UINT uiTexture; // Texture name
	UINT uiSampler; // Sampler name
	bool bMipMapsGenerated;
	bool isCubeMap;
	int tfMinification, tfMagnification;
	int cubeMapIndex = 0;
	std::string sPath;
};


#endif