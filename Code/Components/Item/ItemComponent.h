#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>
#include "CryPhysics/physinterface.h"

#include "ItemComponentProperties.h"

class CItemComponent : public IEntityComponent
{

protected:
	float m_velocity = 500.0f;
	float m_mass = 100.0f;

public:
	CItemComponent() = default;
	virtual ~CItemComponent() {}

	// IEntityComponent
	virtual void Initialize() override;

	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	static void ReflectType(Schematyc::CTypeDesc<CItemComponent>& desc)
	{
		desc.SetGUID("{D8D62548-44EA-4185-8931-6BBDB8AB85AE}"_cry_guid);
		desc.SetEditorCategory("Game");
		desc.SetLabel("Item Component");
		desc.SetDescription("Default Item Component");
		desc.AddMember(&CItemComponent::sItemProperties, 'itpr', "ItemProperties", "Item Properties", "Sets item properties", SItemProperties());
	}

	void LoadGeometry();
	void Physicalize();

	SItemProperties* GetItemProperties() { return &sItemProperties; }

private:
	SItemProperties sItemProperties, sPrevItemProperties;
};