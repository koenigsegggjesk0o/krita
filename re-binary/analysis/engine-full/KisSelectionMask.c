/* Class KisSelectionMask - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSelectionMask @ 00206230 ======

void __thiscall
KisSelectionMask::KisSelectionMask(KisSelectionMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  (*(code *)PTR_KisSelectionMask_0083abe8)();
  return;
}



// ====== KisSelectionMask @ 0020c400 ======

void __thiscall KisSelectionMask::KisSelectionMask(KisSelectionMask *this,KisSelectionMask *param_1)

{
  (*(code *)PTR_KisSelectionMask_0083dcd0)();
  return;
}



// ====== KisSelectionMask @ 005fbd00 ======

/* KisSelectionMask::KisSelectionMask(KisWeakSharedPtr<KisImage>, QString const&) */

void __thiscall
KisSelectionMask::KisSelectionMask(KisSelectionMask *this,KisWeakSharedPtr param_1,QString *param_2)

{
  int iVar1;
  QArrayData *pQVar2;
  undefined *puVar3;
  undefined8 *puVar4;
  QString *pQVar5;
  KoColorSpace *pKVar6;
  KisThreadSafeSignalCompressor *this_00;
  char *pcVar7;
  int *piVar8;
  undefined4 in_register_00000034;
  long *plVar9;
  long in_FS_OFFSET;
  QArrayData *local_68;
  int *piStack_60;
  QColor local_58 [24];
  long local_40;
  
  plVar9 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  if (*plVar9 == 0) {
    local_68 = (QArrayData *)0x0;
    pQVar2 = local_68;
LAB_005fbf79:
    local_68 = pQVar2;
    piStack_60 = (int *)0x0;
  }
  else if (((uint *)plVar9[1] == (uint *)0x0) || ((*(uint *)plVar9[1] & 1) == 0)) {
    local_68 = (QArrayData *)0x0;
    piStack_60 = (int *)0x0;
  }
  else {
    pQVar2 = (QArrayData *)*plVar9;
    piStack_60 = (int *)local_68;
    local_68 = pQVar2;
    if (pQVar2 == (QArrayData *)0x0) goto LAB_005fbf79;
    piVar8 = *(int **)((long)pQVar2 + 0x58);
    if (piVar8 == (int *)0x0) {
      piVar8 = (int *)operator_new(4);
      *piVar8 = 0;
      *(int **)((long)pQVar2 + 0x58) = piVar8;
      LOCK();
      *piVar8 = *piVar8 + 1;
      UNLOCK();
      piVar8 = *(int **)((long)pQVar2 + 0x58);
      pQVar2 = local_68;
    }
    local_68 = pQVar2;
    LOCK();
    *piVar8 = *piVar8 + 2;
    UNLOCK();
    piStack_60 = piVar8;
  }
                    /* try { // try from 005fbd54 to 005fbd58 has its CatchHandler @ 005fbfed */
  KisEffectMask::KisEffectMask((KisEffectMask *)this,(KisWeakSharedPtr)(QObject *)&local_68,param_2)
  ;
  local_68 = (QArrayData *)0x0;
  pQVar2 = local_68;
  local_68 = (QArrayData *)0x0;
  if (piStack_60 != (int *)0x0) {
    LOCK();
    iVar1 = *piStack_60;
    *piStack_60 = *piStack_60 + -2;
    UNLOCK();
    if ((iVar1 < 3) && (piStack_60 != (int *)0x0)) {
      local_68 = pQVar2;
      operator_delete(piStack_60,4);
    }
  }
  puVar3 = PTR_vtable_00837ce8;
  *(undefined **)this = PTR_vtable_00837ce8 + 0x10;
  *(undefined **)(this + 0x30) = puVar3 + 0x248;
  *(undefined **)(this + 0x48) = puVar3 + 0x288;
                    /* try { // try from 005fbda6 to 005fbdaa has its CatchHandler @ 005fbfd5 */
  puVar4 = (undefined8 *)operator_new(0x80);
  *puVar4 = this;
  puVar4[1] = 0;
  puVar4[2] = 0;
  puVar4[3] = 0;
  puVar4[6] = 0;
  puVar4[7] = 0;
  *(undefined (*) [16])(puVar4 + 4) = (undefined  [16])0x0;
                    /* try { // try from 005fbde5 to 005fbde9 has its CatchHandler @ 005fbfe1 */
  pQVar5 = (QString *)KoColorSpaceRegistry::instance();
  local_68 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 005fbdfc to 005fbe23 has its CatchHandler @ 005fbfbd */
  pKVar6 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar5);
  QColor::QColor(local_58,8);
  KoColor::KoColor((KoColor *)(puVar4 + 8),local_58,pKVar6);
  if (*(int *)local_68 != 0) {
    if (*(int *)local_68 == -1) goto LAB_005fbe47;
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 != 0) goto LAB_005fbe47;
  }
  QArrayData::deallocate(local_68,2,8);
