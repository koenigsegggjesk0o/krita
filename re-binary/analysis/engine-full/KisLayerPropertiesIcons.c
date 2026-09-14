/* Class KisLayerPropertiesIcons - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayerPropertiesIcons @ 00201620 ======

void __thiscall KisLayerPropertiesIcons::KisLayerPropertiesIcons(KisLayerPropertiesIcons *this)

{
  (*(code *)PTR_KisLayerPropertiesIcons_008385e0)();
  return;
}



// ====== KisLayerPropertiesIcons @ 0066be30 ======

/* KisLayerPropertiesIcons::KisLayerPropertiesIcons() */

void __thiscall KisLayerPropertiesIcons::KisLayerPropertiesIcons(KisLayerPropertiesIcons *this)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  puVar2 = (undefined8 *)operator_new(8);
  puVar1 = PTR_shared_null_008372c0;
  *(undefined8 **)this = puVar2;
  *puVar2 = puVar1;
                    /* try { // try from 0066be57 to 0066be5b has its CatchHandler @ 0066be63 */
  updateIcons(this);
  return;
}



