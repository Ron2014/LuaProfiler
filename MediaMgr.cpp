#include "MediaMgr.h"
#include "cocos2d.h"
#include "platform/CCPlatformMacros.h"
#include "fmod_errors.h"
#include "fmod.hpp"
#include "platform/CCFileUtils.h"


USING_NS_CC;
using namespace FMOD;

#define MAXCHANNELNUM 128

#define FMODERROR_CHECK( result ) if ( result != FMOD_OK)\
{\
	CCLOG("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));\
	return false;\
}


FMOD_RESULT F_CALLBACK  FILE_OPENCALLBACK(const char *  name, int  unicode, unsigned int *  filesize,
	void **  handle, void **  userdata)
{
	char fmt_name[256];
	int len = strlen(name);
	for (int i = 0; i <= len; ++i)
	{
		if (name[i] == '\\')
		{
			fmt_name[i] = '/';
		}
		else
		{
			fmt_name[i] = name[i];
		}
	}

	const char *filename = fmt_name; //strstr(fmt_name, "/");
	if (!filename)
		return FMOD_ERR_FILE_NOTFOUND;

	if (*filename == '/' || *filename == '\\')
	{
		filename += 1;
	}

	Data* retData = new Data();
	*retData = CCFileUtils::getInstance()->getDataFromFile(filename);
	
	if ( retData->isNull() )
	{
		return FMOD_ERR_FILE_BAD;
	}
	
	else
	{
		*filesize = retData->getSize();
		*handle = retData;
		unsigned int *p = new unsigned int;
		*p = 0;
		*userdata = p;
	}
	return FMOD_OK;
}


FMOD_RESULT F_CALLBACK  FILE_CLOSECALLBACK(void *  handle, void *  userdata)
{

	delete (unsigned int *)userdata;
	Data* pData = (Data *)handle;
	pData->clear();
	delete pData;
	return FMOD_OK;

}



FMOD_RESULT F_CALLBACK  FILE_READCALLBACK(void *  handle, void *  buffer, unsigned int  sizebytes,
	unsigned int *  bytesread, void *  userdata)
{
	Data* pData = (Data *)handle;
	unsigned int *pos = (unsigned int*)userdata;
	if (*pos >= pData->getSize()){
		return FMOD_ERR_FILE_EOF;
	}
	else if (*pos + sizebytes > pData->getSize()){
		*bytesread = pData->getSize() - *pos;
	}
	else {
		*bytesread = sizebytes;
	}
	memcpy(buffer, (pData->getBytes() + *pos), *bytesread);
	*pos += *bytesread;
	return FMOD_OK;

}

FMOD_RESULT F_CALLBACK  FILE_SEEKCALLBACK(void *  handle, unsigned int  pos, void *  userdata)
{
	Data* pData = (Data *)handle;
	if (pos >= pData->getSize()){
		pos = pData->getSize();
	}
	unsigned int *p = (unsigned int*)userdata;
	*p = pos;
	return FMOD_OK;
}
 
CMediaMgr::CMediaMgr():
m_System(NULL),
m_EventSystem(NULL),
m_bInitOK(false),
m_nEventIdx(0),
m_GlobalId(0)
{
	FMOD_RESULT result;
	result = FMOD::EventSystem_Create(&m_EventSystem);
	if (result != FMOD_OK)
	{
		m_EventSystem = NULL;
		return;
	}
	FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_ERROR);
	
	result = m_EventSystem->getSystemObject(&m_System);
	if (result != FMOD_OK)
	{
		m_System = NULL;
		return;
	}
	
	m_EventSystem->init(MAXCHANNELNUM, FMOD_INIT_NORMAL, 0);
	m_System->setFileSystem(FILE_OPENCALLBACK, FILE_CLOSECALLBACK, FILE_READCALLBACK, FILE_SEEKCALLBACK, NULL, NULL, 1024);
	m_bInitOK = true;
}

CMediaMgr::~CMediaMgr()
{
	if (m_EventSystem)
	{
		m_EventSystem->release();
		m_EventSystem = NULL;
	}
	
	if (m_System)
	{
		m_System->release();
		m_System = NULL;
	}
}

CMediaMgr& CMediaMgr::Instance()
{
	static CMediaMgr _instance;
	return _instance;
}

int CMediaMgr::GetGlobalId()
{
	m_GlobalId += 1;
	return m_GlobalId;
}

bool CMediaMgr::IsInitOK()
{
	return m_bInitOK;
}

int CMediaMgr::LoadFevFile(const char* szFev)
{
	if (!IsInitOK())
	{
		return 0;
	}
	
	FMOD::EventProject* eventProject = NULL;
	FMODERROR_CHECK(m_EventSystem->load(szFev,0,&eventProject));
    
	int GlobalId = this->GetGlobalId();
	m_EventProjectMap.insert ( std::pair <int, FMOD::EventProject*>  ( GlobalId, eventProject ) );
	
    return GlobalId;
}

int CMediaMgr::FmodUpdate()
{
	if (!IsInitOK())
	{
		return 0;
	}
	
	FMOD_RESULT result = m_EventSystem->update();
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::Update");
		return 0;
	}
	return 1;
}

