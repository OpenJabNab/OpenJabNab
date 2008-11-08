#ifndef _PLUGINAPIHANDLER_H_
#define _PLUGINAPIHANDLER_H_

#include <QHash>
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

template <class T> class PluginApiSpecificFunctor : public PluginApiFunctor
{
private:
	ApiManager::ApiAnswer * (T::*pFunc)(Account const&, HTTPRequest const&);
	T * obj;

public:
	PluginApiSpecificFunctor(T* _obj, ApiManager::ApiAnswer * (T::*_pFunc)(Account const&, HTTPRequest const&)):obj(_obj),pFunc(_pFunc) {};
	virtual ApiManager::ApiAnswer * call(Account const& a, HTTPRequest const& h) { return (obj->*pFunc)(a, h); };
	virtual ~PluginApiSpecificFunctor() {};
};

template <class T> class PluginBunnyApiSpecificFunctor : public PluginBunnyApiFunctor
{
private:
	ApiManager::ApiAnswer * (T::*pFunc)(Bunny*, Account const&, HTTPRequest const&);
	T * obj;

public:
	PluginBunnyApiSpecificFunctor(T* _obj, ApiManager::ApiAnswer * (T::*_pFunc)(Bunny*, Account const&, HTTPRequest const&)):obj(_obj),pFunc(_pFunc) {};
	virtual ApiManager::ApiAnswer * call(Bunny* b, Account const& a, HTTPRequest const& h) { return (obj->*pFunc)(b, a, h); };
	virtual ~PluginBunnyApiSpecificFunctor() {};
};

class PluginApiHandler
{
public:
	virtual ~PluginApiHandler()
	{
		foreach(PluginApiFunctor * f, apiCalls.values())
			delete f;

		foreach(PluginBunnyApiFunctor * f, bunnyApiCalls.values())
			delete f;
	}

	// Api Call
	ApiManager::ApiAnswer * ProcessApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		// Find an iterator for request
		QHash<QString, PluginApiFunctor*>::iterator it = apiCalls.find(request);
		// If request wasn't found, return an error
		if(it == apiCalls.end())
			return new ApiManager::ApiError(QString("This plugin doesn't support this api call"));

		// Call method
		return it.value()->call(account, hRequest);
	};

	ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny * bunny, Account const& account, QString const& request, HTTPRequest const& hRequest)
	{
		// Find an iterator for request
		QHash<QString, PluginBunnyApiFunctor*>::iterator it = bunnyApiCalls.find(request);
		// If request wasn't found, return an error
		if(it == bunnyApiCalls.end())
			return new ApiManager::ApiError(QString("This plugin doesn't support this api call"));

		// Call method
		return it.value()->call(bunny, account, hRequest);

	};

	virtual void InitApiCalls() {};

protected:
	QHash<QString, PluginApiFunctor*> apiCalls;
	QHash<QString, PluginBunnyApiFunctor*> bunnyApiCalls;
};

#define DECLARE_PLUGIN_BUNNY_API_CALL(API_NAME, CLASS_NAME, FUNC_NAME) bunnyApiCalls.insert(API_NAME, new PluginBunnyApiSpecificFunctor<CLASS_NAME>(this, &CLASS_NAME::FUNC_NAME))
#define DECLARE_PLUGIN_API_CALL(API_NAME, CLASS_NAME, FUNC_NAME) apiCalls.insert(API_NAME, new PluginApiSpecificFunctor<CLASS_NAME>(this, &CLASS_NAME::FUNC_NAME))

#define PLUGIN_API_CALL(FUNC_NAME) ApiManager::ApiAnswer * FUNC_NAME(Account const& account, HTTPRequest const& hRequest)
#define PLUGIN_BUNNY_API_CALL(FUNC_NAME) ApiManager::ApiAnswer * FUNC_NAME(Bunny * bunny, Account const& account, HTTPRequest const& hRequest)
#endif
