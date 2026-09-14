/* Class KisPolygonalGradientShapeStrategy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPolygonalGradientShapeStrategy @ 002012b0 ======

void __thiscall
KisPolygonalGradientShapeStrategy::KisPolygonalGradientShapeStrategy
          (KisPolygonalGradientShapeStrategy *this,QPainterPath *param_1,double param_2)

{
  (*(code *)PTR_KisPolygonalGradientShapeStrategy_00838428)();
  return;
}



// ====== KisPolygonalGradientShapeStrategy @ 004de680 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisPolygonalGradientShapeStrategy::KisPolygonalGradientShapeStrategy(QPainterPath const&, double)
    */

void __thiscall
KisPolygonalGradientShapeStrategy::KisPolygonalGradientShapeStrategy
          (KisPolygonalGradientShapeStrategy *this,QPainterPath *param_1,double param_2)

{
  QPainterPath *this_00;
  long in_FS_OFFSET;
  undefined8 uVar1;
  double dVar2;
  undefined8 local_38;
  long local_30;
  
  this_00 = (QPainterPath *)(this + 0x28);
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisGradientShapeStrategy::KisGradientShapeStrategy((KisGradientShapeStrategy *)this);
  *(undefined **)this = PTR_vtable_00837888 + 0x10;
  QPainterPath::QPainterPath(this_00);
  uVar1 = DAT_00724258;
  *(double *)(this + 0x30) = param_2;
                    /* try { // try from 004de6f4 to 004de738 has its CatchHandler @ 004de778 */
  FUN_004de1e0(_DAT_00724260,uVar1,(QPainterPath *)&local_38,param_1,100);
  uVar1 = *(undefined8 *)(this + 0x28);
  *(undefined8 *)(this + 0x28) = local_38;
  local_38 = uVar1;
  QPainterPath::~QPainterPath((QPainterPath *)&local_38);
  uVar1 = FUN_004dd680(*(undefined8 *)(this + 0x30),this_00,1);
  *(undefined8 *)(this + 0x40) = uVar1;
  dVar2 = (double)FUN_004dd680(*(undefined8 *)(this + 0x30),this_00,0);
  *(double *)(this + 0x38) = dVar2;
  *(double *)(this + 0x48) = DAT_007227c0 / (*(double *)(this + 0x40) - dVar2);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



