#include "StdAfx.h"
#include "Player.h"
#include "Item/ItemComponent.h"

#include <CryRenderer/IRenderAuxGeom.h>
#include <CrySystem/Scaleform/IFlashUI.h>
#include <array>

void CPlayerComponent::Initialize()
{
	// Create the camera component, will automatically update the viewport every frame
	m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	// Register an action, and the callback that will be sent when it's triggered
	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveLeft, activationMode);  });
	// Bind the 'A' key the "moveleft" action
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, EKeyId::eKI_A);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveRight, activationMode);  });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, EKeyId::eKI_D);

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveForward, activationMode);  });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, EKeyId::eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveBack, activationMode);  });
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, EKeyId::eKI_S);

	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { m_mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { m_mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

	m_pInputComponent->RegisterAction("player", "shoot", [this](int activationMode, float value)
	{
		if (activationMode == eIS_Pressed)
		{
			SEntitySpawnParams spawnParams;
			spawnParams.vPosition = m_pEntity->GetWorldPos();
			spawnParams.qRotation = m_pEntity->GetRotation();

			if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
			{
				pEntity->CreateComponentClass<CItemComponent>();
				CryLogAlways("Entity spawned");
			}
		}
	});
	m_pInputComponent->BindAction("player", "shoot", eAID_KeyboardMouse, EKeyId::eKI_Mouse1);

	Revive();
}

uint64 CPlayerComponent::GetEventMask() const
{
	return ENTITY_EVENT_BIT(ENTITY_EVENT_START_GAME) | ENTITY_EVENT_BIT(ENTITY_EVENT_UPDATE);
}

void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
		// Revive the entity when gameplay starts
		Revive();
	}
	break;
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];
		Update(pCtx->fFrameTime);

		const float moveSpeed = 20.5f;
		Vec3 velocity = ZERO;

		// Check input to calculate local space velocity
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveLeft)
		{
			velocity.x -= moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveRight)
		{
			velocity.x += moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveForward)
		{
			velocity.y += moveSpeed * pCtx->fFrameTime;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveBack)
		{
			velocity.y -= moveSpeed * pCtx->fFrameTime;
		}
		Vec3 position = GetEntity()->GetPos();

		auto pDebug = gEnv->pGameFramework->GetIPersistantDebug();
		if (pDebug)
		{
			pDebug->Begin("Player Info", true);

			if (gEnv->p3DEngine->IsUnderWater(GetEntity()->GetPos()))
			{
				pDebug->AddText(10.f, 65.f, 1.5f, ColorF(Vec4(1.f, 0.f, 0.f, 1.f)), 0.1f, "Player is underwater!");
			}
			pDebug->AddText(10.f, 10.f, 1.5f, ColorF(Vec4(0.f, 1.f, 0.f, 1.f)), 0.1f, "X: %f\nY: %f\nZ: %f\n", position.x, position.y, position.z);
		}

		// Update the player's transformation
		Matrix34 transformation = m_pEntity->GetWorldTM();
		transformation.AddTranslation(transformation.TransformVector(velocity));

		// Update entity rotation based on latest input
		Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(transformation));

		const float rotationSpeed = 0.002f;
		ypr.x += m_mouseDeltaRotation.x * rotationSpeed;
		ypr.y += m_mouseDeltaRotation.y * rotationSpeed;

		// Disable roll
		ypr.z = 0;

		transformation.SetRotation33(CCamera::CreateOrientationYPR(ypr));

		// Reset the mouse delta since we "used" it this frame
		m_mouseDeltaRotation = ZERO;

		// Apply set position and rotation to the entity
		m_pEntity->SetWorldTM(transformation);
	}
	break;
	}
}

void CPlayerComponent::Revive()
{
	// Set player transformation, but skip this in the Editor
	if (!gEnv->IsEditor())
	{
		Vec3 playerScale = Vec3(1.f);
		Quat playerRotation = IDENTITY;

		// Position the player in the center of the map
		const float heightOffset = 20.f;
		float terrainCenter = gEnv->p3DEngine->GetTerrainSize() / 2.f;
		float height = gEnv->p3DEngine->GetTerrainZ(terrainCenter, terrainCenter);
		Vec3 playerPosition = Vec3(terrainCenter, terrainCenter, height + heightOffset);

		m_pEntity->SetWorldTM(Matrix34::Create(playerScale, playerRotation, playerPosition));
	}

	// Unhide the entity in case hidden by the Editor
	GetEntity()->Hide(false);

	// Reset input now that the player respawned
	m_inputFlags = 0;
	m_mouseDeltaRotation = ZERO;
}

void CPlayerComponent::Update(float frameTime)
{
	ray_hit hit;
	Vec3 cameraDir = gEnv->pSystem->GetViewCamera().GetViewdir();
	Vec3 cameraPos = gEnv->pSystem->GetViewCamera().GetPosition() + cameraDir;

	static const uint rayFlags = rwi_stop_at_pierceable | rwi_colltype_any;
	if (gEnv->pPhysicalWorld->RayWorldIntersection(cameraPos, (cameraDir * PICKUP_RANGE), ent_all, rayFlags, &hit, 1, m_pEntity->GetPhysicalEntity()))
	{
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(hit.pt - Vec3(.0f, .0f, 1.f), ColorB(255, 0, 0), hit.pt + Vec3(.0f, .0f, 1.f), ColorB(255, 0, 0), 6.f);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(hit.pt - Vec3(.0f, 1.f, 0.f), ColorB(255, 0, 0), hit.pt + Vec3(.0f, 1.f, 0.f), ColorB(255, 0, 0), 6.f);
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(hit.pt - Vec3(1.f, .0f, 1.f), ColorB(255, 0, 0), hit.pt + Vec3(1.f, .0f, 1.f), ColorB(255, 0, 0), 6.f);

		if (hit.pCollider)
		{
			if (auto pColliderEntity = gEnv->pEntitySystem->GetEntityFromPhysics(hit.pCollider))
			{
				// If detected entity is item
				if (auto pColliderItem = pColliderEntity->GetComponent<CItemComponent>())
				{
					ShowPickupMsg(pColliderItem->GetName());
				}
			}
		}
	}
}

void CPlayerComponent::ShowPickupMsg(string itemName)
{
	Vec2 screenCenter = { gEnv->pRenderer->GetWidth() / 2.f, gEnv->pRenderer->GetHeight() / 2.f };
	ColorF pfColor = { 1.f, 1.f, 1.f, 1.f };
	string sPickupMsg = "Pickup " + itemName;
	gEnv->pRenderer->GetIRenderAuxGeom()->Draw2dLabel(screenCenter.x, screenCenter.y, 1.5f, pfColor, true, sPickupMsg);
}

void CPlayerComponent::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
{
	switch (type)
	{
	case EInputFlagType::Hold:
	{
		if (activationMode == eIS_Released)
		{
			m_inputFlags &= ~flags;
		}
		else
		{
			m_inputFlags |= flags;
		}
	}
	break;
	case EInputFlagType::Toggle:
	{
		if (activationMode == eIS_Released)
		{
			// Toggle the bit(s)
			m_inputFlags ^= flags;
		}
	}
	break;
	}
}