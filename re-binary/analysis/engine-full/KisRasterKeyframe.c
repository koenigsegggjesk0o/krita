/* Class KisRasterKeyframe - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRasterKeyframe @ 002026e0 ======

void __thiscall
KisRasterKeyframe::KisRasterKeyframe
          (KisRasterKeyframe *this,KisWeakSharedPtr param_1,int *param_2,int *param_3)

{
  (*(code *)PTR_KisRasterKeyframe_00838e40)();
  return;
}



// ====== KisRasterKeyframe @ 00208c20 ======

void __thiscall
KisRasterKeyframe::KisRasterKeyframe(KisRasterKeyframe *this,KisWeakSharedPtr param_1)

{
  (*(code *)PTR_KisRasterKeyframe_0083c0e0)();
  return;
}



// ====== KisRasterKeyframe @ 006599c0 ======

/* KisRasterKeyframe::KisRasterKeyframe(KisWeakSharedPtr<KisPaintDevice>) */

void __thiscall
KisRasterKeyframe::KisRasterKeyframe(KisRasterKeyframe *this,KisWeakSharedPtr param_1)

{
  uint *puVar1;
  undefined *puVar2;
  bool bVar3;
  undefined4 uVar4;
  long lVar5;
  int *piVar6;
  undefined4 in_register_00000034;
  long *plVar7;
  long in_FS_OFFSET;
  QTextStream *local_68;
  long local_60;
  undefined8 local_58;
  undefined local_50 [16];
  undefined8 local_40;
  long local_30;
  
  plVar7 = (long *)CONCAT44(in_register_00000034,param_1);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisKeyframe::KisKeyframe((KisKeyframe *)this);
  puVar2 = PTR_vtable_008378c0;
  *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  lVar5 = *plVar7;
  *(undefined **)this = puVar2 + 0x10;
  if (lVar5 == 0) {
    *(undefined8 *)(this + 0x20) = 0;
LAB_00659b38:
    *(undefined8 *)(this + 0x28) = 0;
  }
  else if (((uint *)plVar7[1] == (uint *)0x0) || ((*(uint *)plVar7[1] & 1) == 0)) {
    *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  }
  else {
    lVar5 = *plVar7;
    *(long *)(this + 0x20) = lVar5;
    if (lVar5 == 0) goto LAB_00659b38;
    piVar6 = *(int **)(lVar5 + 0x18);
    if (piVar6 == (int *)0x0) {
                    /* try { // try from 00659bd5 to 00659bd9 has its CatchHandler @ 00659c09 */
      piVar6 = (int *)operator_new(4);
      *piVar6 = 0;
      *(int **)(lVar5 + 0x18) = piVar6;
      LOCK();
      *piVar6 = *piVar6 + 1;
      UNLOCK();
      piVar6 = *(int **)(lVar5 + 0x18);
    }
    *(int **)(this + 0x28) = piVar6;
    LOCK();
    *piVar6 = *piVar6 + 2;
    UNLOCK();
    if ((((*(long *)(this + 0x20) != 0) && (*(uint **)(this + 0x28) != (uint *)0x0)) &&
        ((**(uint **)(this + 0x28) & 1) != 0)) && (lVar5 = *(long *)(this + 0x20), lVar5 != 0)) {
      puVar1 = *(uint **)(this + 0x28);
      goto joined_r0x00659b0b;
    }
  }
  kis_assert_exception
            ("m_paintDevice","/builds/graphics/krita/libs/image/kis_raster_keyframe_channel.cpp",
             0x18);
  puVar1 = *(uint **)(this + 0x28);
  lVar5 = *(long *)(this + 0x20);
joined_r0x00659b0b:
                    /* try { // try from 00659a80 to 00659b77 has its CatchHandler @ 00659c09 */
  if ((((puVar1 == (uint *)0x0) || (lVar5 == 0)) || ((*puVar1 & 1) == 0)) &&
     (lVar5 = _41000(), *(char *)(lVar5 + 0x11) != '\0')) {
    lVar5 = _41000();
    local_40 = *(undefined8 *)(lVar5 + 8);
    local_50 = (undefined  [16])0x0;
    local_58 = 2;
    QMessageLogger::warning();
    if (1 < *(int *)(local_68 + 0x28)) {
      *(uint *)(local_68 + 0x48) = *(uint *)(local_68 + 0x48) | 1;
    }
                    /* try { // try from 00659b8e to 00659b92 has its CatchHandler @ 00659c21 */
    kisBacktrace();
                    /* try { // try from 00659ba2 to 00659ba6 has its CatchHandler @ 00659c15 */
    QDebug::putString((QChar *)&local_68,local_60 + *(long *)(local_60 + 0x10));
    if (local_68[0x20] != (QTextStream)0x0) {
                    /* try { // try from 00659bfd to 00659c01 has its CatchHandler @ 00659c15 */
      QTextStream::operator<<(local_68,' ');
    }
    FUN_002dd9a0(&local_60);
    QDebug::~QDebug((QDebug *)&local_68);
  }
  bVar3 = (bool)KisPaintDevice::framesInterface(*(KisPaintDevice **)(this + 0x20));
  local_58 = 0;
  uVar4 = KisPaintDeviceFramesInterface::createFrame
                    (bVar3,0,(QPoint *)0x0,(KUndo2Command *)&local_58);
  *(undefined4 *)(this + 0x18) = uVar4;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisRasterKeyframe @ 00659c30 ======

/* KisRasterKeyframe::KisRasterKeyframe(KisWeakSharedPtr<KisPaintDevice>, int const&, int const&) */

void __thiscall
KisRasterKeyframe::KisRasterKeyframe
          (KisRasterKeyframe *this,KisWeakSharedPtr param_1,int *param_2,int *param_3)

{
  int iVar1;
  long lVar2;
  undefined *puVar3;
  int *piVar4;
  undefined4 in_register_00000034;
  long *plVar5;
  
  plVar5 = (long *)CONCAT44(in_register_00000034,param_1);
  KisKeyframe::KisKeyframe((KisKeyframe *)this);
  puVar3 = PTR_vtable_008378c0;
  *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  lVar2 = *plVar5;
  *(undefined **)this = puVar3 + 0x10;
  if (lVar2 == 0) {
    *(undefined8 *)(this + 0x20) = 0;
  }
  else {
    if (((uint *)plVar5[1] == (uint *)0x0) || ((*(uint *)plVar5[1] & 1) == 0)) {
      *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
      goto LAB_00659c87;
    }
    lVar2 = *plVar5;
    *(long *)(this + 0x20) = lVar2;
    if (lVar2 != 0) {
      piVar4 = *(int **)(lVar2 + 0x18);
      if (piVar4 == (int *)0x0) {
        piVar4 = (int *)operator_new(4);
        *piVar4 = 0;
        *(int **)(lVar2 + 0x18) = piVar4;
        LOCK();
        *piVar4 = *piVar4 + 1;
        UNLOCK();
        piVar4 = *(int **)(lVar2 + 0x18);
      }
      *(int **)(this + 0x28) = piVar4;
      LOCK();
      *piVar4 = *piVar4 + 2;
      UNLOCK();
      goto LAB_00659c87;
    }
  }
  *(undefined8 *)(this + 0x28) = 0;
LAB_00659c87:
  iVar1 = *param_3;
  *(int *)(this + 0x18) = *param_2;
                    /* try { // try from 00659c95 to 00659d39 has its CatchHandler @ 00659d4e */
  KisKeyframe::setColorLabel((KisKeyframe *)this,iVar1);
  if ((((*(long *)(this + 0x20) != 0) && (*(uint **)(this + 0x28) != (uint *)0x0)) &&
      ((**(uint **)(this + 0x28) & 1) != 0)) && (*(long *)(this + 0x20) != 0)) {
    return;
  }
  kis_assert_exception
            ("m_paintDevice","/builds/graphics/krita/libs/image/kis_raster_keyframe_channel.cpp",
             0x24);
  return;
}



