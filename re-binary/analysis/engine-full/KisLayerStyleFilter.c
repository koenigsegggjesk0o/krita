/* Class KisLayerStyleFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayerStyleFilter @ 002036b0 ======

void __thiscall
KisLayerStyleFilter::KisLayerStyleFilter(KisLayerStyleFilter *this,KisLayerStyleFilter *param_1)

{
  (*(code *)PTR_KisLayerStyleFilter_00839628)();
  return;
}



// ====== KisLayerStyleFilter @ 00208800 ======

void __thiscall KisLayerStyleFilter::KisLayerStyleFilter(KisLayerStyleFilter *this,KoID *param_1)

{
  (*(code *)PTR_KisLayerStyleFilter_0083bed0)();
  return;
}



// ====== KisLayerStyleFilter @ 00670e70 ======

/* KisLayerStyleFilter::KisLayerStyleFilter(KoID const&) */

void __thiscall KisLayerStyleFilter::KisLayerStyleFilter(KisLayerStyleFilter *this,KoID *param_1)

{
  KoID *this_00;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837868 + 0x10;
                    /* try { // try from 00670ea3 to 00670ea7 has its CatchHandler @ 00670ecd */
  this_00 = (KoID *)operator_new(0x10);
                    /* try { // try from 00670eae to 00670eb2 has its CatchHandler @ 00670ee5 */
  KoID::KoID(this_00);
  *(KoID **)(this + 0x18) = this_00;
                    /* try { // try from 00670ebd to 00670ec1 has its CatchHandler @ 00670ed9 */
  KoID::operator=(this_00,param_1);
  return;
}



// ====== KisLayerStyleFilter @ 00670f00 ======

/* KisLayerStyleFilter::KisLayerStyleFilter(KisLayerStyleFilter const&) */

void __thiscall
KisLayerStyleFilter::KisLayerStyleFilter(KisLayerStyleFilter *this,KisLayerStyleFilter *param_1)

{
  KoID *this_00;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837868 + 0x10;
                    /* try { // try from 00670f33 to 00670f37 has its CatchHandler @ 00670f5f */
  this_00 = (KoID *)operator_new(0x10);
                    /* try { // try from 00670f3e to 00670f42 has its CatchHandler @ 00670f77 */
  KoID::KoID(this_00);
  *(KoID **)(this + 0x18) = this_00;
                    /* try { // try from 00670f4f to 00670f53 has its CatchHandler @ 00670f6b */
  KoID::operator=(this_00,*(KoID **)(param_1 + 0x18));
  return;
}



