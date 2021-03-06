#include <QString>
#include <QObject>

#include "service.h"
#include "property.h"

class QProcess;

namespace ContextProvider {

class Property;

class TypesTests : public QObject
{
    Q_OBJECT

private:
    Service* service;
    Property* intItem;
    Property* stringItem;
    Property* boolItem;
    Property* doubleItem;
    Property* stringListItem;
    Property* charItem;
    Property* dateItem;
    Property* timeItem;

    QProcess *client;
    bool isReadyToRead;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void typesInReturnValueOfSubscribe();
    void typesInChangeSignal();

public Q_SLOTS:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
