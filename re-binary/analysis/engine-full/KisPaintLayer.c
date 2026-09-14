/* Class KisPaintLayer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintLayer @ 002041e0 ======

void __thiscall KisPaintLayer::KisPaintLayer(KisPaintLayer *this,KisPaintLayer *param_1)

{
  (*(code *)PTR_KisPaintLayer_00839bc0)();
  return;
}



// ====== KisPaintLayer @ 002049b0 ======

void __thiscall
KisPaintLayer::KisPaintLayer
          (KisPaintLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3)

{
  (*(code *)PTR_KisPaintLayer_00839fa8)();
  return;
}



// ====== KisPaintLayer @ 00208780 ======

void __thiscall
KisPaintLayer::KisPaintLayer
          (KisPaintLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3,
          KisSharedPtr param_4)

{
  (*(code *)PTR_KisPaintLayer_0083be90)();
  return;
}



// ====== KisPaintLayer @ 0020a4d0 ======

void __thiscall
KisPaintLayer::KisPaintLayer
          (KisPaintLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3,
          KoColorSpace *param_4)

{
  (*(code *)PTR_KisPaintLayer_0083cd38)();
  return;
}



// ====== KisPaintLayer @ 005da3e0 ======

/* KisPaintLayer::KisPaintLayer(KisPaintLayer const&) */

void __thiscall KisPaintLayer::KisPaintLayer(KisPaintLayer *this,KisPaintLayer *param_1)

{
  long *plVar1;
  long lVar2;
  long *plVar3;
  KisRasterKeyframeChannel *this_00;
  undefined *puVar4;
  bool bVar5;
  undefined8 *puVar6;
  KisPaintDevice *pKVar7;
  undefined8 uVar8;
  KisPaintDevice *pKVar9;
  
  KisLayer::KisLayer((KisLayer *)this,(KisLayer *)param_1);
                    /* try { // try from 005da400 to 005da404 has its CatchHandler @ 005da5f0 */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x38));
  puVar4 = PTR_vtable_00837a48;
  *(undefined **)this = PTR_vtable_00837a48 + 0x10;
  *(undefined **)(this + 0x38) = puVar4 + 0x280;
  *(undefined **)(this + 0x48) = puVar4 + 0x2c0;
                    /* try { // try from 005da42d to 005da431 has its CatchHandler @ 005da5fc */
  puVar6 = (undefined8 *)operator_new(0x38);
  *puVar6 = this;
  puVar6[1] = 0;
  puVar4 = PTR_shared_null_008377d0;
  puVar6[3] = 0;
  puVar6[2] = puVar4;
  puVar6[4] = puVar4;
                    /* try { // try from 005da45b to 005da45f has its CatchHandler @ 005da5e4 */
  FUN_00667180(puVar6 + 5);
  lVar2 = *(long *)(param_1 + 0x50);
  *(undefined *)(puVar6 + 6) = 1;
  *(undefined8 **)(this + 0x50) = puVar6;
  if (*(long *)(lVar2 + 0x18) != 0) {
                    /* try { // try from 005da47d to 005da481 has its CatchHandler @ 005da5fc */
    pKVar7 = (KisPaintDevice *)operator_new(0x28);
                    /* try { // try from 005da499 to 005da49d has its CatchHandler @ 005da5d8 */
    KisPaintDevice::KisPaintDevice
              (pKVar7,*(KisPaintDevice **)(*(long *)(param_1 + 0x50) + 8),1,(KisNode *)this);
    lVar2 = *(long *)(this + 0x50);
    pKVar9 = *(KisPaintDevice **)(lVar2 + 8);
    if (pKVar7 != pKVar9) {
      LOCK();
      *(int *)(pKVar7 + 0x10) = *(int *)(pKVar7 + 0x10) + 1;
      UNLOCK();
      plVar3 = *(long **)(lVar2 + 8);
      *(KisPaintDevice **)(lVar2 + 8) = pKVar7;
      if (plVar3 != (long *)0x0) {
        LOCK();
        plVar1 = plVar3 + 2;
        *(int *)plVar1 = *(int *)plVar1 + -1;
        UNLOCK();
        if (*(int *)plVar1 == 0) {
          (**(code **)(*plVar3 + 0x20))();
        }
      }
      pKVar9 = *(KisPaintDevice **)(*(long *)(this + 0x50) + 8);
    }
                    /* try { // try from 005da4d5 to 005da549 has its CatchHandler @ 005da5fc */
    KisPaintDevice::setSupportsWraparoundMode(pKVar9,true);
    QByteArray::operator=
              ((QByteArray *)(*(long *)(this + 0x50) + 0x10),
               (QByteArray *)(*(long *)(param_1 + 0x50) + 0x10));
    lVar2 = *(long *)(this + 0x50);
    uVar8 = KisPaintDevice::keyframeChannel(*(KisPaintDevice **)(lVar2 + 8));
    *(undefined8 *)(lVar2 + 0x18) = uVar8;
    KisNode::addKeyframeChannel
              ((KisNode *)this,*(KisKeyframeChannel **)(*(long *)(this + 0x50) + 0x18));
    this_00 = *(KisRasterKeyframeChannel **)(*(long *)(this + 0x50) + 0x18);
    bVar5 = (bool)onionSkinEnabled(param_1);
    KisRasterKeyframeChannel::setOnionSkinsEnabled(this_00,bVar5);
    KisBaseNode::enableAnimation((KisBaseNode *)this);
    return;
  }
  pKVar7 = (KisPaintDevice *)operator_new(0x28);
                    /* try { // try from 005da55e to 005da562 has its CatchHandler @ 005da608 */
  KisPaintDevice::KisPaintDevice
            (pKVar7,*(KisPaintDevice **)(*(long *)(param_1 + 0x50) + 8),0,(KisNode *)this);
  lVar2 = *(long *)(this + 0x50);
  pKVar9 = *(KisPaintDevice **)(lVar2 + 8);
  if (pKVar7 != pKVar9) {
    LOCK();
    *(int *)(pKVar7 + 0x10) = *(int *)(pKVar7 + 0x10) + 1;
    UNLOCK();
    plVar3 = *(long **)(lVar2 + 8);
    *(KisPaintDevice **)(lVar2 + 8) = pKVar7;
    if (plVar3 != (long *)0x0) {
      LOCK();
      plVar1 = plVar3 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar3 + 0x20))();
      }
    }
    pKVar9 = *(KisPaintDevice **)(*(long *)(this + 0x50) + 8);
  }
                    /* try { // try from 005da596 to 005da59a has its CatchHandler @ 005da5fc */
  KisPaintDevice::setSupportsWraparoundMode(pKVar9,true);
  QByteArray::operator=
            ((QByteArray *)(*(long *)(this + 0x50) + 0x10),
             (QByteArray *)(*(long *)(param_1 + 0x50) + 0x10));
  return;
}



