/* Class KisLsOverlayFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLsOverlayFilter @ 00204a40 ======

void __thiscall
KisLsOverlayFilter::KisLsOverlayFilter(KisLsOverlayFilter *this,KisLsOverlayFilter *param_1)

{
  (*(code *)PTR_KisLsOverlayFilter_00839ff0)();
  return;
}



// ====== KisLsOverlayFilter @ 0020bcc0 ======

void __thiscall KisLsOverlayFilter::KisLsOverlayFilter(KisLsOverlayFilter *this,Mode param_1)

{
  (*(code *)PTR_KisLsOverlayFilter_0083d930)();
  return;
}



// ====== KisLsOverlayFilter @ 0068d500 ======

/* KisLsOverlayFilter::KisLsOverlayFilter(KisLsOverlayFilter::Mode) */

void __thiscall KisLsOverlayFilter::KisLsOverlayFilter(KisLsOverlayFilter *this,Mode param_1)

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
                    /* try { // try from 0068d54c to 0068d550 has its CatchHandler @ 0068d66c */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString(local_48);
                    /* try { // try from 0068d565 to 0068d569 has its CatchHandler @ 0068d684 */
  local_58 = (QArrayData *)QString::fromAscii_helper("lsoverlay",9);
                    /* try { // try from 0068d57a to 0068d57e has its CatchHandler @ 0068d678 */
  KoID::KoID((KoID *)local_48,(QString *)&local_58,(QString *)&local_50);
                    /* try { // try from 0068d585 to 0068d589 has its CatchHandler @ 0068d660 */
  KisLayerStyleFilter::KisLayerStyleFilter((KisLayerStyleFilter *)this,(KoID *)local_48);
  if (local_40 == (int *)0x0) {
LAB_0068d5a7:
    iVar3 = *(int *)local_58;
    if (iVar3 == 0) goto LAB_0068d630;
LAB_0068d5b1:
    if (iVar3 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_0068d630;
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
    if (*local_40 != 0) goto LAB_0068d5a7;
    operator_delete(local_40,0x10);
    iVar3 = *(int *)local_58;
    if (iVar3 != 0) goto LAB_0068d5b1;
LAB_0068d630:
    QArrayData::deallocate(local_58,2,8);
    iVar3 = *(int *)local_50;
  }
  if (iVar3 != 0) {
    if (iVar3 == -1) goto LAB_0068d5db;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_0068d5db;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_0068d5db:
  puVar2 = PTR_vtable_00836c88;
  *(Mode *)(this + 0x20) = param_1;
  *(undefined **)this = puVar2 + 0x10;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisLsOverlayFilter @ 0068d690 ======

/* KisLsOverlayFilter::KisLsOverlayFilter(KisLsOverlayFilter const&) */

void __thiscall
KisLsOverlayFilter::KisLsOverlayFilter(KisLsOverlayFilter *this,KisLsOverlayFilter *param_1)

{
  KisLayerStyleFilter::KisLayerStyleFilter
            ((KisLayerStyleFilter *)this,(KisLayerStyleFilter *)param_1);
  *(undefined **)this = PTR_vtable_00836c88 + 0x10;
  *(undefined4 *)(this + 0x20) = *(undefined4 *)(param_1 + 0x20);
  return;
}



