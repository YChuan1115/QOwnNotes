#include "calendaritem.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QApplication>
#include <QSqlError>
#include <QRegularExpression>
#include <QUuid>
#include <QSettings>


CalendarItem::CalendarItem()
{
    id = 0;
    hasDirtyData = false;
    icsDataKeyList = QStringList();
    priority = 0;
    sortPriority = 0;
}

int CalendarItem::getId()
{
    return this->id;
}

QString CalendarItem::getSummary()
{
    return this->summary;
}

QString CalendarItem::getUrl()
{
    return this->url;
}

QString CalendarItem::getICSData()
{
    return this->icsData;
}

QString CalendarItem::getUid()
{
    return this->uid;
}

QString CalendarItem::getDescription()
{
    return this->description;
}

int CalendarItem::getPriority()
{
    return this->priority;
}

bool CalendarItem::getHasDirtyData()
{
    return this->hasDirtyData;
}

QString CalendarItem::getLastModifiedString()
{
    return this->lastModifiedString;
}

QString CalendarItem::getETag()
{
    return this->etag;
}

QDateTime CalendarItem::getAlarmDate()
{
    return this->alarmDate;
}

void CalendarItem::setLastModifiedString(QString text)
{
    this->lastModifiedString = text;
}

void CalendarItem::setETag(QString text)
{
    this->etag = text;
}

void CalendarItem::setSummary(QString text)
{
    this->summary = text;
}

void CalendarItem::setUrl(QUrl url)
{
    this->url = url.toString();
}

void CalendarItem::setCalendar(QString text)
{
    this->calendar = text;
}

void CalendarItem::setDescription(QString text)
{
    this->description = text;
}

void CalendarItem::setICSData( QString text )
{
    this->icsData = text;
}

void CalendarItem::setUid( QString text )
{
    this->uid = text;
}

void CalendarItem::setCreated( QDateTime dateTime )
{
    this->created = dateTime;
}

void CalendarItem::setModified( QDateTime dateTime )
{
    this->modified = dateTime;
}

void CalendarItem::setAlarmDate( QDateTime dateTime )
{
    this->alarmDate = dateTime;
}

void CalendarItem::setPriority( int value )
{
    this->priority = value;
}

void CalendarItem::setCompleted( bool value )
{
    this->completed = value;
}

void CalendarItem::updateCompleted( bool value )
{
    this->completed = value;
    if ( value )
    {
        this->completedDate = QDateTime::currentDateTime();
    }
}

/**
 * @brief CalendarItem::addCalendarItemForRequest
 * @param calendar
 * @param url we are using QUrl because be want special characters in the urls translated
 * @return
 */
bool CalendarItem::addCalendarItemForRequest( QString calendar, QUrl url, QString etag, QString lastModifiedString )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "INSERT INTO calendarItem ( calendar, url, etag, last_modified_string ) VALUES ( :calendar, :url, :etag, :last_modified_string )" );
    query.bindValue( ":calendar", calendar );
    query.bindValue( ":url", url );
    query.bindValue( ":etag", etag );
    query.bindValue( ":last_modified_string", lastModifiedString );
    return query.exec();
}

bool CalendarItem::addCalendarItem( QString summary, QString url, QString text )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "INSERT INTO calendarItem ( summary, url, description ) VALUES ( :summary, :url, :description )" );
    query.bindValue( ":summary", summary );
    query.bindValue( ":url", url );
    query.bindValue( ":description", text );
    return query.exec();
}

CalendarItem CalendarItem::fetch( int id )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    CalendarItem calendarItem;

    query.prepare( "SELECT * FROM calendarItem WHERE id = :id" );
    query.bindValue( ":id", id );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else if ( query.first() )
    {
        calendarItem.fillFromQuery( query );
    }

    return calendarItem;
}

