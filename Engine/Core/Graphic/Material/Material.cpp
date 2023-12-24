//
// Created by samsa on 12/22/2023.
//

#include "Material.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Texture/Texture.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

DEFINE_SIMPLE_IMGUI_TYPE( TextureSlot, Texture, Slot )
DEFINE_SIMPLE_IMGUI_TYPE( Material, ColorTexture, NormalTexture )

void
Material::ActivateMaterial( ) const
{
    if ( ColorTexture.Texture != nullptr ) ColorTexture.Texture->BindTexture( ColorTexture.Slot );
    if ( NormalTexture.Texture != nullptr ) NormalTexture.Texture->BindTexture( ColorTexture.Slot );
}
