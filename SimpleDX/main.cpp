//////////////////////////////////////////////////////////////////////////
#include <windows.h> //Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra WinAPI
#include <d3d9.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra DirectX
#include <d3dx9.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra DirectX
#include <math.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra matematyczne
#include <D3D9Types.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra DirectX
//#pragma comment(lib, "dxerr9.lib") // Dolaczamy biblioteke dxerr9.lib
#pragma comment(lib, "dxErr.lib") // Dolaczamy biblioteke dxErr.lib
#pragma comment(lib, "d3d9.lib") // Dolaczamy biblioteke d3d9.lib
#pragma comment(lib, "d3dx9.lib") // Dolaczamy biblioteke d3dx9.lib
#pragma comment(lib, "winmm.lib") // Dolaczamy biblioteke winmm.lib
#define SR(x) if(x){ x->Release(); x = NULL;} // makro do zwalniania zasobow
#define SAFE_RELEASE(x) {if(x) x->Release();x=NULL;} // makro do zwalniania zasobow
#define SAFE_DELETE(x) {if(x) delete(x);x=NULL;} // makro do usuwania zasobow
#define SDEL(x) {if(x) delete(x);x=NULL;} // makro do usuwania zasobow

LPDIRECT3D9 lpD3D = NULL; //Obiekt D3D 
 
LPDIRECT3DDEVICE9 lpD3DDevice = NULL; // Urzadzenie D3D


LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclarationPP ;
LPD3DXCONSTANTTABLE		m_pVertexConstantsPP;
LPD3DXEFFECT			m_pEffectPP;



LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration; // Deklaracja wierzcholkow. W niej podajemy z czego sklada sie struktura wierzcholka

LPD3DXEFFECT m_pEffect; //Wskaznik na efekt

	
int mShader=0;
float mBlurPower=1;



///   depth
LPDIRECT3DTEXTURE9		lpTexDepth;
LPD3DXRENDERTOSURFACE		m_pRenderToDepth;
LPDIRECT3DSURFACE9		pd3dsSurfaceDepth;
////

LPDIRECT3DSURFACE9 		lpBackBufferTmp;

LPDIRECT3DTEXTURE9		lpTexture;
LPD3DXRENDERTOSURFACE		m_pRenderToSurface;
LPDIRECT3DSURFACE9		pd3dsSurface;




//Macierze swiata, widoku oraz projekcji
D3DXMATRIX m_matWorld;
D3DXMATRIX m_matView;
D3DXMATRIX m_matProj;
D3DXMATRIX m_matTrans;
D3DXMATRIX m_matRot;
D3DXMATRIX m_matScale;
// Przesuniecie
D3DXVECTOR3 m_vTranslation = D3DXVECTOR3(0, 0, 10);
int m_iTimer=0;

//Meshes
LPD3DXMESH lpMesh = NULL;

//Textures
LPDIRECT3DTEXTURE9 lpTex = NULL;


struct Vert{

	D3DXVECTOR4 pos;
	float u1,v1;

};

Vert vertices[4];



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//////////////////////////////////////////////////////////////////////////
// Funkcja inicjalizujaca efekt. Tworzy efekt z pliku Shader.fx, tworzy deklaracje wierzcholka itd.
//////////////////////////////////////////////////////////////////////////


