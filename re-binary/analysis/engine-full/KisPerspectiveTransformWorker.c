/* Class KisPerspectiveTransformWorker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPerspectiveTransformWorker @ 0020a6d0 ======

void __thiscall
KisPerspectiveTransformWorker::KisPerspectiveTransformWorker
          (KisPerspectiveTransformWorker *this,KisSharedPtr param_1,QTransform *param_2,bool param_3
          ,QPointer param_4)

{
  (*(code *)PTR_KisPerspectiveTransformWorker_0083ce38)();
  return;
}



// ====== KisPerspectiveTransformWorker @ 00608be0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisPerspectiveTransformWorker::KisPerspectiveTransformWorker(KisSharedPtr<KisPaintDevice>,
   QPointF, double, double, double, bool, QPointer<KoUpdater>) */

void __thiscall
KisPerspectiveTransformWorker::KisPerspectiveTransformWorker
          (double param_1,double param_2,double param_3,double param_4,double param_5,
          KisPerspectiveTransformWorker *this,long *param_7,KisPerspectiveTransformWorker param_8,
          undefined8 *param_9)

{
  long lVar1;
  int *piVar2;
  undefined8 uVar3;
  undefined *puVar4;
  long in_FS_OFFSET;
  QTransform local_1b8 [96];
  QTransform local_158 [96];
  QTransform local_f8 [96];
  undefined8 local_98;
  undefined4 local_90;
  QVector3D local_88 [16];
  undefined8 local_78;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined4 local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  lVar1 = *param_7;
  *(long *)this = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  piVar2 = (int *)*param_9;
  uVar3 = param_9[1];
  *(int **)(this + 8) = piVar2;
  *(undefined8 *)(this + 0x10) = uVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  puVar4 = PTR_shared_null_008377d0;
  *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  *(undefined **)(this + 0x18) = puVar4;
  *(undefined (*) [16])(this + 0x30) = (undefined  [16])0x0;
                    /* try { // try from 00608c66 to 00608dda has its CatchHandler @ 00608e05 */
  QTransform::QTransform((QTransform *)(this + 0x40));
  QTransform::QTransform((QTransform *)(this + 0x98));
  local_88 = (QVector3D  [16])ZEXT416(DAT_00749ac0);
  this[0xf2] = param_8;
  uStack_70 = _UNK_00749ad8;
  local_78 = DAT_00749ad0;
  this[0xf3] = (KisPerspectiveTransformWorker)0x0;
  local_48 = 0;
  local_68 = _DAT_00749ae0;
  uStack_60 = _UNK_00749ae8;
  local_98 = 0x3f800000;
  local_58 = _DAT_00749af0;
  uStack_50 = _UNK_00749af8;
  local_90 = 0;
  QMatrix4x4::rotate((float)((DAT_007231c0 * param_3) / DAT_007231b8),local_88);
  local_90 = 0;
  local_98 = DAT_00749ad0;
  QMatrix4x4::rotate((float)((DAT_007231c0 * param_4) / DAT_007231b8),local_88);
  QMatrix4x4::toTransform((float)param_5);
  QTransform::fromTranslate(param_1,param_2);
  QTransform::inverted((bool *)local_f8);
  QTransform::operator*(local_158,local_f8);
  QTransform::operator*(local_1b8,local_158);
  init(this,(QTransform *)local_1b8);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisPerspectiveTransformWorker @ 00608e20 ======

/* KisPerspectiveTransformWorker::KisPerspectiveTransformWorker(KisSharedPtr<KisPaintDevice>,
   QTransform const&, bool, QPointer<KoUpdater>) */

void __thiscall
KisPerspectiveTransformWorker::KisPerspectiveTransformWorker
          (KisPerspectiveTransformWorker *this,KisSharedPtr param_1,QTransform *param_2,bool param_3
          ,QPointer param_4)

{
  long lVar1;
  int *piVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined4 in_register_00000034;
  undefined4 in_register_00000084;
  
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)this = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  piVar2 = *(int **)CONCAT44(in_register_00000084,param_4);
  uVar3 = ((undefined8 *)CONCAT44(in_register_00000084,param_4))[1];
  *(int **)(this + 8) = piVar2;
  *(undefined8 *)(this + 0x10) = uVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  puVar4 = PTR_shared_null_008377d0;
  *(undefined (*) [16])(this + 0x20) = (undefined  [16])0x0;
  *(undefined **)(this + 0x18) = puVar4;
  *(undefined (*) [16])(this + 0x30) = (undefined  [16])0x0;
                    /* try { // try from 00608e73 to 00608e9c has its CatchHandler @ 00608ea2 */
  QTransform::QTransform((QTransform *)(this + 0x40));
  QTransform::QTransform((QTransform *)(this + 0x98));
  this[0xf2] = (KisPerspectiveTransformWorker)param_3;
  this[0xf3] = (KisPerspectiveTransformWorker)0x0;
  init(this,param_2);
  return;
}



