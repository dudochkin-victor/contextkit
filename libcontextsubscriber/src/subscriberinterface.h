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

#ifndef SUBSCRIBERINTERFACE_H
#define SUBSCRIBERINTERFACE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QDBusAbstractInterface>

class QDBusConnection;
class QDBusPendingCallWatcher;

namespace ContextSubscriber {

class SubscriberInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:
    SubscriberInterface(const QDBusConnection connection, const QString& busName,
                        const QString& objectPath, QObject* parent = 0);

    void subscribe(QSet<QString> keys);
    void unsubscribe(QSet<QString> keys);

Q_SIGNALS:
    void valuesChanged(QMap<QString, QVariant> values);
    void subscribeFinished(QList<QString> keys);
    void subscribeFailed(QList<QString> keys, QString error);
    void Changed(const QMap<QString, QVariant> &values, const QStringList &unknownKeys);

private Q_SLOTS:
    void onSubscribeFinished(QDBusPendingCallWatcher* watcher);
    void onChanged(const QMap<QString, QVariant> &values, const QStringList &unknownKeys);

private:
    SubscriberInterface(const SubscriberInterface& other);
    SubscriberInterface& operator=(const SubscriberInterface& other);
    QMap<QString, QVariant>& mergeNullsWithMap(QMap<QString, QVariant> &map, QStringList nulls) const;

protected:
    void connectNotify(const QMetaMethod *signal);
    void disconnectNotify(const QMetaMethod *signal);

    static const char* interfaceName;
};
} // end namespace
#endif
