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
		{L"FILE", AudioPhrases::File},
		{L"FILE-NEW", AudioPhrases::FileNew},
		{L"FILE-OPEN", AudioPhrases::FileOpen},
		{L"FILE-SAVE", AudioPhrases::FileSave},
		{L"ABOUT", AudioPhrases::About},
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
			if (action == AudioPhrases::ExitApplication)
			{
				CVisCraft::GetInstance()->Close();
			}
			else if (action == AudioPhrases::Brushes)
			{
				m_gui->SetState(GuiState::Brushes);
			}
			else if (action == AudioPhrases::File)
			{
				m_gui->SetState(GuiState::File);
			}
			else if (action == AudioPhrases::About)
			{
				ShellExecute(NULL, "open", "http://www.samoatesgames.com", NULL, NULL, SW_SHOWMAXIMIZED);
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
				m_gui->SetActiveBrush(BrushType::Raise);
			}
			else if (action == AudioPhrases::BrushLower)
			{
				handled = true;
				CVisCraft::GetInstance()->GetGizmo()->SetCurrentBrush(BrushType::Lower);
				m_gui->SetActiveBrush(BrushType::Lower);
			}
			else if (action == AudioPhrases::BrushDeform)
			{
				handled = true;
				CVisCraft::GetInstance()->GetGizmo()->SetCurrentBrush(BrushType::Deform);
				m_gui->SetActiveBrush(BrushType::Deform);
			}
			
			if (handled) 
			{
				m_saidViscraft = false;
				m_gui->SetVisible(false);
			}
			return;
		}

		if (m_gui->GetState() == GuiState::File)
		{
			bool handled = false;
			if (action == AudioPhrases::FileNew)
			{
				handled = true;
				CVisCraft::GetInstance()->NewTerrain();
			}
			else if (action == AudioPhrases::FileOpen)
			{
				handled = true;
				CVisCraft::GetInstance()->OpenTerrain();
			}
			else if (action == AudioPhrases::FileSave)
			{
				handled = true;
				CVisCraft::GetInstance()->SaveTerrain();
			}
			else if (action == AudioPhrases::ExitApplication)
			{
				handled = true;
				CVisCraft::GetInstance()->Close();
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
