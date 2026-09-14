/* Class KisLsStrokeFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLsStrokeFilter @ 00201000 ======

void __thiscall KisLsStrokeFilter::KisLsStrokeFilter(KisLsStrokeFilter *this)

{
  (*(code *)PTR_KisLsStrokeFilter_008382d0)();
  return;
}



// ====== KisLsStrokeFilter @ 00208f10 ======

void __thiscall
KisLsStrokeFilter::KisLsStrokeFilter(KisLsStrokeFilter *this,KisLsStrokeFilter *param_1)

{
  (*(code *)PTR_KisLsStrokeFilter_0083c258)();
  return;
}



// ====== KisLsStrokeFilter @ 00680af0 ======

/* KisLsStrokeFilter::KisLsStrokeFilter() */

void __thiscall KisLsStrokeFilter::KisLsStrokeFilter(KisLsStrokeFilter *this)

{
  int *piVar1;
  int iVar2;
  long in_FS_OFFSET;
  QArrayData *local_58;
  QArrayData *local_50;
  KLocalizedString local_48 [8];
  int *local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  ki18nd((char *)local_48,"krita");
                    /* try { // try from 00680b37 to 00680b3b has its CatchHandler @ 00680c4c */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString(local_48);
                    /* try { // try from 00680b50 to 00680b54 has its CatchHandler @ 00680c64 */
  local_58 = (QArrayData *)QString::fromAscii_helper("lsstroke",8);
                    /* try { // try from 00680b65 to 00680b69 has its CatchHandler @ 00680c58 */
  KoID::KoID((KoID *)local_48,(QString *)&local_58,(QString *)&local_50);
                    /* try { // try from 00680b70 to 00680b74 has its CatchHandler @ 00680c40 */
  KisLayerStyleFilter::KisLayerStyleFilter((KisLayerStyleFilter *)this,(KoID *)local_48);
  if (local_40 == (int *)0x0) {
LAB_00680b92:
    iVar2 = *(int *)local_58;
    if (iVar2 == 0) goto LAB_00680c10;
LAB_00680b9c:
    if (iVar2 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_00680c10;
    }
    iVar2 = *(int *)local_50;
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
    if (*local_40 != 0) goto LAB_00680b92;
    operator_delete(local_40,0x10);
    iVar2 = *(int *)local_58;
    if (iVar2 != 0) goto LAB_00680b9c;
LAB_00680c10:
    QArrayData::deallocate(local_58,2,8);
    iVar2 = *(int *)local_50;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_00680bc6;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_00680bc6;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_00680bc6:
  *(undefined **)this = PTR_vtable_008378f8 + 0x10;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisLsStrokeFilter @ 00680c70 ======

/* KisLsStrokeFilter::KisLsStrokeFilter(KisLsStrokeFilter const&) */

void __thiscall
KisLsStrokeFilter::KisLsStrokeFilter(KisLsStrokeFilter *this,KisLsStrokeFilter *param_1)

{
  KisLayerStyleFilter::KisLayerStyleFilter
            ((KisLayerStyleFilter *)this,(KisLayerStyleFilter *)param_1);
  *(undefined **)this = PTR_vtable_008378f8 + 0x10;
  return;
}