CalendarItem CalendarItem::fetchByUrlAndCalendar( QString url, QString calendar )
{
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "SELECT * FROM calendarItem WHERE url = :url AND calendar = :calendar" );
    query.bindValue( ":url", url );
    query.bindValue( ":calendar", calendar );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else if ( query.first() )
    {
        calendarItem.fillFromQuery( query );
    }

    return calendarItem;
}

CalendarItem CalendarItem::fetchByUid( QString uid )
{
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "SELECT * FROM calendarItem WHERE uid = :uid" );
    query.bindValue( ":uid", uid );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else if ( query.first() )
    {
        calendarItem.fillFromQuery( query );
    }

    return calendarItem;
}

CalendarItem CalendarItem::fetchByUrl( QUrl url )
{
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "SELECT * FROM calendarItem WHERE url = :url" );
    query.bindValue( ":url", url );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else if ( query.first() )
    {
        calendarItem.fillFromQuery( query );
    }

    return calendarItem;
}

bool CalendarItem::remove()
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "DELETE FROM calendarItem WHERE id = :id" );
    query.bindValue( ":id", this->id );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    else
    {
        return true;
    }
}


CalendarItem CalendarItem::calendarItemFromQuery( QSqlQuery query )
{
    CalendarItem calendarItem;
    calendarItem.fillFromQuery( query );
    return calendarItem;
}

bool CalendarItem::fillFromQuery( QSqlQuery query )
{
    this->id = query.value("id").toInt();
    this->summary = query.value("summary").toString();
    this->url = query.value("url").toString();
    this->description = query.value("description").toString();
    this->hasDirtyData = query.value("has_dirty_data").toInt() == 1;
    this->completed = query.value("completed").toInt() == 1;
    this->priority = query.value("priority").toInt();
    this->uid = query.value("uid").toString();
    this->calendar = query.value("calendar").toString();
    this->icsData = query.value("ics_data").toString();
    this->etag = query.value("etag").toString();
    this->lastModifiedString = query.value("last_modified_string").toString();
    this->alarmDate = query.value("alarm_date").toDateTime();
    this->created = query.value("created").toDateTime();
    this->modified = query.value("modified").toDateTime();
    this->completedDate = query.value("completed_date").toDateTime();
    this->sortPriority = query.value("sort_priority").toInt();

    return true;
}

QList<CalendarItem> CalendarItem::fetchAllByCalendar( QString calendar )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    QList<CalendarItem> calendarItemList;

    query.prepare( "SELECT * FROM calendarItem WHERE calendar = :calendar ORDER BY completed ASC, sort_priority DESC, modified DESC" );
    query.bindValue( ":calendar", calendar );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            CalendarItem calendarItem = calendarItemFromQuery( query );
            calendarItemList.append( calendarItem );
        }
    }

    return calendarItemList;
}

QList<CalendarItem> CalendarItem::fetchAll()
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    QList<CalendarItem> calendarItemList;

    query.prepare( "SELECT * FROM calendarItem" );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            CalendarItem calendarItem = calendarItemFromQuery( query );
            calendarItemList.append( calendarItem );
        }
    }

    return calendarItemList;
}

QList<QUrl> CalendarItem::fetchAllUrlsByCalendar( QString calendar )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );
    QList<QUrl> urlList;

    query.prepare( "SELECT url FROM calendarItem WHERE calendar = :calendar" );
    query.bindValue( ":calendar", calendar );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            urlList.append( query.value("url").toString() );
        }
    }

    return urlList;
}

QList<CalendarItem> CalendarItem::search( QString text )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    QList<CalendarItem> calendarItemList;

    query.prepare( "SELECT * FROM calendarItem WHERE description LIKE :text ORDER BY sort_priority DESC" );
    query.bindValue( ":text", "%" + text + "%"  );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            CalendarItem calendarItem = calendarItemFromQuery( query );
            calendarItemList.append( calendarItem );
        }
    }

    return calendarItemList;
}

