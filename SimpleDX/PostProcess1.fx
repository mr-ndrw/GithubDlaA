texture Tex01;

// Format half jest to liczba zmienno przecinkowa o polowie precyzji formatu float
/*half hResDivider;

half hResX;
half hResY;*/


float hResDivider;

float hResX;
float hResY;

sampler2D Sampler1 = sampler_state
{
    Texture = <Tex01>;
    MinFilter = Linear;  
    MagFilter = Linear;
    MipFilter = None;
		AddressU = Mirror;
		AddressV = Mirror;
};


//blurstuff
////////////

static const int g_cKernelSize = 13;

// tablica z której brane sa adresy tekseli wzgledem aktualnie przetwarzanego.
float2 PixelKernel[g_cKernelSize] =
{
    { -6, 0 },
    { -5, 0 },
    { -4, 0 },
    { -3, 0 },
    { -2, 0 },
    { -1, 0 },
    {  0, 0 },
    {  1, 0 },
    {  2, 0 },
    {  3, 0 },
    {  4, 0 },
    {  5, 0 },
    {  6, 0 },
};

//tablica wag bazujaca na rozkladzie Gaussa. Najwyzsza wage ma teksel aktualnie przetwarzany

static const float BlurWeights[g_cKernelSize] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};


static const float BlurWeightsMultiplier = 1;


// Eblurstuff
/////////////




struct VS_INPUT
{
    float4  Pos     : POSITION;
    float4  Tex0    : TEXCOORD0; // u v frame rotation
};

struct VS_OUTPUT
{
    float4  Pos     : POSITION;
    float2  Tex0    : TEXCOORD0;
    float2	Tex1	: TEXCOORD1;
 };


struct VS_OUTPUT_BLUR
{
    float4  Pos     : POSITION;
    float2  Tex0    : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 Color : COLOR0;
};


////////////////////////////////////////

VS_OUTPUT VShade(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT) 0; 

		
    Out.Pos		=	In.Pos;
    Out.Tex0	=	In.Tex0;
    Out.Tex1	=	In.Tex0;
  
    return Out;
}
PS_OUTPUT PShade(VS_OUTPUT In)
{
    PS_OUTPUT Out = (PS_OUTPUT) 0; 
	
	float4 TexBlueMod =	tex2D(Sampler1,	In.Tex0);
	
		Out.Color = TexBlueMod;

    return Out;
}

VS_OUTPUT_BLUR VShadeBlur(VS_INPUT In)
{
    VS_OUTPUT_BLUR Out = (VS_OUTPUT_BLUR) 0; 

		
    Out.Pos		=	In.Pos;
    Out.Tex0	=	In.Tex0;
  
    return Out;
}




PS_OUTPUT PShadeBlur(VS_OUTPUT_BLUR In)
{
    PS_OUTPUT Out = (PS_OUTPUT) 0; 
	
	
	float4 finalBlurred = tex2D( Sampler1, In.Tex0 );
		
		

 	Out.Color     =  finalBlurred;
 	
    return Out;
}

PS_OUTPUT PShadeMono(VS_OUTPUT In)
{
    PS_OUTPUT Out = (PS_OUTPUT) 0; 
	
	float4 tex =	tex2D(Sampler1,	In.Tex0);
	Out.Color = tex;

    return Out;
}



technique Tec0
{
		pass P0
		{
			VertexShader = compile vs_1_1 VShade();
			PixelShader  = compile ps_2_0 PShade();
	        
			ZEnable          = False;
			ZWriteEnable	 = False;
			CullMode         = None;
			AlphaBlendEnable = False;
			AlphaTestEnable  = False;
			//AlphaFunc		 = GREATEREQUAL;
			// AlphaRef		 = 0x00000080;
			FogEnable		 = False;
			srcblend		 = One; 
			destblend		 = Zero;
		}
}

technique Blur
{
		pass P0
		{
			VertexShader = compile vs_1_1 VShadeBlur();
			PixelShader  = compile ps_3_0 PShadeBlur();
	        
			ZEnable          = False;
			ZWriteEnable	 = False;
			CullMode         = None;
			AlphaBlendEnable = False;
			AlphaTestEnable  = False;
			//AlphaFunc		 = GREATEREQUAL;
			// AlphaRef		 = 0x00000080;
			FogEnable		 = False;
			srcblend		 = One; 
			destblend		 = Zero;
		}
}
technique Mono
{
		pass P0
		{
			VertexShader = compile vs_1_1 VShadeBlur();
			PixelShader  = compile ps_3_0 PShadeMono();
	        
			ZEnable          = False;
			ZWriteEnable	 = False;
			CullMode         = None;
			AlphaBlendEnable = False;
			AlphaTestEnable  = False;
			//AlphaFunc		 = GREATEREQUAL;
			// AlphaRef		 = 0x00000080;
			FogEnable		 = False;
			srcblend		 = One; 
			destblend		 = Zero;
		}
}
