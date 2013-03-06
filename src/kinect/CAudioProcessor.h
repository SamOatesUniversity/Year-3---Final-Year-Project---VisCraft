#pragma  once

#include <sapi.h>

class CAudioProcessor {

private:
	struct AudioPhrases {
		enum Enum {
			VisCraft,
			ExitApplication,
			Noof
		};
	};
private:

	bool									m_saidViscraft;

public:
											CAudioProcessor();

											~CAudioProcessor();

	void									Process(
												const SPPHRASEPROPERTY* semantic
											);

};