bool InitEffect()
{


	LPD3DXBUFFER pErrorMsgs = NULL; // Bufor na ewentualne bledy
	DWORD dwFlags = 0;  // Flagi
	dwFlags |= D3DXSHADER_DEBUG | D3DXSHADER_NO_PRESHADER | D3DXSHADER_PREFER_FLOW_CONTROL;

	// Deklarujemy ze w strukturze wierzcholka znajduje sie jedynie pozycja zapisana na 4 float'ach oraz kolor takze na 4 float'ach
	D3DVERTEXELEMENT9 decl[] = 
	{
		{0,0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0,28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	lpD3DDevice->CreateVertexDeclaration( decl, &m_pVertexDeclaration ); // Tworzymy Deklaracje


	HRESULT hr; 
	// Tworzymy efekt z pliku podajac urzadzenie, nazwe pliku, wskaznik na efekt i wskaznik na bufor bledow
	hr = D3DXCreateEffectFromFile(	lpD3DDevice, 
		L"shader.fx",
		NULL, 
		NULL, 
		0, 
		NULL, 
		&m_pEffect, 
		&pErrorMsgs );
	// Jezeli nie udalo nam sie stworzyc efektu to wyrzucamy na ekran informacje z bledem
	if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pErrorMsgs->GetBufferPointer();

		char* tmp = (char*)pCompilErrors;
		size_t size = strlen(tmp)+1 ;


		short* psMyName = new short[size ]; 

		for( int i = 0 ; psMyName[ i ] = tmp[ i ++ ] ; );  //interesujaca pêtla kopiujaca stringa zakonczonego NULLem
		MessageBox(NULL, ((LPCWSTR) psMyName), L"EffectContainer Fx Compile Error", MB_OK|MB_ICONEXCLAMATION);
		delete[] psMyName;
		return false;
	} 
	SR(pErrorMsgs);

	return true;
}



bool InitEffectPP()
{


	LPD3DXBUFFER pErrorMsgs = NULL; // Bufor na ewentualne bledy
	DWORD dwFlags = 0;  // Flagi
	dwFlags |= D3DXSHADER_DEBUG | D3DXSHADER_NO_PRESHADER | D3DXSHADER_PREFER_FLOW_CONTROL;

	// Deklarujemy ze w strukturze wierzcholka znajduje sie jedynie pozycja zapisana na 4 float'ach oraz kolor takze na 4 float'ach
	/*D3DVERTEXELEMENT9 decl[] = 
	{
		{0,0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0,28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};*/

	D3DVERTEXELEMENT9 decl[] = 
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};


	lpD3DDevice->CreateVertexDeclaration( decl, &m_pVertexDeclarationPP ); // Tworzymy Deklaracje

	HRESULT hr = S_OK; 
	// Tworzymy efekt z pliku podajac urzadzenie, nazwe pliku, wskaznik na efekt i wskaznik na bufor bledow
	hr = D3DXCreateEffectFromFile(	lpD3DDevice, 
		L"PostProcess1.fx",
		NULL, 
		NULL, 
		0, 
		NULL, 
		&m_pEffectPP, 
		&pErrorMsgs );
// Jezeli nie udalo nam sie stworzyc efektu to wyrzucamy na ekran informacje z bledem
	if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pErrorMsgs->GetBufferPointer();

		char* tmp = (char*)pCompilErrors;
		size_t size = strlen(tmp)+1 ;


		short* psMyName = new short[size ]; 

		for( int i = 0 ; psMyName[ i ] = tmp[ i ++ ] ; );  //interesujaca pêtla kopiujaca stringa zakonczonego NULLem
		MessageBox(NULL, ((LPCWSTR) psMyName), L"EffectContainer Fx Compile Error", MB_OK|MB_ICONEXCLAMATION);
		delete[] psMyName;
		return false;
	} 
	SR(pErrorMsgs);

	//HRESULT  hr = S_OK;// = lpD3DDevice->CreateRenderTarget(    
	D3DDISPLAYMODE mode;
	lpD3DDevice->GetDisplayMode(0,&mode);

	D3DXCreateTexture(  lpD3DDevice,
		800,
		600,
		1,
		D3DUSAGE_RENDERTARGET ,
		mode.Format,
		D3DPOOL_DEFAULT,
		&lpTexture
		);

	D3DSURFACE_DESC desc;
	hr = lpTexture->GetSurfaceLevel(0, &pd3dsSurface);
	pd3dsSurface->GetDesc(&desc);

	if(FAILED(hr=D3DXCreateRenderToSurface(lpD3DDevice, desc.Width, desc.Height, mode.Format, TRUE, D3DFMT_D16, &m_pRenderToSurface)))
	{
		return false;
	}


	lpD3DDevice->CreateDepthStencilSurface(
		800,
		600,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&pd3dsSurfaceDepth,
		NULL  );

	return true;

}



