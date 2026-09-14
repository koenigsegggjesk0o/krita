/* Class KisBookmarkedConfigurationManager - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBookmarkedConfigurationManager @ 0020d4a0 ======

void __thiscall
KisBookmarkedConfigurationManager::KisBookmarkedConfigurationManager
          (KisBookmarkedConfigurationManager *this,QString *param_1,
          KisSerializableConfigurationFactory *param_2)

{
  (*(code *)PTR_KisBookmarkedConfigurationManager_0083e520)();
  return;
}



// ====== KisBookmarkedConfigurationManager @ 00466dd0 ======

/* KisBookmarkedConfigurationManager::KisBookmarkedConfigurationManager(QString const&,
   KisSerializableConfigurationFactory*) */

void __thiscall
KisBookmarkedConfigurationManager::KisBookmarkedConfigurationManager
          (KisBookmarkedConfigurationManager *this,QString *param_1,
          KisSerializableConfigurationFactory *param_2)

{
  undefined *puVar1;
  QString *this_00;
  
  this_00 = (QString *)operator_new(0x10);
  puVar1 = PTR_shared_null_008377d0;
  *(QString **)this = this_00;
  *(undefined **)this_00 = puVar1;
  QString::operator=(this_00,(QString *)param_1);
  *(KisSerializableConfigurationFactory **)(*(long *)this + 8) = param_2;
  return;
}



