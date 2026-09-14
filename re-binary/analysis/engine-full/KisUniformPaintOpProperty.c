/* Class KisUniformPaintOpProperty - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisUniformPaintOpProperty @ 00204250 ======

void __thiscall
KisUniformPaintOpProperty::KisUniformPaintOpProperty
          (KisUniformPaintOpProperty *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  (*(code *)PTR_KisUniformPaintOpProperty_00839bf8)();
  return;
}



// ====== KisUniformPaintOpProperty @ 002081b0 ======

void __thiscall
KisUniformPaintOpProperty::KisUniformPaintOpProperty
          (KisUniformPaintOpProperty *this,Type param_1,KoID *param_2,KisRestrictedSharedPtr param_3
          ,QObject *param_4)

{
  (*(code *)PTR_KisUniformPaintOpProperty_0083bba8)();
  return;
}



// ====== KisUniformPaintOpProperty @ 0020b6c0 ======

void __thiscall
KisUniformPaintOpProperty::KisUniformPaintOpProperty
          (KisUniformPaintOpProperty *this,Type param_1,SubType param_2,KoID *param_3,
          KisRestrictedSharedPtr param_4,QObject *param_5)

{
  (*(code *)PTR_KisUniformPaintOpProperty_0083d630)();
  return;
}



// ====== KisUniformPaintOpProperty @ 003532d0 ======

/* KisUniformPaintOpProperty::KisUniformPaintOpProperty(KisUniformPaintOpProperty::Type,
   KisUniformPaintOpProperty::SubType, KoID const&, KisRestrictedSharedPtr<KisPaintOpSettings>,
   QObject*) */

void __thiscall
KisUniformPaintOpProperty::KisUniformPaintOpProperty
          (KisUniformPaintOpProperty *this,Type param_1,SubType param_2,KoID *param_3,
          KisRestrictedSharedPtr param_4,QObject *param_5)

{
  long *plVar1;
  long *plVar2;
  Type *pTVar3;
  undefined4 in_register_00000084;
  
  QObject::QObject((QObject *)this,param_5);
  *(undefined **)this = PTR_vtable_00836fd8 + 0x10;
                    /* try { // try from 0035330d to 00353311 has its CatchHandler @ 003533bb */
  pTVar3 = (Type *)operator_new(0x38);
  plVar2 = *(long **)CONCAT44(in_register_00000084,param_4);
  if (plVar2 != (long *)0x0) {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
  }
  *pTVar3 = param_1;
  pTVar3[1] = param_2;
                    /* try { // try from 00353331 to 00353335 has its CatchHandler @ 003533c7 */
  KoID::KoID((KoID *)(pTVar3 + 2),param_3);
  pTVar3[6] = 0;
  pTVar3[7] = 0;
  pTVar3[8] = 0x80000000;
  *(long **)(pTVar3 + 10) = plVar2;
  if (plVar2 != (long *)0x0) {
    plVar1 = plVar2 + 1;
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    *(undefined2 *)(pTVar3 + 0xc) = 0;
    *(Type **)(this + 0x10) = pTVar3;
    LOCK();
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 != 0) {
      return;
    }
                    /* WARNING: Could not recover jumptable at 0x003533b9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar2 + 8))(plVar2);
    return;
  }
  *(Type **)(this + 0x10) = pTVar3;
  *(undefined2 *)(pTVar3 + 0xc) = 0;
  return;
}



// ====== KisUniformPaintOpProperty @ 003533e0 ======

/* KisUniformPaintOpProperty::KisUniformPaintOpProperty(KisUniformPaintOpProperty::Type, KoID
   const&, KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisUniformPaintOpProperty::KisUniformPaintOpProperty
          (KisUniformPaintOpProperty *this,Type param_1,KoID *param_2,KisRestrictedSharedPtr param_3
          ,QObject *param_4)

{
  long *plVar1;
  long *plVar2;
  Type *pTVar3;
  undefined4 in_register_0000000c;
  
  QObject::QObject((QObject *)this,param_4);
  *(undefined **)this = PTR_vtable_00836fd8 + 0x10;
                    /* try { // try from 00353414 to 00353418 has its CatchHandler @ 003534ad */
  pTVar3 = (Type *)operator_new(0x38);
  plVar2 = *(long **)CONCAT44(in_register_0000000c,param_3);
  if (plVar2 != (long *)0x0) {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
  }
  *pTVar3 = param_1;
  pTVar3[1] = 0;
                    /* try { // try from 0035343b to 0035343f has its CatchHandler @ 003534b9 */
  KoID::KoID((KoID *)(pTVar3 + 2),param_2);
  pTVar3[6] = 0;
  pTVar3[7] = 0;
  pTVar3[8] = 0x80000000;
  *(long **)(pTVar3 + 10) = plVar2;
  if (plVar2 != (long *)0x0) {
    plVar1 = plVar2 + 1;
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    *(undefined2 *)(pTVar3 + 0xc) = 0;
    *(Type **)(this + 0x10) = pTVar3;
    LOCK();
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 != 0) {
      return;
    }
                    /* WARNING: Could not recover jumptable at 0x003534ab. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar2 + 8))(plVar2);
    return;
  }
  *(Type **)(this + 0x10) = pTVar3;
  *(undefined2 *)(pTVar3 + 0xc) = 0;
  return;
}



// ====== KisUniformPaintOpProperty @ 003534d0 ======

/* KisUniformPaintOpProperty::KisUniformPaintOpProperty(KoID const&,
   KisRestrictedSharedPtr<KisPaintOpSettings>, QObject*) */

void __thiscall
KisUniformPaintOpProperty::KisUniformPaintOpProperty
          (KisUniformPaintOpProperty *this,KoID *param_1,KisRestrictedSharedPtr param_2,
          QObject *param_3)

{
  long *plVar1;
  long *plVar2;
  undefined8 *puVar3;
  undefined4 in_register_00000014;
  
  QObject::QObject((QObject *)this,param_3);
  *(undefined **)this = PTR_vtable_00836fd8 + 0x10;
                    /* try { // try from 00353503 to 00353507 has its CatchHandler @ 003535a7 */
  puVar3 = (undefined8 *)operator_new(0x38);
  plVar2 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (plVar2 != (long *)0x0) {
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
  }
  *puVar3 = 2;
                    /* try { // try from 00353527 to 0035352b has its CatchHandler @ 003535b3 */
  KoID::KoID((KoID *)(puVar3 + 1),param_1);
  puVar3[3] = 0;
  *(undefined4 *)(puVar3 + 4) = 0x80000000;
  puVar3[5] = plVar2;
  if (plVar2 != (long *)0x0) {
    plVar1 = plVar2 + 1;
    LOCK();
    *(int *)(plVar2 + 1) = *(int *)(plVar2 + 1) + 1;
    UNLOCK();
    *(undefined2 *)(puVar3 + 6) = 0;
    *(undefined8 **)(this + 0x10) = puVar3;
    LOCK();
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 != 0) {
      return;
    }
                    /* WARNING: Could not recover jumptable at 0x003535a5. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar2 + 8))(plVar2);
    return;
  }
  *(undefined8 **)(this + 0x10) = puVar3;
  *(undefined2 *)(puVar3 + 6) = 0;
  return;
}