void BeginFrame(){
	
	lpD3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&lpBackBufferTmp);


	lpD3DDevice->SetDepthStencilSurface(pd3dsSurfaceDepth);


	lpD3DDevice->SetRenderTarget(0,pd3dsSurface);


	(lpD3DDevice)->BeginScene(); //Begins a scene

	lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0);

}

void EndFrame(){

	(lpD3DDevice)->EndScene(); 
	(lpD3DDevice)->SetRenderTarget(0,lpBackBufferTmp);
	(lpD3DDevice)->SetRenderTarget(1,0);
	(lpD3DDevice)->SetRenderTarget(2,0);
	SR(lpBackBufferTmp);

}


void RenderPP(){

	lpD3DDevice->BeginScene();

	lpD3DDevice->SetVertexDeclaration( m_pVertexDeclarationPP );

	// Kod pozwalaj¹cy wygodnie zmieniaæ aktywny shader
	switch(mShader){

		case 0:
			m_pEffectPP->SetTexture("Tex01",lpTexture );	
			m_pEffectPP->SetTechnique("Blur");
			break;
		case 1:
			m_pEffectPP->SetTexture("Tex01",lpTexture );	
			m_pEffectPP->SetTechnique("Tec0");
			break;
		case 2:
			m_pEffectPP->SetTexture("Tex01",lpTexture );	
			m_pEffectPP->SetTechnique("Mono");
			break;
	}



	m_pEffectPP->SetFloat("hResX", 800);
	m_pEffectPP->SetFloat("hResY", 600);
	m_pEffectPP->SetFloat("hResDivider", mBlurPower);


	UINT uPasses;

	if (D3D_OK == m_pEffectPP->Begin(&uPasses, D3DXFX_DONOTSAVESTATE)) 
	{  
		for (UINT uPass = 0; uPass < uPasses; uPass++) 
		{
			m_pEffectPP->BeginPass(uPass);

			//			setShaderConstants();
			m_pEffectPP->CommitChanges();
			lpD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,
				2,
				&vertices,
				sizeof(Vert)
				);

			m_pEffectPP->EndPass();
		}
		m_pEffectPP->End();
	}
	lpD3DDevice->SetRenderTarget(1,0);

	m_pEffectPP->SetTexture("Tex01",0 );	


	// Zerowanie tekstur
	lpD3DDevice->SetTexture(0,0);
	lpD3DDevice->SetTexture(1,0);
	lpD3DDevice->SetTexture(2,0);
	lpD3DDevice->SetTexture(3,0);
	lpD3DDevice->SetTexture(4,0);
	lpD3DDevice->SetTexture(5,0);	
	lpD3DDevice->SetTexture(6,0);


	lpD3DDevice->SetVertexShader(NULL);
	lpD3DDevice->SetPixelShader(NULL);

	lpD3DDevice->EndScene();

	return;

}



//////////////////////////////////////////////////////////////////////////
// Funkcja odpowiedzialna za wyswietlanie
//////////////////////////////////////////////////////////////////////////
/*int Render()
{
	// Czyscimy caly ekran, aby nie pozostaly zadne smieci po poprzedniej klatce. Czyscimy tez z-bufor
	if( lpD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ,
		D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0 )!= D3D_OK){
			// Jezeli nie udalo sie wyczyscic ekranu to wyswietlamy blad
			MessageBox(0,L"Error while clearing device - will exit",L"Error",0);

			return false;
	}

	lpD3DDevice->BeginScene(); //Rozpoczyna scene. Renderowac mozna tylko po jej rozpoczeciu
	// Ustawia deklaracje wierzcholkow

	m_pEffect->SetMatrix("matWorldViewProj", &(m_matWorld*m_matView*m_matProj)); // Przekazujemy do shader'a wymnozone macierze
	m_pEffect->SetMatrix("matWorld", &(m_matWorld)); // Przekazujemy do shader'a wymnozone macierze
	m_pEffect->SetMatrix("matRot", &(m_matRot)); // Przekazujemy do shader'a wymnozone macierze
	m_pEffect->SetVector("mLightPos", &(D3DXVECTOR4(10,10,0,0)));
	m_pEffect->SetVector("mEye", &(D3DXVECTOR4(0,0,0,0)));


	m_pEffect->SetTexture("tex",lpTex);
	lpD3DDevice->SetVertexDeclaration( m_pVertexDeclaration );
	UINT uPasses;
	if (SUCCEEDED(m_pEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE))) 
	{  
		for (UINT uPass = 0; uPass < uPasses; uPass++) 
		{
			// Rozpoczynamy przejscie
			m_pEffect->BeginPass(uPass);
			lpMesh->DrawSubset(0);
			// konczymy przejscie
			m_pEffect->EndPass();

		}
		m_pEffect->End();
	}



	lpD3DDevice->EndScene(); //konczymy scene. Po tym wywolaniu nie wolno juz nic renderowac

	lpD3DDevice->Present(0,0,0,0); //Wyswietlamy na ekranie to co wyrenderowalismy do backbufora


	return 1;
}*/

