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

#ifndef QUEUEDINVOKER_H
#define QUEUEDINVOKER_H

// This is a mock implementation

#include <QObject>
#include <QString>
#include <QStringList>

namespace ContextSubscriber {

class QueuedInvoker : public QObject
{
    Q_OBJECT

public:
    QueuedInvoker();

    // For tests
    void callAllMethodsInQueue();

protected:
    void queueOnce(const char *method);

private:
    QStringList methodsToCall;
};

} // namespace
#endif
