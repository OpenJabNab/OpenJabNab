
#define TTS_FOLDER "tts"

inline TTSInterface::TTSInterface(QString name, QString visualName):ttsName(name)
{
	// The visual name is more user-friendly (for visual-side only)
	if(visualName != QString())
		ttsVisualName = visualName;
	else
		ttsVisualName = name;
	// Create settings object
	QDir dir = QDir(QCoreApplication::applicationDirPath());
	dir.cd("tts");
	settings = new QSettings(dir.absoluteFilePath("tts_"+ttsName+".ini"), QSettings::IniFormat);
	ttsEnable = GetSettings("ttsStatus/Enable", QVariant(true)).toBool();

	// Folder
	QDir folder(GlobalSettings::GetString("Config/RealHttpRoot"));
	// Try to create tts subfolder
	if (!folder.cd(TTS_FOLDER))
	{
		if (!folder.mkdir(TTS_FOLDER))
		{
			LogError(QString("Unable to create "TTS_FOLDER" directory !\n"));
			return;
		}
		folder.cd(TTS_FOLDER);
	}
	if (!folder.cd(name))
	{
		if (!folder.mkdir(name))
		{
			LogError(QString("Unable to create %1 directory !\n").arg(name));
			return;
		}
		folder.cd(name);
	}
	ttsFolder = folder;
	ttsHTTPUrl = "broadcast/ojn_local/"TTS_FOLDER"/"+name+"/%1/%2"; // %1 For voice, %2 for FileName
	// Compute TTS's Http path
}

inline TTSInterface::~TTSInterface()
{
	delete settings;
}

// Settings
inline QVariant TTSInterface::GetSettings(QString const& key, QVariant const& defaultValue) const
{
	return settings->value(key, defaultValue);
}

inline void TTSInterface::SetSettings(QString const& key, QVariant const& value)
{
	settings->setValue(key, value);
	settings->sync();
}

// TTS's name
inline QString const& TTSInterface::GetName() const
{
	return ttsName;
}

inline QString const& TTSInterface::GetVisualName() const
{
	return ttsVisualName;
}

// TTS enable/disable functions
inline bool TTSInterface::GetEnable() const
{
	return ttsEnable;
}

// TTS enable/disable functions
inline void TTSInterface::SetEnable(bool newStatus)
{
	if(newStatus != ttsEnable)
	{
		ttsEnable = newStatus;
		SetSettings("ttsStatus/Enable", QVariant(newStatus)); 
		LogInfo(QString("TTS %1 is now %2").arg(GetVisualName(), GetEnable() ? "enabled" : "disabled"));
	}
}

