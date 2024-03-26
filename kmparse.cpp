#include "kmparse.h"



KMParse::KMParse(QString kmLink) : kmLink(kmLink)
{
    validateKMLink();

    // If link is valid, fetch data
    if (hasValidKMLink())
    {
        fetchData();
    }
}

void KMParse::validateKMLink()
{
    QRegularExpressionMatch match = kmPattern.match(kmLink);
    if (match.hasMatch())
    {
        kmID = match.captured(1);
        kmHash = match.captured(2);

        validKMLink = true;
    }
}

void KMParse::fetchData()
{
    const QUrl kmURL(kmLink);
    QNetworkRequest request = QNetworkRequest(kmURL);
    QNetworkReply* reply = netManager.get(request);

    QEventLoop loop;
    QObject::connect(reply,
                     &QNetworkReply::finished,
                     &loop,
                     [reply, this, &loop]()
                     {
                         QString replyText = reply->readAll();
                         QJsonDocument doc = QJsonDocument::fromJson(replyText.toUtf8());
                         if (doc.isObject())
                         {
                             QJsonObject docObj = doc.object();

                             // Grab data associated with the victim
                             QJsonObject victimObj = docObj.value(VICTIM_KEY).toObject();
                             victimID = QString::number(victimObj.value(CHARACTER_ID_KEY).toInt());
                             shipID = QString::number(victimObj.value(SHIP_TYPE_ID_KEY).toInt());

                             // Grab solar system ID
                             sysID = QString::number(docObj.value(SOLSYS_ID_KEY).toInt());

                             // Only parse the IDs of human attackers and the weapons they used
                             QJsonArray attackers = docObj.value(ATTACKERS_KEY).toArray();
                             foreach (QJsonValue attacker, attackers)
                             {
                                 QJsonObject curAttacker = attacker.toObject();
                                 // Data source guarantees same attacker can't be listed multiple times
                                 if (curAttacker.contains(CHARACTER_ID_KEY))
                                 {
                                     aggressors.append(curAttacker.value(CHARACTER_ID_KEY));
                                 }

                                 // Ensure multiple of the same weapon IDs don't get added
                                 if (curAttacker.contains(WEAPON_TYPE_ID_KEY))
                                 {
                                     QJsonValue curWeaponTypeID = curAttacker.value(WEAPON_TYPE_ID_KEY);
                                     if (!itemIDs.contains(curWeaponTypeID))
                                     {
                                         itemIDs.append(curWeaponTypeID);
                                     }
                                 }
                             }

                             // Query ESI for victim, ship, and solar system names
                             getVictimName();
                             getShipName(); // TODO: Replace with local database query?
                             getSolSysName(); // TODO: Replace with local database query?
                             getItemNames();

                             // Determine format for possessive form
                             shipPossessive = (shipName.endsWith("s")) ? "\'" : "\'s";

                             // Construct itemsDeliveredStr
                             QPair<QString, int> item;
                             foreach (item, itemsDelivered)
                             {
                                 itemsDeliveredStr += DELIVERED_ITEM.arg(item.first, QString::number(item.second));
                             }
                         }
                         reply->deleteLater();
                         loop.exit();
                     });
    loop.exec();
}

void KMParse::getVictimName()
{
    const QUrl victimInfoURL(GET_VICTIM_INFO.arg(victimID));
    QNetworkRequest request = QNetworkRequest(victimInfoURL);
    QNetworkReply* reply = netManager.get(request);

    QEventLoop loop;
    QObject::connect(reply,
                     &QNetworkReply::finished,
                     &loop,
                     [reply, this, &loop]()
                     {
                         QString victimReplyText = reply->readAll();
                         QJsonDocument victimDoc = QJsonDocument::fromJson(victimReplyText.toUtf8());
                         if (victimDoc.isObject())
                         {
                             QJsonObject victimDocObj = victimDoc.object();

                             // Grab victim name
                             victimName = victimDocObj.value(NAME_KEY).toString();
                         }
                         reply->deleteLater();
                         loop.exit();
                     });
    loop.exec();
}

