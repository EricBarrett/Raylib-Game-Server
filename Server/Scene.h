#pragma once

#include "EntityManager.h"

#include <fstream>
#include <iostream>
#include <cmath>

class Scene
{
protected:
	bool m_paused = false;
	size_t m_frame = 0;
	virtual void cleanup() = 0;
	void setPaused(bool paused) {m_paused = paused;}
public:
	EntityManager entities;
	
	Scene() {}
	virtual void update() = 0;
	virtual void sRender() = 0;
	virtual void load(const std::string &path = "") = 0;
	size_t getCurrentFrame() {return m_frame;}
};

class Level : public Scene
{
	bool renderTexture = true;
	bool renderCollision = false;
	bool renderGrid = false;
	Camera2D m_cam = {0};
	
public:
	void update();
	void sRender();
	void load(const std::string &path = "");
	void cleanup() {}
	void sCollision(std::shared_ptr<Entity>);
	void sMovement();
	
	std::string m_filePath;
	Level(const std::string &path = "config/level0.json")
		: Scene(), m_filePath(path) {}
	
};