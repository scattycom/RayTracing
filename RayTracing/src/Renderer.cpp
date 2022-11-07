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
	m_activecamera = &_camera;
	m_activescene = &_scene;

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = perpixel(x, y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_imageData[x + y * m_FinalImage->GetWidth()] = utils::ConvertToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_imageData);
}

glm::vec4 Renderer::perpixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_activecamera->GetPosition();
	ray.Direction = m_activecamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
	
	glm::vec3 _color(0.0f);
	float multiplier = 1.0f;

	int bounces = 1;
	for (int i = 0; i != bounces; i++)
	{

		hitpload payload = TracRay(ray);

		if (payload.index < 0)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			_color += skyColor * multiplier;
			break;
		}

		//���ȼ�������
		//��Դ����0�Ƚϴ�Сȷ����ɫ������
		glm::vec3 lightDir = glm::vec3{ 1,1,1 };
		float lightDesinty = glm::max(glm::dot(payload.normal, lightDir), 0.0f);

		//ȷ����η���õ�����ɫ
		_color += multiplier * lightDesinty * m_activescene->Spheres[payload.index].Albedo;

		//����ÿ�����䶼��˥��
		multiplier *= 1;

		//���¼�����һ�����ߵ���ʼ��ͷ���
		ray.Origin = payload.position;
		ray.Direction = glm::reflect(ray.Direction, payload.normal);

	}

	return glm::vec4{ _color,1.0f };
}

hitpload Renderer::TracRay(const Ray& ray)
{
	int closesSphere = -1;

	float closestDistance = std::numeric_limits<float>::max();

	for(int i=0;i< m_activescene->Spheres.size();i++)
	{
		const Sphere& cloestphere = m_activescene->Spheres[i];
		glm::vec3 origin = ray.Origin - cloestphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - cloestphere.Radius * cloestphere.Radius;

		//�ж��Ƿ��������ཻ
		float dis = b * b - 4.0f * a * c;
		if(dis<0.0f)
			continue;

		float hitdistance = (-b - glm::sqrt(dis)) / (2.0f * a);

		if(hitdistance<closestDistance)
		{
			closestDistance = hitdistance;
			closesSphere = (int)i;
		}
	}
	//ͨ���鿴�����ཻ����������ж��Ƿ�����������
	hitpload p;
	if (closesSphere < 0)
		return	p;

	return hitpoint(ray, closestDistance, closesSphere);

}

hitpload Renderer::hitpoint(const Ray& ray, float distance,int _index)
{
	hitpload pay;
	pay.hitdistance = distance;
	
	const Sphere& spy = m_activescene->Spheres[_index];
	
	glm::vec3 origin = ray.Origin - spy.Position;
	pay.position = origin + ray.Direction * distance;
	pay.normal = glm::normalize(pay.position);

	pay.position += spy.Position;

	pay.hitdistance = distance;
	pay.index = _index;

	return pay;
}
