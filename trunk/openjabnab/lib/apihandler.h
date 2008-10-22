#ifndef _APIHANDLER_H_
#define _APIHANDLER_H_

#include <QHash>
#include <QString>
#include "apimanager.h"
#include "global.h"
#include "httprequest.h"

class Account;
template <class T>
class OJN_EXPORT ApiHandler
{
protected:
	typedef ApiManager::ApiAnswer * (T::*ApiCallFunc)(Account const&, QString const&, HTTPRequest const&);
	QHash<QString, ApiCallFunc> apiCalls;

public:
	// API
	ApiManager::ApiAnswer * ProcessApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		typename QHash<QString, ApiCallFunc>::iterator it = apiCalls.find(request);
		if(it == apiCalls.end())
			return new ApiManager::ApiError(QString("Unknown Api Call : %1<br />Request was : %2").arg(request, hRequest.toString()));

		return (dynamic_cast<T*>(this)->*(it.value()))(account, request, hRequest);
	}
	virtual ~ApiHandler() {}
};

#endif