int CMediaMgr::LoadEventGroup(int projectId,const char* szGroup)
{
	if(!projectId)
	{
		return 0;
	}
	
	EventProjectMapType::iterator it;
	it = m_EventProjectMap.find(projectId);
	if(it == m_EventProjectMap.end())
	{
		return 0;
	}
	
	FMOD::EventProject* eventProject = it->second;
	
	FMOD::EventGroup* eventGroup;
	FMOD_RESULT result = eventProject->getGroup(szGroup, false, &eventGroup);
	if ( result != FMOD_OK )
	{
		CCLOG("LoadEventGroup getgroup %s FMOD error! (%d) %s\n",  szGroup, result, FMOD_ErrorString(result));
		return 0;
	}
	
	result = eventGroup->loadEventData();
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::LoadEventGroup");
		return 0;
	}
	return 1;
}

int CMediaMgr::FreeEventGroup(int projectId,const char* szGroup)
{
	if(!projectId)
	{
		return 0;
	}
	
	EventProjectMapType::iterator it;
	it = m_EventProjectMap.find(projectId);
	if(it == m_EventProjectMap.end())
	{
		return 0;
	}
	
	FMOD::EventProject* eventProject = it->second;
	
	FMOD::EventGroup* eventGroup;
	FMOD_RESULT result = eventProject->getGroup(szGroup, false, &eventGroup);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::FreeEventGroup");
		return 0;
	}

	eventGroup->freeEventData();
	return 1;
}

int CMediaMgr::Play(int projectId,const char* szEvent)
{
	if(!projectId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	
	FMOD::Event* event;
	FMOD_RESULT result = m_EventSystem->getEvent(szEvent, FMOD_EVENT_DEFAULT, &event);
	
	if (result != FMOD_OK)
	{
		return 0;
	}
	
	FMOD_EVENT_STATE state = 0;
	result = event->getState(&state);
	
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::Play");
		return 0;
	}
	
	event->start();
	
	int GlobalId = this->GetGlobalId();
	m_EventMap.insert ( std::pair <int, FMOD::Event*>  ( GlobalId, event ) );
	
    return GlobalId;
}

int CMediaMgr::GetGroupId(int projectId, const char* szGroup)
{
	if(!projectId)
	{
		return 0;
	}
	
	EventProjectMapType::iterator it;
	it = m_EventProjectMap.find(projectId);
	if(it == m_EventProjectMap.end())
	{
		return 0;
	}
	
	FMOD::EventProject* eventProject = it->second;
	
	FMOD::EventGroup* handle;
	FMOD_RESULT result = eventProject->getGroup(szGroup, false, &handle);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::GetGroupId");
		return 0;
	}
	
	int GlobalId = this->GetGlobalId();
	m_EventGroupMap.insert ( std::pair <int, FMOD::EventGroup*>  ( GlobalId, handle ) );
	
    return GlobalId;
}

int CMediaMgr::GetGroupEventId(int projectId, const char* szGroup, const char *szEvent)
{
	if(!projectId)
	{
		return 0;
	}
	
	EventProjectMapType::iterator it;
	it = m_EventProjectMap.find(projectId);
	if(it == m_EventProjectMap.end())
	{
		return 0;
	}
	
	FMOD::EventProject* eventProject = it->second;
	
	FMOD::EventGroup* eventGroup;
	FMOD_RESULT result = eventProject->getGroup(szGroup, false, &eventGroup);
	if (result != FMOD_OK)
	{
		CCLOG("FreeEventGroup getgroup %s FMOD error! (%d) %s\n", szGroup, result, FMOD_ErrorString(result));
		return 0;
	}
	FMOD::Event* handle;
	result = eventGroup->getEvent(szEvent, FMOD_EVENT_INFOONLY, &handle);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::GetGroupEventId");
		return 0;
	}
	
	int GlobalId = this->GetGlobalId();
	m_EventGroupMap.insert(std::pair <int, FMOD::EventGroup*>(GlobalId, eventGroup));
    return GlobalId;
}