int Render()
{

	BeginFrame();

	m_pEffect->SetMatrix("matWorldViewProj", &(m_matWorld*m_matView*m_matProj)); // Przekazujemy do shader'a wymnozone macierze
	m_pEffect->SetMatrix("matWorld", &(m_matWorld)); // Przekazujemy do shader'a wymnozone macierze
	m_pEffect->SetMatrix("matRot", &(m_matRot)); // Przekazujemy do shader'a wymnozone macierze
	m_pEffect->SetVector("mLightPos", &(D3DXVECTOR4(10,10,0,0)));
	m_pEffect->SetVector("mEye", &(D3DXVECTOR4(0,0,0,0)));


	m_pEffect->SetTexture("tex",lpTex);
	lpD3DDevice->SetVertexDeclaration( m_pVertexDeclaration );
	UINT uPasses;
	if (SUCCEEDED(m_pEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE))) 
	{  
		for (UINT uPass = 0; uPass < uPasses; uPass++) 
		{
			// Rozpoczynamy przejscie
			m_pEffect->BeginPass(uPass);
			lpMesh->DrawSubset(0);
			// konczymy przejscie
			m_pEffect->EndPass();

		}
		m_pEffect->End();
	}



	EndFrame();
	RenderPP();

	lpD3DDevice->Present(0,0,0,0); //Wyswietlamy na ekranie to co wyrenderowalismy do backbufora


	return 1;
}




//////////////////////////////////////////////////////////////////////////
//	Update logiki
//////////////////////////////////////////////////////////////////////////
void Update(){


	
	++m_iTimer;
	D3DXMatrixTranslation(&m_matTrans,m_vTranslation.x,m_vTranslation.y,m_vTranslation.z);
	D3DXMatrixRotationYawPitchRoll(&m_matRot,(float)m_iTimer/100,0,0);
	D3DXMatrixScaling(&m_matScale,4,4,4);
	m_matWorld = m_matScale*m_matRot*m_matTrans;


}

