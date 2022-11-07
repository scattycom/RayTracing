#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Scene.h"
#include"Camera.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:

	ExampleLayer()
		:m_camera(45.0f , 0.1f , 100.0f)
	{

		{
			Sphere _sphere;
			_sphere.Position = { 0.0f,0.0f,0.0f };
			_sphere.Radius = 0.5f;
			_sphere.Albedo = { 1.0f,0.0f,0.5f };
			m_scene.Spheres.push_back(_sphere);
		}

		{
			Sphere _sphere;
			_sphere.Position = { 0.0f,-101.0f,0.0f };
			_sphere.Radius = 100.0f;
			_sphere.Albedo = { 0.0f,1.0f,0.5f };
			m_scene.Spheres.push_back(_sphere);
		}

	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");

		ImGui::Text("Last Render Time: %.3fms", m_lastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ViewPort");

		m_viewportwidth = ImGui::GetContentRegionAvail().x;
		m_viewportheight = ImGui::GetContentRegionAvail().y;

		auto image = m_renderer.GetFinalImage();
		if (m_renderer.GetFinalImage())
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }
		, ImVec2{ 0,1 }, ImVec2{ 1,0 });

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Scene");

		for (int i=0;i!=m_scene.Spheres.size();i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_scene.Spheres[i];
			ImGui::DragFloat3("position", glm::value_ptr(sphere.Position));
			ImGui::DragFloat("Radius", &sphere.Radius);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.Albedo));

			ImGui::Separator();
			ImGui::PopID();

		}

		ImGui::End();

		Render();
	}

	virtual void OnUpdate(float ts) override
	{
		m_camera.OnUpdate(ts);
	}

	void Render()
	{
		Timer timer;

		m_renderer.OnResize(m_viewportwidth, m_viewportheight);
		m_camera.OnResize(m_viewportwidth, m_viewportheight);
		m_renderer.render(m_scene,m_camera);

		m_lastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_renderer;
	Camera m_camera;
	Scene m_scene;

	uint32_t m_viewportwidth = 0, m_viewportheight = 0;

	float m_lastRenderTime = 0.0f;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}