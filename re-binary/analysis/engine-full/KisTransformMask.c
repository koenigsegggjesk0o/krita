/* Class KisTransformMask - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTransformMask @ 0020c3d0 ======

void __thiscall KisTransformMask::KisTransformMask(KisTransformMask *this,KisTransformMask *param_1)

{
  (*(code *)PTR_KisTransformMask_0083dcb8)();
  return;
}



// ====== KisTransformMask @ 004c9e50 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTransformMask::KisTransformMask(KisWeakSharedPtr<KisImage>, QString const&) */

void __thiscall
KisTransformMask::KisTransformMask(KisTransformMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  KisDefaultBounds *pKVar1;
  int iVar2;
  undefined auVar3 [8];
  undefined auVar4 [16];
  undefined auVar5 [16];
  undefined8 uVar6;
  undefined *puVar7;
  KisPerspectiveTransformWorker *this_00;
  KisDefaultBounds *pKVar8;
  int *piVar9;
  KisWeakSharedPtr KVar10;
  undefined4 in_register_00000034;
  long *plVar11;
  long *plVar12;
  KisThreadSafeSignalCompressor *this_01;
  long in_FS_OFFSET;
  undefined8 uVar13;
  long *local_b0;
  undefined local_a8 [16];
  undefined local_98 [8];
  int *piStack_90;
  long local_40;
  
  plVar11 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar11 == 0) {
    local_98 = (undefined  [8])0x0;
    auVar3 = local_98;
  }
  else {
    if (((uint *)plVar11[1] == (uint *)0x0) || ((*(uint *)plVar11[1] & 1) == 0)) {
      local_98 = (undefined  [8])0x0;
      piStack_90 = (int *)0x0;
      goto LAB_004c9ea2;
    }
    auVar3 = (undefined  [8])*plVar11;
    piStack_90 = (int *)local_98;
    local_98 = auVar3;
    if (auVar3 != (undefined  [8])0x0) {
      piVar9 = *(int **)((long)auVar3 + 0x58);
      if (piVar9 == (int *)0x0) {
        piVar9 = (int *)operator_new(4);
        *piVar9 = 0;
        *(int **)((long)auVar3 + 0x58) = piVar9;
        LOCK();
        *piVar9 = *piVar9 + 1;
        UNLOCK();
        piVar9 = *(int **)((long)auVar3 + 0x58);
        auVar3 = local_98;
      }
      local_98 = auVar3;
      LOCK();
      *piVar9 = *piVar9 + 2;
      UNLOCK();
      piStack_90 = piVar9;
      goto LAB_004c9ea2;
    }
  }
  local_98 = auVar3;
  piStack_90 = (int *)0x0;
