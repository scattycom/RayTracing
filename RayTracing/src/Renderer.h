#pragma once

#include"Walnut/Image.h"
#include"Walnut/Random.h"

#include"Camera.h"
#include<memory>
#include"Ray.h"
#include"Scene.h"

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void render(const Scene& _scene, const Camera& _camera);

	glm::vec4 perpixel(const Scene& _scene, const Ray& ray);
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	glm::vec4 TracRay(const Scene& _scene, const Ray& ray);

private:

	Scene* m_activescene = nullptr;
	Scene* m_activecamera = nullptr;

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_imageData = nullptr;

};

