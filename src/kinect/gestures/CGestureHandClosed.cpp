#include "CGestureHandClosed.h"


CGestureHandClosed::CGestureHandClosed() : CDeformableTemplateModel()
{
	SetCenterPoint(SAM::TVector<float, 2>(10.0f, 10.0f), SAM::TVector<BYTE, 3>(0, 0, 0), 255);
	AddPoint(SAM::TVector<float, 2>(-9.0f, 0.0f), SAM::TVector<BYTE, 3>(255, 255, 255), 0);
	AddPoint(SAM::TVector<float, 2>(9.0f, 0.0f), SAM::TVector<BYTE, 3>(255, 255, 255), 0);
	AddPoint(SAM::TVector<float, 2>(0.0f, 9.0f), SAM::TVector<BYTE, 3>(255, 255, 255), 0);
}

CGestureHandClosed::~CGestureHandClosed()
{

}
