#include <QDateTime>
#include <QStringList>
#include "plugin_boot.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_boot, PluginBoot)

PluginBoot::PluginBoot():PluginInterface("boot", "Manage Boot requests", RequiredPlugin) {}

bool PluginBoot::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/bc.jsp"))
	{
		QString version = request.GetArg("v");
		QString serialnumber = request.GetArg("m").remove(':');
		
		Bunny * b = BunnyManager::GetBunny(this, serialnumber.toAscii());
		b->Booting();
	
		LogInfo(QString("Requesting BOOT for tag %1 with version %2").arg(serialnumber,version));
		if(GlobalSettings::Get("Config/StandAlone", true) == false)
		{
			request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BootServer"));

			if(GlobalSettings::Get("Config/SaveBootcode", false) == true)
			{
				QString bcFileName = GlobalSettings::Get("Config/Bootcode", "").toString();
				QFile bootcodeFile(bcFileName);
				if(bootcodeFile.open(QFile::WriteOnly))
				{
					bootcodeFile.write(request.reply);
				}
			}
			return true;
		}
		else
		{
			if(GlobalSettings::Get("Config/StandAloneUseLocalBootcode", true) == true)
			{
				QString bcFileName = GlobalSettings::Get("Config/Bootcode", "").toString();
				QFile bootcodeFile(bcFileName);
				if(bootcodeFile.open(QFile::ReadOnly))
				{
					QByteArray dataByteArray = bootcodeFile.readAll();
					request.reply = dataByteArray;
				}
				else
					LogError("Bootcode not found : " + bcFileName);
			}
			else
			{
				request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BootServer"));
			}
			if(b->GetBunnyPassword() == "" && GlobalSettings::Get("Config/StandAlonePatchBootcode", false) == true)
			{
				LogDebug("Analyzing firmware for patch application");

				long address1 = 0x00011A4A;
				long address2 = 0x00011A91;
				long address3 = 0x00011AC9;
				long address4 = 0x00011AEB;
				
				int size1 = 3;
				int size2 = 6;
				int size3 = 6;
				int size4 = 1;
			
				QByteArray origin1;
				QByteArray origin2;
				QByteArray origin3;
				QByteArray origin4;

				QByteArray patch1;
				QByteArray patch2;
				QByteArray patch3;
				QByteArray patch4;

			        origin1.append((char)0x02).append((char)0x3D).append((char)0x00);
			        origin2.append((char)0x02).append((char)0x3D).append((char)0x00).append((char)0x02).append((char)0x3E).append((char)0x00);
			        origin3.append((char)0x02).append((char)0x3D).append((char)0x00).append((char)0x02).append((char)0x3E).append((char)0x00);
			        origin4.append((char)0x02);

			        patch1.append((char)0x07).append((char)0x00).append((char)0x05);
			        patch2.append((char)0x04).append((char)0x05).append((char)0x04).append((char)0x05).append((char)0x04).append((char)0x05);
			        patch3.append((char)0x04).append((char)0x05).append((char)0x04).append((char)0x05).append((char)0x04).append((char)0x05);
			        patch4.append((char)0x01);
				
				bool patch = true;
				if(request.reply.indexOf(origin1, address1) != address1)
				{
					LogDebug("Part 1 : KO");
					patch = false;
				}
				else
					LogDebug("Part 1 : OK");
				if(request.reply.indexOf(origin2, address2) != address2)
				{
					LogDebug("Part 2 : KO");
					patch = false;
				}
				else
					LogDebug("Part 2 : OK");
				if(request.reply.indexOf(origin3, address3) != address3)
				{
					LogDebug("Part 3 : KO");
					patch = false;
				}
				else
					LogDebug("Part 3 : OK");
				if(request.reply.indexOf(origin4, address4) != address4)
				{
					LogDebug("Part 4 : KO");
					patch = false;
				}
				else
					LogDebug("Part 4 : OK");
				if(patch)
				{
					LogDebug("Patching firmware");
					request.reply.replace(address1, size1, patch1);
					request.reply.replace(address2, size2, patch2);
					request.reply.replace(address3, size3, patch3);
					request.reply.replace(address4, size4, patch4);
				}
			}
			return true;
		}
		return false;
	}
	else
		return false;
}
