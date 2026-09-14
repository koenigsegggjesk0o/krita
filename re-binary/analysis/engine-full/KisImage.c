/* Class KisImage - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImage @ 00209be0 ======

void __thiscall
KisImage::KisImage(KisImage *this,KisImage *param_1,KisUndoStore *param_2,bool param_3)

{
  (*(code *)PTR_KisImage_0083c8c0)();
  return;
}



// ====== KisImage @ 0020db70 ======

void __thiscall
KisImage::KisImage(KisImage *this,KisUndoStore *param_1,int param_2,int param_3,
                  KoColorSpace *param_4,QString *param_5)

{
  (*(code *)PTR_KisImage_0083e888)();
  return;
}



// ====== KisImage @ 005204e0 ======

/* KisImage::KisImage(KisUndoStore*, int, int, KoColorSpace const*, QString const&) */

void __thiscall
KisImage::KisImage(KisImage *this,KisUndoStore *param_1,int param_2,int param_3,
                  KoColorSpace *param_4,QString *param_5)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined *puVar7;
  undefined *puVar8;
  void *pvVar9;
  KisImageAnimationInterface *this_00;
  KisGroupLayer *pKVar10;
  int *piVar11;
  long in_FS_OFFSET;
  KisGroupLayer *local_68;
  undefined8 local_60;
  KisImage *local_58;
  int *local_50;
  long local_40;
  
  puVar3 = PTR_vtable_00837fd8 + 0x198;
  puVar4 = PTR_vtable_00837fd8 + 0x208;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  puVar5 = PTR_vtable_00837fd8 + 0x1d8;
  puVar1 = PTR_vtable_00837d28 + 0x10;
  puVar6 = PTR_vtable_00837fd8 + 0x2a0;
  puVar2 = PTR_vtable_00836d88 + 0x10;
  puVar7 = PTR_vtable_00837950 + 0x10;
  puVar8 = PTR_vtable_00837a20 + 0x10;
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)(this + 0x10) = puVar1;
  *(undefined **)(this + 0x18) = puVar7;
  *(undefined **)(this + 0x20) = puVar2;
  *(undefined **)(this + 0x28) = puVar8;
                    /* try { // try from 005205f6 to 005205fa has its CatchHandler @ 00520843 */
  KisNodeFacade::KisNodeFacade((KisNodeFacade *)(this + 0x30));
                    /* try { // try from 00520609 to 0052060d has its CatchHandler @ 0052084f */
  KisNodeGraphListener::KisNodeGraphListener((KisNodeGraphListener *)(this + 0x40));
                    /* try { // try from 00520615 to 00520619 has its CatchHandler @ 005207fb */
  KisShared::KisShared((KisShared *)(this + 0x50));
  puVar1 = PTR_vtable_00837fd8;
  *(undefined **)(this + 0x10) = puVar3;
  *(undefined **)(this + 0x18) = puVar5;
  *(undefined **)(this + 0x20) = puVar4;
  *(undefined **)(this + 0x28) = puVar6;
  *(undefined **)this = puVar1 + 0x10;
  *(undefined **)(this + 0x30) = PTR_vtable_00837fd8 + 0x2c8;
  *(undefined **)(this + 0x40) = PTR_vtable_00837fd8 + 0x2e8;
                    /* try { // try from 00520661 to 00520665 has its CatchHandler @ 0052082b */
  pvVar9 = operator_new(0x180);
                    /* try { // try from 0052066f to 00520673 has its CatchHandler @ 0052081f */
  this_00 = (KisImageAnimationInterface *)operator_new(0x18);
                    /* try { // try from 0052067d to 00520681 has its CatchHandler @ 00520813 */
  KisImageAnimationInterface::KisImageAnimationInterface(this_00,this);
                    /* try { // try from 005206a4 to 005206a8 has its CatchHandler @ 0052081f */
  FUN_00527280(pvVar9,this,param_2,param_3,param_4,param_1,this_00);
  *(void **)(this + 0x60) = pvVar9;
                    /* try { // try from 005206b9 to 0052070c has its CatchHandler @ 0052082b */
  QObject::thread();
  QObject::moveToThread((QThread *)this);
  QObject::connect((QObject *)&local_58,(char *)this,
                   (QObject *)"2sigInternalStopIsolatedModeRequested()",(char *)this,0x732831);
  QMetaObject::Connection::~Connection((Connection *)&local_58);
  QObject::setObjectName((QString *)this);
  pKVar10 = (KisGroupLayer *)operator_new(0x40);
                    /* try { // try from 0052071d to 00520721 has its CatchHandler @ 00520837 */
  local_60 = QString::fromAscii_helper("root",4);
  local_50 = *(int **)(this + 0x58);
  local_58 = this;
  if (local_50 == (int *)0x0) {
                    /* try { // try from 005207c5 to 005207c9 has its CatchHandler @ 005207e6 */
    piVar11 = (int *)operator_new(4);
    *piVar11 = 0;
    *(int **)(this + 0x58) = piVar11;
    LOCK();
    *piVar11 = *piVar11 + 1;
    UNLOCK();
    local_50 = *(int **)(this + 0x58);
  }
  LOCK();
  *local_50 = *local_50 + 2;
  UNLOCK();
                    /* try { // try from 00520755 to 00520759 has its CatchHandler @ 005207f2 */
  KisGroupLayer::KisGroupLayer
            (pKVar10,(KisWeakSharedPtr)(QObject *)&local_58,(QString *)&local_60,0xff,
             (KoColorSpace *)0x0);
  LOCK();
  *(int *)(pKVar10 + 0x10) = *(int *)(pKVar10 + 0x10) + 1;
  UNLOCK();
  local_68 = pKVar10;
                    /* try { // try from 0052076e to 00520772 has its CatchHandler @ 00520807 */
  setRootLayer(this,(KisSharedPtr)&local_68);
  if (local_68 != (KisGroupLayer *)0x0) {
    LOCK();
    pKVar10 = local_68 + 0x10;
    *(int *)pKVar10 = *(int *)pKVar10 + -1;
    UNLOCK();
    if (*(int *)pKVar10 == 0) {
      (**(code **)(*(long *)local_68 + 0x20))();
    }
  }
  FUN_0035b390((QObject *)&local_58);
  FUN_002dd9a0(&local_60);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisImage @ 00520860 ======

