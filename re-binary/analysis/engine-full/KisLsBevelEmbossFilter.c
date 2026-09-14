/* Class KisLsBevelEmbossFilter - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLsBevelEmbossFilter @ 00207950 ======

void __thiscall
KisLsBevelEmbossFilter::KisLsBevelEmbossFilter
          (KisLsBevelEmbossFilter *this,KisLsBevelEmbossFilter *param_1)

{
  (*(code *)PTR_KisLsBevelEmbossFilter_0083b778)();
  return;
}



// ====== KisLsBevelEmbossFilter @ 0020c0e0 ======

void __thiscall KisLsBevelEmbossFilter::KisLsBevelEmbossFilter(KisLsBevelEmbossFilter *this)

{
  (*(code *)PTR_KisLsBevelEmbossFilter_0083db40)();
  return;
}



// ====== KisLsBevelEmbossFilter @ 00683020 ======

/* KisLsBevelEmbossFilter::KisLsBevelEmbossFilter() */

void __thiscall KisLsBevelEmbossFilter::KisLsBevelEmbossFilter(KisLsBevelEmbossFilter *this)

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
                    /* try { // try from 00683067 to 0068306b has its CatchHandler @ 0068317c */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString(local_48);
                    /* try { // try from 00683080 to 00683084 has its CatchHandler @ 00683194 */
  local_58 = (QArrayData *)QString::fromAscii_helper("lsstroke",8);
                    /* try { // try from 00683095 to 00683099 has its CatchHandler @ 00683188 */
  KoID::KoID((KoID *)local_48,(QString *)&local_58,(QString *)&local_50);
                    /* try { // try from 006830a0 to 006830a4 has its CatchHandler @ 00683170 */
  KisLayerStyleFilter::KisLayerStyleFilter((KisLayerStyleFilter *)this,(KoID *)local_48);
  if (local_40 == (int *)0x0) {
LAB_006830c2:
    iVar2 = *(int *)local_58;
    if (iVar2 == 0) goto LAB_00683140;
LAB_006830cc:
    if (iVar2 != -1) {
      LOCK();
      *(int *)local_58 = *(int *)local_58 + -1;
      UNLOCK();
      if (*(int *)local_58 == 0) goto LAB_00683140;
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
    if (*local_40 != 0) goto LAB_006830c2;
    operator_delete(local_40,0x10);
    iVar2 = *(int *)local_58;
    if (iVar2 != 0) goto LAB_006830cc;
LAB_00683140:
    QArrayData::deallocate(local_58,2,8);
    iVar2 = *(int *)local_50;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_006830f6;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_006830f6;
  }
  QArrayData::deallocate(local_50,2,8);
LAB_006830f6:
  *(undefined **)this = PTR_vtable_00837f58 + 0x10;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisLsBevelEmbossFilter @ 006831a0 ======

/* KisLsBevelEmbossFilter::KisLsBevelEmbossFilter(KisLsBevelEmbossFilter const&) */

void __thiscall
KisLsBevelEmbossFilter::KisLsBevelEmbossFilter
          (KisLsBevelEmbossFilter *this,KisLsBevelEmbossFilter *param_1)

{
  KisLayerStyleFilter::KisLayerStyleFilter
            ((KisLayerStyleFilter *)this,(KisLayerStyleFilter *)param_1);
  *(undefined **)this = PTR_vtable_00837f58 + 0x10;
  return;
}



