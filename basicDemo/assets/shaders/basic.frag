#version 330 core
out vec4 FragColor;
uniform sampler2D text;
uniform sampler1D textMat;
uniform int mode;
uniform vec2 kernel;
in vec2 coordText;

vec3 gradient(vec3 image, int mode)
{
	vec3 result;
	ivec2 ires = textureSize( text, 0 );
	 float ResS = float( ires.s );
    float ResT = float( ires.t );

	//Se tomar� como pivote el kernel.x/2 y el kernel.y/2
	//vec2(columna, fila)
    vec2 widthStep = vec2((1./ResS), 0. );
	vec2 heightStep = vec2(0. , (1./ResT));
	vec2 widthHeightStep = vec2((1./ResS), (1./ResT));
	vec2 widthNegativeHeightStep  = vec2((1./ResS), (-1./ResT));
	const vec3 W = vec3( 0.2125, 0.7154, 0.0721 );
	float textureCoord = dot( texture(text, coordText).rgb, W );
	float bottomLeftTexCoord = dot( texture(text, coordText - widthHeightStep).rgb, W );
	float topRightTexCoord = dot( texture(text, coordText + widthHeightStep).rgb, W );
	float topLeftTexCoord = dot( texture(text, coordText - widthNegativeHeightStep).rgb, W );
	float bottomRightTexCoord = dot( texture(text, coordText + widthNegativeHeightStep).rgb, W );
	float leftTexCoord = dot( texture(text, coordText - widthStep).rgb, W );
	float rightTexCoord = dot( texture(text, coordText + widthStep).rgb, W );
	float bottomTexCoord = dot( texture(text, coordText - heightStep).rgb, W );
	float topTexCoord = dot( texture(text, coordText + heightStep).rgb, W );
	//derivatives calculate 
	float dx, dy;
	if(mode == 4){
	//Sobel
		
		dx= - 1. * topLeftTexCoord    - 2. * topTexCoord    - 1. * topRightTexCoord
			+ 1. * bottomLeftTexCoord + 2. * bottomTexCoord + 1. * bottomRightTexCoord;

		dy= - 1. * bottomLeftTexCoord  - 2.* leftTexCoord  - 1. * topLeftTexCoord
			+ 1. * bottomRightTexCoord + 2.* rightTexCoord + 1. * topRightTexCoord ;
	}else if(mode == 5){
	//Prewitt
		dx= - 1. * topLeftTexCoord	 - 1. * topTexCoord	   - 1. * topRightTexCoord 
			+ 1. * bottomLeftTexCoord + 1. *  bottomTexCoord + 1. * bottomRightTexCoord ;
		dy= - 1. * bottomLeftTexCoord  - 1. * leftTexCoord  - 1. * topLeftTexCoord
			+ 1. * bottomRightTexCoord + 1. * rightTexCoord + 1. * topRightTexCoord ;
	}else
	{
	//Roberts
	     dx = 1. * topLeftTexCoord  - 1. * textureCoord;
		 dy = 1. * topRightTexCoord - 1. * textureCoord;
	}
    float magnitude = length( vec2( dx, dy ) );//Distance of two points
    vec3 target;
	//abs value of x's derivative
	//target = vec3( abs(dx), abs(dx), abs(dx) );
	//abs value of y's derivative
    //target = vec3( abs(dy), abs(dy), abs(dy) );
	//magnitude of derivates
    target = vec3( magnitude, magnitude, magnitude );
    result = mix( image, target, 1.0f );
	return target;
}

vec3 promedio(vec3 image)
{
	vec3 result;
	ivec2 ires = textureSize( text, 0 );
	float ResS = float( ires.s );
    float ResT = float( ires.t );
	int middleT = int(kernel.x)/2; 
	int middleS = int(kernel.y)/2;
	
	//Se tomar� como pivote el kernel.x/2 y el kernel.y/2
	//vec2(x,y)
    vec2 widthStep = vec2((1./ResS), 0. );
	vec2 heightStep = vec2(0. , (1./ResT));
	
	vec2 texel = coordText;
	//Maximos
	int top = middleT;
	int bottom = int (kernel.y) % 2 == 0 ? middleT - 1 : middleT;
	int left = middleS;
	int right =  int (kernel.x) % 2 == 0 ? middleS - 1 : middleS;

	vec3 sum = vec3(0.0, 0.0, 0.0);
	for(int ii=0; ii < kernel.y; ii++)
	{
		texel = texel + (heightStep * (top - ii));
		texel = texel - (widthStep * left);
		
		for(int jj=0; jj < kernel.x; jj++)
		{
			vec3 textureAct = texture(text, texel).rgb;
			sum = sum + textureAct;
			texel = texel + widthStep;
		}
		texel = coordText;
	}
	result = (1./ float (kernel.x*kernel.y)) * sum;
	return result;
}


