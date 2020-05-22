#pragma once

#include "jshEngine.h"

class LightGenerator {

	jsh::Entity pointsParent;

	jsh::vec3 center;
	jsh::vec3 distance = { 10.f, 10.f, 10.f };

	int pointsCount = 1;
	float pointsMinRange = 1.f;
	float pointsMaxRange = 1.f;
	float pointsMinIntensity = 1.f;
	float pointsMaxIntensity = 1.f;
	float pointsMinSmoothness = 0.5f;
	float pointsMaxSmoothness = 0.6f;
	bool pointsColored = true;

public:
	void Update(float dt) 
	{
		if (pointsParent) {
			jsh::Transform& trans = jshScene::GetTransform(pointsParent);
			jsh::vec3 rot = trans.GetLocalRotation();
			rot.x += ToRadians(45.f * dt);
			rot.y += ToRadians(10.f * dt);
			trans.SetRotation(rot);
		}
	}

	void Render()
	{
		// Light generator
		if (ImGui::Begin("LightGenerator")) {
			
			ImGui::DragFloat3("Center", &center.x);
			ImGui::DragFloat3("Distance", &distance.x);

			ImGui::Separator();

			ImGui::DragInt("Points Count", &pointsCount);
			if (pointsCount < 0) pointsCount = 0;

			ImGui::Checkbox("PointsColored", &pointsColored);
			ImGui::DragFloat2("Range", &pointsMinRange, 0.5f);
			ImGui::DragFloat2("Intensity", &pointsMinIntensity, 0.5f);
			ImGui::DragFloat2("Smoothness", &pointsMinSmoothness, 0.005f);

			if (ImGui::Button("Generate Points")) {
				if (pointsParent) jshScene::DestroyEntity(pointsParent);

				pointsParent = jshScene::CreateEntity();
				jshScene::CreateSEntities(pointsParent, pointsCount, nullptr, jsh::LightComponent());

				{
					jsh::Transform& parentTrans = jshScene::GetTransform(pointsParent);
					parentTrans.SetPosition(center);
				}

				jsh::Entity* childs;
				uint32 childsCount = 0u;
				jshScene::GetEntitySons(pointsParent, &childs, &childsCount);

				for (uint32 i = 0; i < childsCount; ++i) {
					jsh::LightComponent* light = jshScene::GetComponent<jsh::LightComponent>(childs[i]);
					jsh::Transform& trans = jshScene::GetTransform(childs[i]);

					light->lightType = JSH_LIGHT_TYPE_POINT;

					if (pointsColored) {
						light->color.x = ((float)rand() / RAND_MAX);
						light->color.y = ((float)rand() / RAND_MAX);
						light->color.z = ((float)rand() / RAND_MAX);
					}

					light->range = pointsMinRange + (((float)rand() / RAND_MAX) * (pointsMaxRange - pointsMinRange));
					light->intensity = pointsMinIntensity + (((float)rand() / RAND_MAX) * (pointsMaxIntensity - pointsMinIntensity));
					light->smoothness = pointsMinSmoothness + (((float)rand() / RAND_MAX) * (pointsMaxSmoothness - pointsMinSmoothness));

					jsh::vec3 pos;
					pos.x = ((float)rand() / RAND_MAX) * distance.x - (distance.x / 2.f);
					pos.y = ((float)rand() / RAND_MAX) * distance.y - (distance.y / 2.f);
					pos.z = ((float)rand() / RAND_MAX) * distance.z - (distance.z / 2.f);
					trans.SetPosition(pos);
				}
			}
		}
		ImGui::End();
	}

};