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

#include "contextpropertyinfo.h"
#include "infobackend.h"
#include "sconnect.h"
#include "logging.h"
#include "loggingfeatures.h"
#include <QMutex>
#include <QMutexLocker>

/*!
    \page Introspection

    \brief The Context Framework maintains a registry defining which
    context properties are currently provided and by whom. The
    introspection API of libcontextsubscriber allows you to inspect
    the current state of the registry and observe its changes.

    \section Overview

    The introspection is provided via two classes: ContextRegistryInfo and
    ContextPropertyInfo.

    ContextRegistryInfo provides a high-level view to the registry
    contents.  You can use it to obtain info about the list of
    currently available keys or e.g. get a list of keys for one
    particular provider.  ContextRegistryInfo is a singleton instance
    which is created on the first access.

    ContextPropertyInfo is used to obtain metadata about one
    particular key. Once created, it can be used to retrieve the type
    and provider information (DBus bus type and name) of the
    introspected key. It also provides a signal to listen for changes
    happening to a key.

    \section Usage

    \code
    // To get a list of all keys in the registry
    ContextRegistryInfo *context = ContextRegistryInfo::instance();
    QStringList currentKeys = context->listKeys();
    \endcode

    Using the ContextPropertyInfo is even more straight-forward.

    \code
    // To check the type of a particular key
    ContextPropertyInfo propInfo("Battery.ChargeLevel");
    QString propType = propInfo.type();
    \endcode

    The introspection API in general never asserts (never fails). It'll return empty strings
    on errors or if data is missing. For example:

    \code
    ContextPropertyInfo propInfo("Something.That.Doesnt.Exist");
    propInfo.type();     //  ...returns empty string
    propInfo.doc();      //  ...returns empty string
    \endcode

    You can use this functionality to wait for keys to become available in the registry.
    Just create a ContextPropertyInfo for a key that you're expecting to become present
    and connect to the \c changed signal.

    \code
    ContextPropertyInfo propInfo("Something.That.Doesnt.Exist");
    propInfo.declared(); // false
    // Connect something to the changed signal, keep checking it
    \endcode

    \section xmlvscdb XML vs.CDB

    When the introspection API is first used, a backend choice is being made. \b CDB backend
    (reading data from \c 'cache.cdb' ) is used if the tiny database cache file exists
    in the registry. The standard (slower) \b XML backend is used in other cases.

    It's possible to force a usage of a particular backend.
    This can be done by calling the \c instance method with a string name of the backend:

    \code
    ContextRegistryInfo::instance("cdb"); // or "xml"
    \endcode

    This needs to be done early enough before the introspection API is first used.
    For more information about the \b xml and \cdb backends read the \ref UpdatingContextProviders page.

*/

/*!
    \page MigratingFromDuiValueSpace

    \brief libcontextsubscriber is a replacement library for DuiValueSpace which is deprecated.

    DuiValueSpace, the old subscription library providing the keys, is deprecated. This library
    is a replacement for it, providing better API and better implementation while maintaining the
    same core ideas and structure.

    \section quicklook A quick look

    The following code for creating a handle for a context property:

    \code
    DuiValueSpaceItem topEdge("Context.Screen.TopEdge");
    QObject::connect(&topEdge, SIGNAL(valueChanged()),
                     this, SLOT(topEdgeChanged()));
    \endcode

    becomes:

    \code
    ContextProperty topEdge("Screen.TopEdge");
    QObject::connect(&topEdge, SIGNAL(valueChanged()),
                     this, SLOT(topEdgeChanged()));
    \endcode

    The following code for listing the available context keys:

    \code
    DuiValueSpaceItem::listKeys();
    \endcode

    becomes:

    \code
    ContextRegistryInfo::instance()->listKeys();
    \endcode

    \section prefix The Context. prefix

    In \b DuiValueSpace and accompanying packages, the properties used to
    have a "Context." prefix. For example:

    \code
    Context.Screen.TopEdge
    Context.Screen.IsCovered
    \endcode

    This 'Context.' has been dropped now from \b libcontextsubscriber and
    all the provider packages. Providers now explicitly provide properties
    with keys like:

    \code
    Screen.TopEdge
    Screen.IsCovered
    \endcode

    For compatibility reasons the 'Context.' prefix is still supported in
    newer releases of \b DuiValueSpace. The \b DuiValueSpace library transparently
    adds the 'Context.' prefix to all access functions.

    A call to:

    \code
    DuiValueSpaceItem topEdge("Context.Screen.TopEdge");
    \endcode

    ...is internally in \b DuiValueSpace converted to actual \c Screen.TopEdge
    wire access. This mechanism has been introduced to make the
    \b DuiValueSpace and \b libcontextsubscriber libraries parallel-installable.

    It's expected that all \b DuiValueSpace clients migrate to \b libcontextsubscriber
    eventually and \b DuiValueSpace library will be removed.

    \warning When migrating to \b libcontextsubscriber make sure to remove the 'Context.'
    from you key access paths.
*/

