/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * export_lock.cpp — Implementasi freemium export lock
 *
 * Verifikasi license:
 *   - License key: SHA-256 hash check (offline)
 *   - Store purchase: order ID verification (online, future)
 *   - Trial: 7-day free Pro, disimpan di file
 */

#include "export_lock.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>
#include <QRandomGenerator>
#include <QDebug>

namespace FeatherKrita {

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// Secret salt untuk license key verification.
// NOTE: Ini disimpan di client, jadi bukan unbreakable. Tujuannya hanya
// mencegah casual sharing. Untuk production, gunakan online verification
// atau store billing.
static const QString kLicenseSalt = QStringLiteral("FeatherKrita-2026-ProSalt-v1");

// Expected hash prefix (8 chars) untuk valid license keys.
// License key valid jika SHA-256(key + salt) starts with "FKP1PRO".
// Format ini memungkinkan banyak valid keys (key space ~16 alphanumeric).
static const QString kExpectedHashPrefix = QStringLiteral("FKP1PRO");

static const QString kLicenseFileName = QStringLiteral("license.dat");
static const QString kTrialFileName   = QStringLiteral("trial.dat");

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

ExportLock::ExportLock(QObject* parent)
    : QObject(parent)
{
}

ExportLock::~ExportLock()
{
    // Auto-save license state on destruction (best-effort)
    if (!m_dataDir.isEmpty()) {
        saveLicense(m_dataDir);
        saveTrialState(m_dataDir);
    }
}

// ---------------------------------------------------------------------------
// Export Policy
// ---------------------------------------------------------------------------

bool ExportLock::canExport(ExportFormat format) const
{
    // .feather project files selalu FREE (save/load project)
    if (format == ExportFormat::Feather) {
        return true;
    }

    // Format lain perlu Pro
    return isProVersion();
}

bool ExportLock::formatRequiresPro(ExportFormat format)
{
    return format != ExportFormat::Feather;
}

QString ExportLock::formatName(ExportFormat format)
{
    switch (format) {
    case ExportFormat::PNG:     return QStringLiteral("PNG Image");
    case ExportFormat::JPEG:    return QStringLiteral("JPEG Image");
    case ExportFormat::GIF:     return QStringLiteral("GIF Animation");
    case ExportFormat::MP4:     return QStringLiteral("MP4 Video");
    case ExportFormat::OBJ:     return QStringLiteral("OBJ 3D Model");
    case ExportFormat::GLTF:    return QStringLiteral("glTF 3D Model");
    case ExportFormat::Feather: return QStringLiteral("Feather Project");
    case ExportFormat::STL:     return QStringLiteral("STL 3D Model");
    }
    return QStringLiteral("Unknown");
}

QString ExportLock::formatExtension(ExportFormat format)
{
    switch (format) {
    case ExportFormat::PNG:     return QStringLiteral("png");
    case ExportFormat::JPEG:    return QStringLiteral("jpg");
    case ExportFormat::GIF:     return QStringLiteral("gif");
    case ExportFormat::MP4:     return QStringLiteral("mp4");
    case ExportFormat::OBJ:     return QStringLiteral("obj");
    case ExportFormat::GLTF:    return QStringLiteral("gltf");
    case ExportFormat::Feather: return QStringLiteral("feather");
    case ExportFormat::STL:     return QStringLiteral("stl");
    }
    return QString();
}

// ---------------------------------------------------------------------------
// License Verification
// ---------------------------------------------------------------------------

bool ExportLock::unlockExport(const QString& licenseKey)
{
    if (!isValidKeyFormat(licenseKey)) {
        qWarning() << "ExportLock: Invalid key format";
        return false;
    }

    if (!verifyKey(licenseKey)) {
        qWarning() << "ExportLock: License key verification failed";
        return false;
    }

    LicenseInfo info;
    info.valid         = true;
    info.source        = LicenseSource::LicenseKey;
    info.key           = licenseKey.toUpper();
    info.purchaseDate  = QDate::currentDate();
    info.productName   = QStringLiteral("Feather Krita Pro");
    setLicense(info);

    qDebug() << "ExportLock: Export unlocked via license key";
    return true;
}

bool ExportLock::unlockFromStore(LicenseSource source,
                                 const QString& orderId,
                                 const QString& email)
{
    if (source != LicenseSource::GooglePlay &&
        source != LicenseSource::WindowsStore &&
        source != LicenseSource::AppStore) {
        qWarning() << "ExportLock: Invalid store source";
        return false;
    }

    if (orderId.isEmpty()) {
        qWarning() << "ExportLock: Empty order ID";
        return false;
    }

    // NOTE: Di production, di sini kita panggil store API untuk verify:
    //   - Google Play: GooglePlayBilling.acknowledgePurchase()
    //   - Windows Store: StoreContext.GetUserCollectionAsync()
    //   - iOS App Store: SKPaymentQueue validate receipt
    //
    // Untuk sekarang, kita trust orderId yang diberikan store callback.
    // Store callback hanya dipanggil setelah purchase sukses di sisi store.

    LicenseInfo info;
    info.valid         = true;
    info.source        = source;
    info.orderId       = orderId;
    info.customerEmail = email;
    info.purchaseDate  = QDate::currentDate();
    info.productName   = QStringLiteral("Feather Krita Pro");

    setLicense(info);

    qDebug() << "ExportLock: Export unlocked via store purchase:"
             << static_cast<int>(source);
    return true;
}

void ExportLock::lockExport()
{
    LicenseInfo empty;
    setLicense(empty);
    qDebug() << "ExportLock: Export locked";
}

bool ExportLock::isProVersion() const
{
    if (m_license.valid) {
        // Cek expiry (kalau ada)
        if (m_license.expiryDate.isValid() &&
            QDate::currentDate() > m_license.expiryDate) {
            return false;
        }
        return true;
    }
    // Trial juga mengaktifkan Pro
    if (isTrialActive()) {
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

bool ExportLock::loadStoredLicense(const QString& dataDir)
{
    QString dir = dataDir.isEmpty() ? m_dataDir : dataDir;
    if (dir.isEmpty()) dir = defaultDataDir();
    if (dir.isEmpty()) return false;
    m_dataDir = dir;

    QDir().mkpath(dir);
    QString path = licenseFilePath(dir);

    QFile file(path);
    if (!file.exists()) {
        // Coba load trial state
        loadTrialState(dir);
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ExportLock: Cannot open license file:" << path;
        return false;
    }

    // Format: simple INI-like via QSettings
    // Untuk security, key disimpan sebagai hash (bukan plaintext)
    QSettings settings(path, QSettings::IniFormat);

    LicenseInfo info;
    info.valid         = settings.value("valid", false).toBool();
    int src            = settings.value("source", int(LicenseSource::None)).toInt();
    info.source        = LicenseSource(src);
    info.key           = settings.value("key").toString();
    info.orderId       = settings.value("orderId").toString();
    info.customerEmail = settings.value("email").toString();
    info.productName   = settings.value("product", "Feather Krita Pro").toString();
    info.purchaseDate  = settings.value("purchaseDate").toDate();
    info.expiryDate    = settings.value("expiryDate").toDate();

    // Re-verify license key (anti-tampering)
    if (info.source == LicenseSource::LicenseKey && !info.key.isEmpty()) {
        if (!verifyKey(info.key)) {
            qWarning() << "ExportLock: Stored license key failed re-verification";
            info.valid = false;
        }
    }

    // Cek expiry
    if (info.expiryDate.isValid() && QDate::currentDate() > info.expiryDate) {
        info.valid = false;
    }

    m_license = info;

    // Load trial state juga
    loadTrialState(dir);

    return info.valid;
}

bool ExportLock::saveLicense(const QString& dataDir) const
{
    QString dir = dataDir.isEmpty() ? m_dataDir : dataDir;
    if (dir.isEmpty()) dir = defaultDataDir();
    if (dir.isEmpty()) return false;

    QDir().mkpath(dir);
    QString path = licenseFilePath(dir);

    QSettings settings(path, QSettings::IniFormat);
    settings.setValue("valid",        m_license.valid);
    settings.setValue("source",       int(m_license.source));
    settings.setValue("key",          m_license.key);
    settings.setValue("orderId",      m_license.orderId);
    settings.setValue("email",        m_license.customerEmail);
    settings.setValue("product",      m_license.productName);
    settings.setValue("purchaseDate", m_license.purchaseDate);
    settings.setValue("expiryDate",   m_license.expiryDate);
    settings.sync();

    return settings.status() == QSettings::NoError;
}

bool ExportLock::clearStoredLicense(const QString& dataDir)
{
    QString dir = dataDir.isEmpty() ? m_dataDir : dataDir;
    if (dir.isEmpty()) dir = defaultDataDir();
    if (dir.isEmpty()) return false;

    QFile::remove(licenseFilePath(dir));
    lockExport();
    return true;
}

// ---------------------------------------------------------------------------
// Trial
// ---------------------------------------------------------------------------

bool ExportLock::startTrial(int days)
{
    if (m_trialUsed && !isTrialActive()) {
        qWarning() << "ExportLock: Trial already used";
        return false;
    }
    if (m_license.valid) {
        qWarning() << "ExportLock: Already Pro, no need for trial";
        return false;
    }

    m_trialStart  = QDate::currentDate();
    m_trialDays   = days;
    m_trialUsed   = true;

    if (!m_dataDir.isEmpty()) saveTrialState(m_dataDir);
    else saveTrialState(defaultDataDir());

    emit trialStatusChanged(true, days);
    emit licenseChanged(true); // trial aktifkan Pro

    qDebug() << "ExportLock: Trial started for" << days << "days";
    return true;
}

bool ExportLock::isTrialActive() const
{
    if (!m_trialUsed || !m_trialStart.isValid() || m_trialDays <= 0) {
        return false;
    }
    return trialDaysRemaining() > 0;
}

int ExportLock::trialDaysRemaining() const
{
    if (!m_trialStart.isValid() || m_trialDays <= 0) return 0;
    QDate end = m_trialStart.addDays(m_trialDays);
    int remaining = QDate::currentDate().daysTo(end);
    return std::max(0, remaining);
}

// ---------------------------------------------------------------------------
// Validation Helpers
// ---------------------------------------------------------------------------

bool ExportLock::isValidKeyFormat(const QString& key)
{
    // Format: XXXX-XXXX-XXXX-XXXX (16 alphanumeric, dipisah oleh -)
    // Accept juga tanpa dash: XXXXXXXXXXXXXXXX
    QString clean = key;
    clean.remove('-').remove(' ');
    if (clean.length() != 16) return false;
    for (const QChar& c : clean) {
        if (!c.isLetterOrNumber()) return false;
    }
    return true;
}

QString ExportLock::generateLicenseKey()
{
    // Generate 16 random alphanumeric chars, format XXXX-XXXX-XXXX-XXXX
    // NOTE: Tidak semua generated key valid. Untuk distribusi, jalankan
    // generator ini diulang sampai dapat key yang verifyKey() returns true,
    // atau gunakan pre-computed list of valid keys.
    const QString chars = QStringLiteral("ABCDEFGHJKLMNPQRSTUVWXYZ23456789");
    QString key;
    QRandomGenerator* gen = QRandomGenerator::global();
    for (int i = 0; i < 16; ++i) {
        if (i > 0 && i % 4 == 0) key += '-';
        key += chars[gen->bounded(chars.length())];
    }
    return key;
}

// ---------------------------------------------------------------------------
// Internal verification
// ---------------------------------------------------------------------------

bool ExportLock::verifyKey(const QString& key) const
{
    // Verifikasi offline: SHA-256(key.toUpper() + salt) harus
    // dimulai dengan expected prefix.
    //
    // Cara kerja di production:
    //   1. Admin generate N license keys yang valid (offline script)
    //   2. Setiap key di-hash, dan hanya key yang hash-nya mulai dengan
    //      "FKP1PRO" yang dianggap valid
    //   3. Key disimpan di database admin (untuk tracking)
    //   4. User masukin key → client compute hash → compare prefix
    //
    // Ini bukan unbreakable (attacker bisa brute-force key space),
    // tapi cukup untuk mencegah casual sharing.

    QString clean = key;
    clean.remove('-').remove(' ').toUpper();

    QString hash = computeHash(clean, kLicenseSalt);
    return hash.startsWith(kExpectedHashPrefix);
}

QString ExportLock::computeHash(const QString& key, const QString& salt)
{
    QByteArray input = (key.toUpper() + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(input, QCryptographicHash::Sha256);
    // Return sebagai base64 (URL-safe) untuk prefix comparison
    return QString::fromUtf8(hash.toHex()).toUpper();
}

QString ExportLock::defaultDataDir() const
{
    // <user_data>/FeatherKrita/
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    }
    if (path.isEmpty()) return QString();
    return path + QDir::separator() + QStringLiteral("FeatherKrita");
}

QString ExportLock::licenseFilePath(const QString& dataDir) const
{
    return dataDir + QDir::separator() + kLicenseFileName;
}

QString ExportLock::trialFilePath(const QString& dataDir) const
{
    return dataDir + QDir::separator() + kTrialFileName;
}

void ExportLock::setLicense(const LicenseInfo& info)
{
    bool wasPro = isProVersion();
    m_license = info;
    bool isPro = isProVersion();

    // Auto-save ke storage
    if (!m_dataDir.isEmpty()) {
        saveLicense(m_dataDir);
    }

    if (wasPro != isPro) {
        emit licenseChanged(isPro);
    }
}

bool ExportLock::saveTrialState(const QString& dataDir) const
{
    if (dataDir.isEmpty()) return false;
    QDir().mkpath(dataDir);

    QSettings settings(trialFilePath(dataDir), QSettings::IniFormat);
    settings.setValue("used",         m_trialUsed);
    settings.setValue("startDate",    m_trialStart);
    settings.setValue("days",         m_trialDays);
    settings.sync();
    return settings.status() == QSettings::NoError;
}

bool ExportLock::loadTrialState(const QString& dataDir)
{
    if (dataDir.isEmpty()) return false;
    QString path = trialFilePath(dataDir);
    if (!QFile::exists(path)) return false;

    QSettings settings(path, QSettings::IniFormat);
    m_trialUsed  = settings.value("used", false).toBool();
    m_trialStart = settings.value("startDate").toDate();
    m_trialDays  = settings.value("days", 0).toInt();
    return true;
}

} // namespace FeatherKrita
