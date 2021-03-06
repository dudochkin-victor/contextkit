/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QtTest/QtTest>
#include <QtCore>
#include "fileutils.h"
#include "infoxmlbackend.h"
#include "contexttyperegistryinfo.h"

ContextTypeRegistryInfo* ContextTypeRegistryInfo::registryInstance = new ContextTypeRegistryInfo();

/* Mocked ContextTypeRegistryInfo */

ContextTypeRegistryInfo::ContextTypeRegistryInfo()
{
}

ContextTypeRegistryInfo* ContextTypeRegistryInfo::instance()
{
    return registryInstance;
}

AssocTree ContextTypeRegistryInfo::typeDefinitionForName(QString name)
{
    return AssocTree();
}

class InfoXmlBackendUnitTest : public QObject
{
    Q_OBJECT
    InfoXmlBackend *backend;

private:
    bool inSpyHasOneInList(QSignalSpy &spy, const QString &v);

private Q_SLOTS:
    void initTestCase();
    void paths();
    void name();
    void listKeys();
    void typeInfoForKey();
    void docForKey();
    void keyDeclared();
    void keyDeprecated();
    void providersForKey();
    void dynamics();
    void cleanupTestCase();
};

bool InfoXmlBackendUnitTest::inSpyHasOneInList(QSignalSpy &spy, const QString &v)
{
    if (spy.count() == 0)
        return false;

    while (spy.count() > 0) {
        QList<QVariant> args = spy.takeFirst();
        Q_FOREACH (QString s, args.at(0).toStringList()) {
            if (s == v)
                return true;
        }
    }

    return false;
}

void InfoXmlBackendUnitTest::initTestCase()
{
    utilCopyLocalAtomically("providers1.src", "providers1.context");
    utilCopyLocalWithRemove("providers2v1.src", "providers2.context");
    utilCopyLocalWithRemove("providers6.src", "providers6.context");
    utilSetEnv("CONTEXT_PROVIDERS", "./");
    utilSetEnv("CONTEXT_CORE_DECLARATIONS", "/dev/null");
    backend = new InfoXmlBackend();
}

void InfoXmlBackendUnitTest::listKeys()
{
    QStringList keys = backend->listKeys();

    QVERIFY(! keys.contains("System.ProcessingData"));

    QCOMPARE(keys.count(), 11);
    QVERIFY(keys.contains("Battery.ChargePercentage"));
    QVERIFY(keys.contains("Battery.LowBattery"));
    QVERIFY(keys.contains("Key.With.Attribute"));
    QVERIFY(keys.contains("Key.With.bool"));
    QVERIFY(keys.contains("Key.With.integer"));
    QVERIFY(keys.contains("Key.With.string"));
    QVERIFY(keys.contains("Key.With.double"));
    QVERIFY(keys.contains("Key.With.complex"));
    QVERIFY(keys.contains("Key.Deprecated"));
    QVERIFY(keys.contains("Battery.Charging"));
    QVERIFY(keys.contains("Battery.Voltage"));
}

void InfoXmlBackendUnitTest::name()
{
    QCOMPARE(backend->name(), QString("xml"));
}

void InfoXmlBackendUnitTest::typeInfoForKey()
{
    QCOMPARE(backend->typeInfoForKey("Battery.ChargePercentage").name(), QString(""));
    QCOMPARE(backend->typeInfoForKey("Key.With.Attribute").name(), QString("bool"));
    QCOMPARE(backend->typeInfoForKey("Battery.LowBattery").name(), QString("bool"));
    QCOMPARE(backend->typeInfoForKey("Key.With.bool").name(), QString("bool"));
    QCOMPARE(backend->typeInfoForKey("Key.With.integer").name(), QString("integer"));
    QCOMPARE(backend->typeInfoForKey("Key.With.string").name(), QString("string"));
    QCOMPARE(backend->typeInfoForKey("Key.With.double").name(), QString("double"));
    QCOMPARE(backend->typeInfoForKey("Battery.Charging").name(), QString("bool"));
    QCOMPARE(backend->typeInfoForKey("Battery.Voltage").name(), QString("integer"));
    QCOMPARE(backend->typeInfoForKey("Does.Not.Exist").name(), QString(""));

    ContextTypeInfo complexType = backend->typeInfoForKey("Key.With.complex");
    QCOMPARE(complexType.name(), QString("double"));
    QCOMPARE(complexType.parameterValue("min"), QVariant("0"));
    QCOMPARE(complexType.parameterValue("max"), QVariant("10"));
}

void InfoXmlBackendUnitTest::docForKey()
{
    QCOMPARE(backend->docForKey("Battery.ChargePercentage"), QString());
    QCOMPARE(backend->docForKey("Battery.LowBattery"), QString("This is true when battery is low"));
    QCOMPARE(backend->docForKey("Key.With.bool"), QString());
    QCOMPARE(backend->docForKey("Key.With.integer"), QString());
    QCOMPARE(backend->docForKey("Key.With.string"), QString());
    QCOMPARE(backend->docForKey("Key.With.double"), QString());
    QCOMPARE(backend->docForKey("Key.With.complex"), QString());
    QCOMPARE(backend->docForKey("Battery.Charging"), QString("This is true when battery is charging"));
    QCOMPARE(backend->docForKey("Does.Not.Exist"), QString());
}

