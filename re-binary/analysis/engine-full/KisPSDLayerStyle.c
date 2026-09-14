/* Class KisPSDLayerStyle - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPSDLayerStyle @ 00201370 ======

void __thiscall
KisPSDLayerStyle::KisPSDLayerStyle(KisPSDLayerStyle *this,QString *param_1,QSharedPointer param_2)

{
  (*(code *)PTR_KisPSDLayerStyle_00838488)();
  return;
}



// ====== KisPSDLayerStyle @ 002082d0 ======

void __thiscall KisPSDLayerStyle::KisPSDLayerStyle(KisPSDLayerStyle *this,KisPSDLayerStyle *param_1)

{
  (*(code *)PTR_KisPSDLayerStyle_0083bc38)();
  return;
}



// ====== KisPSDLayerStyle @ 006cf250 ======

/* KisPSDLayerStyle::KisPSDLayerStyle(KisPSDLayerStyle const&) */

void __thiscall KisPSDLayerStyle::KisPSDLayerStyle(KisPSDLayerStyle *this,KisPSDLayerStyle *param_1)

{
  void *pvVar1;
  
  KoResource::KoResource((KoResource *)this,(KoResource *)param_1);
  *(undefined **)this = PTR_vtable_00837860 + 0x10;
                    /* try { // try from 006cf276 to 006cf27a has its CatchHandler @ 006cf2a7 */
  pvVar1 = operator_new(0x16b0);
                    /* try { // try from 006cf286 to 006cf28a has its CatchHandler @ 006cf2b3 */
  FUN_006d44a0(pvVar1,*(undefined8 *)(param_1 + 0x10));
  *(void **)(this + 0x10) = pvVar1;
                    /* try { // try from 006cf292 to 006cf2a1 has its CatchHandler @ 006cf2a7 */
  KoResource::valid();
  KoResource::setValid(SUB81(this,0));
  return;
}



// ====== KisPSDLayerStyle @ 006cf8c0 ======

/* KisPSDLayerStyle::KisPSDLayerStyle(QString const&, QSharedPointer<KisResourcesInterface>) */

void __thiscall
KisPSDLayerStyle::KisPSDLayerStyle(KisPSDLayerStyle *this,QString *param_1,QSharedPointer param_2)

{
  int *piVar1;
  QArrayData *pQVar2;
  int *piVar3;
  void *pvVar4;
  undefined8 *puVar5;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  undefined8 local_50;
  undefined8 local_48;
  int *piStack_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KoResource::KoResource((KoResource *)this,(QString *)param_1);
  *(undefined **)this = PTR_vtable_00837860 + 0x10;
                    /* try { // try from 006cf8fd to 006cf901 has its CatchHandler @ 006cfa27 */
  pvVar4 = operator_new(0x16b0);
  local_48 = *(undefined8 *)CONCAT44(in_register_00000014,param_2);
  piVar3 = (int *)((undefined8 *)CONCAT44(in_register_00000014,param_2))[1];
  piStack_40 = piVar3;
  if (piVar3 == (int *)0x0) {
                    /* try { // try from 006cf9f3 to 006cf9f7 has its CatchHandler @ 006cfa4b */
    FUN_006d5180(pvVar4,&local_48);
    *(void **)(this + 0x10) = pvVar4;
  }
  else {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    piVar3[1] = piVar3[1] + 1;
    UNLOCK();
                    /* try { // try from 006cf935 to 006cf939 has its CatchHandler @ 006cfa33 */
    FUN_006d5180(pvVar4,&local_48);
    *(void **)(this + 0x10) = pvVar4;
    LOCK();
    piVar1 = piVar3 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piVar3 + 2))(piVar3);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      operator_delete(piVar3,0x10);
    }
  }
                    /* try { // try from 006cf962 to 006cf966 has its CatchHandler @ 006cfa27 */
  ki18nd((char *)&local_48,"krita");
                    /* try { // try from 006cf96f to 006cf973 has its CatchHandler @ 006cfa3f */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_48);
  puVar5 = *(undefined8 **)(this + 0x10);
  pQVar2 = (QArrayData *)*puVar5;
  *puVar5 = local_50;
  if (*(int *)pQVar2 != 0) {
    if (*(int *)pQVar2 == -1) goto LAB_006cf9a5;
    LOCK();
    *(int *)pQVar2 = *(int *)pQVar2 + -1;
    UNLOCK();
    if (*(int *)pQVar2 != 0) {
      puVar5 = *(undefined8 **)(this + 0x10);
      goto LAB_006cf9a5;
    }
  }
  QArrayData::deallocate(pQVar2,2,8);
  puVar5 = *(undefined8 **)(this + 0x10);
LAB_006cf9a5:
  *(undefined2 *)(puVar5 + 3) = 7;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



