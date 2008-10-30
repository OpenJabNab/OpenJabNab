#ifndef _APIHANDLER_H_
#define _APIHANDLER_H_

#include <QHash>
#include <QString>
#include "account.h"
#include "apimanager.h"
#include "global.h"
#include "httprequest.h"

class Account;

// Template to declaration ApiCallFunc<T> type
template<class T>
struct ApiCallFunc
{
	typedef ApiManager::ApiAnswer * (T::*Type)(Account const&, HTTPRequest const&);
};

// Template to declaration ApiCallsMap<T> which is a QMap<QString, ApiCallFunc<T>>
template <class T>
class OJN_EXPORT ApiCallsMap : public QMap<QString, typename ApiCallFunc<T>::Type>
{
public:
	ApiCallsMap()
	{
		T::InitApiCalls();
	}
};

// Template to declaration ApiHandler<T> which dispatch ProcessApiCalls to T's methods
template <class T>
class OJN_EXPORT ApiHandler
{
protected:
	static ApiCallsMap<typename T> apiCalls;

public:
	ApiManager::ApiAnswer * ProcessApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		// Find an iterator for request
		typename ApiCallsMap<typename T>::iterator it = apiCalls.find(request);
		// If request wasn't found, return an error
		if(it == apiCalls.end())
			return new ApiManager::ApiError(QString("Unknown Api Call : %1<br />Request was : %2").arg(request, hRequest.toString()));

		// Call method
		return (dynamic_cast<T*>(this)->*(it.value()))(account, hRequest);
	}
	virtual ~ApiHandler() {}
};

// ApiCallsMap<T> definition
template <class T> ApiCallsMap<typename T> ApiHandler<T>::apiCalls;

// API Call
#define API_CALL(FUNC_NAME) ApiManager::ApiAnswer * FUNC_NAME(Account const& account, HTTPRequest const& hRequest)
#define DECLARE_API_CALL(API_NAME, FUNC_NAME) apiCalls.insert(API_NAME, FUNC_NAME)
#endif

