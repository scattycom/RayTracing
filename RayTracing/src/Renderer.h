#pragma once

#include"Walnut/Image.h"
#include"Walnut/Random.h"

#include"Camera.h"
#include<memory>
#include"Ray.h"
#include"Scene.h"


struct hitpload
{
	float hitdistance;
	glm::vec3 position;
	glm::vec3 normal;

	int index = -1;

};

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void render(const Scene& _scene, const Camera& _camera);

	glm::vec4 perpixel(uint32_t x, uint32_t y);
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	hitpload TracRay(const Ray& ray);
	hitpload hitpoint(const Ray& ray, float distance, int _index);
private:

	const Scene* m_activescene = nullptr;
	const Camera* m_activecamera = nullptr;

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_imageData = nullptr;

};