// ====== KisPaintLayer @ 005da950 ======

/* KisPaintLayer::KisPaintLayer(KisWeakSharedPtr<KisImage>, QString const&, unsigned char,
   KisSharedPtr<KisPaintDevice>) */

void __thiscall
KisPaintLayer::KisPaintLayer
          (KisPaintLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3,
          KisSharedPtr param_4)

{
  int iVar1;
  KisPaintDevice *pKVar2;
  undefined auVar3 [8];
  undefined auVar4 [16];
  undefined auVar5 [16];
  undefined *puVar6;
  undefined8 *puVar7;
  KisDefaultBounds *pKVar8;
  int *piVar9;
  KisWeakSharedPtr KVar10;
  undefined4 in_register_00000034;
  long *plVar11;
  undefined4 in_register_00000084;
  KisPaintDevice *this_00;
  long in_FS_OFFSET;
  KisDefaultBounds *local_60;
  undefined local_58 [8];
  int *piStack_50;
  long local_40;
  
  plVar11 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar11 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar3 = local_58;
LAB_005dac79:
    local_58 = auVar3;
    piStack_50 = (int *)0x0;
  }
  else if (((uint *)plVar11[1] == (uint *)0x0) || ((*(uint *)plVar11[1] & 1) == 0)) {
    local_58 = (undefined  [8])0x0;
    piStack_50 = (int *)0x0;
  }
  else {
    auVar3 = (undefined  [8])*plVar11;
    piStack_50 = (int *)local_58;
    local_58 = auVar3;
    if (auVar3 == (undefined  [8])0x0) goto LAB_005dac79;
    piVar9 = *(int **)((long)auVar3 + 0x58);
    if (piVar9 == (int *)0x0) {
      piVar9 = (int *)operator_new(4);
      *piVar9 = 0;
      *(int **)((long)auVar3 + 0x58) = piVar9;
      LOCK();
      *piVar9 = *piVar9 + 1;
      UNLOCK();
      piVar9 = *(int **)((long)auVar3 + 0x58);
      auVar3 = local_58;
    }
    local_58 = auVar3;
    LOCK();
    *piVar9 = *piVar9 + 2;
    UNLOCK();
    piStack_50 = piVar9;
  }
  KVar10 = (KisWeakSharedPtr)local_58;
                    /* try { // try from 005da9af to 005da9b3 has its CatchHandler @ 005dad38 */
  KisLayer::KisLayer((KisLayer *)this,KVar10,param_2,param_3);
  piVar9 = piStack_50;
  auVar4._8_8_ = 0;
  auVar4._0_8_ = piStack_50;
  _local_58 = auVar4 << 0x40;
  if (piVar9 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar9;
    *piVar9 = *piVar9 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar9 != (int *)0x0)) {
      operator_delete(piVar9,4);
    }
  }
                    /* try { // try from 005da9e0 to 005da9e4 has its CatchHandler @ 005dad68 */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x38));
  puVar6 = PTR_vtable_00837a48;
  *(undefined **)this = PTR_vtable_00837a48 + 0x10;
  *(undefined **)(this + 0x38) = puVar6 + 0x280;
  *(undefined **)(this + 0x48) = puVar6 + 0x2c0;
                    /* try { // try from 005daa0d to 005daa11 has its CatchHandler @ 005dad50 */
  puVar7 = (undefined8 *)operator_new(0x38);
  *puVar7 = this;
  puVar7[1] = 0;
  puVar6 = PTR_shared_null_008377d0;
  puVar7[3] = 0;
  puVar7[2] = puVar6;
  puVar7[4] = puVar6;
                    /* try { // try from 005daa3b to 005daa3f has its CatchHandler @ 005dad8c */
  FUN_00667180(puVar7 + 5);
  pKVar2 = *(KisPaintDevice **)CONCAT44(in_register_00000084,param_4);
  this_00 = (KisPaintDevice *)puVar7[1];
  *(undefined *)(puVar7 + 6) = 1;
  *(undefined8 **)(this + 0x50) = puVar7;
  if (pKVar2 != this_00) {
    if (pKVar2 != (KisPaintDevice *)0x0) {
      LOCK();
      *(int *)(pKVar2 + 0x10) = *(int *)(pKVar2 + 0x10) + 1;
      UNLOCK();
      this_00 = (KisPaintDevice *)puVar7[1];
    }
    puVar7[1] = pKVar2;
    if (this_00 != (KisPaintDevice *)0x0) {
      LOCK();
      pKVar2 = this_00 + 0x10;
      *(int *)pKVar2 = *(int *)pKVar2 + -1;
      UNLOCK();
      if (*(int *)pKVar2 == 0) {
        (**(code **)(*(long *)this_00 + 0x20))(this_00);
      }
    }
    this_00 = *(KisPaintDevice **)(*(long *)(this + 0x50) + 8);
  }
                    /* try { // try from 005daa85 to 005daa89 has its CatchHandler @ 005dad50 */
  pKVar8 = (KisDefaultBounds *)operator_new(0x20);
  if (*plVar11 == 0) {
    local_58 = (undefined  [8])0x0;
    auVar3 = local_58;
  }
  else {
    if (((uint *)plVar11[1] == (uint *)0x0) || ((*(uint *)plVar11[1] & 1) == 0)) {
      local_58 = (undefined  [8])0x0;
      piStack_50 = (int *)0x0;
      goto LAB_005daab5;
    }
    auVar3 = (undefined  [8])*plVar11;
    local_58 = auVar3;
    if (auVar3 != (undefined  [8])0x0) {
      piVar9 = *(int **)((long)auVar3 + 0x58);
      if (piVar9 == (int *)0x0) {
                    /* try { // try from 005dad15 to 005dad19 has its CatchHandler @ 005dad74 */
        piVar9 = (int *)operator_new(4);
        *piVar9 = 0;
        *(int **)((long)auVar3 + 0x58) = piVar9;
        LOCK();
        *piVar9 = *piVar9 + 1;
        UNLOCK();
        piVar9 = *(int **)((long)auVar3 + 0x58);
        auVar3 = local_58;
      }
      local_58 = auVar3;
      LOCK();
      *piVar9 = *piVar9 + 2;
      UNLOCK();
      piStack_50 = piVar9;
      goto LAB_005daab5;
    }
  }
  local_58 = auVar3;
  piStack_50 = (int *)0x0;
