#include "soundEngine.h"

CSoundEngine GSoundEngine;

void CSoundEngine::Init()
{
	m_irrklangEngine = irrklang::createIrrKlangDevice();
}

void CSoundEngine::Release()
{
	m_irrklangEngine->drop();
}

CStaticSound CSoundEngine::CreateStaticSound(char const* path) const
{
	CStaticSound sound;
	sound.m_saveSound = m_irrklangEngine->addSoundSourceFromFile(path, irrklang::ESM_AUTO_DETECT, true);
	return sound;
}

CPlayedSound CSoundEngine::Play2DSound(CStaticSound& staticSound) const
{
	CPlayedSound playedSound;
	playedSound.m_playedSound = m_irrklangEngine->play2D(staticSound.m_saveSound, false, false, true);
	return playedSound;
}

bool CPlayedSound::IsFinished() const
{
	return m_playedSound->isFinished();
}
