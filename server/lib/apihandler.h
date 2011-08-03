#ifndef _APIHANDLER_H_
#define _APIHANDLER_H_

#include <QHash>
#include <QString>
#include "apimanager.h"
#include "global.h"
#include "httprequest.h"
#include "log.h"

class Account;

// Template to declaration ApiCallFunc<T> type
template<class T>
struct ApiCallFunc
{
	typedef ApiManager::ApiAnswer * (T::*Type)(Account const&, HTTPRequest const&);
};

// Template to declaration ApiCallsMap<T> which is a QMap<QString, QPair<ApiCallFunc<T>, QStringList> >
template <class T>
class OJN_EXPORT ApiCallsMap : public QMap<QString, QPair<typename ApiCallFunc<T>::Type, QStringList> > {};

// Template to declaration ApiHandler<T> which dispatch ProcessApiCalls to T's methods
template <class T>
class OJN_EXPORT ApiHandler
{
public:
	ApiManager::ApiAnswer * ProcessApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		// Find an iterator for request
		typename ApiCallsMap<T>::iterator it = apiCalls.find(request);
		// If request wasn't found, return an error
		if(it == apiCalls.end())
			return new ApiManager::ApiError(QString("Unknown Api Call : %1<br />Request was : %2").arg(request, hRequest.toString()));

		// Check args
		foreach(QString arg, it->second)
		{
			if(!hRequest.HasArg(arg))
			{
				return new ApiManager::ApiError(QString("Argument '%1' is missing").arg(arg));
			}
		}

		// Call method
		return (dynamic_cast<T*>(this)->*(it.value().first))(account, hRequest);
	}
	virtual ~ApiHandler() {}

protected:
	static void createApiCall(QString const& funcSig, typename ApiCallFunc<T>::Type f)
	{
		QRegExp rx("(.*)\\((.*)\\)");
		if(rx.indexIn(funcSig) != -1)
		{
			QString funcName = rx.cap(1);
			QStringList args = rx.cap(2).split(',', QString::SkipEmptyParts);
			apiCalls.insert(funcName, qMakePair(f, args));
		}
		else
		{
			LogError(QString("Invalid Api Signature : %1").arg(funcSig));
		}
	}

	static ApiCallsMap<T> apiCalls;
};

// ApiCallsMap<T> definition
template <class T> ApiCallsMap<T> ApiHandler<T>::apiCalls;

// API Call
#define API_CALL(FUNC_NAME) ApiManager::ApiAnswer * FUNC_NAME(Account const& account, HTTPRequest const& hRequest)
#define DECLARE_API_CALL(API_NAME, FUNC_NAME) createApiCall(API_NAME, FUNC_NAME)
#endif
