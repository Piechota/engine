#pragma once
#include "utility/irrKlang.h"

class CSoundEngine;
class CStaticSound
{
	friend class CSoundEngine;
		
private:
	irrklang::ISoundSource* m_saveSound;
};
class CPlayedSound
{
	friend class CSoundEngine;

private:
	irrklang::ISound* m_playedSound;

public:
	bool IsFinished() const;
};

class CSoundEngine
{
private:
	irrklang::ISoundEngine* m_irrklangEngine;

public:
	void Init();
	void Release();

	CStaticSound CreateStaticSound(char const* path) const;
	CPlayedSound Play2DSound(CStaticSound& staticSound) const;
};

extern CSoundEngine GSoundEngine;
extern CStaticSound GSounds[];