/*!
    \class ContextPropertyInfo

    \brief A class to introspect a context property details.

    This class is used to obtain information about a given key in the context registry.
    The information can be provided either from xml files or from a cdb database.
    It's possible to query the type, the provider and the documentation of the given
    key/property.
*/

/// Constructs a new ContextPropertyInfo for \a key with the given \a parent.
/// The object can be used to perform introspection on the given \a key.
/// \param key The full name of the key.
ContextPropertyInfo::ContextPropertyInfo(const QString &key, QObject *parent)
    : QObject(parent)
{
    keyName = key;

    if (key != "") {
        InfoBackend* infoBackend = InfoBackend::instance();
        sconnect(infoBackend, SIGNAL(keyChanged(QString)),
                 this, SLOT(onKeyChanged(QString)));

        cachedTypeInfo = infoBackend->typeInfoForKey(keyName);
        cachedDoc = infoBackend->docForKey(keyName);
        cachedDeclared = infoBackend->keyDeclared(keyName);
        cachedProviders = infoBackend->providersForKey(keyName);
    }
}

/// Returns the full name of the introspected key.
QString ContextPropertyInfo::key() const
{
    QMutexLocker lock(&cacheLock);
    return keyName;
}

/// Returns the doc (documentation) for the introspected key.
QString ContextPropertyInfo::doc() const
{
    QMutexLocker lock(&cacheLock);
    return cachedDoc;
}

/// Returns the old-style type name for the introspected key. To be deprecated soon.
QString ContextPropertyInfo::type() const
{
    QMutexLocker lock(&cacheLock);

    QString typeInfoName = cachedTypeInfo.name();
    if (typeInfoName == "integer")
        return "INT";
    else if (typeInfoName == "bool")
        return "TRUTH";
    else if (typeInfoName == "string")
        return "STRING";
    else if (typeInfoName == "number")
        return "DOUBLE";
    else
        return "";
}

/// Returns the advanced type info for the introspected key.
ContextTypeInfo ContextPropertyInfo::typeInfo() const
{
    QMutexLocker lock(&cacheLock);
    return cachedTypeInfo;
}

/// DEPRECATED Returns true if the key exists in the registry.
/// This function is deprecated, use declared() instead.
bool ContextPropertyInfo::exists() const
{
    contextWarning() << F_DEPRECATION << "ContextPropertyInfo::exists() is deprecated.";
    QMutexLocker lock(&cacheLock);
    return cachedDeclared;
}

/// Returns true if the key is declared in the registry
/// (it "exists").
bool ContextPropertyInfo::declared() const
{
    QMutexLocker lock(&cacheLock);
    return cachedDeclared;
}

/// Returns true if the key is provided by someone.
bool ContextPropertyInfo::provided() const
{
    QMutexLocker lock(&cacheLock);
    return (cachedProviders.size() > 0);
}

/// Returns true if the key is deprecated.
bool ContextPropertyInfo::deprecated() const
{
    InfoBackend* infoBackend = InfoBackend::instance();
    return infoBackend->keyDeprecated(keyName);
}

/// DEPRECATED Returns the name of the plugin supplying this property.
/// This function is deprecated, use providers() instead.
QString ContextPropertyInfo::plugin() const
{
    contextWarning() << F_DEPRECATION << "ContextPropertyInfo::plugin() is deprecated.";
    return plugin_i();
}

QString ContextPropertyInfo::plugin_i() const
{
    QMutexLocker lock(&cacheLock);
    if (cachedProviders.size() == 0)
        return "";
    else
        return cachedProviders.at(0).plugin;
}

/// DEPRECATED Returns the construction parameter for the Provider supplying this property
/// This function is deprecated, use providers() instead.
QString ContextPropertyInfo::constructionString() const
{
    contextWarning() << F_DEPRECATION << "ContextPropertyInfo::constructionString() is deprecated.";
    return constructionString_i();
}

QString ContextPropertyInfo::constructionString_i() const
{
    QMutexLocker lock(&cacheLock);
    if (cachedProviders.size() == 0)
        return "";
    else
        return cachedProviders.at(0).constructionString;
}

/// DEPRECATED Returns the dbus name of the provider supplying this
/// property/key. This function is maintained for backwards
/// compatibility. Use providers() instead.
QString ContextPropertyInfo::providerDBusName_i() const
{
    QMutexLocker lock(&cacheLock);
    if (cachedProviders.size() == 0)
        return "";
    else {
        QString plg = cachedProviders.at(0).plugin;
        QString constructionString = cachedProviders.at(0).constructionString;
        if (plg == "contextkit-dbus")
            return constructionString.split(":").last();
        else
            return "";
    }
}