LAB_005daab5:
                    /* try { // try from 005daabb to 005daabf has its CatchHandler @ 005dad5c */
  KisDefaultBounds::KisDefaultBounds(pKVar8,KVar10);
  LOCK();
  *(int *)(pKVar8 + 8) = *(int *)(pKVar8 + 8) + 1;
  UNLOCK();
  local_60 = pKVar8;
                    /* try { // try from 005daad2 to 005daad6 has its CatchHandler @ 005dad80 */
  KisPaintDevice::setDefaultBounds(this_00,(KisSharedPtr)&local_60);
  if (local_60 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar8 = local_60 + 8;
    *(int *)pKVar8 = *(int *)pKVar8 + -1;
    UNLOCK();
    if (*(int *)pKVar8 == 0) {
      (**(code **)(*(long *)local_60 + 8))();
    }
  }
  local_58 = (undefined  [8])0x0;
  auVar3 = local_58;
  local_58 = (undefined  [8])0x0;
  if (piStack_50 != (int *)0x0) {
    LOCK();
    iVar1 = *piStack_50;
    *piStack_50 = *piStack_50 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piStack_50 != (int *)0x0)) {
      local_58 = auVar3;
      operator_delete(piStack_50,4);
    }
  }
                    /* try { // try from 005dab1e to 005dab22 has its CatchHandler @ 005dad50 */
  KisPaintDevice::setSupportsWraparoundMode(*(KisPaintDevice **)(*(long *)(this + 0x50) + 8),true);
  pKVar2 = *(KisPaintDevice **)(*(long *)(this + 0x50) + 8);
  piVar9 = *(int **)(this + 0x18);
  local_58 = (undefined  [8])this;
  auVar3 = (undefined  [8])this;
  if (piVar9 == (int *)0x0) {
                    /* try { // try from 005dacb5 to 005dacb9 has its CatchHandler @ 005dad50 */
    piVar9 = (int *)operator_new(4);
    *piVar9 = 0;
    *(int **)(this + 0x18) = piVar9;
    LOCK();
    *piVar9 = *piVar9 + 1;
    UNLOCK();
    piVar9 = *(int **)(this + 0x18);
    auVar3 = local_58;
  }
  local_58 = auVar3;
  LOCK();
  *piVar9 = *piVar9 + 2;
  UNLOCK();
  piStack_50 = piVar9;
                    /* try { // try from 005dab4c to 005dab50 has its CatchHandler @ 005dad44 */
  KisPaintDevice::setParentNode(pKVar2,KVar10);
  piVar9 = piStack_50;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = piStack_50;
  _local_58 = auVar5 << 0x40;
  if (piVar9 != (int *)0x0) {
    LOCK();
    iVar1 = *piVar9;
    *piVar9 = *piVar9 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piVar9 != (int *)0x0)) {
      operator_delete(piVar9,4);
    }
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintLayer @ 005dada0 ======