void CalendarItem::updateSortPriority() {
    if ( priority == 0 )
    {
        sortPriority = 0;
    }
    else
    {
        sortPriority = ( 10 - priority ) * 10;
    }
}

/**
 * @brief Updates all priorities of calendar items
 */
void CalendarItem::updateAllSortPriorities() {
    QList<CalendarItem> calendarItemList = fetchAll();

    QListIterator<CalendarItem> itr ( calendarItemList );
    while ( itr.hasNext() )
    {
       CalendarItem calItem = itr.next();
       calItem.updateSortPriority();
       calItem.store();
    }
}

//
// inserts or updates a CalendarItem object in the database
//
bool CalendarItem::store() {
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    this->updateSortPriority();

    if ( this->id > 0 )
    {
        query.prepare( "UPDATE calendarItem SET "
                       "summary = :summary, url = :url, description = :description, has_dirty_data = :has_dirty_data, completed = :completed, "
                       "calendar = :calendar, uid = :uid, ics_data = :ics_data, "
                       "etag = :etag, last_modified_string = :last_modified_string, "
                       "alarm_date = :alarm_date, priority = :priority, created = :created, modified = :modified, completed_date = :completed_date, sort_priority = :sort_priority "
                       "WHERE id = :id" );
        query.bindValue( ":id", this->id );
    }
    else
    {
        query.prepare( "INSERT INTO calendarItem"
                       "( summary, url, description, calendar, uid, ics_data, etag, last_modified_string, has_dirty_data, completed, alarm_date, priority, created, modified, completed_date, sort_priority ) "
                       "VALUES ( :summary, :url, :description, :calendar, :uid, :ics_data, :etag, :last_modified_string, :has_dirty_data, :completed, :alarm_date, :priority, :created, :modified, :completed_date, :sort_priority )");
    }

    query.bindValue( ":summary", this->summary );
    query.bindValue( ":url", this->url );
    query.bindValue( ":description", this->description );
    query.bindValue( ":has_dirty_data", this->hasDirtyData ? 1 : 0 );
    query.bindValue( ":completed", this->completed ? 1 : 0 );
    query.bindValue( ":calendar", this->calendar );
    query.bindValue( ":uid", this->uid );
    query.bindValue( ":ics_data", this->icsData );
    query.bindValue( ":etag", this->etag );
    query.bindValue( ":last_modified_string", this->lastModifiedString );
    query.bindValue( ":alarm_date", this->alarmDate );
    query.bindValue( ":priority", this->priority );
    query.bindValue( ":created", this->created );
    query.bindValue( ":modified", this->modified );
    query.bindValue( ":completed_date", this->completedDate );
    query.bindValue( ":sort_priority", this->sortPriority );

    // on error
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    // on insert
    else if ( this->id == 0 )
    {
        this->id = query.lastInsertId().toInt();
    }

    return true;
}

/**
 * @brief Generates the new ICS data for the CalendarItem to send to the calendar server
 * @return
 */
