#version 330 core
out vec4 FragColor;
uniform sampler2D text;
uniform sampler1D textMat;
in vec2 coordText;

void main()
{	
	vec3 image = texelFetch(text, ivec2(coordText+vec2(0.5)),0).rgb;
	float vect = texture1D(textMat, 1.5/49.0).r*255;
	float vec = texelFetch(textMat, 4, 2).r;
	//vect += 0.3f;
	vec4 result;
	float umbral = 0.4f;
	const vec3 W = vec3( 0.2125, 0.7154, 0.0721 );
	float luminance = dot( image, W );
	result =vec4(vec3(image), 1.0f);
	

    FragColor = result;
}