#ifndef IdaDatabaseList_h
#define IdaDatabaseList_h

//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaDatabaseList.h 1.1
//
//   File:      IdaDatabaseList.h
//   Revision:  1.1
//   Date:      17-NOV-2010 10:08:44
//
//   DESCRIPTION:
//
//
//<CE-------------------------------------------------------------------


static const char * SCCS_Id_IdaDatabaseList_h = "@(#) IdaDatabaseList.h 1.1";



#include <map.h>


struct DatabaseListElem
{
	RefId	dbId;
	RefId	objectId;
	RefId	backupObjectId;
};

typedef s_multimap < RefId, DatabaseListElem, s_less<RefId> > tDatabaseList;



////////////////////////////////////////////////////////////////////////////////////////////////////
/**
	Diese Klasse speichert eine Liste von Paaren bestehend aus
	einer DatabaseID und der zugehoerigen ObjektID.
	Die Klasse wird vom WebProcess benoetigt um die Requests an das
	richtige TdfAccess Objekt senden zu koennen
*/
class DatabaseList : protected tDatabaseList
{
	public:
		DatabaseList() {};
		~DatabaseList() {};

		/**
		  Einen DbId / ObjectId - Paar eintragen
		  */
		Void addDb(RefId dbid, RefId objectid)
		{
			DatabaseListElem databaseListElem;
			databaseListElem.dbId		= dbid;
			databaseListElem.objectId	= objectid;
			// Objekt in die Liste eintragen
#ifdef OS_AIX_4
			s_pair<RefId, DatabaseListElem> listItem(dbid, databaseListElem);
#else
			s_pair<const RefId, DatabaseListElem> listItem(dbid, databaseListElem);
#endif
			insert(listItem);

		}

                Void addDb(RefId dbid, RefId objectid, RefId backupobjectid )
                {
                        DatabaseListElem databaseListElem;
                        databaseListElem.dbId           = dbid;
                        databaseListElem.objectId       = objectid;
                        databaseListElem.backupObjectId = backupobjectid;
                        // Objekt in die Liste eintragen
#ifdef OS_AIX_4
                        s_pair<RefId, DatabaseListElem> listItem(dbid, databaseListElem);
#else
                        s_pair<const RefId, DatabaseListElem> listItem(dbid, databaseListElem);
#endif
                        insert(listItem);

                }

		/**
		  Einen ObjectId anhand ihrer DbId finden
		  Wenn die DbId nicht vorhanden ist wird
		  -1 zurueck geliefert.
		  */
		RefId findOIDofDB(RefId dbid)
		{
			iterator cursor = begin();
			while (cursor != end())
			{
				if ((*cursor).first == dbid)
				{
                    return (*cursor).second.objectId;
				}
				cursor++;
			}
			return -1;
		}

                RefId findBackupOIDofDB(RefId dbid)
                {
                        iterator cursor = begin();
                        while (cursor != end())
                        {
                                if ((*cursor).first == dbid)
                                {
                    return (*cursor).second.backupObjectId;
                                }
                                cursor++;
                        }
                        return -1;
                }

};


#endif	// IdaDatabaseList_h


// *** EOF ***


