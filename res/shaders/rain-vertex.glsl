varying vec4 v_color;
varying vec2 fragCoord;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	v_color = gl_Color;
	fragCoord = vec2(gl_MultiTexCoord0);
}
