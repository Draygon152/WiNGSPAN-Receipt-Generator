#ifndef KMPARSE_H
#define KMPARSE_H

#include <qstring.h>
#include <qpair.h>
#include <qlist.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qeventloop.h>
#include <qregularexpression.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkrequest.h>
#include <qnetworkreply.h>



class KMParse
{
    private:
        const QRegularExpression kmPattern = QRegularExpression("https://esi.evetech.net/v1/killmails/(\\d+)/(\\w+)/[?]datasource=tranquility");

        // KILLMAIL JSON ACCESS KEYS
        const QString ATTACKERS_KEY = "attackers";
        const QString CHARACTER_ID_KEY = "character_id";
        const QString VICTIM_KEY = "victim";
        const QString SHIP_TYPE_ID_KEY = "ship_type_id";
        const QString WEAPON_TYPE_ID_KEY = "weapon_type_id";
        const QString SOLSYS_ID_KEY = "solar_system_id";
        const QString NAME_KEY = "name";
        const QString ITEM_ID_KEY = "id";

        QNetworkAccessManager netManager;

        // REQUEST URL TEMPLATES
        // Format with victimID
        QString GET_VICTIM_INFO = "https://esi.evetech.net/latest/characters/%1/?datasource=tranquility";
        // Format with shipID
        QString GET_SHIP_INFO = "https://esi.evetech.net/latest/universe/types/%1/?datasource=tranquility&language=en";
        // Format with sysID
        QString GET_SOLSYS_INFO = "https://esi.evetech.net/latest/universe/systems/%1/?datasource=tranquility&language=en";

        // POST TARGETS
        QString POST_UNIVERSE_NAMES = "https://esi.evetech.net/latest/universe/names/?datasource=tranquility";

        // POST HEADERS
        QPair<QString, QString> acceptHeader = QPair<QString, QString>("accept", "application/json");
        QPair<QString, QString> contentTypeHeader = QPair<QString, QString>("Content-Type", "application/json");
        QPair<QString, QString> cacheControlHeader = QPair<QString, QString>("Cache-Control", "no-cache");

        // DELIVERY TEMPLATES
        QString DELIVERED_ITEM = "<a href=\"showinfo:%2\">%1</a><br>";

        // TODO: Automated addition of aggressors to the receipt?
        QJsonArray aggressors; // Must access aggressor IDs using toInt
        QJsonArray itemIDs;
        QJsonArray itemNames;

        QList<int> aggressorIDs;
        QList<QPair<QString, int>> itemsDelivered;

        QString kmLink;
        QString kmID;
        QString kmHash;
        QString victimID;
        QString victimName;
        QString shipID;
        QString shipName;
        QString sysID;
        QString sysName;
        QString shipPossessive;
        QString itemsDeliveredStr;
        // We met at...
        QString location;

        bool validKMLink = false;

        void validateKMLink();
        void fetchData();
        void getVictimName();
        void getShipName();
        void getSolSysName();
        void getItemNames();

    public:
        KMParse(QString kmLink);

        bool hasValidKMLink();
        void setLocation(QString location);
        QList<QString> getOutputArgs();
};

#endif // KMPARSE_H