/* KisImage::KisImage(KisImage const&, KisUndoStore*, bool) */

void __thiscall
KisImage::KisImage(KisImage *this,KisImage *param_1,KisUndoStore *param_2,bool param_3)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined *puVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined *puVar10;
  undefined *puVar11;
  void *pvVar12;
  KisImageAnimationInterface *this_00;
  KisImageAnimationInterface *pKVar13;
  undefined8 uVar14;
  long in_FS_OFFSET;
  QObject local_48 [8];
  long local_40;
  
  puVar7 = PTR_vtable_00837fd8;
  puVar3 = PTR_vtable_00837fd8 + 0x198;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  puVar4 = PTR_vtable_00837fd8 + 0x1d8;
  puVar5 = PTR_vtable_00837fd8 + 0x208;
  puVar6 = PTR_vtable_00837fd8 + 0x2a0;
  puVar1 = PTR_vtable_00837d28 + 0x10;
  puVar10 = PTR_vtable_00837950 + 0x10;
  puVar2 = PTR_vtable_00836d88 + 0x10;
  puVar11 = PTR_vtable_00837a20 + 0x10;
  QObject::QObject((QObject *)this,(QObject *)0x0);
  *(undefined **)(this + 0x10) = puVar1;
  *(undefined **)(this + 0x18) = puVar10;
  *(undefined **)(this + 0x20) = puVar2;
  *(undefined **)(this + 0x28) = puVar11;
                    /* try { // try from 00520952 to 00520956 has its CatchHandler @ 00520b01 */
  KisNodeFacade::KisNodeFacade((KisNodeFacade *)(this + 0x30));
                    /* try { // try from 00520965 to 00520969 has its CatchHandler @ 00520b19 */
  KisNodeGraphListener::KisNodeGraphListener((KisNodeGraphListener *)(this + 0x40));
                    /* try { // try from 00520975 to 00520979 has its CatchHandler @ 00520b0d */
  KisShared::KisShared((KisShared *)(this + 0x50));
  *(undefined **)this = puVar7 + 0x10;
  *(undefined **)(this + 0x30) = puVar7 + 0x2c8;
  *(undefined **)(this + 0x40) = puVar7 + 0x2e8;
  *(undefined **)(this + 0x10) = puVar3;
  *(undefined **)(this + 0x18) = puVar4;
  *(undefined **)(this + 0x20) = puVar5;
  *(undefined **)(this + 0x28) = puVar6;
                    /* try { // try from 005209ae to 005209b2 has its CatchHandler @ 00520ae9 */
  pvVar12 = operator_new(0x180);
                    /* try { // try from 005209bb to 005209bf has its CatchHandler @ 00520add */
  this_00 = (KisImageAnimationInterface *)operator_new(0x18);
                    /* try { // try from 005209c7 to 005209d9 has its CatchHandler @ 00520af5 */
  pKVar13 = (KisImageAnimationInterface *)animationInterface(param_1);
  KisImageAnimationInterface::KisImageAnimationInterface(this_00,pKVar13,this);
  if (param_2 == (KisUndoStore *)0x0) {
                    /* try { // try from 00520aad to 00520ab1 has its CatchHandler @ 00520add */
    param_2 = (KisUndoStore *)operator_new(0x10);
    *(undefined (*) [16])param_2 = (undefined  [16])0x0;
                    /* try { // try from 00520abf to 00520ac3 has its CatchHandler @ 00520b25 */
    KisUndoStore::KisUndoStore(param_2);
    *(undefined **)param_2 = PTR_vtable_00836cf0 + 0x10;
  }
                    /* try { // try from 005209e7 to 00520a21 has its CatchHandler @ 00520add */
  uVar14 = colorSpace(param_1);
  uVar8 = height(param_1);
  uVar9 = width(param_1);
  FUN_00527280(pvVar12,this,uVar9,uVar8,uVar14,param_2,this_00);
  *(void **)(this + 0x60) = pvVar12;
                    /* try { // try from 00520a32 to 00520a84 has its CatchHandler @ 00520ae9 */
  QObject::thread();
  QObject::moveToThread((QThread *)this);
  QObject::connect(local_48,(char *)this,(QObject *)"2sigInternalStopIsolatedModeRequested()",
                   (char *)this,0x732831);
  QMetaObject::Connection::~Connection((Connection *)local_48);
  copyFromImageImpl(this,param_1,(-(uint)!param_3 & 0xfffffffc) + 5);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