int CMediaMgr::GetEventId(int eventId, const char* szEvent)
{
	if(!eventId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	FMOD::Event* handle;
	FMOD_RESULT result = m_EventSystem->getEvent(szEvent, FMOD_EVENT_INFOONLY, &handle);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::GetEventId");
		return 0;
	}
	
	int GlobalId = this->GetGlobalId();
	m_EventMap.insert ( std::pair <int, FMOD::Event*>  ( GlobalId, handle ) );
	
    return GlobalId;
}

int CMediaMgr::GetEventChannelGroupId(int eventId)
{
	if(!eventId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return 0;
	}
	
	FMOD::Event* event = it->second;
	
	FMOD::ChannelGroup* channelGroup = NULL;
	FMOD_RESULT result = event->getChannelGroup(&channelGroup);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result, "CMediaMgr::GetEventChannelGroupId");
		return 0;
	}
	
	int GlobalId = this->GetGlobalId();
	m_ChannelGroupMap.insert(std::pair <int, FMOD::ChannelGroup*>(GlobalId, channelGroup));
	
    return GlobalId;
}

int CMediaMgr::SetEventVolume(int eventId, float volume)
{
	if(!eventId)
	{
		return 0;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return 0;
	}
	
	FMOD::Event* event = it->second;
	
	if (event != NULL)
	{
		FMOD_RESULT result = event->setVolume(volume);
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::SetEventVolume");
			return 0;
		}
		return 1;
	}
	return 0;
}

float CMediaMgr::GetEventVolume(int eventId)
{
	if(!eventId)
	{
		return -1;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return 0;
	}
	
	FMOD::Event* event = it->second;
	
	if (event != NULL)
	{
		float volume;
		FMOD_RESULT result = event->getVolume(&volume);
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::GetEventVolume");
			return -1;
		}
		return volume;
	}
	return -1;
}

int CMediaMgr::SetEventPaused(int eventId, bool paused)
{
	if(!eventId)
	{
		return 0;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return 0;
	}
	
	FMOD::Event* event = it->second;
	
	if (event != NULL)
	{
		FMOD_RESULT result = event->setPaused(paused);
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::SetEventPaused");
			return 0;
		}
		return 1;
	}
	return 0;
}

int CMediaMgr::GetEventPaused(int eventId)
{
	if(!eventId)
	{
		return 0;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return 0;
	}
	
	FMOD::Event* event = it->second;
	
	bool pause= false;
	if (event != NULL)
	{
		FMOD_RESULT result = event->getPaused(&pause);
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::GetEventPaused");
			return 0;
		}
		return 1;
	}
	if (pause)
	{
		return 1;
	}
	return 0;
}

int CMediaMgr::SetEventMute(int eventId, bool mute)
{
	if(!eventId)
	{
		return 0;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return 0;
	}
	
	FMOD::Event* event = it->second;
	
	if (event != NULL)
	{
		FMOD_RESULT result = event->setMute(mute);
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::GetEventMute");
			return 0;
		}
		return 1;
	}
	return 0;
}

int CMediaMgr::GetEventMute(int eventId)
{
	if(!eventId)
	{
		return 0;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return 0;
	}
	
	FMOD::Event* event = it->second;
	
	bool mute=false;
	if (event != NULL)
	{
		FMOD_RESULT result = event->getMute(&mute);
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::GetEventMute");
			return 0;
		}
	}
	if (mute) 
		return 1;
	else
		return 0;
}

void CMediaMgr::EventStart(int eventId)
{
	if(!eventId)
	{
		return;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return;
	}
	
	FMOD::Event* event = it->second;
	
	if (event != NULL)
	{
		FMOD_RESULT result = event->start();
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::EventStart");
		}
	}
}

void CMediaMgr::EventStop(int eventId, bool isimmediate)
{
	if(!eventId)
	{
		return;
	}
	
	EventMapType::iterator it;
	it = m_EventMap.find(eventId);
	if(it == m_EventMap.end())
	{
		return;
	}
	
	FMOD::Event* event = it->second;
	
	if (event != NULL)
	{
		FMOD_EVENT_STATE state = 0;
		if (event->getState(&state) != FMOD_OK)
			return;

		FMOD_RESULT result = event->stop(isimmediate);
		if (result != FMOD_OK)
		{
			WRITEFMODERROR(result, "CMediaMgr::EventStop");
		}
	}
}

