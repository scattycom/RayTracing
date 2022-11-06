#include"Renderer.h"

namespace utils
{
	static uint32_t ConvertToRGBA(const glm::vec4 color)
	{
		uint8_t r = uint8_t(color.r * 255.0f);
		uint8_t g = uint8_t(color.g * 255.0f);
		uint8_t b = uint8_t(color.b * 255.0f);
		uint8_t a = uint8_t(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;

		return result;
	}
}


void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_imageData;
	m_imageData = new uint32_t[width * height];

}

void Renderer::render(const Scene& _scene, const Camera& _camera)
{
	Ray ray;
	ray.Origin = _camera.GetPosition();

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			ray.Direction = _camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

			glm::vec4 color = perpixel(_scene,ray);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_imageData[x + y * m_FinalImage->GetWidth()] = utils::ConvertToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_imageData);
}

glm::vec4 Renderer::perpixel(const Scene& _scene, const Ray& ray)
{



	return glm::vec4{ 1.0f,0.0f,0.5f,1.0f };
}

glm::vec4 Renderer::TracRay(const Scene& _scene, const Ray& ray)
{
	const Sphere* cloestP = nullptr;
	float closestDistance = std::numeric_limits<float>::max();

	for (const Sphere& _sphere: _scene.Spheres)
	{
		glm::vec3 origin = ray.Origin - _sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - _sphere.Radius * _sphere.Radius;

		float dis = b * b - 4.0f * a * c;

		if(dis<0.0f)
			continue;

		float hitdistance= (-b - glm::sqrt(dis)) / (2.0f * a);

		if(hitdistance<closestDistance)
		{
			cloestP = &_sphere;
			closestDistance = hitdistance;
		}
	}

	if (cloestP == nullptr)
		return glm::vec4{ 0.0f,0.0f,0.0f,1.0f };

	glm::vec3 origin = ray.Origin - cloestP->Position;
	glm::vec3 normalvec = origin + closestDistance * ray.Direction;
	glm::vec3 normal = glm::normalize(normalvec);

	glm::vec3 lightDir = glm::normalize(glm::vec3(1, 1, 1));
	float lightIntensity = glm::max(glm::dot(lightDir, normal), 0.0f);

	glm::vec3 color = cloestP->Albedo;
	color *= lightIntensity;
	return glm::vec4{ color,1.0f };

}
