/* Class KisPropertiesConfiguration - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPropertiesConfiguration @ 00201d70 ======

void __thiscall
KisPropertiesConfiguration::KisPropertiesConfiguration
          (KisPropertiesConfiguration *this,KisPropertiesConfiguration *param_1)

{
  (*(code *)PTR_KisPropertiesConfiguration_00838988)();
  return;
}



// ====== KisPropertiesConfiguration @ 00206510 ======

void __thiscall
KisPropertiesConfiguration::KisPropertiesConfiguration(KisPropertiesConfiguration *this)

{
  (*(code *)PTR_KisPropertiesConfiguration_0083ad58)();
  return;
}



// ====== KisPropertiesConfiguration @ 0020a330 ======

void __thiscall
KisPropertiesConfiguration::KisPropertiesConfiguration(KisPropertiesConfiguration *this)

{
  (*(code *)PTR_KisPropertiesConfiguration_0083cc68)();
  return;
}



// ====== KisPropertiesConfiguration @ 005e9050 ======

/* KisPropertiesConfiguration::KisPropertiesConfiguration() */

void __thiscall
KisPropertiesConfiguration::KisPropertiesConfiguration(KisPropertiesConfiguration *this)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined8 *puVar3;
  
  KisSerializableConfiguration::KisSerializableConfiguration((KisSerializableConfiguration *)this);
  *(undefined **)this = PTR_vtable_008374d0 + 0x10;
                    /* try { // try from 005e9075 to 005e9079 has its CatchHandler @ 005e9097 */
  puVar3 = (undefined8 *)operator_new(0x10);
  puVar2 = PTR_shared_null_008372c0;
  *(undefined8 **)(this + 0x18) = puVar3;
  puVar1 = PTR_shared_null_00836c40;
  *puVar3 = puVar2;
  puVar3[1] = puVar1;
  return;
}



// ====== KisPropertiesConfiguration @ 005ecb70 ======

/* KisPropertiesConfiguration::KisPropertiesConfiguration(KisPropertiesConfiguration const&) */

void __thiscall
KisPropertiesConfiguration::KisPropertiesConfiguration
          (KisPropertiesConfiguration *this,KisPropertiesConfiguration *param_1)

{
  code *pcVar1;
  long *plVar2;
  _func_void_Node_ptr_void_ptr *p_Var3;
  long lVar4;
  ulong *puVar5;
  long *plVar6;
  int *piVar7;
  undefined8 uVar8;
  long lVar9;
  _func_void_Node_ptr *p_Var10;
  
  KisSerializableConfiguration::KisSerializableConfiguration
            ((KisSerializableConfiguration *)this,(KisSerializableConfiguration *)param_1);
  *(undefined **)this = PTR_vtable_008374d0 + 0x10;
                    /* try { // try from 005ecb9d to 005ecba1 has its CatchHandler @ 005eccd6 */
  plVar6 = (long *)operator_new(0x10);
  plVar2 = *(long **)(param_1 + 0x18);
  piVar7 = (int *)*plVar2;
  if (*piVar7 == 0) {
                    /* try { // try from 005ecc00 to 005ecc3f has its CatchHandler @ 005ecce2 */
    lVar9 = QMapDataBase::createData();
    *plVar6 = lVar9;
    if (*(long *)(*plVar2 + 0x10) != 0) {
      uVar8 = FUN_00322080(*(long *)(*plVar2 + 0x10),lVar9);
      lVar4 = *plVar6;
      *(undefined8 *)(lVar9 + 0x10) = uVar8;
      puVar5 = *(ulong **)(lVar4 + 0x10);
      *puVar5 = (ulong)((uint)*puVar5 & 3) | lVar4 + 8U;
      QMapDataBase::recalcMostLeftNode();
    }
  }
  else {
    if (*piVar7 != -1) {
      LOCK();
      *piVar7 = *piVar7 + 1;
      UNLOCK();
      piVar7 = (int *)*plVar2;
    }
    *plVar6 = (long)piVar7;
  }
  lVar9 = plVar2[1];
  plVar6[1] = lVar9;
  if (1 < *(int *)(lVar9 + 0x10) + 1U) {
    LOCK();
    *(int *)(lVar9 + 0x10) = *(int *)(lVar9 + 0x10) + 1;
    UNLOCK();
  }
  p_Var3 = (_func_void_Node_ptr_void_ptr *)plVar6[1];
  if ((((byte)p_Var3[0x28] & 1) != 0) || (*(uint *)(p_Var3 + 0x10) < 2)) {
    *(long **)(this + 0x18) = plVar6;
    return;
  }
                    /* try { // try from 005ecc6c to 005eccc7 has its CatchHandler @ 005eccca */
  lVar9 = QHashData::detach_helper(p_Var3,FUN_00352320,0x352350,0x18);
  p_Var10 = (_func_void_Node_ptr *)plVar6[1];
  pcVar1 = p_Var10 + 0x10;
  if (*(int *)(p_Var10 + 0x10) != 0) {
    if (*(int *)(p_Var10 + 0x10) == -1) goto LAB_005ecc8e;
    LOCK();
    *(int *)pcVar1 = *(int *)pcVar1 + -1;
    UNLOCK();
    if (*(int *)pcVar1 != 0) goto LAB_005ecc8e;
    p_Var10 = (_func_void_Node_ptr *)plVar6[1];
  }
  QHashData::free_helper(p_Var10);
LAB_005ecc8e:
  plVar6[1] = lVar9;
  *(long **)(this + 0x18) = plVar6;
  return;
}