vec3[49] quicksort(vec3 colors[49], int l, int h){
// Create an auxiliary stack 
	int tam = h - l + 1;
	int stack[49];
    // initialize top of stack 
    int topStack = -1; 
  
    // push initial values of l and h to stack 
    ++topStack;
	stack[topStack] = l; 
    ++topStack;
	stack[topStack] = h; 
  
    // Keep popping from stack while is not empty 
    while (topStack >= 0) { 
        // Pop h and l 
        h = stack[topStack--]; 
        l = stack[topStack--]; 
  
        // Set pivot element at its correct position 
        // in sorted array 
        
		//inicio particion 
		int i = (l - 1); 
  
		for (int j = l; j <= h - 1; j++) { 
			if (colors[j].r < colors[h].r ) { 
				i++; 
				//swap
				vec3 temp = colors[j];
				colors[j] = colors[j+1];
				colors[j+1] = temp;
			} 
		} 
		//swap
		vec3 temp = colors[i+1];
		colors[i + 1] = colors[h];
		colors[h] = temp;
		
		int p = i+1;
		//fin particion

        // If there are elements on left side of pivot, 
        // then push left side to stack 
        if (p - 1 > l) { 
            stack[++topStack] = l; 
            stack[++topStack] = p - 1; 
        } 
  
        // If there are elements on right side of pivot, 
        // then push right side to stack 
        if (p + 1 < h) { 
            stack[++topStack] = p + 1; 
            stack[++topStack] = h; 
        } 
    } 
	return colors;
}


vec3 mediana(vec3 image)
{
	vec3 result;
    //const ivec2 kernel= ivec2(7,7);
	int nums=int(kernel.x)*int(kernel.y);

	vec3 colors[49];
	ivec2 ires = textureSize( text, 0 );
	float ResS = float( ires.s );
    float ResT = float( ires.t );
	int middleT = int(kernel.x)/2; 
	int middleS = int(kernel.y)/2;
	
	//Se tomar� como pivote el kernel.x/2 y el kernel.y/2
	//vec2(x,y)
    vec2 widthStep = vec2((1./ResS), 0.);
	vec2 heightStep = vec2(0. , (1./ResT));
	vec2 texel = coordText;
	//Maximos
	int top = middleT;
	int left = middleS;
	
	vec3 sum = vec3(0.0, 0.0, 0.0);
	int pos = 0;
	//Pasar colores del kernel a un arreglo
	for(int ii=0; ii < kernel.y; ii++)
	{
		texel = texel + (heightStep * (top - ii));
		texel = texel - (widthStep * left);
		
		for(int jj=0; jj < kernel.x; jj++)
		{
			vec3 textureAct = texture(text, texel).rgb;

			colors[pos] = textureAct;
			pos++;
			texel = texel + widthStep;
		}
		texel = coordText;
	}
	//Sort
	int n = nums;
	int newn;
	do{
		newn = 0;
		for (int j = 0; j < n-1; j++){
			if (colors[j].r > colors[j+1].r 
			&& colors[j].b > colors[j+1].b 
			&& colors[j].g > colors[j+1].g)
			{
				vec3 temp = colors[j];
				colors[j] = colors[j+1];
				colors[j+1] = temp;
				newn=j;
			}
		}
		n= newn;
				
	}while(n>0);

	//La mediana es el n�mero que est� 
	//en medio del conjunto ya ordenado
	int medio = int(ceil( nums/2.0 ));

	result = colors[medio];
	return result;
}


// sirve para los casos imparximpar iguales
vec3 gradientWithFor(vec3 image, int mode){
	vec3 result;
	ivec2 ires = textureSize( text, 0 );
	float ResS = float( ires.s );
    float ResT = float( ires.t );
    //ivec2 kernel = ivec2(3,3);
	int middleT = int(kernel.x)/2; 
	int middleS = int(kernel.y)/2;
	
	
	//Se tomar� como pivote el kernel.x/2 y el kernel.y/2
	//vec2(x,y)
    vec2 widthStep = vec2((1./ResS), 0. );
	vec2 heightStep = vec2(0. , (1./ResT));
	const vec3 W = vec3( 0.2125, 0.7154, 0.0721 );
	float dx = 0;
	float dy = 0;
	vec2 texel = coordText;
	//Maximos
	int top = middleT;
	int left = middleS;

	float midValue;
	float leftRightValue;
	//Sovel
	if(mode == 4){
		midValue = 2;
		leftRightValue = 1;
	
	}
	//Prewitt
	else if(mode ==5){
		 midValue = 1;
		 leftRightValue = 1;
	}
	//Roberts
	else if(mode == 6){
		 midValue = 1;
		 leftRightValue = 1;
	}
	//Calculo de dy
	//Esquina superior maxima
	int midS = 1;
	for(int ii=0; ii < int(kernel.x); ii++)
	{
		texel = texel + (heightStep * (top - ii));
		texel = texel - (widthStep * left);
		
		if(ii == middleT) midS = 0;
		if(ii > middleT) midS = -1;
		for(int jj=0; jj < int(kernel.y); jj++)
		{
			float textureAct = dot( texture(text, texel).rgb, W );
	
			if(jj < middleS)
			{
				dx = dx - textureAct * (midS == 0 ? midValue : leftRightValue);
				dy = dy - textureAct * leftRightValue * midS;
			}else if(jj > middleS)
			{
				dx = dx + textureAct * (midS == 0 ? midValue : leftRightValue);
				dy = dy - textureAct * leftRightValue * midS;
			}
			else
			{
				dy = dy - textureAct * midValue * midS;
			}
			texel = texel + widthStep;
		}
		midS = 1;
		texel = coordText;
	}
	float magnitude = length( vec2( dx, dy ) );//Distance of two points
    vec3 target;
	//abs value of x's derivative
	//target = vec3( abs(dx), abs(dx), abs(dx) );
	//abs value of y's derivative
    //target = vec3( abs(dy), abs(dy), abs(dy) );
	//magnitude of derivates
    target = vec3( magnitude, magnitude, magnitude );
    result = mix( image, target, 1.0f );
	//1result = vec3(kernel.x,kernel.y, 1.0);
	return result;
}