void KMParse::getShipName()
{
    const QUrl shipInfoURL(GET_SHIP_INFO.arg(shipID));
    QNetworkRequest request = QNetworkRequest(shipInfoURL);
    QNetworkReply* reply = netManager.get(request);

    QEventLoop loop;
    QObject::connect(reply,
                     &QNetworkReply::finished,
                     &loop,
                     [reply, this, &loop]()
                     {
                         QString shipReplyText = reply->readAll();
                         QJsonDocument shipDoc = QJsonDocument::fromJson(shipReplyText.toUtf8());
                         if (shipDoc.isObject())
                         {
                             QJsonObject shipDocObj = shipDoc.object();

                             // Grab ship name
                             shipName = shipDocObj.value(NAME_KEY).toString();
                         }
                         reply->deleteLater();
                         loop.exit();
                     });
    loop.exec();
}

void KMParse::getSolSysName()
{
    const QUrl solSysInfoURL(GET_SOLSYS_INFO.arg(sysID));
    QNetworkRequest request = QNetworkRequest(solSysInfoURL);
    QNetworkReply* reply = netManager.get(request);

    QEventLoop loop;
    QObject::connect(reply,
                     &QNetworkReply::finished,
                     &loop,
                     [reply, this, &loop]()
                     {
                         QString solSysReplyText = reply->readAll();
                         QJsonDocument solSysDoc = QJsonDocument::fromJson(solSysReplyText.toUtf8());
                         if (solSysDoc.isObject())
                         {
                             QJsonObject solSysDocObj = solSysDoc.object();

                             // Grab solar system name
                             sysName = solSysDocObj.value(NAME_KEY).toString();
                         }
                         reply->deleteLater();
                         loop.exit();
                     });
    loop.exec();
}

void KMParse::getItemNames()
{
    const QUrl itemPOSTURL(POST_UNIVERSE_NAMES);
    QNetworkRequest request = QNetworkRequest(itemPOSTURL);
    request.setRawHeader(acceptHeader.first.toUtf8(), acceptHeader.second.toUtf8());
    request.setRawHeader(contentTypeHeader.first.toUtf8(), contentTypeHeader.second.toUtf8());
    request.setRawHeader(cacheControlHeader.first.toUtf8(), cacheControlHeader.second.toUtf8());

    QByteArray data = QJsonDocument(itemIDs).toJson();
    QNetworkReply* reply = netManager.post(request, data);

    QEventLoop loop;
    QObject::connect(reply,
                     &QNetworkReply::finished,
                     &loop,
                     [reply, this, &loop]()
                     {
                         QString itemPOSTReplyText = reply->readAll();
                         QJsonDocument itemPOSTDoc = QJsonDocument::fromJson(itemPOSTReplyText.toUtf8());
                         if (itemPOSTDoc.isArray())
                         {
                             QJsonArray itemPOSTDocArr = itemPOSTDoc.array();
                             foreach (QJsonValue item, itemPOSTDocArr)
                             {
                                 QJsonObject curItem = item.toObject();
                                 QPair<QString, int> curItemPair(curItem.value(NAME_KEY).toString(), curItem.value(ITEM_ID_KEY).toInt());
                                 itemsDelivered.append(curItemPair);
                             }
                         }
                         reply->deleteLater();
                         loop.exit();
                     });
    loop.exec();
}

bool KMParse::hasValidKMLink()
{
    return validKMLink;
}

void KMParse::setLocation(QString location)
{
    this->location = location;
}

QList<QString> KMParse::getOutputArgs()
{
    return QList<QString>({itemsDeliveredStr,
                           kmID,
                           kmHash,
                           victimID,
                           victimName,
                           shipID,
                           shipName,
                           shipPossessive,
                           sysID,
                           sysName,
                           location
    });
}
