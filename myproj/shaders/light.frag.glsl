//vec3 light_position = vec3(0,1,1);
//vec4 light_color = vec4(1,1,1,1);

uniform vec3 mylight_position;
uniform vec4 mylight_color;
uniform vec3 mylight_direction;
uniform int mylight_type;

uniform bool silhouette;

in vec4 myvertex;
in vec3 mynormal;

void main (void) 
{        
	vec3 eyepos = vec3(0,0,0) ; 

	vec4 _mypos = gl_ModelViewMatrix * myvertex ; 
    vec3 mypos = (_mypos.xyz / _mypos.w);

   	vec3 lightpos = mylight_position.xyz;

	vec3 normal = normalize(gl_NormalMatrix*mynormal);

	vec3 eyedir = normalize(eyepos - mypos) ;


	vec3 lightdir = (mypos - mypos);

	if(mylight_type == 3) // SPOT
	{
		lightdir = normalize(-mylight_direction);
		if(dot(lightdir,-normalize(mypos-lightpos)) > 0)
		{
			vec3 reflectdir = normalize( reflect(-lightdir, normal) );

			//gl_FragColor = vec4(1,0,0,1) *pow(dot(lightdir,-normalize(mypos-lightpos)),20);
			gl_FragColor = (vec4(1,0,0,0)*mylight_color*max(dot(lightdir, normal),0) 
					   + vec4(0.5,0.5,0.5,0)*mylight_color*pow(max(dot(reflectdir, eyedir),0), 20))
					   *pow(dot(lightdir,-normalize(mypos-lightpos)),30);
		}
	}

	else
	{
		if(mylight_type == 1) // POINT_LIGHT
		{
			lightdir = normalize (lightpos - mypos) ;
		}
		else if(mylight_type == 2) // DIRECTIONAL
		{
			lightdir = normalize(-mylight_direction);
		}
	

		vec3 reflectdir = normalize( reflect(-lightdir, normal) );

		gl_FragColor = vec4(1,0,0,0)*mylight_color*max(dot(lightdir, normal),0) 
					   + vec4(0.5,0.5,0.5,0)*mylight_color*pow(max(dot(reflectdir, eyedir),0), 20);
	}

	if(silhouette) gl_FragColor = vec4(0,0,0.1/(abs(dot(eyedir, normal))),1 );
	

}
