#pragma once

#include "jshEngine.h"

struct ParticleComponent : public jsh::Component<ParticleComponent> {
	float creationTime;
	float lifeTime;
	float alpha = 0;

	jsh::vec3 velocity;
	float rotationVel;

	ParticleComponent() : creationTime(0.f), lifeTime(0.f), velocity(), rotationVel(0.f) {}
	ParticleComponent(float t0, float t1, jsh::vec3 vel, float rotationVel) : creationTime(t0), lifeTime(t1), velocity(vel), rotationVel(rotationVel) {}
};
jshDefineComponent(ParticleComponent);

class ParticleGenerator {

	float m_GenerationTime = 0.0005f;
	float m_GenTimeCount = 0u;
	float m_TimeCount = 0u;

	float m_MinSize = 0.1f;
	float m_MaxSize = 0.3f;

	float m_AlphaTime = 0.3f;

	float m_MinLifeTime = 0.3f;
	float m_MaxLifeTime = 0.6f;

	jsh::vec3 m_Direction = {0.f, 0.f, 0.f};
	jsh::vec3 m_Deviation = {0.f, 0.f, 0.f};

	jsh::Entity m_Parent;

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
	
public:

	void Initialize()
	{
		m_Parent = jshScene::CreateEntity();
	}

	void Update(float dt, float x, float y, float z, bool create) {

		m_TimeCount += dt;
		m_GenTimeCount += dt;

		while (m_GenerationTime <= m_GenTimeCount) {
			m_GenTimeCount -= m_GenerationTime;

			if (!create) break;

			constexpr jsh::Color colors[3] = {
				jshColor::RED,
				jshColor::GREEN,
				jshColor::BLUE,
			};
			
			jsh::vec3 vel;
			vel.x = random(m_Direction.x - m_Deviation.x / 2.f, m_Direction.x + m_Deviation.x / 2.f);
			vel.y = random(m_Direction.y - m_Deviation.y / 2.f, m_Direction.y + m_Deviation.y / 2.f);
			vel.z = random(m_Direction.z - m_Deviation.z / 2.f, m_Direction.z + m_Deviation.z / 2.f);

			float rotationVel = random(4.f, 200.f);

			float lifeTime = random(m_MinLifeTime, m_MaxLifeTime);

			jsh::Entity entity = jshScene::CreateSEntity(m_Parent, jsh::SpriteComponent(colors[uint32(random(3))]), ParticleComponent(m_TimeCount, lifeTime, vel, ToRadians(rotationVel)));
			jsh::Transform& trans = jshScene::GetTransform(entity);
			trans.SetPosition({ x, y, z });
			trans.SetRotation({ 0.f, 0.f, ToRadians(random() * 360.f) });

			float size = random(m_MinSize, m_MaxSize);
			trans.SetScale({ size, size, 0.f });
		}

		jsh::Entity* sons;
		uint32 size = 0u;
		jshScene::GetEntitySons(m_Parent, &sons, &size);

		for (uint32 i = 0; i < size; ++i) {
			jsh::Entity entity = sons[i];

			ParticleComponent* particleComp = jshScene::GetComponent<ParticleComponent>(entity);
			jsh::SpriteComponent* spriteComp = jshScene::GetComponent<jsh::SpriteComponent>(entity);
			jsh::Transform& trans = jshScene::GetTransform(entity);

			jsh::vec3 pos = trans.GetLocalPosition();
			pos += particleComp->velocity * dt;
			trans.SetPosition(pos);

			jsh::vec3 rot = trans.GetLocalRotation();
			rot.z += particleComp->rotationVel * dt;
			trans.SetRotation(rot);

			jsh::Color& color = spriteComp->color;
			if (particleComp->alpha > 0) {
				float a = color.w;
				a -= particleComp->alpha * dt;
				if (a > 255.f) a = 255.f;
				else if (a < 0.f) a = 0.f;
				color.w = (byte)a;
			}
			else if (abs(particleComp->creationTime + particleComp->lifeTime - m_TimeCount) <= m_AlphaTime) {
				particleComp->alpha = 255.f / abs(particleComp->creationTime + particleComp->lifeTime - m_TimeCount);
			}
		}

		for (int32 i = size - 1; i >= 0; --i) {
			jsh::Entity entity = sons[i];
			ParticleComponent* particleComp = jshScene::GetComponent<ParticleComponent>(entity);
			if (particleComp->creationTime + particleComp->lifeTime <= m_TimeCount) jshScene::DestroyEntity(entity);
		}
	}

#ifdef JSH_IMGUI
	void ShowImGuiWindow() {
		if (ImGui::Begin("Particles")) {
			ImGui::SliderFloat("Generation Time", &m_GenerationTime, 0.0005f, 1.f);
			ImGui::DragFloat2("Size", &m_MinSize, 0.01f);
			ImGui::SliderFloat("Alpha Time", &m_AlphaTime, 0.0001f, 1.f);
			ImGui::DragFloat2("Life Time", &m_MinLifeTime, 0.01f);
			ImGui::DragFloat3("Direction", &m_Direction.x, 0.01f);
			ImGui::DragFloat3("Deviation", &m_Deviation.x, 0.01f);
		}
		ImGui::End();
	}
#endif

};