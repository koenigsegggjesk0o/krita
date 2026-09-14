/* Class KisPaintOpPreset - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintOpPreset @ 00206e90 ======

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this)

{
  (*(code *)PTR_KisPaintOpPreset_0083b218)();
  return;
}



// ====== KisPaintOpPreset @ 0020b8a0 ======

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this,KisPaintOpPreset *param_1)

{
  (*(code *)PTR_KisPaintOpPreset_0083d720)();
  return;
}



// ====== KisPaintOpPreset @ 00338210 ======

/* KisPaintOpPreset::KisPaintOpPreset() */

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this)

{
  undefined *puVar1;
  undefined (*pauVar2) [16];
  undefined8 *puVar3;
  undefined4 *puVar4;
  undefined8 uVar5;
  long in_FS_OFFSET;
  QArrayData *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 0033823e to 00338242 has its CatchHandler @ 00338329 */
  KoResource::KoResource((KoResource *)this,(QString *)&local_28);
  if (*(int *)local_28 != 0) {
    if (*(int *)local_28 == -1) goto LAB_00338264;
    LOCK();
    *(int *)local_28 = *(int *)local_28 + -1;
    UNLOCK();
    if (*(int *)local_28 != 0) goto LAB_00338264;
  }
  QArrayData::deallocate(local_28,2,8);
LAB_00338264:
  *(undefined **)this = PTR_vtable_00837f38 + 0x10;
                    /* try { // try from 00338278 to 0033827c has its CatchHandler @ 0033834d */
  pauVar2 = (undefined (*) [16])operator_new(0x30);
  *pauVar2 = (undefined  [16])0x0;
                    /* try { // try from 0033828c to 003382b1 has its CatchHandler @ 00338341 */
  puVar3 = (undefined8 *)operator_new(0x10);
  *puVar3 = &PTR_FUN_0081fc38;
  puVar3[1] = this;
  *(undefined8 **)pauVar2[1] = puVar3;
  puVar4 = (undefined4 *)operator_new(0x18);
  *(undefined8 **)(puVar4 + 4) = puVar3;
  *(code **)(puVar4 + 2) = FUN_00340790;
  puVar4[1] = 1;
  *puVar4 = 1;
  *(undefined4 **)(pauVar2[1] + 8) = puVar4;
                    /* try { // try from 003382de to 003382e2 has its CatchHandler @ 00338335 */
  uVar5 = QString::fromAscii_helper("5.0",3);
  *(undefined8 *)pauVar2[2] = uVar5;
  puVar1 = PTR_shared_null_00837830;
  *(undefined (**) [16])(this + 0x10) = pauVar2;
  *(undefined **)(pauVar2[2] + 8) = puVar1;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintOpPreset @ 0033c620 ======

/* KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset const&) */

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this,KisPaintOpPreset *param_1)

{
  QArrayData *pQVar1;
  undefined *puVar2;
  char cVar3;
  char cVar4;
  undefined (*pauVar5) [16];
  undefined8 *puVar6;
  undefined4 *puVar7;
  undefined8 uVar8;
  long in_FS_OFFSET;
  QArrayData *local_58 [5];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KoResource::KoResource((KoResource *)this,(KoResource *)param_1);
  *(undefined **)this = PTR_vtable_00837f38 + 0x10;
                    /* try { // try from 0033c65c to 0033c660 has its CatchHandler @ 0033c840 */
  pauVar5 = (undefined (*) [16])operator_new(0x30);
  *pauVar5 = (undefined  [16])0x0;
                    /* try { // try from 0033c670 to 0033c695 has its CatchHandler @ 0033c87c */
  puVar6 = (undefined8 *)operator_new(0x10);
  *puVar6 = &PTR_FUN_0081fc38;
  puVar6[1] = this;
  *(undefined8 **)pauVar5[1] = puVar6;
  puVar7 = (undefined4 *)operator_new(0x18);
  *(undefined8 **)(puVar7 + 4) = puVar6;
  *(code **)(puVar7 + 2) = FUN_00340790;
  puVar7[1] = 1;
  *puVar7 = 1;
  *(undefined4 **)(pauVar5[1] + 8) = puVar7;
                    /* try { // try from 0033c6c3 to 0033c6c7 has its CatchHandler @ 0033c888 */
  uVar8 = QString::fromAscii_helper("5.0",3);
  *(undefined8 *)pauVar5[2] = uVar8;
  puVar2 = PTR_shared_null_00837830;
  *(undefined (**) [16])(this + 0x10) = pauVar5;
  *(undefined **)(pauVar5[2] + 8) = puVar2;
                    /* try { // try from 0033c6e6 to 0033c709 has its CatchHandler @ 0033c840 */
  settings();
  if (local_58[0] != (QArrayData *)0x0) {
    LOCK();
    pQVar1 = local_58[0] + 8;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_58[0] + 8))();
    }
    settings();
                    /* try { // try from 0033c710 to 0033c714 has its CatchHandler @ 0033c864 */
    setSettings(this,(KisPinnedSharedPtr)local_58);
    if (local_58[0] != (QArrayData *)0x0) {
      LOCK();
      pQVar1 = local_58[0] + 8;
      *(int *)pQVar1 = *(int *)pQVar1 + -1;
      UNLOCK();
      if (*(int *)pQVar1 == 0) {
        (**(code **)(*(long *)local_58[0] + 8))();
      }
    }
  }
                    /* try { // try from 0033c72c to 0033c763 has its CatchHandler @ 0033c840 */
  cVar3 = KoResource::isDirty();
  cVar4 = KoResource::isDirty();
  if (cVar3 != cVar4) {
    kis_safe_assert_recoverable
              ("isDirty() == rhs.isDirty()",
               "/builds/graphics/krita/libs/image/brushengine/kis_paintop_preset.cpp",0x6a);
  }
  settings();
                    /* try { // try from 0033c772 to 0033c776 has its CatchHandler @ 0033c84c */
  KoResource::setValid(SUB81(this,0));
  if (local_58[0] != (QArrayData *)0x0) {
    LOCK();
    pQVar1 = local_58[0] + 8;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_58[0] + 8))();
    }
  }
                    /* try { // try from 0033c795 to 0033c797 has its CatchHandler @ 0033c840 */
  (**(code **)(*(long *)param_1 + 0x40))(local_58,param_1);
                    /* try { // try from 0033c79e to 0033c7a2 has its CatchHandler @ 0033c858 */
  KoResource::setName((QString *)this);
  if (*(int *)local_58[0] != 0) {
    if (*(int *)local_58[0] == -1) goto LAB_0033c7bc;
    LOCK();
    *(int *)local_58[0] = *(int *)local_58[0] + -1;
    UNLOCK();
    if (*(int *)local_58[0] != 0) goto LAB_0033c7bc;
  }
  QArrayData::deallocate(local_58[0],2,8);