LAB_004c9ea2:
  KVar10 = (KisWeakSharedPtr)(QTransform *)local_98;
                    /* try { // try from 004c9ead to 004c9eb1 has its CatchHandler @ 004ca42e */
  KisEffectMask::KisEffectMask((KisEffectMask *)this,KVar10,param_2);
  piVar9 = piStack_90;
  auVar4._8_8_ = 0;
  auVar4._0_8_ = piStack_90;
  _local_98 = auVar4 << 0x40;
  if (piVar9 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar9;
    *piVar9 = *piVar9 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar9 != (int *)0x0)) {
      operator_delete(piVar9,4);
    }
  }
  puVar7 = PTR_vtable_00836bd8;
  *(undefined **)this = PTR_vtable_00836bd8 + 0x10;
  *(undefined **)(this + 0x30) = puVar7 + 0x248;
  *(undefined **)(this + 0x48) = puVar7 + 0x288;
                    /* try { // try from 004c9f00 to 004c9f04 has its CatchHandler @ 004ca446 */
  this_00 = (KisPerspectiveTransformWorker *)operator_new(400);
  plVar12 = (long *)*plVar11;
  if (((plVar12 == (long *)0x0) || ((uint *)plVar11[1] == (uint *)0x0)) ||
     ((*(uint *)plVar11[1] & 1) == 0)) {
    plVar12 = (long *)0x0;
  }
  else {
    LOCK();
    *(int *)(plVar12 + 10) = *(int *)(plVar12 + 10) + 1;
    UNLOCK();
  }
  local_a8 = (undefined  [16])0x0;
                    /* try { // try from 004c9f32 to 004c9f36 has its CatchHandler @ 004ca452 */
  QTransform::QTransform((QTransform *)local_98);
  local_b0 = (long *)0x0;
                    /* try { // try from 004c9f58 to 004c9f5c has its CatchHandler @ 004ca3da */
  KisPerspectiveTransformWorker::KisPerspectiveTransformWorker
            (this_00,(KisSharedPtr)&local_b0,(QTransform *)local_98,true,(QPointer)local_a8);
  if (local_b0 != (long *)0x0) {
    LOCK();
    plVar11 = local_b0 + 2;
    *(int *)plVar11 = *(int *)plVar11 + -1;
    UNLOCK();
    if (*(int *)plVar11 == 0) {
      (**(code **)(*local_b0 + 0x20))();
    }
  }
  if ((int *)local_a8._0_8_ != (int *)0x0) {
    LOCK();
    *(int *)local_a8._0_8_ = *(int *)local_a8._0_8_ + -1;
    UNLOCK();
    if ((*(int *)local_a8._0_8_ == 0) && ((int *)local_a8._0_8_ != (int *)0x0)) {
      operator_delete((void *)local_a8._0_8_,0x10);
    }
  }
                    /* try { // try from 004c9f86 to 004c9f99 has its CatchHandler @ 004ca3fe */
  KisTransformMaskParamsFactoryRegistry::instance();
  pKVar8 = (KisDefaultBounds *)operator_new(0x20);
  local_98 = (undefined  [8])plVar12;
  if (plVar12 == (long *)0x0) {
    piStack_90 = (int *)0x0;
  }
  else {
    piVar9 = (int *)plVar12[0xb];
    auVar3 = (undefined  [8])plVar12;
    if (piVar9 == (int *)0x0) {
                    /* try { // try from 004ca335 to 004ca339 has its CatchHandler @ 004ca46f */
      piVar9 = (int *)operator_new(4);
      *piVar9 = 0;
      plVar12[0xb] = (long)piVar9;
      LOCK();
      *piVar9 = *piVar9 + 1;
      UNLOCK();
      piVar9 = (int *)plVar12[0xb];
      auVar3 = local_98;
    }
    local_98 = auVar3;
    LOCK();
    *piVar9 = *piVar9 + 2;
    UNLOCK();
    piStack_90 = piVar9;
  }
                    /* try { // try from 004c9fc7 to 004c9fcb has its CatchHandler @ 004ca3f2 */
  KisDefaultBounds::KisDefaultBounds(pKVar8,KVar10);
  local_a8._0_8_ = pKVar8;
  LOCK();
  *(int *)(pKVar8 + 8) = *(int *)(pKVar8 + 8) + 1;
  UNLOCK();
                    /* try { // try from 004c9fe6 to 004c9fea has its CatchHandler @ 004ca40a */
  KisTransformMaskParamsFactoryRegistry::createAnimatedParamsHolder((int)this_00 + 0xf8);
  if ((long *)local_a8._0_8_ != (long *)0x0) {
    LOCK();
    plVar11 = (long *)(local_a8._0_8_ + 8);
    *(int *)plVar11 = *(int *)plVar11 + -1;
    UNLOCK();
    if (*(int *)plVar11 == 0) {
      (**(code **)(*(long *)local_a8._0_8_ + 8))();
    }
  }
  local_98 = (undefined  [8])0x0;
  auVar3 = local_98;
  local_98 = (undefined  [8])0x0;
  if (piStack_90 != (int *)0x0) {
    LOCK();
    iVar2 = *piStack_90;
    *piStack_90 = *piStack_90 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piStack_90 != (int *)0x0)) {
      local_98 = auVar3;
      operator_delete(piStack_90,4);
    }
  }
                    /* try { // try from 004ca031 to 004ca035 has its CatchHandler @ 004ca3e6 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(this_00 + 0x108),0);
  *(undefined8 *)(this_00 + 0x128) = 0;
  *(undefined (*) [16])(this_00 + 0x118) = (undefined  [16])0x0;
  uVar6 = DAT_00721778;
  uVar13 = _DAT_00721770;
  *(undefined2 *)(this_00 + 0x110) = 0;
  this_00[0x130] = (KisPerspectiveTransformWorker)0x0;
  *(undefined8 *)(this_00 + 0x138) = 0;
  *(undefined8 *)(this_00 + 0x140) = uVar13;
  *(undefined8 *)(this_00 + 0x148) = uVar6;
                    /* try { // try from 004ca07b to 004ca07f has its CatchHandler @ 004ca3ce */
  pKVar8 = (KisDefaultBounds *)operator_new(0x20);
  local_98 = (undefined  [8])plVar12;
  if (plVar12 == (long *)0x0) {
    piStack_90 = (int *)0x0;
  }
  else {
    piVar9 = (int *)plVar12[0xb];
    auVar3 = (undefined  [8])plVar12;
    if (piVar9 == (int *)0x0) {
                    /* try { // try from 004ca35d to 004ca361 has its CatchHandler @ 004ca416 */
      piVar9 = (int *)operator_new(4);
      *piVar9 = 0;
      plVar12[0xb] = (long)piVar9;
      LOCK();
      *piVar9 = *piVar9 + 1;
      UNLOCK();
      piVar9 = (int *)plVar12[0xb];
      auVar3 = local_98;
    }
    local_98 = auVar3;
    LOCK();
    *piVar9 = *piVar9 + 2;
    UNLOCK();
    piStack_90 = piVar9;
  }
                    /* try { // try from 004ca0ad to 004ca0b1 has its CatchHandler @ 004ca3c0 */
  KisDefaultBounds::KisDefaultBounds(pKVar8,KVar10);
  pKVar1 = pKVar8 + 8;
  LOCK();
  *(int *)(pKVar8 + 8) = *(int *)(pKVar8 + 8) + 1;
  UNLOCK();
  *(KisDefaultBounds **)(this_00 + 0x150) = pKVar8;
  LOCK();
  *(int *)(pKVar8 + 8) = *(int *)(pKVar8 + 8) + 1;
  UNLOCK();
  *(undefined (*) [16])(this_00 + 0x158) = (undefined  [16])0x0;
  LOCK();
  *(int *)pKVar1 = *(int *)pKVar1 + -1;
  UNLOCK();
  if (*(int *)pKVar1 == 0) {
    (**(code **)(*(long *)pKVar8 + 8))(pKVar8);
  }
  piVar9 = piStack_90;
  auVar5._8_8_ = 0;
  auVar5._0_8_ = piStack_90;
  _local_98 = auVar5 << 0x40;
  if (piVar9 != (int *)0x0) {
    LOCK();
    iVar2 = *piVar9;
    *piVar9 = *piVar9 + -2;
    UNLOCK();
    if ((iVar2 < 3) && (piVar9 != (int *)0x0)) {
      operator_delete(piVar9,4);
    }
  }
  this_01 = (KisThreadSafeSignalCompressor *)(this_00 + 0x168);
                    /* try { // try from 004ca114 to 004ca118 has its CatchHandler @ 004ca43a */
  KisThreadSafeSignalCompressor::KisThreadSafeSignalCompressor(this_01,3000,0);
  uVar13 = DAT_007227c8;
  *(KisPerspectiveTransformWorker **)(this + 0x50) = this_00;
  *(undefined8 *)(this_00 + 0x188) = 0;
  *(undefined8 *)(this_00 + 0x180) = uVar13;
  if (plVar12 != (long *)0x0) {
    LOCK();
    plVar11 = plVar12 + 10;
    *(int *)plVar11 = *(int *)plVar11 + -1;
    UNLOCK();
    if (*(int *)plVar11 == 0) {
      (**(code **)(*plVar12 + 0x20))(plVar12);
    }
    this_01 = (KisThreadSafeSignalCompressor *)(*(long *)(this + 0x50) + 0x168);
  }
                    /* try { // try from 004ca16c to 004ca1ac has its CatchHandler @ 004ca422 */
  QObject::connect((QObject *)local_98,(char *)this_01,(QObject *)"2timeout()",(char *)this,0x7268d1
                  );
  QMetaObject::Connection::~Connection((Connection *)local_98);
  QObject::connect((QObject *)local_98,(char *)this,
                   (QObject *)"2sigInternalForceStaticImageUpdate()",(char *)this,0x72f828);
  QMetaObject::Connection::~Connection((Connection *)local_98);
  KisImageConfig::KisImageConfig((KisImageConfig *)local_98,true);
                    /* try { // try from 004ca1b0 to 004ca1b4 has its CatchHandler @ 004ca463 */
  uVar13 = KisImageConfig::transformMaskOffBoundsReadArea((KisImageConfig *)local_98);
  *(undefined8 *)(*(long *)(this + 0x50) + 0x180) = uVar13;
  KisImageConfig::~KisImageConfig((KisImageConfig *)local_98);
                    /* try { // try from 004ca1ce to 004ca1d2 has its CatchHandler @ 004ca422 */
  KisBaseNode::setSupportsLodMoves((KisBaseNode *)this,false);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisTransformMask @ 004ca480 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTransformMask::KisTransformMask(KisTransformMask const&) */

