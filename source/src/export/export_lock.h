/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * export_lock.h — Freemium export lock untuk Feather-Krita app
 *
 * Model bisnis: FREE TO USE, PAY TO EXPORT.
 * - Drawing, editing, saving project files: GRATIS
 * - Export ke PNG/JPEG/OBJ/GLTF/MP4/GIF: TERKUNCI (perlu Pro license)
 *
 * Verifikasi license:
 *   1. License key (offline, cryptographic hash check)
 *   2. Store purchase (Google Play Billing / Windows Store)
 *
 * File ini TIDAK mengubah kode Krita. Hanya mengatur policy export.
 */

#ifndef EXPORT_LOCK_H
#define EXPORT_LOCK_H

#include <QString>
#include <QDate>
#include <QObject>

namespace FeatherKrita {

/**
 * ExportFormat — format export yang dikunci
 */
enum class ExportFormat {
    PNG,
    JPEG,
    GIF,
    MP4,
    OBJ,
    GLTF,
    Feather,        // .feather project file (FREE, tidak terkunci)
    STL
};

/**
 * LicenseSource — sumber license
 */
enum class LicenseSource {
    None,           // Belum ada license
    LicenseKey,     // License key (offline verification)
    GooglePlay,     // Google Play Billing purchase
    WindowsStore,   // Microsoft Store purchase
    AppStore        // iOS App Store purchase (future)
};

/**
 * LicenseInfo — info license yang tersimpan
 */
struct LicenseInfo {
    bool          valid       = false;
    LicenseSource source      = LicenseSource::None;
    QString       key;            // License key (jika LicenseKey)
    QString       orderId;        // Store order ID (jika dari store)
    QDate         purchaseDate;
    QDate         expiryDate;     // Optional expiry (default: lifetime)
    QString       customerEmail;
    QString       productName;    // "Feather Krita Pro"
};

/**
 * ExportLock — Manages freemium export policy
 *
 * Cara pakai:
 *
 *   ExportLock lock;
 *   lock.loadStoredLicense();
 *
 *   if (lock.canExport(ExportFormat::PNG)) {
 *       // ... lakukan export PNG
 *   } else {
 *       // Tampilkan "Upgrade to Pro" dialog
 *       showUpgradeDialog();
 *   }
 *
 *   // Setelah user bayar & dapat license key:
 *   bool ok = lock.unlockExport("XXXX-XXXX-XXXX-XXXX");
 */
class ExportLock : public QObject {
    Q_OBJECT

public:
    explicit ExportLock(QObject* parent = nullptr);
    ~ExportLock();

    // === Export Policy ===

    /**
     * Cek apakah user bisa export ke format tertentu
     *
     * Format .feather (project file) selalu FREE.
     * Format lain (PNG, JPEG, OBJ, dll) perlu Pro license.
     */
    bool canExport(ExportFormat format) const;

    /**
     * Cek apakah format tertentu memerlukan Pro license
     */
    static bool formatRequiresPro(ExportFormat format);

    /**
     * Get nama format (untuk display)
     */
    static QString formatName(ExportFormat format);

    /**
     * Get ekstensi file untuk format
     */
    static QString formatExtension(ExportFormat format);

    // === License Verification ===

    /**
     * Unlock export dengan license key (offline verification)
     *
     * License key format: XXXX-XXXX-XXXX-XXXX (16 alphanumeric)
     * Verifikasi: SHA-256 hash dari key + secret salt == expected hash
     *
     * @param licenseKey License key dari user
     * @return true jika license valid & export unlocked
     */
    bool unlockExport(const QString& licenseKey);

    /**
     * Unlock export dari store purchase
     *
     * @param source Source store (GooglePlay / WindowsStore / AppStore)
     * @param orderId Order ID dari store
     * @param email   Email customer (optional)
     * @return true jika purchase valid
     */
    bool unlockFromStore(LicenseSource source,
                         const QString& orderId,
                         const QString& email = QString());

    /**
     * Lock export (logout / revoke license)
     */
    void lockExport();

    /**
     * Cek apakah user punya Pro version
     */
    bool isProVersion() const;

    /**
     * Get info license saat ini
     */
    LicenseInfo licenseInfo() const { return m_license; }

    // === Persistence ===

    /**
     * Load license dari persistent storage (file/settings)
     * File: <app_data>/license.dat
     *
     * @param dataDir App data directory
     * @return true jika license loaded & valid
     */
    bool loadStoredLicense(const QString& dataDir = QString());

    /**
     * Save license ke persistent storage
     */
    bool saveLicense(const QString& dataDir = QString()) const;

    /**
     * Hapus license dari storage
     */
    bool clearStoredLicense(const QString& dataDir = QString());

    /**
     * Set custom data directory (untuk testing)
     */
    void setDataDirectory(const QString& dir) { m_dataDir = dir; }
    QString dataDirectory() const { return m_dataDir; }

    // === Trial (optional, untuk flexibility) ===

    /**
     * Start trial period (e.g. 7 days free Pro)
     * @param days Trial duration in days
     * @return true jika trial berhasil dimulai
     */
    bool startTrial(int days = 7);

    /**
     * Cek apakah trial sedang aktif
     */
    bool isTrialActive() const;

    /**
     * Get sisa hari trial (0 jika expired)
     */
    int trialDaysRemaining() const;

    // === Validation Helpers ===

    /**
     * Validate license key format (XXXX-XXXX-XXXX-XXXX)
     */
    static bool isValidKeyFormat(const QString& key);

    /**
     * Generate license key (untuk admin/store tools)
     * Format: 16 alphanumeric dalam 4 group
     */
    static QString generateLicenseKey();

signals:
    /**
     * Emitted saat license status berubah (locked/unlocked)
     */
    void licenseChanged(bool isPro);

    /**
     * Emitted saat trial dimulai atau expired
     */
    void trialStatusChanged(bool active, int daysRemaining);

private:
    LicenseInfo m_license;
    QString     m_dataDir;

    // Trial state
    QDate       m_trialStart;
    int         m_trialDays = 0;
    bool        m_trialUsed = false;

    // === Internal verification ===

    /**
     * Verify license key offline (cryptographic hash check)
     * License key di-hash dengan secret salt dan dibandingkan
     * dengan expected hash yang sudah di-hardcode.
     *
     * NOTE: Ini bukan cryptography yang unbreakable. Tujuannya hanya
     * untuk mencegah casual sharing of license keys. Untuk protection
     * yang lebih kuat, gunakan store-based verification.
     */
    bool verifyKey(const QString& key) const;

    /**
     * Compute hash dari license key + salt
     */
    static QString computeHash(const QString& key, const QString& salt);

    /**
     * Get default data directory
     */
    QString defaultDataDir() const;

    /**
     * Get license file path
     */
    QString licenseFilePath(const QString& dataDir) const;

    /**
     * Get trial file path
     */
    QString trialFilePath(const QString& dataDir) const;

    /**
     * Update internal state & emit signals
     */
    void setLicense(const LicenseInfo& info);

    /**
     * Save/load trial state
     */
    bool saveTrialState(const QString& dataDir) const;
    bool loadTrialState(const QString& dataDir);
};

} // namespace FeatherKrita

#endif // EXPORT_LOCK_H
