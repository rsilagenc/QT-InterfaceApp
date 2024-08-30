#include "datahandler.h"
#include <openssl/rand.h>

dataHandler::dataHandler(QObject *parent)
    : QObject{parent}
{
    content.TokenHWID = HWIDgenerator();
    content.HWIDCount = QByteArray::fromHex("00");
    content.DateOfExpiry = QDateTime::currentDateTime().addDays(30);
}

QByteArray dataHandler::HWIDgenerator()
{
    const int hwid_length = 4;
    unsigned char hwid[hwid_length];
    if (!RAND_bytes(hwid, hwid_length))
    {
        qCritical() << "Error generating random IV";
    }
    return (reinterpret_cast<char*>(hwid));
}

std::unique_ptr<dataHandler> uniqueDataHandler = std::make_unique<dataHandler>();