QString CalendarItem::generateNewICSData() {
    qDebug() << __func__ << " - 'icsData before': " << icsData;

    // generate a new icsDataHash
    generateICSDataHash();

//    qDebug() << __func__ << " - 'icsDataHash after generateICSDataHash': " << icsDataHash;
//    qDebug() << __func__ << " - 'icsDataKeyList after generateICSDataHash': " << icsDataKeyList->join( "," );

    // update the icsDataHash
    icsDataHash["SUMMARY"] = summary;
    icsDataHash["DESCRIPTION"] = description;
    icsDataHash["UID"] = uid;
    icsDataHash["PRIORITY"] = QString::number( priority );
    icsDataHash["PERCENT-COMPLETE"] = QString::number( completed ? 100 : 0 );
    icsDataHash["STATUS"] = completed ? "COMPLETED" : "NEEDS-ACTION";
    icsDataHash["CREATED"] = created.toUTC().toString( ICS_DATETIME_FORMAT );
    icsDataHash["LAST-MODIFIED"] = modified.toUTC().toString( ICS_DATETIME_FORMAT );

    if ( completed )
    {
        icsDataHash["COMPLETED"] = completedDate.toUTC().toString( ICS_DATETIME_FORMAT );
    }
    else
    {
        // remove the "COMPLETED" ics data attribute if the todo item is not completed
        icsDataHash.remove( "COMPLETED" );
        icsDataKeyList.removeAll( "COMPLETED" );
    }

    removeICSDataBlock( "VALARM" );

    // set the alarm if needed
    if ( alarmDate.isValid() )
    {
        addVALARMBlockToICS();
    }

    // check for new keys so that we can send them to the calendar server
    updateICSDataKeyListFromHash();

//    qDebug() << __func__ << " - 'icsDataHash after updateICSDataKeyListFromHash': " << icsDataHash;
//    qDebug() << __func__ << " - 'icsDataKeyList after updateICSDataKeyListFromHash': " << icsDataKeyList->join( "," );

    icsData.clear();

    // loop through every line in the icsDataHash in the correct order
    for ( int i = 0; i < icsDataKeyList.size(); ++i )
    {
        QString key = icsDataKeyList.at( i );
        QString realKey = key;
        // cut out the numbers at the end
        realKey.replace( QRegularExpression( "\\d*$" ), "" );

        QString line = icsDataHash.value( key );

        // escape "\"s
        line.replace( "\\", "\\\\" );
        // convert newlines
        line.replace( "\n", "\\n" );

        // add a new ics data line
        icsData += realKey + ":" + line + "\n";
    }

    qDebug() << __func__ << " - 'icsData after': " << icsData;

    return icsData;
}

/**
 * @brief Checks if there are new entries in the icsDataHash (e.g. when we want to set a new property that wasn't there before) and updates the icsDataKeyList
 */
void CalendarItem::updateICSDataKeyListFromHash() {

    QStringList keysToAdd;

    // look for new keys
    QHashIterator<QString, QString> i( icsDataHash );
    while ( i.hasNext() )
    {
        i.next();
        QString key = i.key();

        // we found a new key
        if ( !icsDataKeyList.contains( key ) )
        {
            keysToAdd.append( key );
        }
    }

    if ( keysToAdd.size() > 0 )
    {
        int indexToAddKeys = -1;

        // search for the index where we should add the new keys
        for ( int i = 0; i < icsDataKeyList.size(); ++i )
        {
            QString key = icsDataKeyList.at( i );

            if ( key.startsWith( "BEGIN" ) && ( icsDataHash.value( key ) == "VTODO" ) )
            {
                // we want to add our new element after the BEGIN:VTODO
                indexToAddKeys = i + 1;
                break;
            }
        }

        // abort if we didn't find an index to add our new keys
        if ( indexToAddKeys == -1 ) {
            return;
        }

        // add the new keys
        for ( int i = 0; i < keysToAdd.size(); ++i )
        {
            QString key = keysToAdd.at( i );

            icsDataKeyList.insert( indexToAddKeys, key );
        }
    }
}

//
// deletes all calendarItems of a calendar in the database
//
bool CalendarItem::deleteAllByCalendar( QString calendar ) {
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "DELETE FROM calendarItem WHERE calendar = :calendar" );
    query.bindValue( ":calendar", calendar );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    else
    {
        return true;
    }
}

//
// checks if the current calendarItem still exists in the database
//
bool CalendarItem::exists() {
    CalendarItem calendarItem = CalendarItem::fetch( this->id );
    return calendarItem.id > 0;
}

bool CalendarItem::isFetched() {
    return ( this->id > 0 );
}

bool CalendarItem::isCompleted() {
    return this->completed;
}

