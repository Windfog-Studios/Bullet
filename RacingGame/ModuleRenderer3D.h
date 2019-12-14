#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	bool Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, SDL_RendererFlip flip = SDL_FLIP_NONE, int pivot_x = INT_MAX, int pivot_y = INT_MAX);
	void OnResize(int width, int height);
	

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ProjectionMatrix;
	SDL_Renderer* renderer;
};