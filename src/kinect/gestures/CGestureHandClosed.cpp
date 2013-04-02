#include "CGestureHandClosed.h"


CGestureHandClosed::CGestureHandClosed() : CDeformableTemplateModel()
{
	SetCenterPoint(SAM::TVector<float, 2>(20.0f, 20.0f), SAM::TVector<BYTE, 3>(0, 0, 0), 255);
	AddPoint(SAM::TVector<float, 2>(-18.0f, 0.0f), SAM::TVector<BYTE, 3>(255, 255, 255), 0);
	AddPoint(SAM::TVector<float, 2>(18.0f, 0.0f), SAM::TVector<BYTE, 3>(255, 255, 255), 0);
	AddPoint(SAM::TVector<float, 2>(0.0f, -18.0f), SAM::TVector<BYTE, 3>(255, 255, 255), 0);
}

CGestureHandClosed::~CGestureHandClosed()
{

}
