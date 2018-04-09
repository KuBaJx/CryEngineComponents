#include "StdAfx.h"
#include "ItemComponent.h"

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>

static void RegisterItemComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CItemComponent));
		{

		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterItemComponent)

void CItemComponent::Initialize()
{
	LoadGeometry();
	Physicalize();

	m_pEntity->SetViewDistRatio(255);

	if (auto* pPhysics = GetEntity()->GetPhysics())
	{
		pe_action_impulse impulseAction;

		impulseAction.impulse = GetEntity()->GetWorldRotation().GetColumn1() * m_velocity;
		pPhysics->Action(&impulseAction);
	}
}

uint64 CItemComponent::GetEventMask() const
{
	return	ENTITY_EVENT_BIT(ENTITY_EVENT_COLLISION) | 
			ENTITY_EVENT_BIT(ENTITY_EVENT_UPDATE)	 |
			ENTITY_EVENT_BIT(ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED);
}

void CItemComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_COLLISION:
		{
			//
		}
		break;
	case ENTITY_EVENT_UPDATE:
		{

		}
		break;
	case ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED:
		{
			if (sItemProperties != sPrevItemProperties)
			{
				sPrevItemProperties = sItemProperties; // Set new properties, so we can compare it again with new value
				LoadGeometry();
				Physicalize();
			}
		}
	}
}

void CItemComponent::LoadGeometry()
{
	string sGeomPath = GetItemProperties()->sRenderProperties.sGeomPath.value.c_str();
	string sMatPath = GetItemProperties()->sRenderProperties.sItemMaterial.value.c_str();

	auto pItemMat = gEnv->p3DEngine->GetMaterialManager()->GetDefaultMaterial();

	// If empty, load default geom and material
	if (sGeomPath.empty())
	{
		m_pEntity->LoadGeometry(GEOMETRY_SLOT, "Assets/Objects/generic/default/primitive_sphere.cgf");
		return;
	}
	if (!sMatPath.empty())
	{
		pItemMat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(sMatPath);
		m_pEntity->SetMaterial(pItemMat);
	}

	m_pEntity->LoadGeometry(GEOMETRY_SLOT, sGeomPath);
	m_pEntity->SetMaterial(pItemMat);
}

void CItemComponent::Physicalize()
{
	SEntityPhysicalizeParams physParams;
	physParams.type = PE_RIGID;
	physParams.mass = GetItemProperties()->sPhysicsProperties.fMass;
	m_pEntity->Physicalize(physParams);
}