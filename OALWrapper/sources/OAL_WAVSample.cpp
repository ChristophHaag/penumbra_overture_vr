/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_WAVSample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing WAV Sample data 
*/

#include "OALWrapper/OAL_WAVSample.h"
#include "OALWrapper/OAL_Buffer.h"
#include "OALWrapper/OAL_Helper.h"
#include "OALWrapper/OAL_Device.h"

//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void Load ( const string &asFilename )
//	-	Loads sample data from a WAV file
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cOAL_WAVSample::CreateFromFile(const wstring &asFilename)
{
	DEF_FUNC_NAME("cOAL_WAVSample::Load()");
	FUNC_USES_AL;
	
	if(mbStatus==false)
		return false;

	Reset();

	ALenum	status;
	ALvoid	*pPCMBuffer = NULL;
	ALsizei	lSize;

	msFilename = asFilename;

	// TEMP: Need to get rid of the ALUT funcs to use Unicode here :S
	string sFilename = WString2String(asFilename);
	///////////////////////////////////////////////////////////////
	// This worked indeed, but didnt return a freeable pointer. 
	// Will be used when some fix is found
	//pPCMBuffer = alutLoadMemoryFromFile ( asFilename.c_str(), &eFormat, &lDataSize, &fFrequency );
	
	#if defined(__APPLE__)
	alutLoadWAVFile ( (ALbyte*) sFilename.c_str(), &mFormat, &pPCMBuffer, &lSize, &mlFrequency);
	#else
	alutLoadWAVFile ( (ALbyte*) sFilename.c_str(), &mFormat, &pPCMBuffer, &lSize, &mlFrequency, AL_FALSE);
	#endif
	status = alutGetError ();
	switch (status)
	{
		case ALUT_ERROR_NO_ERROR:
			break;
        default:
			mbStatus = false;
			break;
	}

	cOAL_Buffer* pBuffer = mvBuffers.front();
	if(pBuffer->Feed(pPCMBuffer, lSize)==false)
	{
		mlBuffersUsed = 1;
		//free( pPCMBuffer );
		alutUnloadWAV( mFormat, pPCMBuffer, lSize, mlFrequency);
		status = alutGetError();
		mbStatus = false;
		return false;
	}

	RUN_AL_FUNC(alGetBufferi( pBuffer->GetObjectID(), AL_CHANNELS, &mlChannels ));
	
	mlSamples = lSize/(mlChannels*GetBytesPerSample());

	mfTotalTime = ((double)mlSamples)/mlFrequency;
	
	
//	free( pPCMBuffer );
	alutUnloadWAV( mFormat, pPCMBuffer, lSize, mlFrequency);
	status = alutGetError ();

	return true;
}

//------------------------------------------------------------------


