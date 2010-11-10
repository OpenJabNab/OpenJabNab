#ifndef _PLUGINAPIHANDLER_H_
#define _PLUGINAPIHANDLER_H_

#include <QPair>
#include <QHash>
#include <QRegExp>
#include "apimanager.h"

class Account; 
class Bunny;
class HTTPRequest;

class PluginApiFunctor
{
public:
	virtual ApiManager::ApiAnswer * call(Account const&, HTTPRequest const&) = 0;
	virtual ~PluginApiFunctor() {};
};

class PluginBunnyApiFunctor
{
public:
	virtual ApiManager::ApiAnswer * call(Bunny *, Account const&, HTTPRequest const&) = 0;
	virtual ~PluginBunnyApiFunctor() {};
};

class PluginZtampApiFunctor
{
public:
	virtual ApiManager::ApiAnswer * call(Ztamp *, Account const&, HTTPRequest const&) = 0;
	virtual ~PluginZtampApiFunctor() {};
};

template <class T> class PluginApiSpecificFunctor : public PluginApiFunctor
{
private:
	ApiManager::ApiAnswer * (T::*pFunc)(Account const&, HTTPRequest const&);
	T * obj;

public:
	PluginApiSpecificFunctor(T* _obj, ApiManager::ApiAnswer * (T::*_pFunc)(Account const&, HTTPRequest const&)):pFunc(_pFunc),obj(_obj) {};
	virtual ApiManager::ApiAnswer * call(Account const& a, HTTPRequest const& h) { return (obj->*pFunc)(a, h); };
	virtual ~PluginApiSpecificFunctor() {};
};

template <class T> class PluginBunnyApiSpecificFunctor : public PluginBunnyApiFunctor
{
private:
	ApiManager::ApiAnswer * (T::*pFunc)(Bunny*, Account const&, HTTPRequest const&);
	T * obj;

public:
	PluginBunnyApiSpecificFunctor(T* _obj, ApiManager::ApiAnswer * (T::*_pFunc)(Bunny*, Account const&, HTTPRequest const&)):pFunc(_pFunc),obj(_obj) {};
	virtual ApiManager::ApiAnswer * call(Bunny* b, Account const& a, HTTPRequest const& h) { return (obj->*pFunc)(b, a, h); };
	virtual ~PluginBunnyApiSpecificFunctor() {};
};


template<class T>
struct function
{
	typedef QPair<T*, QStringList> Type;
};


typedef function<PluginBunnyApiFunctor>::Type bunnyApiFunction;
typedef function<PluginZtampApiFunctor>::Type ztampApiFunction;
typedef function<PluginApiFunctor>::Type apiFunction;

class PluginApiHandler
{
public:
	virtual ~PluginApiHandler()
	{
		foreach(apiFunction f, apiCalls.values())
			delete f.first;

		foreach(bunnyApiFunction f, bunnyApiCalls.values())
			delete f.first;
	}

	// Api Call
	ApiManager::ApiAnswer * ProcessApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		// Find an iterator for request
		QHash<QString, apiFunction>::iterator it = apiCalls.find(request);
		// If request wasn't found, return an error
		if(it == apiCalls.end())
			return new ApiManager::ApiError(QString("This plugin doesn't support this api call"));

		// Check args
		foreach(QString arg, it->second)
		{
			if(!hRequest.HasArg(arg))
			{
				return new ApiManager::ApiError(QString("Argument '%1' is missing").arg(arg));
			}
		}

		// Call method
		return it.value().first->call(account, hRequest);
	};

	ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny * bunny, Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		// Find an iterator for request
		QHash<QString, bunnyApiFunction>::iterator it = bunnyApiCalls.find(request);
		// If request wasn't found, return an error
		if(it == bunnyApiCalls.end())
			return new ApiManager::ApiError(QString("This plugin doesn't support this api call"));

		// Check args
		foreach(QString arg, it->second)
		{
			if(!hRequest.HasArg(arg))
			{
				return new ApiManager::ApiError(QString("Argument '%1' is missing").arg(arg));
			}
		}

		// Call method
		return it.value().first->call(bunny, account, hRequest);

	};

	ApiManager::ApiAnswer * ProcessZtampApiCall(Ztamp * ztamp, Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		// Find an iterator for request
		QHash<QString, ztampApiFunction>::iterator it = ztampApiCalls.find(request);
		// If request wasn't found, return an error
		if(it == ztampApiCalls.end())
			return new ApiManager::ApiError(QString("This plugin doesn't support this api call"));

		// Check args
		foreach(QString arg, it->second)
		{
			if(!hRequest.HasArg(arg))
			{
				return new ApiManager::ApiError(QString("Argument '%1' is missing").arg(arg));
			}
		}

		// Call method
		return it.value().first->call(ztamp, account, hRequest);

	};

	virtual void InitApiCalls() {};

protected:
	template<class T>
	void createApiCall(QHash<QString, typename function<T>::Type> & apiList, QString funcSig, T * f)
	{
		QRegExp rx("(.*)\\((.*)\\)");
		if(rx.indexIn(funcSig) != -1)
		{
			QString funcName = rx.cap(1);
			QStringList args = rx.cap(2).split(',', QString::SkipEmptyParts);
			apiList.insert(funcName, qMakePair(f, args));
		}
		else
		{
			LogError(QString("Invalid Api Signature : %1").arg(funcSig));
		}
	}

	QHash<QString, apiFunction> apiCalls;
	QHash<QString, bunnyApiFunction> bunnyApiCalls;
	QHash<QString, ztampApiFunction> ztampApiCalls;
};

#define DECLARE_PLUGIN_BUNNY_API_CALL(API_NAME, CLASS_NAME, FUNC_NAME) createApiCall<PluginBunnyApiFunctor>(bunnyApiCalls, API_NAME, new PluginBunnyApiSpecificFunctor<CLASS_NAME>(this, &CLASS_NAME::FUNC_NAME))
#define DECLARE_PLUGIN_ZTAMP_API_CALL(API_NAME, CLASS_NAME, FUNC_NAME) createApiCall<PluginZtampApiFunctor>(ztampApiCalls, API_NAME, new PluginZtampApiSpecificFunctor<CLASS_NAME>(this, &CLASS_NAME::FUNC_NAME))
#define DECLARE_PLUGIN_API_CALL(API_NAME, CLASS_NAME, FUNC_NAME) createApiCall<PluginApiFunctor>(apiCalls, API_NAME, new PluginApiSpecificFunctor<CLASS_NAME>(this, &CLASS_NAME::FUNC_NAME))

#define PLUGIN_API_CALL(FUNC_NAME) ApiManager::ApiAnswer * FUNC_NAME(Account const& account, HTTPRequest const& hRequest)
#define PLUGIN_BUNNY_API_CALL(FUNC_NAME) ApiManager::ApiAnswer * FUNC_NAME(Bunny * bunny, Account const& account, HTTPRequest const& hRequest)
#define PLUGIN_ZTAMP_API_CALL(FUNC_NAME) ApiManager::ApiAnswer * FUNC_NAME(Ztamp * ztamp, Account const& account, HTTPRequest const& hRequest)
#endif
