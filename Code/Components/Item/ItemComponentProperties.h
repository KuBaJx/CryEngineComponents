#pragma once

#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/Reflection/TypeDesc.h>

enum ItemGeometrySlot
{
	GEOMETRY_SLOT = 1
};

// RENDER PROPERTIES
struct SRenderProperties
{
	inline bool operator==(const SRenderProperties& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
	inline bool operator!=(const SRenderProperties& rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

	Schematyc::GeomFileName		sGeomPath;
	Schematyc::MaterialFileName sItemMaterial;
};
static void ReflectType(Schematyc::CTypeDesc<SRenderProperties>& desc)
{
	desc.SetGUID("{24DD2B80-A971-43FE-9F6E-B63072AA93A4}"_cry_guid);
	desc.SetLabel("Render Properties");
	desc.SetDescription("Item render properties, like geometry, material, etc..");
	desc.AddMember(&SRenderProperties::sGeomPath, 'geom', "GeometryPath", "Geometry", "Sets custom geometry", "");
	desc.AddMember(&SRenderProperties::sItemMaterial, 'smat', "MaterialPath", "Material", "Sets custom material", "");
}

// PHYSICS PROPERTIES
struct SPhysicsProperties
{
	inline bool operator==(const SPhysicsProperties& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
	inline bool operator!=(const SPhysicsProperties& rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

	float fMass = 10.f;
};
static void ReflectType(Schematyc::CTypeDesc<SPhysicsProperties>& desc)
{
	desc.SetGUID("{961DD387-71CA-4143-A35B-790E16FCF0CB}"_cry_guid);
	desc.SetLabel("Physics Properties");
	desc.SetDescription("Physics properties of item. Mass, density, etc..");
	desc.AddMember(&SPhysicsProperties::fMass, 'mass', "Mass", "Mass", "Mass of the item", 0.f);
}

// MAIN PROPERTY STRUCT
struct SItemProperties
{
	inline bool operator==(const SItemProperties& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
	inline bool operator!=(const SItemProperties& rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

	SRenderProperties sRenderProperties;
	SPhysicsProperties sPhysicsProperties;


};
static void ReflectType(Schematyc::CTypeDesc<SItemProperties>& desc)
{
	desc.SetGUID("{AB735B6D-8258-4CD1-BFAF-FF56AE5682B3}"_cry_guid);
	desc.SetLabel("Item Properties");
	desc.SetDescription("Item render properties, like geometry, material, etc..");
	desc.AddMember(&SItemProperties::sRenderProperties, 'renp', "RenderProperties", "Render settings", "Render settings of the item", SRenderProperties());
	desc.AddMember(&SItemProperties::sPhysicsProperties, 'phyp', "PhysicsProperties", "Physics settings", "Physics settings of the item", SPhysicsProperties());
}