bool CalendarItem::updateWithICSData( QString icsData )
{
    this->icsData = icsData;

    // parse and transform the ics data to a hash with the data
    generateICSDataHash();

    // we only need VTODO items!
//    if ( ( icsDataHash["BEGIN1"] != "VTODO" ) && ( icsDataHash["BEGIN"] != "VTODO" ) ) {
//        return false;
//    }

    summary = icsDataHash.contains( "SUMMARY" ) ? icsDataHash["SUMMARY"] : "";
    completed = icsDataHash.contains( "PERCENT-COMPLETE" ) ? icsDataHash["PERCENT-COMPLETE"] == "100" : false;
    uid = icsDataHash.contains( "UID" ) ? icsDataHash["UID"] : "";
    description = icsDataHash.contains( "DESCRIPTION" ) ? icsDataHash["DESCRIPTION"] : "";
    priority = icsDataHash.contains( "PRIORITY" ) ? icsDataHash["PRIORITY"].toInt() : 0;
    created = icsDataHash.contains( "CREATED" ) ? QDateTime::fromString( icsDataHash["CREATED"], ICS_DATETIME_FORMAT ) : QDateTime::currentDateTime();
    modified = icsDataHash.contains( "LAST-MODIFIED" ) ? QDateTime::fromString( icsDataHash["LAST-MODIFIED"], ICS_DATETIME_FORMAT ) : QDateTime::currentDateTime();

    QString alarmDateString = getICSDataAttributeInBlock( "VALARM", "TRIGGER;VALUE=DATE-TIME" );
    alarmDate = QDateTime();

    if ( alarmDateString != "" )
    {
        QDateTime dateTime = QDateTime::fromString( alarmDateString, ICS_DATETIME_FORMAT );
        // convert the UTC from the server to local time, because sqlite doesn't understand time zones
        alarmDate = QDateTime( dateTime.date(), dateTime.time(), Qt::UTC ).toLocalTime();
    }

//    qDebug() << __func__ << " - 'alarmDate': " << alarmDate;
//    qDebug() << __func__ << " - 'alarmDate': " << alarmDate.isNull();

    return this->store();
}

/**
 * @brief Searches for an attribute in a block in the ics data
 * @param block
 * @param attributeName
 * @return
 */
QString CalendarItem::getICSDataAttributeInBlock( QString block, QString attributeName )
{
    bool blockFound = false;
    for ( int i = 0; i < icsDataKeyList.size(); ++i )
    {
        QString key = icsDataKeyList.at( i );
        QString value = icsDataHash.value( key );

        if ( key.startsWith( "BEGIN" ) && ( value == block ) )
        {
            blockFound = true;
        }

        if ( blockFound )
        {
            if ( key.startsWith( attributeName ) )
            {
                return value;
            }
        }
    }

    return "";
}

/**
 * @brief Removes a block in the ics data
 * @param block
 */