void InfoXmlBackendUnitTest::keyDeclared()
{
    Q_FOREACH (QString key, backend->listKeys())
        QCOMPARE(backend->keyDeclared(key), true);

    QCOMPARE(backend->keyDeclared("Does.Not.Exist"), false);
    QCOMPARE(backend->keyDeclared("Battery.Charging"), true);
}

void InfoXmlBackendUnitTest::keyDeprecated()
{
    QCOMPARE(backend->keyDeprecated("Key.With.bool"), false);
    QCOMPARE(backend->keyDeprecated("Key.does.not.exist"), false);
    QCOMPARE(backend->keyDeprecated("Key.Deprecated"), true);
}

void InfoXmlBackendUnitTest::paths()
{
    QVERIFY(InfoXmlBackend::registryPath() == QString("./") ||
            InfoXmlBackend::registryPath() == QString("."));
    QCOMPARE(InfoXmlBackend::coreDeclPath(), QString("/dev/null"));
}

void InfoXmlBackendUnitTest::providersForKey()
{
    QList <ContextProviderInfo> list1 = backend->providersForKey("Battery.Charging");
    QCOMPARE(list1.count(), 1);
    QCOMPARE(list1.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(list1.at(0).constructionString, QString("system:org.freedesktop.ContextKit.contextd2"));

    QList <ContextProviderInfo> list2 = backend->providersForKey("Does.Not.Exist");
    QCOMPARE(list2.count(), 0);
}

void InfoXmlBackendUnitTest::dynamics()
{
    // Sanity check
    QStringList keys = backend->listKeys();
    QCOMPARE(keys.count(), 11);
    QVERIFY(keys.contains("Battery.Charging"));
    QCOMPARE(backend->keyDeclared("System.Active"), false);

    // Setup the spy observers
    QSignalSpy spy1(backend, SIGNAL(keysRemoved(QStringList)));
    QSignalSpy spy2(backend, SIGNAL(keysChanged(QStringList)));
    QSignalSpy spy3(backend, SIGNAL(keyChanged(QString)));
    QSignalSpy spy4(backend, SIGNAL(keysAdded(QStringList)));

    utilCopyLocalWithRemove("providers2v2.src", "providers2.context");
    utilCopyLocalWithRemove("providers3.src", "providers3.context");
    utilCopyLocalWithRemove("providers4.src", "providers4.context");
    utilCopyLocalWithRemove("providers5.src", "providers5.context");

    // Again, some basic check
    QCOMPARE(backend->keyDeclared("System.Active"), true);
    QCOMPARE(backend->typeInfoForKey("Battery.Charging").name(), QString("integer"));
    QCOMPARE(backend->typeInfoForKey("System.Active").name(), QString("bool"));
    QCOMPARE(backend->docForKey("System.Active"), QString("This is true when system is active"));

    // Test emissions
    QVERIFY(inSpyHasOneInList(spy1, "Battery.Voltage"));
    QVERIFY(inSpyHasOneInList(spy2, "Battery.Charging"));
    QVERIFY(inSpyHasOneInList(spy3, "Battery.Charging"));
    QVERIFY(inSpyHasOneInList(spy4, "System.Active"));

    // Check providers
    QList <ContextProviderInfo> list1 = backend->providersForKey("Battery.Charging");

    QCOMPARE(list1.count(), 1);
    QCOMPARE(list1.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(list1.at(0).constructionString, QString("system:org.freedesktop.ContextKit.contextd2"));

    QList <ContextProviderInfo> list2 = backend->providersForKey("System.Active");
    QCOMPARE(list2.count(), 3);
    QCOMPARE(list2.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(list2.at(0).constructionString, QString("system:com.nokia.daemon"));
    QCOMPARE(list2.at(1).plugin, QString("test.so"));
    QCOMPARE(list2.at(1).constructionString, QString("some-string"));
    QCOMPARE(list2.at(2).plugin, QString("another.so"));
    QCOMPARE(list2.at(2).constructionString, QString("some-other-string"));

    QList <ContextProviderInfo> list3 = backend->providersForKey("Battery.Voltage");
    QCOMPARE(list3.count(), 0);
}

void InfoXmlBackendUnitTest::cleanupTestCase()
{
     QFile::remove("providers1.context");
     QFile::remove("providers2.context");
     QFile::remove("providers3.context");
     QFile::remove("providers4.context");
     QFile::remove("providers5.context");
     QFile::remove("providers6.context");
}

#include "infoxmlbackendunittest.moc"
QTEST_MAIN(InfoXmlBackendUnitTest);
