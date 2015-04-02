#pragma once

/*

*/
#include "fmod_event.hpp"

#include <map>

#ifndef WRITEFMODERROR
#define WRITEFMODERROR(error,log) WriteError(error,log)
#endif

class CMediaMgr
{
private:
	FMOD::System *m_System;
    FMOD::EventSystem *m_EventSystem;
    typedef std::map<int,FMOD::EventProject*> EventProjectMapType;
    typedef std::map<int,FMOD::Event*> EventMapType;
    typedef std::map<int,FMOD::EventGroup*> EventGroupType;
    typedef std::map<int,FMOD::ChannelGroup*> ChannelGroupType;
	typedef std::map<int, FMOD::MusicSystem*> MusicSystemType;
    
    EventProjectMapType m_EventProjectMap;
    EventMapType m_EventMap;
    EventGroupType m_EventGroupMap;
    ChannelGroupType m_ChannelGroupMap;
	MusicSystemType m_MusicSystemMap;
    
	bool m_bInitOK;
	int m_nEventIdx;
    int m_GlobalId;
    
public:
	CMediaMgr();
	~CMediaMgr();
    
	static CMediaMgr& Instance();
    
    int GetGlobalId();
    
	bool IsInitOK();
	void WriteError(FMOD_RESULT error,const char* log);
    
	int LoadFevFile(const char* szFev);
    
    int FmodUpdate();
    
    //sound
	int LoadEventGroup(int projectId, const char* szGroup );
	int FreeEventGroup(int projectId, const char* szGroup);
	int Play(int projectId, const char* szEvent);
    
	int GetGroupId(int projectId, const char* szGroup);
	int GetGroupEventId(int projectId, const char* szGroup, const char *szEvent);
	int GetEventId(int projectId, const char* szEvent);
    int GetEventChannelGroupId(int eventId);
    
	int SetEventVolume(int eventId, float volume);
	float GetEventVolume(int eventId);
	int SetEventPaused(int eventId, bool paused);
	int GetEventPaused(int eventId);
	int SetEventMute(int eventId, bool mute);
	int GetEventMute(int eventId);
	void EventStart(int eventId);
	void EventStop(int eventId, bool isimmediate);
    
    //music
    int GetMusicId();
    int SetMusicVolume(int musicId,float volume);
    float GetMusicVolume(int musicId);
    int SetMusicPaused(int musicId,bool paused);
	int GetMusicPaused(int musicId);
    int SetMusicMute(int musicId,bool mute);
	int GetMusicMute(int musicId);
	int LoadMusicData(int musicId, int res, unsigned  int mod);
    int FreeMusicData(int musicId,bool waituntilready);
};