/* KisPaintLayer::KisPaintLayer(KisWeakSharedPtr<KisImage>, QString const&, unsigned char) */

void __thiscall
KisPaintLayer::KisPaintLayer
          (KisPaintLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3)

{
  long *plVar1;
  int iVar2;
  long *plVar3;
  KisPaintLayer *pKVar4;
  ulong uVar5;
  undefined auVar6 [16];
  undefined *puVar7;
  undefined8 uVar8;
  undefined8 *puVar9;
  KisPaintDevice *this_00;
  KisDefaultBounds *pKVar10;
  long lVar11;
  KoColorSpace *pKVar12;
  int *piVar13;
  undefined4 in_register_00000034;
  ulong *puVar14;
  long in_FS_OFFSET;
  KisDefaultBounds *local_98;
  QArrayData *local_90;
  QArrayData *local_88;
  QTextStream *local_80;
  undefined local_78 [16];
  KisPaintLayer *local_68;
  int *piStack_60;
  undefined8 uStack_58;
  undefined8 local_50;
  long local_40;
  
  local_78._8_8_ = local_78._0_8_;
  puVar14 = (ulong *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*puVar14 == 0) {
    local_68 = (KisPaintLayer *)0x0;
LAB_005db149:
    piStack_60 = (int *)0x0;
  }
  else if (((uint *)puVar14[1] == (uint *)0x0) || ((*(uint *)puVar14[1] & 1) == 0)) {
    local_68 = (KisPaintLayer *)0x0;
    piStack_60 = (int *)0x0;
  }
  else {
    pKVar4 = (KisPaintLayer *)*puVar14;
    local_68 = pKVar4;
    if (pKVar4 == (KisPaintLayer *)0x0) goto LAB_005db149;
    piStack_60 = *(int **)(pKVar4 + 0x58);
    if (piStack_60 == (int *)0x0) {
      piVar13 = (int *)operator_new(4);
      *piVar13 = 0;
      *(int **)(pKVar4 + 0x58) = piVar13;
      LOCK();
      *piVar13 = *piVar13 + 1;
      UNLOCK();
      piStack_60 = *(int **)(pKVar4 + 0x58);
    }
    LOCK();
    *piStack_60 = *piStack_60 + 2;
    UNLOCK();
  }
                    /* try { // try from 005dadff to 005dae03 has its CatchHandler @ 005db358 */
  KisLayer::KisLayer((KisLayer *)this,(KisWeakSharedPtr)&local_68,param_2,param_3);
  local_68 = (KisPaintLayer *)0x0;
  if (piStack_60 != (int *)0x0) {
    LOCK();
    iVar2 = *piStack_60;
    *piStack_60 = *piStack_60 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piStack_60 != (int *)0x0)) {
      operator_delete(piStack_60,4);
    }
  }
                    /* try { // try from 005dae35 to 005dae39 has its CatchHandler @ 005db340 */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x38));
  puVar7 = PTR_vtable_00837a48;
  *(undefined **)this = PTR_vtable_00837a48 + 0x10;
  *(undefined **)(this + 0x38) = puVar7 + 0x280;
  *(undefined **)(this + 0x48) = puVar7 + 0x2c0;
                    /* try { // try from 005dae62 to 005dae66 has its CatchHandler @ 005db2d4 */
  puVar9 = (undefined8 *)operator_new(0x38);
  puVar7 = PTR_shared_null_008377d0;
  *puVar9 = this;
  puVar9[1] = 0;
  puVar9[2] = puVar7;
  puVar9[3] = 0;
  puVar9[4] = puVar7;
                    /* try { // try from 005dae90 to 005dae94 has its CatchHandler @ 005db34c */
  FUN_00667180(puVar9 + 5);
  *(undefined *)(puVar9 + 6) = 1;
  *(undefined8 **)(this + 0x50) = puVar9;
                    /* try { // try from 005daea4 to 005daea8 has its CatchHandler @ 005db2d4 */
  this_00 = (KisPaintDevice *)operator_new(0x28);
  local_90 = (QArrayData *)puVar7;
                    /* try { // try from 005daeb6 to 005daeba has its CatchHandler @ 005db304 */
  pKVar10 = (KisDefaultBounds *)operator_new(0x20);
  if (*puVar14 == 0) {
    local_78._0_8_ = 0;
    uVar5 = local_78._0_8_;
LAB_005db131:
    local_78._0_8_ = uVar5;
    local_78._8_8_ = 0;
  }
  else if (((uint *)puVar14[1] == (uint *)0x0) || ((*(uint *)puVar14[1] & 1) == 0)) {
    local_78 = (undefined  [16])0x0;
  }
  else {
    uVar5 = *puVar14;
    local_78._0_8_ = uVar5;
    if (uVar5 == 0) goto LAB_005db131;
    piVar13 = *(int **)(uVar5 + 0x58);
    if (piVar13 == (int *)0x0) {
                    /* try { // try from 005db29d to 005db2a1 has its CatchHandler @ 005db2ec */
      piVar13 = (int *)operator_new(4);
      *piVar13 = 0;
      *(int **)(uVar5 + 0x58) = piVar13;
      LOCK();
      *piVar13 = *piVar13 + 1;
      UNLOCK();
      piVar13 = *(int **)(uVar5 + 0x58);
      uVar5 = local_78._0_8_;
    }
    local_78._0_8_ = uVar5;
    local_78._8_8_ = piVar13;
    LOCK();
    *piVar13 = *piVar13 + 2;
    UNLOCK();
  }
                    /* try { // try from 005daef0 to 005daef4 has its CatchHandler @ 005db334 */
  KisDefaultBounds::KisDefaultBounds(pKVar10,(KisWeakSharedPtr)local_78);
  LOCK();
  *(int *)(pKVar10 + 8) = *(int *)(pKVar10 + 8) + 1;
  UNLOCK();
  local_98 = pKVar10;
                    /* try { // try from 005daf16 to 005daf2f has its CatchHandler @ 005db323 */
  if ((((*puVar14 == 0) || ((uint *)puVar14[1] == (uint *)0x0)) || ((*(uint *)puVar14[1] & 1) == 0))
     && (lVar11 = _41000(), *(char *)(lVar11 + 0x11) != '\0')) {
    lVar11 = _41000();
    local_50 = *(undefined8 *)(lVar11 + 8);
    local_68 = (KisPaintLayer *)0x2;
    piStack_60 = (int *)0x0;
    uStack_58 = 0;
    QMessageLogger::warning();
    if (1 < *(int *)(local_80 + 0x28)) {
      *(uint *)(local_80 + 0x48) = *(uint *)(local_80 + 0x48) | 1;
    }
                    /* try { // try from 005db1f9 to 005db1fd has its CatchHandler @ 005db2f8 */
    kisBacktrace();
                    /* try { // try from 005db20f to 005db213 has its CatchHandler @ 005db2e0 */
    QDebug::putString((QChar *)&local_80,(ulong)(local_88 + *(long *)(local_88 + 0x10)));
    if (local_80[0x20] != (QTextStream)0x0) {
                    /* try { // try from 005db2c5 to 005db2c9 has its CatchHandler @ 005db2e0 */
      QTextStream::operator<<(local_80,' ');
    }
    if (*(int *)local_88 == 0) {
LAB_005db240:
      QArrayData::deallocate(local_88,2,8);
    }
    else if (*(int *)local_88 != -1) {
      LOCK();
      *(int *)local_88 = *(int *)local_88 + -1;
      UNLOCK();
      if (*(int *)local_88 == 0) goto LAB_005db240;
    }
    QDebug::~QDebug((QDebug *)&local_80);
  }
  pKVar12 = (KoColorSpace *)KisImage::colorSpace((KisImage *)*puVar14);
  piVar13 = *(int **)(this + 0x18);
  local_68 = this;
  if (piVar13 == (int *)0x0) {
                    /* try { // try from 005db185 to 005db1dc has its CatchHandler @ 005db323 */
    piVar13 = (int *)operator_new(4);
    *piVar13 = 0;
    *(int **)(this + 0x18) = piVar13;
    LOCK();
    *piVar13 = *piVar13 + 1;
    UNLOCK();
    piVar13 = *(int **)(this + 0x18);
  }
  LOCK();
  *piVar13 = *piVar13 + 2;
  UNLOCK();
  piStack_60 = piVar13;
                    /* try { // try from 005daf68 to 005daf6c has its CatchHandler @ 005db315 */
  KisPaintDevice::KisPaintDevice
            (this_00,(KisWeakSharedPtr)&local_68,pKVar12,(KisSharedPtr)&local_98,
             (QString *)&local_90);
  lVar11 = *(long *)(this + 0x50);
  if (this_00 != *(KisPaintDevice **)(lVar11 + 8)) {
    LOCK();
    *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
    UNLOCK();
    plVar3 = *(long **)(lVar11 + 8);
    *(KisPaintDevice **)(lVar11 + 8) = this_00;
    if (plVar3 != (long *)0x0) {
      LOCK();
      plVar1 = plVar3 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar3 + 0x20))();
      }
    }
  }
  local_68 = (KisPaintLayer *)0x0;
  if (piStack_60 != (int *)0x0) {
    LOCK();
    iVar2 = *piStack_60;
    *piStack_60 = *piStack_60 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piStack_60 != (int *)0x0)) {
      operator_delete(piStack_60,4);
    }
  }
  if (local_98 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar10 = local_98 + 8;
    *(int *)pKVar10 = *(int *)pKVar10 + -1;
    UNLOCK();
    if (*(int *)pKVar10 == 0) {
      (**(code **)(*(long *)local_98 + 8))();
    }
  }
  uVar8 = local_78._8_8_;
  auVar6._8_8_ = 0;
  auVar6._0_8_ = local_78._8_8_;
  local_78 = auVar6 << 0x40;
  if ((int *)uVar8 == (int *)0x0) {
LAB_005daff1:
    iVar2 = *(int *)local_90;
  }
  else {
    LOCK();
    iVar2 = *(int *)uVar8;
    *(int *)uVar8 = *(int *)uVar8 + -2;
    UNLOCK();
    if ((2 < iVar2) || ((int *)uVar8 == (int *)0x0)) goto LAB_005daff1;
    operator_delete((void *)uVar8,4);
    iVar2 = *(int *)local_90;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_005db00c;
    LOCK();
    *(int *)local_90 = *(int *)local_90 + -1;
    UNLOCK();
    if (*(int *)local_90 != 0) goto LAB_005db00c;
  }
  QArrayData::deallocate(local_90,2,8);