vec3 roberts(vec3 image)
{
	vec3 result;
	ivec2 ires = textureSize( text, 0 );
	float ResS = float( ires.s );
    float ResT = float( ires.t );
    //ivec2 kernel = ivec2(3,3);
	int middleT = int(kernel.x)/2; 
	int middleS = int(kernel.y)/2;
	
	
	//Se tomará como pivote el kernel.x/2 y el kernel.y/2
	//vec2(x,y)
    vec2 widthStep = vec2((1./ResS), 0. );
	vec2 heightStep = vec2(0. , (1./ResT));
	vec2 widthHeightStep = vec2((1./ResS), (1./ResT));
	vec2 widthNegativeHeightStep  = vec2((1./ResS), (-1./ResT));
	const vec3 W = vec3( 0.2125, 0.7154, 0.0721 );
	float textureCoord = dot( texture(text, coordText).rgb, W );

	float dx = 0;
	float dy = 0;
	vec2 texel = coordText;
	//Maximos
	int top = middleT;
	int bottom = int (kernel.x) % 2 == 0 ? middleT - 1 : middleT;
	int left = middleS;
	int right =  int (kernel.y) % 2 == 0 ? middleS - 1 : middleS;


	float midValue;
	float leftRightValue;
	//Maximos
	
	vec2 texelX=coordText, texelY=coordText;
	int N = int(kernel.x) < int(kernel.y) ? int(kernel.x) : int(kernel.y); 
	
	N = N % 2 == 0 ? N : N-1;
	int middle = N/2;
	float peso = middle;
	for(int ii=0; ii < N; ii++)
	{

		texelY = texelY + (widthHeightStep * (middle - ii));
		texelX = texelX - (widthNegativeHeightStep * (middle - ii));//revisar
	
		dx = dx + dot( texture(text, texelX).rgb, W ) * peso;
		dy = dy + dot( texture(text, texelY).rgb, W ) * peso;
		peso--;
		if(peso == 0) peso = -1;
		texelX = coordText;
		texelY = coordText;

	}

	float magnitude = length( vec2( dx, dy ) );//Distance of two points
    vec3 target;
    target = vec3( magnitude, magnitude, magnitude );
    result = mix( image, target, 1.0f );
	return result;
}

vec3 toon(vec3 image)
{
	vec3 result;
	vec3 edges = roberts(image);
	result = image;
	result = result - edges;
	//toon
	vec3 stepSize = vec3(10.0);
	result *= stepSize;
    
    result = round(result);
    
    result /= stepSize;
	return result;
}

vec3 LoG(vec3 image){

	vec3 result;
	float vect = texture1D(textMat, 0).r;
	// floata = texelFetch
	result = vec3(vect+0.5);
	return result;
}
void main()
{
	
	vec3 image = texture2D(text, coordText).rgb;
	vec4 result;
	float umbral = 0.4f;
	const vec3 W = vec3( 0.2125, 0.7154, 0.0721 );
	float luminance = dot( image, W );
	//	0 normal
	//	1 negative
	//	2 grey
	//	3 baw
	//	4 sobel
	//	5 prewitt
	//	6 roberts
	//	7 promedio
	//	8 mediana
	//	9 lapofGaus
	//	10 toonshading

	if(mode == 0)
	{
		//normal
		result =vec4(image, 1.0f);
	}else if(mode == 1)
	{
		//negative
		result =vec4(1-image, 1.0f);
	}else if(mode == 2)
	{
		result =vec4(vec3(luminance), 1.0f);
	}else if(mode == 3){
		float bin = 1.0f;
	    if(luminance < umbral){
			bin = 0.0f;
		}
		result =vec4(vec3(bin), 1.0f);
	}else if(mode == 4 || mode == 5){
		result = vec4(gradientWithFor(image, mode), 1.0f);
	}else if(mode == 6){
		result = vec4(roberts(image), 1.0f);
	}else if(mode == 7){
		result = vec4(promedio(image), 1.0f);
	}else if(mode == 8){
		result = vec4(mediana(image), 1.0f);
	}else if(mode == 9){
		result = vec4(LoG(image), 1.0f);
	}else{
		result = vec4(toon(image), 1.0f);
		
	}

    FragColor = result;

}