/**********************************************************************
 *  Copyright (c) 2008-2013, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#include <energyplus/ForwardTranslator.hpp>
#include <model/Screen.hpp>
#include <utilities/idd/OS_WindowMaterial_Screen_FieldEnums.hxx>

#include <utilities/idd/WindowMaterial_Screen_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

using namespace openstudio::model;

namespace openstudio {

namespace energyplus {

boost::optional<IdfObject> ForwardTranslator::translateScreen( Screen & modelObject )
{
  IdfObject idfObject( openstudio::IddObjectType::WindowMaterial_Screen);

  m_idfObjects.push_back(idfObject);

  idfObject.setString(WindowMaterial_ScreenFields::Name, modelObject.name().get());
  
  OptionalString s = modelObject.getString(OS_WindowMaterial_ScreenFields::ReflectedBeamTransmittanceAccountingMethod, false, true);
  if (s){
    idfObject.setString(WindowMaterial_ScreenFields::ReflectedBeamTransmittanceAccountingMethod, *s);
  }

  OptionalDouble d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::DiffuseSolarReflectance, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::DiffuseSolarReflectance, *d);
  }else{
    LOG(Error, "Missing required input 'Diffuse Solar Reflectance' for WindowMaterial:Screen named '" << modelObject.name().get() << "'");
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::DiffuseVisibleReflectance, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::DiffuseVisibleReflectance, *d);
  }else{
    LOG(Error, "Missing required input 'Diffuse Visible Reflectance' for WindowMaterial:Screen named '" << modelObject.name().get() << "'");
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::ThermalHemisphericalEmissivity, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::ThermalHemisphericalEmissivity, *d);
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::Conductivity, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::Conductivity, *d);
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::ScreenMaterialSpacing, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::ScreenMaterialSpacing, *d);
  }else{
    LOG(Error, "Missing required input 'Screen Material Spacing' for WindowMaterial:Screen named '" << modelObject.name().get() << "'");
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::ScreenMaterialDiameter, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::ScreenMaterialDiameter, *d);
  }else{
    LOG(Error, "Missing required input 'Screen Material Diameter' for WindowMaterial:Screen named '" << modelObject.name().get() << "'");
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::ScreentoGlassDistance, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::ScreentoGlassDistance, *d);
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::TopOpeningMultiplier, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::TopOpeningMultiplier, *d);
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::BottomOpeningMultiplier, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::BottomOpeningMultiplier, *d);
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::LeftSideOpeningMultiplier, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::LeftSideOpeningMultiplier, *d);
  }

  d = modelObject.getDouble(OS_WindowMaterial_ScreenFields::RightSideOpeningMultiplier, false);
  if (d){
    idfObject.setDouble(WindowMaterial_ScreenFields::RightSideOpeningMultiplier, *d);
  }

  s = modelObject.getString(OS_WindowMaterial_ScreenFields::AngleofResolutionforScreenTransmittanceOutputMap, false, true);
  if (s){
    idfObject.setString(WindowMaterial_ScreenFields::AngleofResolutionforScreenTransmittanceOutputMap, *s);
  }

  return boost::optional<IdfObject>(idfObject);
}

} // energyplus

} // openstudio

