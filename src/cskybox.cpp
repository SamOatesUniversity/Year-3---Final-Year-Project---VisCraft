#include "cskybox.h"

CSkyBox::CSkyBox()
{
	m_mesh = nullptr;
}

CSkyBox::~CSkyBox()
{

}

bool CSkyBox::Create(
		CRenderer *renderer
	)
{
	m_mesh = new CMesh();
	m_mesh->LoadMesh(renderer, "skybox/skysphere.obj");

	return true;
}

void CSkyBox::Render( 
		CRenderer *renderer 
	)
{
	//m_mesh->PrepareRender(renderer);


	//m_mesh->Draw(renderer);
}