bool CalendarItem::removeICSDataBlock( QString block )
{
    bool blockFound = false;
    bool doReturn = false;

    // make a copy of the data
    QHash<QString, QString> icsDataHashCopy = icsDataHash;
    QStringList icsDataKeyListCopy = icsDataKeyList;

    for ( int i = 0; i < icsDataKeyList.size(); ++i )
    {
        QString key = icsDataKeyList.at( i );
        QString value = icsDataHash.value( key );

        // look for the begin block
        if ( key.startsWith( "BEGIN" ) && ( value == block ) )
        {
            blockFound = true;
        }

        if ( blockFound )
        {
            // look for the end block
            if ( key.startsWith( "END" ) && ( value == block ) )
            {
                doReturn = true;
            }

            // remove the attributes
            icsDataHashCopy.remove( key );
            icsDataKeyListCopy.removeOne( key );

            if ( doReturn )
            {
                // write the data back and end
                icsDataHash = icsDataHashCopy;
                icsDataKeyList = icsDataKeyListCopy;
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief Adds the VALARM block to the ics data
 */
bool CalendarItem::addVALARMBlockToICS()
{
    // make a copy of the data
    QHash<QString, QString> icsDataHashCopy = icsDataHash;
    QStringList icsDataKeyListCopy = icsDataKeyList;

    bool foundBegin = false;

    for ( int i = 0; i < icsDataKeyList.size(); ++i )
    {
        QString key = icsDataKeyList.at( i );
        QString value = icsDataHash.value( key );

        // look for the VTODO begin block
        if ( key.startsWith( "BEGIN" ) && ( value == "VTODO" ) )
        {
            foundBegin = true;
        }

        // add the VALARM block at the end of the VTODO block
        if ( foundBegin && key.startsWith( "END" ) && ( value == "VTODO" ) )
        {
            QString addKey;
            addKey = findFreeHashKey( &icsDataHashCopy, "BEGIN" );
            icsDataHashCopy[addKey] = "VALARM";
            icsDataKeyListCopy.insert( i, addKey );

            addKey = findFreeHashKey( &icsDataHashCopy, "ACTION" );
            icsDataHashCopy[addKey] = "DISPLAY";
            icsDataKeyListCopy.insert( ++i, addKey );

            addKey = findFreeHashKey( &icsDataHashCopy, "DESCRIPTION" );
            icsDataHashCopy[addKey] = "Reminder";
            icsDataKeyListCopy.insert( ++i, addKey );

            addKey = findFreeHashKey( &icsDataHashCopy, "TRIGGER;VALUE=DATE-TIME" );
            icsDataHashCopy[addKey] = alarmDate.toUTC().toString( ICS_DATETIME_FORMAT );
            icsDataKeyListCopy.insert( ++i, addKey );

            addKey = findFreeHashKey( &icsDataHashCopy, "END" );
            icsDataHashCopy[addKey] = "VALARM";
            icsDataKeyListCopy.insert( ++i, addKey );

            // write the data back and end
            icsDataHash = icsDataHashCopy;
            icsDataKeyList = icsDataKeyListCopy;
            return true;
        }
    }

    return false;
}

/**
 * @brief Parses and transforms the ics data to a hash with the data
 * @param icsData
 * @return
 */
void CalendarItem::generateICSDataHash()
{
    QRegularExpression regex;
    QRegularExpressionMatch match;
    QString lastKey;
    icsDataKeyList.clear();
    icsDataHash.clear();

    QStringList iscDataLines = icsData.split( "\n" );

    QListIterator<QString> i( iscDataLines );
    while ( i.hasNext() )
    {
        QString line = i.next();
        line.replace( "\r", "" );

        if ( line == "" ) {
            continue;
        }

        // multi-line text stats with a space
        if ( !line.startsWith( " " ) )
        {
            // remove the trailing \n
            if ( line.endsWith( "\n" ) ) {
                line.chop( 1 );
            }

            // parse key and value
            regex.setPattern( "^([A-Z\\-_=;]+):(.*)$" );
            match = regex.match( line );

            // set last key for multi line texts
            lastKey = match.captured(1);

            if ( lastKey == "" ) {
                continue;
            }

            // find a free key
            lastKey = findFreeHashKey( &icsDataHash, lastKey );

            // add new key / value pair to the hash
            icsDataHash[lastKey] = decodeICSDataLine( match.captured(2) );
//            hash.insert( lastKey, decodeICSDataLine( match.captured(2) ) );

            // add the key to the key order list
            icsDataKeyList.append( lastKey );
        }
        else
        {
            // remove the trailing \n
            if ( line.endsWith( "\n" ) ) {
                line.chop( 1 );
            }

            // remove leading space
            regex.setPattern( "^ (.+)$" );
            match = regex.match( line );

            // add text to last line
            icsDataHash[lastKey] += decodeICSDataLine( match.captured(1) );
        }
    }
}

/**
 * @brief Finds a free key in a hash
 * @param hash
 * @param key
 * @param number
 * @return
 */
QString CalendarItem::findFreeHashKey( QHash<QString, QString> *hash, QString key, int number )
{
    // give up after 1000 tries
    if ( number >= 1000 )
    {
        return key;
    }

    QString newKey = key;

    // if number is bigger than 0 add the number to the key
    if ( number > 0 )
    {
        newKey = key + QString::number( number );
    }

    // if the key is taken find increase number and try to find a new key
    if ( hash->contains( newKey ) )
    {
        return findFreeHashKey( hash, key, ++number );
    }

    return newKey;
}


/**
 * @brief Decodes an ics data line
 * @param line
 * @return
 */
QString CalendarItem::decodeICSDataLine( QString line )
{
//    qDebug() << __func__ << " - 'before line': " << line;

    // replace \n with newlines
    // we have to replace this twice, because of the first character that gets replaces in multiple \n
    line.replace( QRegularExpression( "([^\\\\])\\\\n" ), "\\1\n" );
    line.replace( QRegularExpression( "([^\\\\])\\\\n" ), "\\1\n" );

    // replace "\\" with "\"
    line.replace( "\\\\", "\\" );

    // replace "\," with ","
    line.replace( "\\,", "," );

//    qDebug() << __func__ << " - 'after line':  " << line;
    return line;
}

CalendarItem CalendarItem::createNewTodoItem( QString summary, QString calendar )
{
    QUuid uuid = QUuid::createUuid();
    QString uuidString = uuid.toString();
    uuidString.replace( "{", "" ).replace( "}", "" );

    CalendarItem calItem;
    calItem.setSummary( summary );
    calItem.setCalendar( calendar );
    calItem.setUrl( QUrl( getCurrentCalendarUrl() + "qownnotes-" + uuidString + ".ics" ) );
    calItem.setICSData( "BEGIN:VCALENDAR\nVERSION:2.0\nPRODID:ownCloud Calendar\nCALSCALE:GREGORIAN\nBEGIN:VTODO\nEND:VTODO\nEND:VCALENDAR" );
    calItem.setUid( uuidString );

    QDateTime dateTime = QDateTime::currentDateTime();
    calItem.setCreated( dateTime );
    calItem.setModified( dateTime );

    calItem.generateNewICSData();

    if ( calItem.store() )
    {
        qDebug() << __func__ << " - 'calItem': " << calItem;
        qDebug() << __func__ << " - 'calItem.getICSData()': " << calItem.getICSData();
    }

    return calItem;
}

/**
 * @brief Returns the index of the currently selected todo list in the todo dialog
 * @return
 */
int CalendarItem::getCurrentCalendarIndex()
{
    QSettings settings;

    QString todoListSelectorSelectedItem = settings.value( "TodoDialog/todoListSelectorSelectedItem" ).toString();
    if ( todoListSelectorSelectedItem != "" )
    {
        QStringList todoCalendarEnabledList = settings.value( "ownCloud/todoCalendarEnabledList" ).toStringList();

        // search for the text in the todoCalendarEnabledList
        int index = todoCalendarEnabledList.indexOf( todoListSelectorSelectedItem );

        if ( index >= 0 )
        {
            // return the index of the todo list selector if we found it
            return index;
        }
    }

    return -1;
}

/**
 * @brief Returns the url of the currently selected todo list in the todo dialog
 * @return
 */
QString CalendarItem::getCurrentCalendarUrl()
{
    QSettings settings;

    int index = getCurrentCalendarIndex();

    if ( index >= 0 )
    {
        QStringList todoCalendarEnabledUrlList = settings.value( "ownCloud/todoCalendarEnabledUrlList" ).toStringList();
        return todoCalendarEnabledUrlList[index];
    }

    return "";
}

QDebug operator<<(QDebug dbg, const CalendarItem &calendarItem)
{
    dbg.nospace() << "CalendarItem: <id>" << calendarItem.id << " <summary>" << calendarItem.summary << " <url>" << calendarItem.url << " <calendar>" << calendarItem.calendar;
    return dbg.space();
}