//////////////////////////////////////////////////////////////////////////
// Funkcja inicjujaca DirectX
// Tworzy urzadzenie, bufor wierzcholkow itd
//////////////////////////////////////////////////////////////////////////
bool InitDx(HWND hWnd)
{
	//Tworzymy obiekt D3D
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if(!lpD3D){
		// Jesli sie nie udalo to wyswietlamy blad
		MessageBox(0,L"D3D object initialization error - no DirectX runtime?",L"Error",0);
		return false;
	}

	// ustawiamy parametry wyswietlania. Mozna tu ustawic rozdzielczosc, formaty buforow itd
	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp,sizeof(pp));				//Czyscimy aby nie miec w parametrach smieci z pamieci
	pp.Windowed = true;						// Wyswietlamy w okienkt
	pp.BackBufferFormat = D3DFMT_UNKNOWN;   // Nieznany format backbufora
	pp.SwapEffect = D3DSWAPEFFECT_COPY;		// wybieramy sposob zamiany back i frontbufora
	pp.EnableAutoDepthStencil = true;		// ustawiamy automatyczny bufor glebokosci 
	pp.AutoDepthStencilFormat = D3DFMT_D16;	// ustawiamy  format 16 bitowy bufora glebokosci
	
	// Probujemy stworzyc urzadzenie DirectX. W zaleznosci od mozliwosci karty graficznej mozemy nie moc stworzyc niektorych 
	// konfiguracji urzadzenia. Probujemy zatem od najlepszej do najgorszej konfiguracji
	if(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,&lpD3DDevice) != D3D_OK){
		MessageBox(0,L"Error while creating device (hardware vertex processing) - attempting to create software device",L"Error",0);
		
	}

	/*if(lpD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&pp,&lpD3DDevice) != D3D_OK){
				MessageBox(0,L"Error while creating REF device  - exiting",L"Error",0);
				return false; 
			}*/

	D3DXLoadMeshFromX(L"tiger.x",0,lpD3DDevice,0,0,0,0,&lpMesh);


	LPD3DXMESH pTempSysMemMesh = NULL;
	D3DVERTEXELEMENT9 decl[] = 
	{
		{0,0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0,28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	/*D3DVERTEXELEMENT9 decl[] = 
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};*/


	lpMesh->CloneMesh( D3DXMESH_SYSTEMMEM, decl,
		lpD3DDevice, &pTempSysMemMesh );

	SAFE_RELEASE( lpMesh );
	if( pTempSysMemMesh ) lpMesh = pTempSysMemMesh;
	D3DXComputeNormals(lpMesh, NULL);
	//Loading textures from files
	if(D3DXCreateTextureFromFile(lpD3DDevice,L"tiger.bmp",&lpTex) != D3D_OK){
		MessageBox(0,L"Error while loading texture",L"Error",0);
	}	


	//Inicializacja macierzy
	D3DXMatrixIdentity(&m_matWorld); 
	D3DXMatrixLookAtLH(&m_matView, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,10), &D3DXVECTOR3(0,1,0));
	D3DXMatrixPerspectiveFovLH(&m_matProj,D3DX_PI/4,800/600,1,1400);
	

	vertices[0].pos.x=-1.0f;			
	vertices[0].pos.y=1.0f;
	vertices[0].pos.z=0;
	vertices[0].pos.w=1.0f;
	vertices[0].u1=0;
	vertices[0].v1=0;
	vertices[1].pos.x=1.0f ;			
	vertices[1].pos.y=1.0f;
	vertices[1].pos.z=0;
	vertices[1].pos.w=1.0f;
	vertices[1].u1=1;
	vertices[1].v1=0;
	vertices[2].pos.x=-1.0f;			
	vertices[2].pos.y=-1.0f;
	vertices[2].pos.z=0;
	vertices[2].pos.w=1.0f;
	vertices[2].u1=0;
	vertices[2].v1=1;
	vertices[3].pos.x=1.0f ;			
	vertices[3].pos.y=-1.0f;
	vertices[3].pos.z=0;
	vertices[3].pos.w=1.0f;
	vertices[3].u1=1;
	vertices[3].v1=1;



	// wywolujemy funkcje InitEffectPP()
	//return InitEffect();
	return InitEffect() && InitEffectPP();


}
//////////////////////////////////////////////////////////////////////////
// Funkcja kasujaca obiekty i zwalniajaca zasoby.
//////////////////////////////////////////////////////////////////////////
int DelDx()
{
	SR(lpMesh);
	SR(lpTex);
	SR(m_pEffect);
	SR(m_pVertexDeclaration);



	SR(lpD3DDevice);
	SR(lpD3D);



	SR(m_pEffectPP);
	SR(m_pVertexDeclarationPP);

	SR(lpBackBufferTmp);
	SR(pd3dsSurfaceDepth);
	SR(pd3dsSurface);
	SR(lpTexture);
	SR(m_pRenderToSurface);


	return 1;
}
//////////////////////////////////////////////////////////////////////////
// Glowna funkcja w aplikacjach Win32. Tworzone jest tu okno i wywolywana petla wiadomosci
//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	MSG  msg ;    
	HWND hWnd;
	WNDCLASS wndClass;


	wndClass.style          = CS_HREDRAW | CS_VREDRAW;	//Styl okna - bedzie sie odmalowywalo przy kazdym przesunieciu lub zmianie rozdzielczosci   
	wndClass.lpfnWndProc    = WndProc;								//Wskazujemy procedure przetwarzajaca komunikaty okna
	wndClass.cbClsExtra     = 0;
	wndClass.cbWndExtra     = 0;
	wndClass.hInstance      = hInstance;							//Ustawiamy w oknie instancje naszego programu
	wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);		// Ladujemy z zasobow systemowych 
	wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);			// Domyslny kursor i ikone
	wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Kolor tla okna
	wndClass.lpszMenuName   = NULL;									// Nie tworzymy menu
	wndClass.lpszClassName  = TEXT("SimpleDX");				// Nazwa klasy okna, wyswietlana w naglowku okna

	RegisterClass(&wndClass);		//Rejestrujemy klase okna w systemie


	hWnd = CreateWindow(
		TEXT("SimpleDX"),   // nazwa klasy okna
		TEXT("SimpleDX"),  // tytul okna
		WS_OVERLAPPEDWINDOW,      // styl
		CW_USEDEFAULT,            // pozycja x okna
		CW_USEDEFAULT,            // pozycja y okna
		800,					  // szerokosc okna 
		600,					  // wysokosc okna
		NULL,                     
		NULL,                     
		hInstance,                
		NULL);                    



	RECT rect = { 0, 0, 800, 600 }; //Tworzymy prostokat o wymiarach 800x600
	AdjustWindowRect( &rect, GetWindowLong( hWnd, GWL_STYLE ), FALSE ); //Skalujemy okno tak aby obszar roboczy faktycznie mial 800x600px
	SetWindowPos( hWnd, 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE );

	ShowWindow(hWnd, nCmdShow);	//Wyswietlamy okno
	UpdateWindow(hWnd);	
	// Jesli nie uda sie zainicjowac DirectX to wychodzimy z programu
	if(!InitDx(hWnd)){
		return(0);
	}
	// Petla wiadomosci
	do
	{
		// sprawdzamy czy jest wiadomosc.
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			// Jesli jest wiadomosc to ja tlumaczy
			TranslateMessage(&msg);
			// I przkazujemy do okna
			DispatchMessage(&msg);
		}
		else // Jesli nie ma wiadomosci
		{
			Update();
			if(!Render()){ // Probujemy renderowac. Ale jesli sie nie uda
				DelDx(); // to usuwamy zasoby
				return 0; // i wychodzimy z programu
			}

		}
	}
	while(msg.message != WM_QUIT); 

	// Usuwamy zasoby 
	DelDx();

	return (int) msg.wParam;
}


LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

	
	switch(msg)  
	{
	case WM_DESTROY: // Zamkniecie okna

		PostQuitMessage(0); // Ta funkcja dodaje do kolejki wiadomosci WM_QUIT
		return 0;

	case WM_KEYDOWN: // Wcisniecie klawisza

		switch(wParam){

	case VK_UP:
		m_vTranslation.y +=0.1;
		break;
	case VK_DOWN:
		m_vTranslation.y -=0.1;
		break;
	case VK_RIGHT:
		m_vTranslation.x +=0.1;
		break;
	case VK_LEFT:
		m_vTranslation.x -=0.1;
		break;
	case VK_SPACE:
		break;
	case VK_ESCAPE: // jesli klawisz ESC
		PostQuitMessage(0); // Wylaczamy program
		break;



		case 'B':
		mShader = 0;
		break;
	case 'N':
		mShader = 1;
		break;
	case 'M':
		mShader = 2;
		break;
	case 'T':
		mBlurPower+=0.5;
		break;
	case 'Y':
		mBlurPower-=0.5;
		break;


	
		}

	}

	// wszystkie inne wiadomosci sa obslugiwane w sposob domyslny
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
