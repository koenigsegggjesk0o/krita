/* Class KisLsSatinFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLsSatinFilter @ 00202fd0 ======

void __thiscall KisLsSatinFilter::KisLsSatinFilter(KisLsSatinFilter *this,KisLsSatinFilter *param_1)

{
  (*(code *)PTR_KisLsSatinFilter_008392b8)();
  return;
}



// ====== KisLsSatinFilter @ 00209750 ======

void __thiscall KisLsSatinFilter::KisLsSatinFilter(KisLsSatinFilter *this)

{
  (*(code *)PTR_KisLsSatinFilter_0083c678)();
  return;
}



// ====== KisLsSatinFilter @ 0067dfa0 ======

/* KisLsSatinFilter::KisLsSatinFilter() */

void __thiscall KisLsSatinFilter::KisLsSatinFilter(KisLsSatinFilter *this)

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
                    /* try { // try from 0067dfe7 to 0067dfeb has its CatchHandler @ 0067e0fc */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString(local_48);
                    /* try { // try from 0067e000 to 0067e004 has its CatchHandler @ 0067e114 */
  local_58 = (QArrayData *)QString::fromAscii_helper("lssatin",7);
                    /* try { // try from 0067e015 to 0067e019 has its CatchHandler @ 0067e108 */
  KoID::KoID((KoID *)local_48,(QString *)&local_58,(QString *)&local_50);
                    /* try { // try from 0067e020 to 0067e024 has its CatchHandler @ 0067e0f0 */
  KisLayerStyleFilter::KisLayerStyleFilter((KisLayerStyleFilter *)this,(KoID *)local_48);
  if (local_40 == (int *)0x0) {
LAB_0067e042:
    iVar2 = *(int *)local_58;
    if (iVar2 == 0) goto LAB_0067e0c0;
LAB_0067e04c:
    if (iVar2 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_0067e0c0;
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
    if (*local_40 != 0) goto LAB_0067e042;
    operator_delete(local_40,0x10);
    iVar2 = *(int *)local_58;
    if (iVar2 != 0) goto LAB_0067e04c;
LAB_0067e0c0:
    QArrayData::deallocate(local_58,2,8);
    iVar2 = *(int *)local_50;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_0067e076;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_0067e076;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_0067e076:
  *(undefined **)this = PTR_vtable_00837510 + 0x10;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisLsSatinFilter @ 0067e120 ======

/* KisLsSatinFilter::KisLsSatinFilter(KisLsSatinFilter const&) */

void __thiscall KisLsSatinFilter::KisLsSatinFilter(KisLsSatinFilter *this,KisLsSatinFilter *param_1)

{
  KisLayerStyleFilter::KisLayerStyleFilter
            ((KisLayerStyleFilter *)this,(KisLayerStyleFilter *)param_1);
  *(undefined **)this = PTR_vtable_00837510 + 0x10;
  return;
}