/// DEPRECATED Returns the dbus name of the provider supplying this
/// property/key. This function is maintained for backwards
/// compatibility. Use listProviders() instead.
QString ContextPropertyInfo::providerDBusName() const
{
    contextWarning() << F_DEPRECATION << "ContextPropertyInfo::providerDBusName() is deprecated.";
    return providerDBusName_i();
}
QDBusConnection::BusType ContextPropertyInfo::providerDBusType_i() const
{
    QMutexLocker lock(&cacheLock);
    QString busType = "";

    if (cachedProviders.size() > 0) {
        QString plg = cachedProviders.at(0).plugin;
        QString constructionString = cachedProviders.at(0).constructionString;
        if (plg == "contextkit-dbus")
            busType = constructionString.split(":").first();
    }

    if (busType == "system")
        return QDBusConnection::SystemBus;
    else /* if (busType == "session") */
        return QDBusConnection::SessionBus;
}


/// DEPRECATED Returns the bus type of the provider supplying this property/key.
/// Ie. if it's a session bus or a system bus. This function is
/// maintained for backwards compatibility. Use listProviders() instead.
QDBusConnection::BusType ContextPropertyInfo::providerDBusType() const
{
    contextWarning() << F_DEPRECATION << "ContextPropertyInfo::providerDBusType() is deprecated.";
    return providerDBusType_i();
}

/* Slots */

/// This slot is connected to the \a keyChanged signal of the
/// actual infobackend instance. It's executed on every change to any
/// of the keys. We first check if the data concerns us. Next we
/// update the cached values and fire the actual signals.
void ContextPropertyInfo::onKeyChanged(const QString& key)
{
    QMutexLocker lock(&cacheLock);

    if (key != keyName)
        return;

    // Update caches
    cachedTypeInfo = InfoBackend::instance()->typeInfoForKey(keyName);
    cachedDoc = InfoBackend::instance()->docForKey(keyName);
    cachedDeclared = InfoBackend::instance()->keyDeclared(keyName);
    cachedProviders = InfoBackend::instance()->providersForKey(keyName);

    // Release the lock before emitting the signals; otherwise
    // listeners trying to access cached values would create a
    // deadlock.
    lock.unlock();

    // Emit the needed signals
    Q_EMIT changed(keyName);
    Q_EMIT typeChanged(type());
    Q_EMIT existsChanged(cachedDeclared);
    Q_EMIT providedChanged((cachedProviders.size() > 0));

    Q_EMIT providerChanged(providerDBusName_i());
    Q_EMIT providerDBusTypeChanged(providerDBusType_i());
    Q_EMIT pluginChanged(plugin_i(), constructionString_i());
}

/// Returns a list of providers that provide this key.
const QList<ContextProviderInfo> ContextPropertyInfo::providers() const
{
    QMutexLocker lock(&cacheLock);
    return cachedProviders;
}

/// Called when people connect to signals. Used to emit deprecation warnings
/// when people connect to deprecated signals.
void ContextPropertyInfo::connectNotify(const QMetaMethod *signal)
{
    QObject::connectNotify(*signal);

    if (*signal == QMetaMethod::fromSignal(&ContextPropertyInfo::providerChanged))
        contextWarning() << F_DEPRECATION << "ContextPropertyInfo::providerChanged signal is deprecated.";
    else if (*signal == QMetaMethod::fromSignal(&ContextPropertyInfo::providerDBusTypeChanged))
        contextWarning() << F_DEPRECATION << "ContextPropertyInfo::providerDBusTypeChanged signal is deprecated.";
    else if (*signal == QMetaMethod::fromSignal(&ContextPropertyInfo::typeChanged))
        contextWarning() << F_DEPRECATION << "ContextPropertyInfo::typeChanged signal is deprecated.";
    else if (*signal == QMetaMethod::fromSignal(&ContextPropertyInfo::existsChanged))
        contextWarning() << F_DEPRECATION << "ContextPropertyInfo::existsChanged signal is deprecated.";
    else if (*signal == QMetaMethod::fromSignal(&ContextPropertyInfo::providedChanged))
        contextWarning() << F_DEPRECATION << "ContextPropertyInfo::providedChanged signal is deprecated.";
    else if (*signal == QMetaMethod::fromSignal(&ContextPropertyInfo::pluginChanged))
        contextWarning() << F_DEPRECATION << "ContextPropertyInfo::pluginChanged signal is deprecated.";
}

/// Returns resolution strategy for this property. Resolution strategy defines how values
/// are computed in relation to multiple providers being present for one property.
ContextPropertyInfo::ResolutionStrategy ContextPropertyInfo::resolutionStrategy() const
{
    return ContextPropertyInfo::LastValue;
}
