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
		{L"BRUSHES", AudioPhrases::Brushes},
		{L"BRUSH-RAISE", AudioPhrases::BrushRaise},
		{L"BRUSH-LOWER", AudioPhrases::BrushLower},
		{L"BRUSH-DEFORM", AudioPhrases::BrushDeform},
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
		m_gui->SetState(GuiState::MainMenu);
	}
	else if (m_saidViscraft)
	{
		if (action == AudioPhrases::Cancel)
		{
			m_saidViscraft = false;
			m_gui->SetVisible(false);
			return;
		}

		if (m_gui->GetState() == GuiState::MainMenu)
		{
			if (action == AudioPhrases::ExitApplication && m_gui->GetState() == GuiState::MainMenu)
			{
				CVisCraft::GetInstance()->Close();
			}
			else if (action == AudioPhrases::Brushes)
			{
				m_gui->SetState(GuiState::Brushes);
			}
			return;
		}

		if (m_gui->GetState() == GuiState::Brushes)
		{
			bool handled = false;
			if (action == AudioPhrases::BrushRaise)
			{
				handled = true;
				CVisCraft::GetInstance()->GetGizmo()->SetCurrentBrush(BrushType::Raise);
			}
			else if (action == AudioPhrases::BrushLower)
			{
				handled = true;
				CVisCraft::GetInstance()->GetGizmo()->SetCurrentBrush(BrushType::Lower);
			}
			else if (action == AudioPhrases::BrushDeform)
			{
				handled = true;
				CVisCraft::GetInstance()->GetGizmo()->SetCurrentBrush(BrushType::Deform);
			}

			if (handled) 
			{
				m_saidViscraft = false;
				m_gui->SetVisible(false);
			}
			return;
		}

	}
}
