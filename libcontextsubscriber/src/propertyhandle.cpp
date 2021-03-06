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

#include "propertyhandle.h"
#include "provider.h"
#include "sconnect.h"
#include "contextpropertyinfo.h"
#include "contexttypeinfo.h"
#include "contextregistryinfo.h"
#include "contextproviderinfo.h"
#include "dbusnamelistener.h"
#include "logging.h"
#include "loggingfeatures.h"

#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QReadLocker>
#include <QWriteLocker>

#include <stdlib.h>

namespace ContextSubscriber {

static const QDBusConnection::BusType commanderDBusType = QDBusConnection::SessionBus;
static const QString commanderDBusName = "org.freedesktop.ContextKit.Commander";
const ContextProviderInfo PropertyHandle::commanderInfo("contextkit-dbus", "session:org.freedesktop.ContextKit.Commander");

DBusNameListener* PropertyHandle::commanderListener = new DBusNameListener(commanderDBusType, commanderDBusName);
bool PropertyHandle::commandingEnabled = true;
bool PropertyHandle::typeCheckEnabled = false;

/*!
  \class PropertyHandle

  \brief A common handle for a context property.

  Only one handle exists at a time for a context property, no matter
  how much ContextProperty objects are created for it.

  Communication with the provider is done through the \c myProvider \c
  Provider instance, which is updated when needed
  because of registry changes.  Handling of disappearance from the
  DBus and then reappearance on the DBus of the same provider is
  handled privately by \c Provider.  If we don't know
  the current provider for this handle, then the \c myProvider pointer
  is 0.

  PropertyHandle and Provider instances are never deleted;
  they stick around until the process is terminated.

  All of the PropertyHandle instances and Property provider instances
  are always created with the \c QCoreApplication's thread as the
  thread where they live.  This is needed, because user threads can go
  away and we would like to have only one DBus connection.
*/

PropertyHandle::PropertyHandle(const QString& key)
    : myInfo(0), subscribeCount(0), myKey(key)
{
    // Read the information about the provider. This needs to be
    // done before calling updateProvider.
    myInfo = new ContextPropertyInfo(myKey, this);

    if (myInfo->deprecated())
        contextWarning() << F_DEPRECATION << "Property is deprecated:" << myKey;

    // Start listening to changes in property introspection (e.g., added to registry, plugin changes)
    sconnect(myInfo, SIGNAL(changed(QString)),
             this, SLOT(updateProvider()));

    // Check whether we should listen to commander at all.  This extra if just
    // optimizes away getenv in case we've already checked it and noticed
    // commanding is not enabled.
    if (commandingEnabled) {
        const char *commanding = getenv("CONTEXT_COMMANDING");
        if (!commanding)
            commandingEnabled = false;
    }

    if (commandingEnabled) {
        // Start listening for the context commander, and also initiate a
        // NameHasOwner check.

        // Because of the waitForSubscription() feature, we immediately need to
        // subscribe to the real providers when the commander presence becomes
        // known.  So, these connect()s need to be synchronous (not queued).
        sconnect(commanderListener, SIGNAL(nameAppeared()),
                 this, SLOT(updateProvider()));
        sconnect(commanderListener, SIGNAL(nameDisappeared()),
                 this, SLOT(updateProvider()));

        commanderListener->startListening(true);

        // Check if commander is already there:
        DBusNameListener::ServicePresence commanderPresence = commanderListener->isServicePresent();
        if (commanderPresence != DBusNameListener::Unknown) {
            // The status of the commander is known, so we can connect to
            // the provider (or commander) immediately.
            updateProvider();
        }
        // Otherwise, delay connecting to the provider until we know
        // whether commander is present.
    }
    else {
        // Don't wait for the commander; just connect to the real provider
        // immediately.
        updateProvider();
    }

    // Move the PropertyHandle (and all children) to main thread.
    moveToThread(QCoreApplication::instance()->thread());
}

void PropertyHandle::ignoreCommander()
{
    commandingEnabled = false;
}

void PropertyHandle::setTypeCheck(bool typeCheck)
{
    typeCheckEnabled = typeCheck;
}

/// Decides who is the current provider of this property and sets up
/// \c myProvider accordingly.  If the provider has changed then
/// renews the subscriptions.
void PropertyHandle::updateProvider()
{
    QList<Provider*> newProviders;
    contextDebug() << F_PLUGINS;

    if (commandingEnabled && commanderListener->isServicePresent() == DBusNameListener::Present) {
        // If commander is present it should be able to override the
        // property, so connect to it.
        newProviders << Provider::instance(commanderInfo);
        Provider::instance(commanderInfo)->clearValues();
    } else {
        // The myInfo object doesn't have to be re-created, because it
        // just routes the function calls to a registry backend.

        Q_FOREACH (ContextProviderInfo info, myInfo->providers())
            newProviders << Provider::instance(info);
        contextDebug() << newProviders.size() << "providers for" << myKey;
    }
    if (subscribeCount > 0) {
        // Unsubscribe from old providers and subscribe to the new ones.
        Q_FOREACH (Provider *oldprovider, myProviders)
            oldprovider->unsubscribe(myKey);
        pendingSubscriptions.clear();
        Q_FOREACH (Provider *newprovider, newProviders)
            if (newprovider->subscribe(myKey))
                pendingSubscriptions << newprovider;
    }
    myProviders = newProviders;
    // If all subscriptions succeeded immediately, then we have to trigger
    // recomputing the value now.  Otherwise we rely on the
    // subscribeFinished signal.
    if (subscribeCount > 0 && pendingSubscriptions.empty())
        onValueChanged();
}

/// Sets \c subscribePending to false.
void PropertyHandle::setSubscribeFinished(Provider *provider)
{
    pendingSubscriptions.remove(provider);
}

/// Increase the \c subscribeCount of this context property and
/// subscribe to it through the \c myProvider instance if neccessary.
void PropertyHandle::subscribe()
{
    contextDebug() << F_THREADS << "PropertyHandle::subscribe" << QThread::currentThread();

    QMutexLocker locker(&subscribeCountLock);
    ++subscribeCount;
    if (subscribeCount == 1) {
        pendingSubscriptions.clear();
        Q_FOREACH (Provider *provider, myProviders)
            if (provider->subscribe(myKey))
                pendingSubscriptions << provider;
    }
}

/// Decrease the \c subscribeCount of this context property and
/// unsubscribe from it through the \c myProvider instance if
/// neccessary.
void PropertyHandle::unsubscribe()
{
    QMutexLocker locker(&subscribeCountLock);
    --subscribeCount;
    if (subscribeCount == 0) {
        pendingSubscriptions.clear();
        Q_FOREACH (Provider *provider, myProviders)
            provider->unsubscribe(myKey);
    }
}

QString PropertyHandle::key() const
{
    return myKey;
}

QVariant PropertyHandle::value() const
{
    QReadLocker lock(&valueLock);
    return myValue;
}

bool PropertyHandle::isSubscribePending() const
{
    // We wait until commander presence is unknown ...
    if (commandingEnabled &&
        commanderListener->isServicePresent() == DBusNameListener::Unknown)
        return true;
    // ... or until we get some value ...
    if (!myValue.isNull())
        return false;
    // ... or all pending subscriptions finished.
    return pendingSubscriptions.size() != 0;
}

/// Used by the \c HandleSignalRouter to change the value of the
/// property.  Before changing the value it checks the type if type
/// checks are enabled.  The verification errors are signalled on the
/// stderr.  After the check it updates the value and emits the
/// valueChanged() signal.
void PropertyHandle::onValueChanged()
{
    bool found = false;
    TimedValue latest = QVariant();

    Q_FOREACH (Provider *provider, myProviders) {
        TimedValue current = provider->get(myKey);
        if (current.value.isNull())
            continue;
        if (!found) {
            found = true;
            latest = current;
        } else if (latest < current)
            latest = current;
    }
    QVariant newValue;
    if (found)
        newValue = latest.value;

    if (typeCheckEnabled && !newValue.isNull()) {
        ContextTypeInfo typeInfo = myInfo->typeInfo();
        if (!typeInfo.typeCheck(newValue)) {
            contextCritical() << F_TYPES << "Type check failed for" << myKey
                              << "wanted:" << typeInfo.name();
            return;
        }
    }

    QWriteLocker lock(&valueLock);
    // Since QVariant(QVariant::Int) == QVariant(0), it's not enough to check
    // whether myValue and newValue are unequal.  Also, for completeness we
    // don't want to lose a valueChanged signal if the type changes.
    if (myValue != newValue ||
        myValue.isNull() != newValue.isNull() ||
        myValue.type() != newValue.type())
    {
        myValue = newValue;
        Q_EMIT valueChanged();
    }
}

void PropertyHandle::blockUntilSubscribed()
{
    // Call blockUntilSubscribed once per each provider in pendingSubscriptions.
    // Making the call might or might not result in removing the provider from
    // pendingSubscriptions (depending on whether some events on the way are
    // queued or not), so make no assumptions on that.
    QSet<Provider*> pendingSubscriptionsCopy = pendingSubscriptions;
    while (pendingSubscriptionsCopy.size() > 0) {
        Provider* provider = *(pendingSubscriptionsCopy.constBegin());
        provider->blockUntilSubscribed(myKey);
        pendingSubscriptionsCopy.remove(provider);
    }
}

const ContextPropertyInfo* PropertyHandle::info() const
{
    return myInfo;
}

PropertyHandle* PropertyHandle::instance(const QString& key)
{
    // Container for singletons
    static QMap<QString, PropertyHandle*> handleInstances;

    // Protect the handleInstances. Documentation of QMap doesn't tell
    // if it can be read concurrently, so, read-write locking might
    // not be enough.

    static QMutex handleInstancesLock;
    QMutexLocker locker(&handleInstancesLock);
    if (!handleInstances.contains(key)) {
        // The handle does not exist, so create it
        handleInstances.insert(key, new PropertyHandle(key));
    }

    return handleInstances[key];
}

} // end namespace
