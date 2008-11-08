
inline PluginInterface::PluginInterface(QString name, QString visualName, PluginType type):pluginName(name), pluginType(type)
{
	// The visual name is more user-friendly (for visual-side only)
	if(visualName != QString())
		pluginVisualName = visualName;
	else
		pluginVisualName = name;
	// Create settings object
	QDir dir = QDir(QCoreApplication::applicationDirPath());
	dir.cd("plugins");
	settings = new QSettings(dir.absoluteFilePath("plugin_"+pluginName+".ini"), QSettings::IniFormat);
	pluginEnable = GetSettings("pluginStatus/Enable", QVariant(true)).toBool();
	// Compute Plugin's Http path
	httpFolder = QString("%1/%2/%3").arg(GlobalSettings::GetString("Config/HttpRoot"), GlobalSettings::GetString("Config/HttpPluginsFolder"), pluginName);
}

inline PluginInterface::~PluginInterface()
{
	delete settings;
}

// Settings
inline QVariant PluginInterface::GetSettings(QString const& key, QVariant const& defaultValue) const
{
	return settings->value(key, defaultValue);
}

inline void PluginInterface::SetSettings(QString const& key, QVariant const& value)
{
	settings->setValue(key, value);
	settings->sync();
}

// Plugin's name
inline QString const& PluginInterface::GetName() const
{
	return pluginName;
}

inline QString const& PluginInterface::GetVisualName() const
{
	return pluginVisualName;
}

// Plugin enable/disable functions
inline bool PluginInterface::GetEnable() const
{
	return pluginEnable;
}

// Plugin type
inline int PluginInterface::GetType() const
{
	return pluginType;
}

// Plugin enable/disable functions
inline void PluginInterface::SetEnable(bool newStatus)
{
	if(newStatus != pluginEnable)
	{
		pluginEnable = newStatus;
		SetSettings("pluginStatus/Enable", QVariant(newStatus)); 
		Log::Info(QString("Plugin %1 is now %2").arg(GetVisualName(), GetEnable() ? "enabled" : "disabled"));
		if(pluginType == BunnyPlugin)
			BunnyManager::PluginStateChanged(this);
	}
}

// HTTP Data folder
inline QDir * PluginInterface::GetLocalHTTPFolder() const
{
	QDir pluginsFolder(GlobalSettings::GetString("Config/RealHttpRoot"));
	QString httpPluginsFolder = GlobalSettings::GetString("Config/HttpPluginsFolder");
	if (!pluginsFolder.cd(httpPluginsFolder))
	{
		if (!pluginsFolder.mkdir(httpPluginsFolder))
		{
			Log::Error(QString("Unable to create %1 directory !\n").arg(httpPluginsFolder));
			return NULL;
		}
		pluginsFolder.cd(httpPluginsFolder);
	}
	if (!pluginsFolder.cd(pluginName))
	{
		if (!pluginsFolder.mkdir(pluginName))
		{
			Log::Error(QString("Unable to create %1/%2 directory !\n").arg(httpPluginsFolder, pluginName));
			return NULL;
		}
		pluginsFolder.cd("tts");
	}
	return new QDir(pluginsFolder);
}

inline QByteArray PluginInterface::GetBroadcastHTTPPath(QString f) const
{
	return QString("broadcast/%1/%2").arg(httpFolder, f).toAscii();
}
