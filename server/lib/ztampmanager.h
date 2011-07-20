#ifndef _ZTAMPMANAGER_H_
#define _ZTAMPMANAGER_H_

#include <QHash>
#include <QVector>
#include "global.h"
#include "apihandler.h"
#include "apimanager.h"
#include "ztamp.h"

class Account;
class Ztamp;
class HTTPRequest;
class PluginInterface;
class OJN_EXPORT ZtampManager : public ApiHandler<ZtampManager>
{
	friend class PluginAuth;
	friend class ApiManager;
	friend class PluginManager;
public:
	static ZtampManager & Instance();
	static Ztamp * GetZtamp(QByteArray const&);
	static Ztamp * GetZtamp(PluginInterface *, QByteArray const&);
	static void PluginStateChanged(PluginInterface *);
	static inline void Init() { InitApiCalls(); };
	static void LoadZtamps();
	static void Close();

	// API
	static void InitApiCalls();

	int GetZtampCount();

protected:
	static QVector<Ztamp *> GetZtamps();
	static void PluginLoaded(PluginInterface *);
	static void PluginUnloaded(PluginInterface *);

	// API
	API_CALL(Api_GetListOfZtamps);
	API_CALL(Api_GetListOfAllZtamps);

private:
	ZtampManager();
	void LoadAllZtamps();
	QDir ztampsDir;
	static QHash<QByteArray, Ztamp *> listOfZtamps;
};

inline void ZtampManager::LoadZtamps()
{
	Instance().LoadAllZtamps();
}

#endif
