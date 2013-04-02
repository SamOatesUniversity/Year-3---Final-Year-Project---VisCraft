#include "CAudioProcessor.h"
#include "../cviscraft.h"

CAudioProcessor::CAudioProcessor(
		CGui *gui
	) : m_saidViscraft(false)
{
	m_gui = gui;
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
		{L"CANCEL", AudioPhrases::Cancel},
	};

	AudioPhrases::Enum action = AudioPhrases::Noof;
	for (unsigned int actionIndex = 0; actionIndex < AudioPhrases::Noof; ++actionIndex)
	{
		if (wcscmp(Map[actionIndex].pszSpeechTag, tag) == NULL)
		{
			action = Map[actionIndex].action;
			break;
		}
	}

	// unknown phrase
	if (action == AudioPhrases::Noof) 
	{
		return;
	}

	// handle phrase
	if (action == AudioPhrases::VisCraft)
	{
		m_saidViscraft = true;
		m_gui->SetVisible(true);
	}
	else if (m_saidViscraft)
	{
		if (action == AudioPhrases::ExitApplication)
		{
			CVisCraft::GetInstance()->Close();
		}
		else if (action == AudioPhrases::Cancel)
		{
			m_saidViscraft = false;
			m_gui->SetVisible(false);
		}
	}


	if (action != AudioPhrases::VisCraft)
	{
		m_saidViscraft = false;
		m_gui->SetVisible(false);
	}
}
