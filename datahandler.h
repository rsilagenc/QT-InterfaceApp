#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QDateTime>
#include <QByteArray>
#include <memory>
#include <QDebug>

class dataHandler : public QObject
{
    Q_OBJECT
public:
    explicit dataHandler(QObject *parent = nullptr);
        QByteArray HWIDgenerator();
    enum Identifier
    {
        ID1 = 0X65,
    };
    struct Content
    {
        Identifier identifier = ID1;
        QByteArray cellValueArray;
        QByteArray TokenHWID;
        QByteArray HWIDCount;
        QDateTime DateOfExpiry;
        QByteArray HWID;
    };
    Content content;
private:


    Content currentTime;

};

extern std::unique_ptr<dataHandler> uniqueDataHandler;


#endif // DATAHANDLER_H
