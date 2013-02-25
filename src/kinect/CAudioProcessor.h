#pragma  once

#include <sapi.h>

class CAudioProcessor {

private:
	struct AudioPhrases {
		enum Enum {
			ExitApplication,
			Tyrone,
			Noof
		};
	};
private:

public:
											CAudioProcessor();

											~CAudioProcessor();

	void									Process(
												const SPPHRASEPROPERTY* semantic
											);

};