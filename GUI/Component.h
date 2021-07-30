#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <variant>
#include <vector>

#include "raylib.h"

enum ComponentKey {BBOX, TRANSFORM, ASSET, GRAVITY, CONTROLLER, DURATION, ANIMATION, EMITTER};
enum CState {JUMPING, SHOOTING, STRAFING, DUCKING, DASHING, IDLING};
enum Direction {NA, FRONT, BACK, BELOW, ABOVE};
class Entity;

class CTransform
{
public:
	ComponentKey type = TRANSFORM;
	Vector2 pos;
	Vector2 scale;
	float rotation;
	
	CTransform(const Vector2 &p = Vector2{0.0, 0.0}, const Vector2 &s = Vector2{1.0, 1.0}, float a = 0.0)
		: pos(p), scale(s), rotation(a) {}
};

class CAsset
{
public:
	ComponentKey type = ASSET;
	const std::string name;
	Rectangle renderArea;
	int frames;
	int currentFrame;
	Color col;
	
	CAsset(const std::string &n, int f = 1, int cf = 1, const Color &c = WHITE)
		: name(n), renderArea((Rectangle){0.0f, 0.0f, 64.0f * f, 64.0f}), frames(f), currentFrame(cf), col(c) {}
		
	CAsset(const std::string &n, Rectangle &ra, int f = 1, int cf = 1, const Color &c = WHITE)
		: name(n), renderArea(ra), frames(f), currentFrame(cf), col(c) {}
};

class CBBox
{
public:
	ComponentKey type = BBOX;
	Vector2 bounds;
	Vector2 radi;
	
	CBBox(const Vector2 &b = Vector2{0.0, 0.0})
		: bounds(b), radi(Vector2{b.x / 2, b.y / 2}) {}
	CBBox(std::shared_ptr<CTransform> transform)
	{
		bounds = (Vector2) {64 * transform->scale.x, 64 * transform->scale.y};
		radi = (Vector2) {32 * transform->scale.x, 32 * transform->scale.y};
	}
};

class CGravity
{
public:
	ComponentKey type = GRAVITY;
	float force;
	
	CGravity(float f = 10.0f)
		: force(f) {}
};

class CController
{
public:
	ComponentKey type = CONTROLLER;
	CState animState;
	std::string adjSurface;
	Direction adjSurfaceSide;
	int frame;
	Vector2 vel;
	Vector2 accel;
	
	CController(CState anim = IDLING, const std::string &aS = "", Direction aSS = NA, int frameStarted = 1, const Vector2 &v = (Vector2){0.0f , 0.0f}, const Vector2 &a = (Vector2){0.0f, 0.0f})
		: animState(anim), adjSurface(aS), adjSurfaceSide(aSS), frame(frameStarted), vel(v), accel(a) {}
};

class CDuration
{
public:
	ComponentKey type = DURATION;
	int frames;
	int creation;
	
	CDuration(int f, int fc)
		: frames(f), creation(fc) {}
};

class CAnim
{
public:
	ComponentKey type = ANIMATION;
	std::string name;
	int startedOn;
	
	CAnim(const std::string &n, int start = 0)
		: name(n), startedOn(start) {}
};

class CEmitter
{
public:
	ComponentKey type = EMITTER;
	std::map< std::string, std::shared_ptr<Entity> > projectiles;
	Vector2 pos;
	
	CEmitter(std::map< std::string, std::shared_ptr<Entity> > m, const Vector2 &p = Vector2{0.5, 0.5})
		: projectiles(m), pos(p) {}
	
	CEmitter(const Vector2 &p = Vector2{0.5, 0.5})
		: pos(p) {}
};