//ÒôÀÖ
int CMediaMgr::GetMusicId()
{
	if (!IsInitOK())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = NULL;
	
	FMOD_RESULT result = m_EventSystem->getMusicSystem(&musicSystem);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::GetMusicId");
		return 0;
	}
	
	int GlobalId = this->GetGlobalId();
	m_MusicSystemMap.insert(std::pair <int, FMOD::MusicSystem*>(GlobalId, musicSystem));
	
    return GlobalId;
}

int CMediaMgr::SetMusicVolume(int musicId,float volume)
{
	if(!musicId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	FMOD_RESULT result = musicSystem->setVolume(volume);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::SetMusicVolume");
		return 0;
	}
	return 1;
}

float CMediaMgr::GetMusicVolume(int musicId)
{
	if(!musicId)
	{
		return -1.0;
	}
	
	if (!IsInitOK())
	{
		return -1.0;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	float volume;
	FMOD_RESULT result = musicSystem->getVolume(&volume);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::GetMusicVolume");
		return -1.0;
	}
	
	return volume;
}

int CMediaMgr::SetMusicPaused(int musicId,bool paused)
{
	if(!musicId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	FMOD_RESULT result = musicSystem->setPaused(paused);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::SetMusicPaused");
		return 0;
	}
	
	return 1;
}

int CMediaMgr::GetMusicPaused(int musicId)
{
	if(!musicId)
	{
		return -1;
	}
	
	if (!IsInitOK())
	{
		return -1;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	bool paused;
	FMOD_RESULT result = musicSystem->getPaused(&paused);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::GetMusicPaused");
		return -1;
	}
	
	if(paused)
	{
		return 1;
	}
	return 0;
}

int CMediaMgr::SetMusicMute(int musicId,bool mute)
{
	if(!musicId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	FMOD_RESULT result = musicSystem->setMute(mute);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::SetMusicMute");
		return 0;
	}
	return 1;
}

int CMediaMgr::GetMusicMute(int musicId)
{
	if(!musicId)
	{
		return -1;
	}
	
	if (!IsInitOK())
	{
		return -1;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	bool mute;
	FMOD_RESULT result = musicSystem->getMute(&mute);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::GetMusicMute");
		return -1;
	}
	
	if(mute)
	{
		return 1;
	}
	return 0;
}

int CMediaMgr::LoadMusicData(int musicId,int res, unsigned  int mod)
{
	if(!musicId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	FMOD_EVENT_RESOURCE resource;
	FMOD_EVENT_MODE mode = (FMOD_EVENT_MODE)mod;
	
	switch (res)
	{
	    case 0:
			resource = FMOD_EVENT_RESOURCE_STREAMS_AND_SAMPLES;
			break;
	    case 1:
			resource = FMOD_EVENT_RESOURCE_STREAMS;
			break;
	    case 2:
			resource = FMOD_EVENT_RESOURCE_SAMPLES;
			break;
		default:
			resource = FMOD_EVENT_RESOURCE_STREAMS_AND_SAMPLES;
			break;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	FMOD_RESULT result = musicSystem->loadSoundData(resource,mode);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::LoadMusicData");
		return 0;
	}
	return 1;
}

int CMediaMgr::FreeMusicData(int musicId,bool waituntilready)
{
	if(!musicId)
	{
		return 0;
	}
	
	if (!IsInitOK())
	{
		return 0;
	}
	
	MusicSystemType::iterator it;
	it = m_MusicSystemMap.find(musicId);
	if(it == m_MusicSystemMap.end())
	{
		return 0;
	}
	
	FMOD::MusicSystem* musicSystem = it->second;
	
	FMOD_RESULT result = musicSystem->freeSoundData(waituntilready);
	if (result != FMOD_OK)
	{
		WRITEFMODERROR(result,"CMediaMgr::FreeMusicData");
		return 0;
	}
	return 1;
}

void CMediaMgr::WriteError(FMOD_RESULT error, const char* log)
{
	const char * begin = "--------------fmod error begin-----------------";
	const char * end = "--------------fmod error end-------------------";
	//printf("%s\n%s:\n%s\n%s\n", begin, log, FMOD_ErrorString(error), end);
	CCLOG("%s\n%s:\n%s\n%s\n", begin, log,FMOD_ErrorString(error), end);
}


