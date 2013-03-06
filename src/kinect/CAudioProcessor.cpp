#include "CAudioProcessor.h"


CAudioProcessor::CAudioProcessor() : m_saidViscraft(false)
{

}

CAudioProcessor::~CAudioProcessor()
{

}

void CAudioProcessor::Process( 
		const SPPHRASEPROPERTY* semantic 
	)
{
	LPCWSTR tag = semantic->pszValue;

	struct SpeechTagToAction
	{
		LPCWSTR pszSpeechTag;
		AudioPhrases::Enum action;
	};

	static const SpeechTagToAction Map[] =
	{
		{L"VISCRAFT", AudioPhrases::VisCraft},
		{L"EXIT", AudioPhrases::ExitApplication},
	};

	AudioPhrases::Enum action = AudioPhrases::Noof;
	for (unsigned int actionIndex = 0; actionIndex < AudioPhrases::Noof; ++actionIndex)
	{
		if (wcscmp(Map[actionIndex].pszSpeechTag, tag) == NULL)
		{
			action = Map[actionIndex].action;
		}
	}

	// unknown phrase
	if (action == AudioPhrases::Noof) 
	{
		m_saidViscraft = false;
		return;
	}

	// handle phrase
	if (action == AudioPhrases::VisCraft)
	{
		m_saidViscraft = true;
	}
	else if (m_saidViscraft)
	{
		if (action == AudioPhrases::ExitApplication)
		{
			exit(0);
		}
	}


	if (action != AudioPhrases::VisCraft)
	{
		m_saidViscraft = false;
	}
}
