#pragma once

#include "jshEngine.h"

struct Particle {

	jsh::vec3 position;
	jsh::vec2 size;
	float rotation;
	float creationTime;
	float lifeTime;
	float alpha = 0;

	jsh::vec3 velocity;
	float rotationVel;
	jsh::Color4b color;

	Particle() : rotation(0.f), creationTime(0.f), lifeTime(0.f), velocity(), rotationVel(0.f) {}
	Particle(jsh::vec3 position, jsh::vec2 size, float rotation, float t0, float t1, jsh::vec3 vel, float rotationVel, jsh::Color4b color) : position(position), size(size), rotation(rotation), creationTime(t0), lifeTime(t1), velocity(vel), rotationVel(rotationVel), color(color) {}
};

class ParticleGenerator {

	float m_GenerationTime = 0.001f;
	uint32 m_GenerationCount = 1u;
	float m_GenTimeCount = 0u;
	float m_TimeCount = 0u;

	float m_MinSize = 0.1f;
	float m_MaxSize = 0.3f;

	float m_AlphaTime = 0.3f;

	float m_MinLifeTime = 0.3f;
	float m_MaxLifeTime = 0.6f;

	jsh::vec3 m_Direction = {0.f, 0.f, 0.f};
	jsh::vec3 m_Deviation = {5.f, 5.f, 5.f};

	jsh::Entity m_Parent;
	std::vector<Particle> particles;

	bool red;
	bool green;
	bool blue;
	bool white;
	bool yellow;
	bool orange;

	std::vector<jsh::Color4b> colors;

	float random()
	{
		return (float)rand() / RAND_MAX;
	}

	float random(float max)
	{
		return random() * max;
	}

	float random(float min, float max)
	{
		return min + random(max - min);
	}
	
	jsh::Color4b GetRandomColor()
	{
		if (colors.empty()) return jshColor4b::WHITE;
		else {
			return colors[random(colors.size())];
		}
	}

public:

	void Initialize()
	{
		m_Parent = jshScene::CreateEntity();
	}

	void Update(float dt, float x, float y, float z, bool create) {

		m_TimeCount += dt;
		m_GenTimeCount += dt;

		while (m_GenerationTime <= m_GenTimeCount) {

			particles.reserve(m_GenerationCount);
			m_GenTimeCount -= m_GenerationTime;
			for (uint32 i = 0; i < m_GenerationCount; ++i) {

				if (!create) break;

				jsh::vec3 vel;
				vel.x = random(m_Direction.x - m_Deviation.x / 2.f, m_Direction.x + m_Deviation.x / 2.f);
				vel.y = random(m_Direction.y - m_Deviation.y / 2.f, m_Direction.y + m_Deviation.y / 2.f);
				vel.z = random(m_Direction.z - m_Deviation.z / 2.f, m_Direction.z + m_Deviation.z / 2.f);

				float rotationVel = random(4.f, 200.f);

				float lifeTime = random(m_MinLifeTime, m_MaxLifeTime);
				float size = random(m_MinSize, m_MaxSize);
				particles.emplace_back(Particle({ x, y, z }, { size, size }, ToRadians(random() * 360.f), m_TimeCount, lifeTime, vel, ToRadians(rotationVel), GetRandomColor()));
			}
		}

		for (uint32 i = 0; i < particles.size(); ++i) {

			Particle& particle = particles[i];

			particle.position += particle.velocity * dt;
			particle.rotation += particle.rotationVel * dt;

			if (particle.alpha > 0) {
				float a = particle.color.w;
				a -= particle.alpha * dt;
				if (a > 255.f) a = 255.f;
				else if (a < 0.f) a = 0.f;
				particle.color.w = (uint8)a;
			}
			else if (abs(particle.creationTime + particle.lifeTime - m_TimeCount) <= m_AlphaTime) {
				particle.alpha = 255.f / abs(particle.creationTime + particle.lifeTime - m_TimeCount);
			}
		}

		for (int32 i = int32(particles.size() - 1u); i >= 0; --i) {
			Particle& particle = particles[i];
			if (particle.creationTime + particle.lifeTime <= m_TimeCount) particles.erase(particles.begin() + i);
		}

	}

#ifdef JSH_IMGUI
	void ShowImGuiWindow() {
		if (ImGui::Begin("Particles")) {
			ImGui::Text("Count: %u", particles.size());
			ImGui::SliderFloat("Generation Time", &m_GenerationTime, 0.001f, 1.f);
			int g = m_GenerationCount;
			ImGui::DragInt("Generation Count", &g);
			if (g < 0) g = 0;
			m_GenerationCount = g;
			ImGui::DragFloat2("Size", &m_MinSize, 0.01f);
			ImGui::SliderFloat("Alpha Time", &m_AlphaTime, 0.0001f, 1.f);
			ImGui::DragFloat2("Life Time", &m_MinLifeTime, 0.01f);
			ImGui::DragFloat3("Direction", &m_Direction.x, 0.01f);
			ImGui::DragFloat3("Deviation", &m_Deviation.x, 0.01f);

			if (ImGui::BeginCombo("Colors", "Colors")) {

				ImGui::Checkbox("Red", &red);
				ImGui::Checkbox("Green", &green);
				ImGui::Checkbox("Blue", &blue);
				ImGui::Checkbox("White", &white);
				ImGui::Checkbox("Yelow", &yellow);
				ImGui::Checkbox("Orange", &orange);

				colors.clear();
				if (red) colors.push_back(jshColor4b::RED);
				if (green) colors.push_back(jshColor4b::GREEN);
				if (blue) colors.push_back(jshColor4b::BLUE);
				if (white) colors.push_back(jshColor4b::WHITE);
				if (yellow) colors.push_back(jshColor4b::YELLOW);
				if (orange) colors.push_back(jshColor4b::ORANGE);

				ImGui::EndCombo();
			}

		}
		ImGui::End();
	}
#endif

	void Render()
	{
		jsh::SpriteRenderer& renderer = jshRenderer::GetSpriteRenderer();

		renderer.ReserveInstances(uint32(particles.size()));

		for (auto it = particles.begin(); it != particles.end(); ++it) {
			Particle& particle = *it;
			renderer.DrawQuad({ particle.position.x, particle.position.y }, particle.size, particle.rotation, particle.color, 0.f);
		}

		jshImGui(ShowImGuiWindow());
	}

};