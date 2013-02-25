#include "CAudioProcessor.h"


CAudioProcessor::CAudioProcessor()
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
		{L"EXIT", AudioPhrases::ExitApplication},
		{L"TYRONE", AudioPhrases::Tyrone},
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
		return;

	// handle phrase
	if (action == AudioPhrases::ExitApplication)
	{
		exit(0);
	}
	else if (action == AudioPhrases::Tyrone)
	{
		ShellExecute(NULL, "open", "http://lmgtfy.com/?q=Tyrone+is+a+girl", NULL, NULL, SW_SHOWMAXIMIZED);
	}
}