LAB_005fbe47:
  *(undefined8 **)(this + 0x50) = puVar4;
                    /* try { // try from 005fbe50 to 005fbe68 has its CatchHandler @ 005fbfd5 */
  setActive(this,false);
  KisBaseNode::setSupportsLodMoves((KisBaseNode *)this,false);
  this_00 = (KisThreadSafeSignalCompressor *)operator_new(0x18);
                    /* try { // try from 005fbe79 to 005fbe7d has its CatchHandler @ 005fbfc9 */
  KisThreadSafeSignalCompressor::KisThreadSafeSignalCompressor(this_00,0x32,2);
  *(KisThreadSafeSignalCompressor **)(*(long *)(this + 0x50) + 0x38) = this_00;
                    /* try { // try from 005fbea0 to 005fbee3 has its CatchHandler @ 005fbfd5 */
  QObject::connect((QObject *)&local_68,(char *)this_00,(QObject *)"2timeout()",(char *)this,
                   0x749500);
  QMetaObject::Connection::~Connection((Connection *)&local_68);
  pcVar7 = (char *)KisImageConfigNotifier::instance();
  QObject::connect((QObject *)&local_68,pcVar7,(QObject *)"2configChanged()",(char *)this,0x72df3b);
  QMetaObject::Connection::~Connection((Connection *)&local_68);
  FUN_005fb510(*(undefined8 *)(this + 0x50),0);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSelectionMask @ 005fc000 ======

/* KisSelectionMask::KisSelectionMask(KisSelectionMask const&) */

void __thiscall KisSelectionMask::KisSelectionMask(KisSelectionMask *this,KisSelectionMask *param_1)

{
  undefined *puVar1;
  undefined8 *puVar2;
  QString *pQVar3;
  KoColorSpace *pKVar4;
  KisThreadSafeSignalCompressor *this_00;
  char *pcVar5;
  long in_FS_OFFSET;
  QArrayData *local_60;
  QColor local_58 [24];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisEffectMask::KisEffectMask((KisEffectMask *)this,(KisEffectMask *)param_1);
  puVar1 = PTR_vtable_00837ce8;
  *(undefined **)this = PTR_vtable_00837ce8 + 0x10;
  *(undefined **)(this + 0x30) = puVar1 + 0x248;
  *(undefined **)(this + 0x48) = puVar1 + 0x288;
                    /* try { // try from 005fc052 to 005fc056 has its CatchHandler @ 005fc1dd */
  puVar2 = (undefined8 *)operator_new(0x80);
  *puVar2 = this;
  puVar2[1] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
  puVar2[6] = 0;
  puVar2[7] = 0;
  *(undefined (*) [16])(puVar2 + 4) = (undefined  [16])0x0;
                    /* try { // try from 005fc091 to 005fc095 has its CatchHandler @ 005fc1d1 */
  pQVar3 = (QString *)KoColorSpaceRegistry::instance();
  local_60 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 005fc0ad to 005fc0d4 has its CatchHandler @ 005fc1b9 */
  pKVar4 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar3);
  QColor::QColor(local_58,8);
  KoColor::KoColor((KoColor *)(puVar2 + 8),local_58,pKVar4);
  if (*(int *)local_60 != 0) {
    if (*(int *)local_60 == -1) goto LAB_005fc0f8;
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 != 0) goto LAB_005fc0f8;
  }
  QArrayData::deallocate(local_60,2,8);
LAB_005fc0f8:
  *(undefined8 **)(this + 0x50) = puVar2;
                    /* try { // try from 005fc101 to 005fc105 has its CatchHandler @ 005fc1dd */
  this_00 = (KisThreadSafeSignalCompressor *)operator_new(0x18);
                    /* try { // try from 005fc113 to 005fc117 has its CatchHandler @ 005fc1c5 */
  KisThreadSafeSignalCompressor::KisThreadSafeSignalCompressor(this_00,300,0);
  *(KisThreadSafeSignalCompressor **)(*(long *)(this + 0x50) + 0x38) = this_00;
                    /* try { // try from 005fc13a to 005fc17d has its CatchHandler @ 005fc1dd */
  QObject::connect((QObject *)&local_60,(char *)this_00,(QObject *)"2timeout()",(char *)this,
                   0x749500);
  QMetaObject::Connection::~Connection((Connection *)&local_60);
  pcVar5 = (char *)KisImageConfigNotifier::instance();
  QObject::connect((QObject *)&local_60,pcVar5,(QObject *)"2configChanged()",(char *)this,0x72df3b);
  QMetaObject::Connection::~Connection((Connection *)&local_60);
  FUN_005fb510(*(undefined8 *)(this + 0x50),0);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