LAB_0033c7bc:
                    /* try { // try from 0033c7c2 to 0033c7c6 has its CatchHandler @ 0033c840 */
  KoResource::image();
                    /* try { // try from 0033c7cd to 0033c7d1 has its CatchHandler @ 0033c870 */
  KoResource::setImage((QImage *)this);
  QImage::~QImage((QImage *)local_58);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintOpPreset @ 0033cfa0 ======

/* KisPaintOpPreset::KisPaintOpPreset(QString const&) */

void __thiscall KisPaintOpPreset::KisPaintOpPreset(KisPaintOpPreset *this,QString *param_1)

{
  undefined *puVar1;
  int iVar2;
  undefined (*pauVar3) [16];
  undefined8 *puVar4;
  undefined4 *puVar5;
  undefined8 uVar6;
  long in_FS_OFFSET;
  QArrayData *local_48;
  QArrayData *local_40;
  QArrayData *local_38;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KoResource::KoResource((KoResource *)this,(QString *)param_1);
  *(undefined **)this = PTR_vtable_00837f38 + 0x10;
                    /* try { // try from 0033cfdc to 0033cfe0 has its CatchHandler @ 0033d1a2 */
  pauVar3 = (undefined (*) [16])operator_new(0x30);
  *pauVar3 = (undefined  [16])0x0;
                    /* try { // try from 0033cff0 to 0033d015 has its CatchHandler @ 0033d1ba */
  puVar4 = (undefined8 *)operator_new(0x10);
  *puVar4 = &PTR_FUN_0081fc38;
  puVar4[1] = this;
  *(undefined8 **)pauVar3[1] = puVar4;
  puVar5 = (undefined4 *)operator_new(0x18);
  *(undefined8 **)(puVar5 + 4) = puVar4;
  *(code **)(puVar5 + 2) = FUN_00340790;
  puVar5[1] = 1;
  *puVar5 = 1;
  *(undefined4 **)(pauVar3[1] + 8) = puVar5;
                    /* try { // try from 0033d042 to 0033d046 has its CatchHandler @ 0033d1ae */
  uVar6 = QString::fromAscii_helper("5.0",3);
  *(undefined8 *)pauVar3[2] = uVar6;
  puVar1 = PTR_shared_null_00837830;
  *(undefined (**) [16])(this + 0x10) = pauVar3;
  *(undefined **)(pauVar3[2] + 8) = puVar1;
                    /* try { // try from 0033d063 to 0033d067 has its CatchHandler @ 0033d1a2 */
  name((KisPaintOpPreset *)&local_48);
                    /* try { // try from 0033d074 to 0033d078 has its CatchHandler @ 0033d18a */
  local_38 = (QArrayData *)QString::fromAscii_helper(" ",1);
                    /* try { // try from 0033d08a to 0033d08e has its CatchHandler @ 0033d179 */
  local_40 = (QArrayData *)QString::fromAscii_helper("_",1);
                    /* try { // try from 0033d0ac to 0033d0bb has its CatchHandler @ 0033d196 */
  QString::replace((QString *)&local_48,(QString *)&local_40,(CaseSensitivity)&local_38);
  KoResource::setName((QString *)this);
  if (*(int *)local_40 == 0) {
LAB_0033d130:
    QArrayData::deallocate(local_40,2,8);
    iVar2 = *(int *)local_38;
    if (iVar2 != 0) goto LAB_0033d0e2;
LAB_0033d14a:
    QArrayData::deallocate(local_38,2,8);
    iVar2 = *(int *)local_48;
  }
  else {
    if (*(int *)local_40 != -1) {
      LOCK();
      *(int *)local_40 = *(int *)local_40 + -1;
      UNLOCK();
      if (*(int *)local_40 == 0) goto LAB_0033d130;
    }
    iVar2 = *(int *)local_38;
    if (iVar2 == 0) goto LAB_0033d14a;
LAB_0033d0e2:
    if (iVar2 != -1) {
      LOCK();
      *(int *)local_38 = *(int *)local_38 + -1;
      UNLOCK();
      if (*(int *)local_38 == 0) goto LAB_0033d14a;
    }
    iVar2 = *(int *)local_48;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_0033d10b;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0033d10b;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0033d10b:
  if (local_30 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