void __thiscall KisTransformMask::KisTransformMask(KisTransformMask *this,KisTransformMask *param_1)

{
  undefined8 *puVar1;
  undefined uVar2;
  uint uVar3;
  long *plVar4;
  QArrayData *pQVar5;
  long lVar6;
  undefined8 uVar7;
  undefined *puVar8;
  long *plVar9;
  int *piVar10;
  QArrayData *pQVar11;
  KisKeyframeChannel *pKVar12;
  QArrayData *pQVar13;
  int iVar14;
  undefined8 *puVar15;
  long *plVar16;
  QArrayData *pQVar17;
  QArrayData *pQVar18;
  undefined8 *puVar19;
  long lVar20;
  long lVar21;
  QArrayData *pQVar22;
  long in_FS_OFFSET;
  QArrayData *local_98;
  QArrayData *local_90;
  QArrayData *local_88;
  undefined4 local_80;
  long local_78 [8];
  long local_38 [2];
  
  local_38[1] = *(long *)(in_FS_OFFSET + 0x28);
  KisEffectMask::KisEffectMask((KisEffectMask *)this,(KisEffectMask *)param_1);
  puVar8 = PTR_vtable_00836bd8;
  *(undefined **)this = PTR_vtable_00836bd8 + 0x10;
  *(undefined **)(this + 0x30) = puVar8 + 0x248;
  *(undefined **)(this + 0x48) = puVar8 + 0x288;
                    /* try { // try from 004ca4d4 to 004ca4d8 has its CatchHandler @ 004cad18 */
  plVar9 = (long *)operator_new(400);
  plVar16 = *(long **)(param_1 + 0x50);
  lVar21 = *plVar16;
  *plVar9 = lVar21;
  if (lVar21 != 0) {
    LOCK();
    *(int *)(lVar21 + 0x10) = *(int *)(lVar21 + 0x10) + 1;
    UNLOCK();
  }
  lVar21 = plVar16[2];
  piVar10 = (int *)plVar16[1];
  plVar9[1] = plVar16[1];
  plVar9[2] = lVar21;
  if (piVar10 != (int *)0x0) {
    LOCK();
    *piVar10 = *piVar10 + 1;
    UNLOCK();
  }
  piVar10 = (int *)plVar16[3];
  if (*piVar10 == 0) {
    if (*(char *)((long)piVar10 + 0xb) < '\0') {
      lVar21 = QArrayData::allocate(0x10,8,(ulong)(piVar10[2] & 0x7fffffff),0);
      plVar9[3] = lVar21;
      if (lVar21 == 0) {
        qBadAlloc();
        lVar21 = plVar9[3];
      }
      *(byte *)(lVar21 + 0xb) = *(byte *)(lVar21 + 0xb) | 0x80;
    }
    else {
      lVar21 = QArrayData::allocate(0x10,8,(long)piVar10[1],0);
      plVar9[3] = lVar21;
      if (lVar21 == 0) {
                    /* try { // try from 0022ad38 to 0022ad4a has its CatchHandler @ 0022adcd */
        qBadAlloc();
        lVar21 = plVar9[3];
      }
    }
    if ((*(uint *)(lVar21 + 8) & 0x7fffffff) != 0) {
      lVar20 = plVar16[3];
      iVar14 = *(int *)(lVar20 + 4);
      puVar15 = (undefined8 *)(*(long *)(lVar20 + 0x10) + lVar20);
      puVar19 = puVar15 + (long)iVar14 * 2;
      lVar20 = (*(long *)(lVar21 + 0x10) + lVar21) - (long)puVar15;
      for (; puVar15 != puVar19; puVar15 = puVar15 + 2) {
        puVar1 = (undefined8 *)((long)puVar15 + lVar20);
        uVar7 = puVar15[1];
        *puVar1 = *puVar15;
        puVar1[1] = uVar7;
      }
      *(int *)(lVar21 + 4) = iVar14;
    }
  }
  else {
    if (*piVar10 != -1) {
      LOCK();
      *piVar10 = *piVar10 + 1;
      UNLOCK();
      piVar10 = (int *)plVar16[3];
    }
    plVar9[3] = (long)piVar10;
  }
  lVar21 = plVar16[5];
  lVar20 = plVar16[6];
  lVar6 = plVar16[7];
  plVar9[4] = plVar16[4];
  plVar9[5] = lVar21;
  plVar9[6] = lVar20;
  plVar9[7] = lVar6;
  lVar21 = plVar16[9];
  plVar9[8] = plVar16[8];
  plVar9[9] = lVar21;
  lVar21 = plVar16[0xb];
  plVar9[10] = plVar16[10];
  plVar9[0xb] = lVar21;
  lVar21 = plVar16[0xd];
  plVar9[0xc] = plVar16[0xc];
  plVar9[0xd] = lVar21;
  lVar21 = plVar16[0xf];
  plVar9[0xe] = plVar16[0xe];
  plVar9[0xf] = lVar21;
  lVar21 = plVar16[0x11];
  plVar9[0x10] = plVar16[0x10];
  plVar9[0x11] = lVar21;
  plVar9[0x12] = plVar16[0x12];
  lVar21 = plVar16[0x14];
  plVar9[0x13] = plVar16[0x13];
  plVar9[0x14] = lVar21;
  lVar21 = plVar16[0x16];
  plVar9[0x15] = plVar16[0x15];
  plVar9[0x16] = lVar21;
  lVar21 = plVar16[0x18];
  plVar9[0x17] = plVar16[0x17];
  plVar9[0x18] = lVar21;
  lVar21 = plVar16[0x1a];
  plVar9[0x19] = plVar16[0x19];
  plVar9[0x1a] = lVar21;
  lVar21 = plVar16[0x1c];
  plVar9[0x1b] = plVar16[0x1b];
  plVar9[0x1c] = lVar21;
  plVar9[0x1d] = plVar16[0x1d];
  plVar4 = (long *)plVar16[0x1f];
  *(undefined4 *)(plVar9 + 0x1e) = *(undefined4 *)(plVar16 + 0x1e);
                    /* try { // try from 004ca60d to 004ca60f has its CatchHandler @ 004cacc4 */
  (**(code **)(*plVar4 + 0x38))(plVar9 + 0x1f);
                    /* try { // try from 004ca61c to 004ca620 has its CatchHandler @ 004cad24 */
  QReadWriteLock::QReadWriteLock((QReadWriteLock *)(plVar9 + 0x21),0);
  plVar9[0x25] = 0;
  *(undefined (*) [16])(plVar9 + 0x23) = (undefined  [16])0x0;
  lVar6 = DAT_00721778;
  lVar20 = _DAT_00721770;
  *(undefined2 *)(plVar9 + 0x22) = 0;
  uVar2 = *(undefined *)(plVar16 + 0x26);
  plVar9[0x27] = 0;
  *(undefined *)(plVar9 + 0x26) = uVar2;
  lVar21 = plVar16[0x2a];
  plVar9[0x28] = lVar20;
  plVar9[0x29] = lVar6;
  plVar9[0x2a] = lVar21;
  if (lVar21 != 0) {
    LOCK();
    *(int *)(lVar21 + 8) = *(int *)(lVar21 + 8) + 1;
    UNLOCK();
  }
  plVar9[0x2b] = plVar16[0x2b];
  plVar9[0x2c] = plVar16[0x2c];
                    /* try { // try from 004ca6b1 to 004ca6b5 has its CatchHandler @ 004cacdc */
  KisThreadSafeSignalCompressor::KisThreadSafeSignalCompressor
            ((KisThreadSafeSignalCompressor *)(plVar9 + 0x2d),3000,0);
  lVar21 = plVar16[0x30];
  *(long **)(this + 0x50) = plVar9;
  plVar9[0x31] = 0;
  plVar9[0x30] = lVar21;
                    /* try { // try from 004ca6f6 to 004ca6fa has its CatchHandler @ 004cad00 */
  QObject::connect((QObject *)&local_98,(char *)(plVar9 + 0x2d),(QObject *)"2timeout()",(char *)this
                   ,0x7268d1);
  QMetaObject::Connection::~Connection((Connection *)&local_98);
                    /* try { // try from 004ca712 to 004ca716 has its CatchHandler @ 004cacb8 */
  KoID::id();
                    /* try { // try from 004ca723 to 004ca727 has its CatchHandler @ 004caca0 */
  KoID::id();
                    /* try { // try from 004ca734 to 004ca738 has its CatchHandler @ 004cac94 */
  KoID::id();
                    /* try { // try from 004ca745 to 004ca749 has its CatchHandler @ 004cac88 */
  KoID::id();
                    /* try { // try from 004ca756 to 004ca75a has its CatchHandler @ 004cac7c */
  KoID::id();
                    /* try { // try from 004ca767 to 004ca76b has its CatchHandler @ 004cacac */
  KoID::id();
                    /* try { // try from 004ca778 to 004ca77c has its CatchHandler @ 004cace8 */
  KoID::id();
                    /* try { // try from 004ca789 to 004ca78d has its CatchHandler @ 004cad0c */
  KoID::id();
  plVar16 = local_38;
                    /* try { // try from 004ca79d to 004ca7a1 has its CatchHandler @ 004cacd0 */
  KoID::id();
  pQVar11 = (QArrayData *)QArrayData::allocate(8,8,9,0);
  if (pQVar11 == (QArrayData *)0x0) {
                    /* try { // try from 0022adfd to 0022ae01 has its CatchHandler @ 0022ae07 */
    qBadAlloc();
  }
  lVar21 = *(long *)(pQVar11 + 0x10);
  plVar9 = local_78;
  do {
    while( true ) {
      pQVar13 = (QArrayData *)((long)plVar9 + (long)(pQVar11 + (lVar21 - (long)local_78)));
      piVar10 = (int *)*plVar9;
      plVar9 = plVar9 + 1;
      *(int **)pQVar13 = piVar10;
      if (*piVar10 + 1U < 2) break;
      LOCK();
      *piVar10 = *piVar10 + 1;
      UNLOCK();
      if (plVar9 == local_38 + 1) goto LAB_004ca806;
    }
  } while (plVar9 != local_38 + 1);
LAB_004ca806:
  *(undefined4 *)(pQVar11 + 4) = 9;
  while( true ) {
    pQVar13 = (QArrayData *)*plVar16;
    if (*(int *)pQVar13 == 0) {
      QArrayData::deallocate(pQVar13,2,8);
    }
    else if (*(int *)pQVar13 != -1) {
      LOCK();
      *(int *)pQVar13 = *(int *)pQVar13 + -1;
      UNLOCK();
      if (*(int *)pQVar13 == 0) {
        QArrayData::deallocate((QArrayData *)*plVar16,2,8);
      }
    }
    if (local_78 == plVar16) break;
    plVar16 = plVar16 + -1;
  }
  if (*(int *)pQVar11 == 0) {
    if ((char)pQVar11[0xb] < '\0') {
      pQVar13 = (QArrayData *)
                QArrayData::allocate(8,8,(ulong)(*(uint *)(pQVar11 + 8) & 0x7fffffff),0);
      local_98 = pQVar13;
      if (pQVar13 == (QArrayData *)0x0) {
                    /* try { // try from 0022ad1e to 0022ad22 has its CatchHandler @ 0022ad2f */
        qBadAlloc();
      }
      pQVar13[0xb] = (QArrayData)((byte)pQVar13[0xb] | 0x80);
      lVar21 = *(long *)(pQVar13 + 0x10);
      uVar3 = *(uint *)(pQVar13 + 8);
    }
    else {
      pQVar13 = (QArrayData *)QArrayData::allocate(8,8,(long)*(int *)(pQVar11 + 4),0);
      local_98 = pQVar13;
      if (pQVar13 == (QArrayData *)0x0) {
                    /* try { // try from 0022adec to 0022adf0 has its CatchHandler @ 0022ad2f */
        qBadAlloc();
      }
      lVar21 = *(long *)(pQVar13 + 0x10);
      uVar3 = *(uint *)(pQVar13 + 8);
    }
    if ((uVar3 & 0x7fffffff) == 0) {
      pQVar17 = pQVar13 + lVar21;
      lVar21 = (long)*(int *)(pQVar13 + 4) << 3;
    }
    else {
      iVar14 = *(int *)(pQVar11 + 4);
      pQVar17 = pQVar13 + lVar21;
      pQVar18 = pQVar11 + *(long *)(pQVar11 + 0x10);
      if (pQVar18 == pQVar18 + (long)iVar14 * 8) {
        lVar21 = 0;
      }
      else {
        pQVar13 = pQVar17;
        do {
          while( true ) {
            pQVar22 = pQVar13 + 8;
            piVar10 = *(int **)(pQVar13 + ((long)pQVar18 - (long)pQVar17));
            *(int **)pQVar13 = piVar10;
            pQVar13 = pQVar22;
            if (*piVar10 + 1U < 2) break;
            LOCK();
            *piVar10 = *piVar10 + 1;
            UNLOCK();
            if (pQVar22 == pQVar17 + ((long)(pQVar18 + (long)iVar14 * 8) - (long)pQVar18))
            goto LAB_004caae9;
          }
        } while (pQVar22 != pQVar17 + ((long)(pQVar18 + (long)iVar14 * 8) - (long)pQVar18));
LAB_004caae9:
        iVar14 = *(int *)(pQVar11 + 4);
        pQVar17 = local_98 + *(long *)(local_98 + 0x10);
        lVar21 = (long)iVar14 << 3;
        pQVar13 = local_98;
      }
      *(int *)(pQVar13 + 4) = iVar14;
    }
  }
  else {
    if (*(int *)pQVar11 != -1) {
      LOCK();
      *(int *)pQVar11 = *(int *)pQVar11 + 1;
      UNLOCK();
    }
    lVar21 = (long)*(int *)(pQVar11 + 4) << 3;
    pQVar17 = pQVar11 + *(long *)(pQVar11 + 0x10);
    pQVar13 = pQVar11;
    local_98 = pQVar11;
  }
  pQVar22 = pQVar17 + lVar21;
  local_80 = 1;
  local_88 = pQVar22;
  pQVar18 = local_98;
  for (; local_98 = pQVar18, local_90 = pQVar17, pQVar17 != pQVar22; pQVar17 = pQVar17 + 8) {
                    /* try { // try from 004ca8b1 to 004ca8c3 has its CatchHandler @ 004cacf4 */
    pKVar12 = (KisKeyframeChannel *)
              (**(code **)(**(long **)(*(long *)(this + 0x50) + 0xf8) + 0x20))
                        (*(long **)(*(long *)(this + 0x50) + 0xf8),pQVar17);
    if (pKVar12 != (KisKeyframeChannel *)0x0) {
      KisNode::addKeyframeChannel((KisNode *)this,pKVar12);
    }
    pQVar18 = local_98;
  }
  if (*(int *)pQVar13 == 0) {
LAB_004cabd0:
    pQVar17 = pQVar18 + *(long *)(pQVar18 + 0x10);
    pQVar13 = pQVar17 + (long)*(int *)(pQVar18 + 4) * 8;
joined_r0x004cabe9:
    pQVar22 = pQVar17;
    if (pQVar17 != pQVar13) {
      do {
        pQVar17 = pQVar22 + 8;
        pQVar5 = *(QArrayData **)pQVar22;
        if (*(int *)pQVar5 == 0) {
          QArrayData::deallocate(pQVar5,2,8);
        }
        else {
          if (*(int *)pQVar5 == -1) goto joined_r0x004cabe9;
          LOCK();
          *(int *)pQVar5 = *(int *)pQVar5 + -1;
          UNLOCK();
          if (*(int *)pQVar5 != 0) goto joined_r0x004cabe9;
          QArrayData::deallocate(*(QArrayData **)pQVar22,2,8);
        }
        pQVar22 = pQVar17;
        if (pQVar13 == pQVar17) break;
      } while( true );
    }
    QArrayData::deallocate(pQVar18,8,8);
  }
  else if (*(int *)pQVar13 != -1) {
    LOCK();
    *(int *)pQVar13 = *(int *)pQVar13 + -1;
    UNLOCK();
    if (*(int *)pQVar13 == 0) goto LAB_004cabd0;
  }
  if (*(int *)pQVar11 == 0) {
LAB_004cab40:
    pQVar17 = pQVar11 + *(long *)(pQVar11 + 0x10);
    pQVar13 = pQVar17 + (long)*(int *)(pQVar11 + 4) * 8;
joined_r0x004cab52:
    pQVar18 = pQVar17;
    if (pQVar17 != pQVar13) {
      do {
        pQVar17 = pQVar18 + 8;
        pQVar22 = *(QArrayData **)pQVar18;
        if (*(int *)pQVar22 == 0) {
          QArrayData::deallocate(pQVar22,2,8);
        }
        else {
          if (*(int *)pQVar22 == -1) goto joined_r0x004cab52;
          LOCK();
          *(int *)pQVar22 = *(int *)pQVar22 + -1;
          UNLOCK();
          if (*(int *)pQVar22 != 0) goto joined_r0x004cab52;
          QArrayData::deallocate(*(QArrayData **)pQVar18,2,8);
        }
        pQVar18 = pQVar17;
        if (pQVar13 == pQVar17) break;
      } while( true );
    }
    if (local_38[1] == *(long *)(in_FS_OFFSET + 0x28)) {
      QArrayData::deallocate(pQVar11,8,8);
      return;
    }
  }
  else {
    if (*(int *)pQVar11 != -1) {
      LOCK();
      *(int *)pQVar11 = *(int *)pQVar11 + -1;
      UNLOCK();
      if (*(int *)pQVar11 == 0) goto LAB_004cab40;
    }
    if (local_38[1] == *(long *)(in_FS_OFFSET + 0x28)) {
      return;
    }
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