LAB_005db00c:
                    /* try { // try from 005db019 to 005db01d has its CatchHandler @ 005db2d4 */
  KisPaintDevice::setSupportsWraparoundMode(*(KisPaintDevice **)(*(long *)(this + 0x50) + 8),true);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPaintLayer @ 005db370 ======

/* KisPaintLayer::KisPaintLayer(KisWeakSharedPtr<KisImage>, QString const&, unsigned char,
   KoColorSpace const*) */

void __thiscall
KisPaintLayer::KisPaintLayer
          (KisPaintLayer *this,KisWeakSharedPtr param_1,QString *param_2,uchar param_3,
          KoColorSpace *param_4)

{
  long *plVar1;
  int iVar2;
  undefined *puVar3;
  undefined8 *puVar4;
  KisPaintDevice *this_00;
  KisDefaultBounds *pKVar5;
  long lVar6;
  int *piVar7;
  undefined4 in_register_00000034;
  long *plVar8;
  long in_FS_OFFSET;
  KisDefaultBounds *local_88;
  QArrayData *local_80;
  KisPaintLayer *local_78;
  int *local_70;
  long local_68;
  int *piStack_60;
  undefined8 uStack_58;
  undefined8 local_50;
  long local_40;
  
  plVar8 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar8 == 0) {
    local_68 = 0;
LAB_005db7d9:
    piStack_60 = (int *)0x0;
  }
  else if (((uint *)plVar8[1] == (uint *)0x0) || ((*(uint *)plVar8[1] & 1) == 0)) {
    local_68 = 0;
    piStack_60 = (int *)0x0;
  }
  else {
    lVar6 = *plVar8;
    local_68 = lVar6;
    if (lVar6 == 0) goto LAB_005db7d9;
    piStack_60 = *(int **)(lVar6 + 0x58);
    if (piStack_60 == (int *)0x0) {
      piVar7 = (int *)operator_new(4);
      *piVar7 = 0;
      *(int **)(lVar6 + 0x58) = piVar7;
      LOCK();
      *piVar7 = *piVar7 + 1;
      UNLOCK();
      piStack_60 = *(int **)(lVar6 + 0x58);
    }
    LOCK();
    *piStack_60 = *piStack_60 + 2;
    UNLOCK();
  }
                    /* try { // try from 005db3cf to 005db3d3 has its CatchHandler @ 005db92c */
  KisLayer::KisLayer((KisLayer *)this,(KisWeakSharedPtr)&local_68,param_2,param_3);
  local_68 = 0;
  if (piStack_60 != (int *)0x0) {
    LOCK();
    iVar2 = *piStack_60;
    *piStack_60 = *piStack_60 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piStack_60 != (int *)0x0)) {
      operator_delete(piStack_60,4);
    }
  }
                    /* try { // try from 005db405 to 005db409 has its CatchHandler @ 005db920 */
  KisIndirectPaintingSupport::KisIndirectPaintingSupport
            ((KisIndirectPaintingSupport *)(this + 0x38));
  puVar3 = PTR_vtable_00837a48;
  *(undefined **)this = PTR_vtable_00837a48 + 0x10;
  *(undefined **)(this + 0x38) = puVar3 + 0x280;
  *(undefined **)(this + 0x48) = puVar3 + 0x2c0;
                    /* try { // try from 005db432 to 005db436 has its CatchHandler @ 005db950 */
  puVar4 = (undefined8 *)operator_new(0x38);
  puVar3 = PTR_shared_null_008377d0;
  *puVar4 = this;
  puVar4[1] = 0;
  puVar4[2] = puVar3;
  puVar4[3] = 0;
  puVar4[4] = puVar3;
                    /* try { // try from 005db460 to 005db464 has its CatchHandler @ 005db944 */
  FUN_00667180(puVar4 + 5);
  *(undefined *)(puVar4 + 6) = 1;
  *(undefined8 **)(this + 0x50) = puVar4;
  if (param_4 == (KoColorSpace *)0x0) {
    if ((((*plVar8 == 0) || ((uint *)plVar8[1] == (uint *)0x0)) || ((*(uint *)plVar8[1] & 1) == 0))
       && (lVar6 = _41000(), *(char *)(lVar6 + 0x11) != '\0')) {
      lVar6 = _41000();
      local_50 = *(undefined8 *)(lVar6 + 8);
      local_68 = 2;
      piStack_60 = (int *)0x0;
      uStack_58 = 0;
      QMessageLogger::warning();
      if (1 < *(int *)(local_78 + 0x28)) {
        *(uint *)(local_78 + 0x48) = *(uint *)(local_78 + 0x48) | 1;
      }
                    /* try { // try from 005db74e to 005db752 has its CatchHandler @ 005db8f2 */
      kisBacktrace();
                    /* try { // try from 005db764 to 005db768 has its CatchHandler @ 005db8e6 */
      QDebug::putString((QChar *)&local_78,(ulong)(local_80 + *(long *)(local_80 + 0x10)));
      if (*(QTextStream *)(local_78 + 0x20) != (QTextStream)0x0) {
                    /* try { // try from 005db8b5 to 005db8b9 has its CatchHandler @ 005db8e6 */
        QTextStream::operator<<((QTextStream *)local_78,' ');
      }
      if (*(int *)local_80 == 0) {
LAB_005db898:
        QArrayData::deallocate(local_80,2,8);
      }
      else if (*(int *)local_80 != -1) {
        LOCK();
        *(int *)local_80 = *(int *)local_80 + -1;
        UNLOCK();
        if (*(int *)local_80 == 0) goto LAB_005db898;
      }
      QDebug::~QDebug((QDebug *)&local_78);
    }
    param_4 = (KoColorSpace *)KisImage::colorSpace((KisImage *)*plVar8);
  }
                    /* try { // try from 005db47d to 005db481 has its CatchHandler @ 005db950 */
  this_00 = (KisPaintDevice *)operator_new(0x28);
  local_80 = (QArrayData *)puVar3;
                    /* try { // try from 005db48f to 005db493 has its CatchHandler @ 005db90a */
  pKVar5 = (KisDefaultBounds *)operator_new(0x20);
  if (*plVar8 == 0) {
    local_68 = 0;
LAB_005db7b9:
    piStack_60 = (int *)0x0;
  }
  else if (((uint *)plVar8[1] == (uint *)0x0) || ((*(uint *)plVar8[1] & 1) == 0)) {
    local_68 = 0;
    piStack_60 = (int *)0x0;
  }
  else {
    lVar6 = *plVar8;
    local_68 = lVar6;
    if (lVar6 == 0) goto LAB_005db7b9;
    piVar7 = *(int **)(lVar6 + 0x58);
    if (piVar7 == (int *)0x0) {
                    /* try { // try from 005db875 to 005db879 has its CatchHandler @ 005db8c4 */
      piVar7 = (int *)operator_new(4);
      *piVar7 = 0;
      *(int **)(lVar6 + 0x58) = piVar7;
      LOCK();
      *piVar7 = *piVar7 + 1;
      UNLOCK();
      piVar7 = *(int **)(lVar6 + 0x58);
    }
    LOCK();
    *piVar7 = *piVar7 + 2;
    UNLOCK();
    piStack_60 = piVar7;
  }
                    /* try { // try from 005db4c4 to 005db4c8 has its CatchHandler @ 005db938 */
  KisDefaultBounds::KisDefaultBounds(pKVar5,(KisWeakSharedPtr)&local_68);
  LOCK();
  *(int *)(pKVar5 + 8) = *(int *)(pKVar5 + 8) + 1;
  UNLOCK();
  local_70 = *(int **)(this + 0x18);
  local_88 = pKVar5;
  local_78 = this;
  if (local_70 == (int *)0x0) {
                    /* try { // try from 005db815 to 005db819 has its CatchHandler @ 005db8d0 */
    piVar7 = (int *)operator_new(4);
    *piVar7 = 0;
    *(int **)(this + 0x18) = piVar7;
    LOCK();
    *piVar7 = *piVar7 + 1;
    UNLOCK();
    local_70 = *(int **)(this + 0x18);
  }
  LOCK();
  *local_70 = *local_70 + 2;
  UNLOCK();
                    /* try { // try from 005db511 to 005db515 has its CatchHandler @ 005db8fe */
  KisPaintDevice::KisPaintDevice
            (this_00,(KisWeakSharedPtr)&local_78,param_4,(KisSharedPtr)&local_88,
             (QString *)&local_80);
  lVar6 = *(long *)(this + 0x50);
  if (this_00 != *(KisPaintDevice **)(lVar6 + 8)) {
    LOCK();
    *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
    UNLOCK();
    plVar8 = *(long **)(lVar6 + 8);
    *(KisPaintDevice **)(lVar6 + 8) = this_00;
    if (plVar8 != (long *)0x0) {
      LOCK();
      plVar1 = plVar8 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar8 + 0x20))();
      }
    }
  }
  local_78 = (KisPaintLayer *)0x0;
  if (local_70 != (int *)0x0) {
    LOCK();
    iVar2 = *local_70;
    *local_70 = *local_70 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (local_70 != (int *)0x0)) {
      operator_delete(local_70,4);
    }
  }
  if (local_88 != (KisDefaultBounds *)0x0) {
    LOCK();
    pKVar5 = local_88 + 8;
    *(int *)pKVar5 = *(int *)pKVar5 + -1;
    UNLOCK();
    if (*(int *)pKVar5 == 0) {
      (**(code **)(*(long *)local_88 + 8))();
    }
  }
  local_68 = 0;
  if (piStack_60 == (int *)0x0) {
LAB_005db59a:
    iVar2 = *(int *)local_80;
  }
  else {
    LOCK();
    iVar2 = *piStack_60;
    *piStack_60 = *piStack_60 + -2;
    UNLOCK();
    if ((2 < iVar2) || (piStack_60 == (int *)0x0)) goto LAB_005db59a;
    operator_delete(piStack_60,4);
    iVar2 = *(int *)local_80;
  }
  if (iVar2 != 0) {
    if (iVar2 == -1) goto LAB_005db5b5;
    LOCK();
    *(int *)local_80 = *(int *)local_80 + -1;
    UNLOCK();
    if (*(int *)local_80 != 0) goto LAB_005db5b5;
  }
  QArrayData::deallocate(local_80,2,8);
LAB_005db5b5:
                    /* try { // try from 005db5c2 to 005db731 has its CatchHandler @ 005db950 */
  KisPaintDevice::setSupportsWraparoundMode(*(KisPaintDevice **)(*(long *)(this + 0x50) + 8),true);
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



