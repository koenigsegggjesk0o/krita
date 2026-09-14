/* Class KisLsDropShadowFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLsDropShadowFilter @ 00205e90 ======

void __thiscall
KisLsDropShadowFilter::KisLsDropShadowFilter(KisLsDropShadowFilter *this,Mode param_1)

{
  (*(code *)PTR_KisLsDropShadowFilter_0083aa18)();
  return;
}



// ====== KisLsDropShadowFilter @ 00209b10 ======

void __thiscall
KisLsDropShadowFilter::KisLsDropShadowFilter
          (KisLsDropShadowFilter *this,KisLsDropShadowFilter *param_1)

{
  (*(code *)PTR_KisLsDropShadowFilter_0083c858)();
  return;
}



// ====== KisLsDropShadowFilter @ 0067be00 ======

/* KisLsDropShadowFilter::KisLsDropShadowFilter(KisLsDropShadowFilter::Mode) */

void __thiscall
KisLsDropShadowFilter::KisLsDropShadowFilter(KisLsDropShadowFilter *this,Mode param_1)

{
  int *piVar1;
  undefined *puVar2;
  int iVar3;
  long in_FS_OFFSET;
  QArrayData *local_58;
  QArrayData *local_50;
  KLocalizedString local_48 [8];
  int *local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  ki18nd((char *)local_48,"krita");
                    /* try { // try from 0067be4c to 0067be50 has its CatchHandler @ 0067bf6c */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString(local_48);
                    /* try { // try from 0067be65 to 0067be69 has its CatchHandler @ 0067bf84 */
  local_58 = (QArrayData *)QString::fromAscii_helper("lsdropshadow",0xc);
                    /* try { // try from 0067be7a to 0067be7e has its CatchHandler @ 0067bf78 */
  KoID::KoID((KoID *)local_48,(QString *)&local_58,(QString *)&local_50);
                    /* try { // try from 0067be85 to 0067be89 has its CatchHandler @ 0067bf60 */
  KisLayerStyleFilter::KisLayerStyleFilter((KisLayerStyleFilter *)this,(KoID *)local_48);
  if (local_40 == (int *)0x0) {
LAB_0067bea7:
    iVar3 = *(int *)local_58;
    if (iVar3 == 0) goto LAB_0067bf30;
LAB_0067beb1:
    if (iVar3 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_0067bf30;
    }
    iVar3 = *(int *)local_50;
  }
  else {
    LOCK();
    piVar1 = local_40 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_40 + 2))(local_40);
    }
    LOCK();
    *local_40 = *local_40 + -1;
    UNLOCK();
    if (*local_40 != 0) goto LAB_0067bea7;
    operator_delete(local_40,0x10);
    iVar3 = *(int *)local_58;
    if (iVar3 != 0) goto LAB_0067beb1;
LAB_0067bf30:
    QArrayData::deallocate(local_58,2,8);
    iVar3 = *(int *)local_50;
  }
  if (iVar3 != 0) {
    if (iVar3 == -1) goto LAB_0067bedb;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_0067bedb;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_0067bedb:
  puVar2 = PTR_vtable_00837cf8;
  *(Mode *)(this + 0x20) = param_1;
  *(undefined **)this = puVar2 + 0x10;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisLsDropShadowFilter @ 0067bf90 ======

/* KisLsDropShadowFilter::KisLsDropShadowFilter(KisLsDropShadowFilter const&) */

void __thiscall
KisLsDropShadowFilter::KisLsDropShadowFilter
          (KisLsDropShadowFilter *this,KisLsDropShadowFilter *param_1)

{
  KisLayerStyleFilter::KisLayerStyleFilter
            ((KisLayerStyleFilter *)this,(KisLayerStyleFilter *)param_1);
  *(undefined **)this = PTR_vtable_00837cf8 + 0x10;
  *(undefined4 *)(this + 0x20) = *(undefined4 *)(param_1 + 0x20);
  return;
}



