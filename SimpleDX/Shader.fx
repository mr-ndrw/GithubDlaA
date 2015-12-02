float4x4 matWorldViewProj; // Wymnozone macierze
float4x4 matWorld;
float4x4 matRot;
Texture tex;
float3	mLightPos;
float3	mEye;

struct VS_INPUT // Struktura wejsciowa Vertex Shadera
{
    float4  Pos     : POSITION; // Pozycja 
    float3  Normal	: NORMAL0; // Wektor normalny 
    float2	TexUV	: TEXCOORD0;   
};

struct VS_OUTPUT // Struktura wyjsciowa vertex Shadera i wejsciowa Pixel Shadera
{
    float4  Pos     : POSITION;
    float3  Normal	: TEXCOORD0; // Wektor normalny 
    float3  LightDir	: TEXCOORD1; // Wektor normalny 
    float2	TexUV	: TEXCOORD2;   
    float3 View		: TEXCOORD3;
 };

struct PS_OUTPUT // Struktura wyjsciowa Pixel Shadera
{
	float4 Color : COLOR; // kolor
};

uniform sampler2D Sampler0 = sampler_state
{
	Texture = <tex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
	AddressV = Wrap;
};

////////////////////////////////////////

VS_OUTPUT VShade(VS_INPUT In) // Ta funkcja wykonuje sie jako pierwsza. Dostaje od aplikacji pozycje i kolor wierzcholka
{
    VS_OUTPUT Out = (VS_OUTPUT) 0; // Zerujemy strukture wyjsciowa
    Out.Pos			=	mul(In.Pos,matWorldViewProj); // Jako pozycje wyjsciowa ustawiamy lokalna pozycje wierzcholka przemnozona przez macierze Swiata, Widoku i Projekcji
    Out.Normal		=	normalize(mul(In.Normal,matRot)); // Kolor po prostu przekazujemy
    Out.TexUV		=	In.TexUV; // Kolor po prostu przekazujemy
    Out.LightDir	=	normalize(mLightPos - mul(In.Pos,matWorld));
    Out.View		=	normalize(mul(In.Pos,matWorld) - mEye);
    return Out; // Zwracamy wypelniona strukture
}

PS_OUTPUT PShade(VS_OUTPUT In) // Ta funkcja wykonuje sie jako nastepna. Dostaje te dane, ktore zwrocila poprzednia funkcja
{
    PS_OUTPUT Out = (PS_OUTPUT) 0; // Zerujemy strukture wyjsciowa
    
    
    float4 texFragment = tex2D( Sampler0, In.TexUV);
    float diffuse = dot(In.Normal, In.LightDir);
    float3 R = reflect( In.LightDir,In.Normal);
    float specular = saturate(dot(In.View, R));
    specular = pow(specular,6);
    
    Out.Color     = saturate(texFragment*diffuse +specular); // Kolor po prostu przekazujemy
     
    return Out; // Zwracamy wypelniona strukture
}

technique tec0
{
    pass P0
    {
        VertexShader = compile vs_2_0 VShade(); // Ustawiamy funkcje Vertex shadera oraz wersje shadera, ktorej bedziemy uzywac (2.0)
        PixelShader  = compile ps_2_0 PShade(); // Ustawiamy funkcje Pixel shadera oraz wersje shadera, ktorej bedziemy uzywac (2.0)
        
        ZEnable          = True; // Wlaczamy z-bufor
        ZWriteEnable	 = True; // Wlaczamy zapis do z-bufora
        CullMode         = None; // Wylaczamy usuwanie krawedzie odwroconych tylem do kamery
    }
}