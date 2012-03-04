local gl = require("opengl")
local GL = gl
local sketch = require("opengl.sketch")
local Texture = require("opengl.Texture")
local Shader = require("opengl.Shader")

--[[
@see http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
--]]

local ctx = "Respect to IQ"
local win = Window(ctx, 0, 0, 1024, 768)
win.sync = true
local glsl = [=[

Shader{
	name = "distance.fields",
	description = [[ Render ray-marching though a distance field ]],
	language = "GLSL",
	parameters = {
		Parameter{
			1, name = "param1", type = "float",
		},
		Parameter{
			1, name = "param2", type = "float",
		},
		Parameter{
			1, name = "param3", type = "float",
		},
		Parameter{
			1, name = "param4", type = "float",
		},
	},

--------------------------------------------------------------------------
--------------------------------------------------------------------------
--	Vertex Program
--------------------------------------------------------------------------
Program{
	type = "vertex",[[

varying vec2 coord;

void main() {
	// perform standard transform on vertex
	gl_Position = ftransform();
	coord = gl_Vertex.xy; 
}

]]},
--------------------------------------------------------------------------
--------------------------------------------------------------------------
--	Fragment Program
--------------------------------------------------------------------------
Program{
	type = "fragment",[[
	
uniform float param1, param2, param3, param4;

varying vec2 coord;

float pi = 3.141592653589793;

/// SPACE TRANSFORMATIONS ///

// for distance ops, the transforms are inverted:
vec3 translate( vec3 p, vec3 tx ) {
	return p - tx;
}
vec3 rotateXY( vec3 p, float theta ) {
	float c = cos(theta);
	float s = sin(theta);
	return vec3(p.x*c - p.y*s, p.x*s + p.y*c, p.z);
}
vec3 rotateYZ( vec3 p, float theta ) {
	float c = cos(theta);
	float s = sin(theta);
	return vec3(p.x, p.y*c - p.z*s, p.y*s + p.z*c);
}
vec3 rotateXZ( vec3 p, float theta ) {
	float c = cos(theta);
	float s = sin(theta);
	return vec3(p.x*c - p.z*s, p.y, p.x*s + p.z*c);
}

// repetition:
vec3 opRepeat( vec3 p, vec3 c ) {
	vec3 q = mod(p,c)-0.5*c;
	return q;
}

/// DISTANCE OPERATIONS ///

// union is the closest of two distances:
float opUnion( float d1, float d2 )
{
    return min(d1,d2);
}

// intersection is farthest:
float opIntersect( float d1, float d2 )
{
    return max(d1,d2);
}

// subtraction:
float opSubtract( float d1, float d2 ){
    return max(-d1,d2);
}

// blending:
float opBlend( vec3 p, float d1, float d2 ){
    float bfact = smoothstep( length(p), 0., 1. );
	return mix( d1, d2, bfact );
}

/// DISTANCE FUNCTIONS ///

// roundedness:
float lengthN(vec3 p, float n) {
	return pow(pow(p.x,n) + pow(p.y,n) + pow(p.z,n), 1./n);
}

// distance of p from a sphere radius s (at origin):
float sdSphere( vec3 p, float s ) {
	return length(p)-s;
}

// distance of p from a box of dim b:
float udBox( vec3 p, vec3 b ) {
  return length(max(abs(p)-b, 0.0));
}

// hexagoanal prism
float udHexPrism( vec3 p, vec2 h )
{
    vec3 q = abs(p);
    return max(q.z-h.y,max(q.x+q.y*0.57735,q.y)-h.x);
}

// twist
vec3 opTwist( vec3 p, float angle )
{
    float c = cos(angle*p.y);
    float s = sin(angle*p.y);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xz,p.y);
    return q;
}





// the scene
float map( vec3 p0 ) {
	
	vec3 p1 = rotateXZ(p0, pi * param1);
	vec3 p2 = translate(p1, vec3(0, 0.25 * param1, 0));
	vec3 p3 = translate(p1, vec3(0.95, 0, 0));
	vec3 p4 = rotateYZ(p3, 1. * pi * param2);
	vec3 p5 = translate(p0, vec3(sin(pi * param1), 0, 0));
	vec3 p6 = rotateYZ(p2, param1);
	vec3 p7 = opRepeat(p4, vec3(0.5, 0.5, 0.5));
	vec3 p8 = rotateYZ(p0, pi * param1);
	vec3 p9 = opTwist ( p5, 4.*param2 );

	float s1 = sdSphere(p8, 0.5);
	float s2 = udBox(p7, vec3(0.55, 0.1, 0.8));
	float s3 = udBox(p9, vec3(0.5, 0.2, 0.8));
	float s4 = udHexPrism( p8, vec2(0.5, 0.7) );
	
	float t1 = opUnion(s1, s3);
	float t2 = opIntersect(s2, s4);
	
	float t3 = mix(t1, s3, param2);
	float t4 = mix(t2, s1, param3);
	
	//return mix(h, g, 0.85);
	
	return mix(t4, t3, param4);

}


// shadow ray:
float shadow( in vec3 ro, in vec3 rd, float mint, float maxt, float mindt, float k ) {
	float res = 1.;
	for( float t=mint; t < maxt; ) {
		float h = map(ro + rd*t);
		// blurry penumbra:
		res = min( res, k*h/t );
		// keep looking:
		t += h; // * (1.+t); // * 0.5;	// bigger jumps as it goes???
		if( h<mindt ) {
			// in shadow:
            return 0.0;
		}
		
	}
    return res;
}

void main() {
	vec3 light3 = vec3(3, 4, 14);
	vec3 light2 = vec3(1, -3, 7);
	vec3 light1 = vec3(5, 4, 2);
	vec3 color1 = vec3(0.2, 0, 0.5);
	vec3 color2 = vec3(0, 0.4, 0.4);
	vec3 color3 = vec3(0.1, 0.1, 0.6);
	vec3 ambient = vec3(0.1, 0.1, 0.1);
	
	vec3 normal = vec3(0, 0, -1);
	vec3 color = vec3(0, 0, 0);
	
	// ortho ray:
	vec3 ro = vec3(coord, 5.);
	vec3 rd = vec3(0, 0, -1);
	vec3 p = ro;

	// persp:
	rd = normalize(vec3(coord, 0) - vec3(0, 0, 15.));
	
	// initial eye-ray to find object intersection:
	float mindt = 0.001 + 0.0001;
	float mint = mindt;
	float maxt = 12.;
	float t=mint;
	float h = maxt;
	int steps = 0;
	for (t; t<maxt;) {
		h = map(p);
		t += h * 0.5; //(1.+0.5*t/maxt);	// slight speedup
		//t += h;
		p = ro + t*rd;
		//steps++;
		if (h < mindt) {
			break;
		}
	}
		
	// lighting:
	float ao = 0.;
	if (t<maxt) {
		color = ambient;
	
		// Normals computed by central differences on the distance field at the shading point (gradient approximation).
		float gradient_eps = 0.001;
		vec3 gradient = vec3( 
			map(p+vec3(gradient_eps,0,0)) - map(p-vec3(gradient_eps,0,0)),
			map(p+vec3(0,gradient_eps,0)) - map(p-vec3(0,gradient_eps,0)),
			map(p+vec3(0,0,gradient_eps)) - map(p-vec3(0,0,gradient_eps))  
		);
		normal = normalize(gradient);

		// Ambient Occlusion:
		// sample 5 neighbors in direction of normal
		float dao = 0.05; // delta between AO samples
		float aok = 3.0;
		float weight = 1.;
		for (int i=0; i<5; i++) {
			float dist = float(i*i)*dao;
			float factor = dist - map(p + normal*dist);
			ao += weight * factor;
			weight *= 0.5;	// decreasing importance
		}
		ao = 1. - aok * ao;

		// compute ray to light source:
		vec3 ldir1 = normalize(light1 - p);
		vec3 ldir2 = normalize(light2 - p);
		vec3 ldir3 = normalize(light3 - p);

		// diffuse:
		float ln1 = abs(dot(ldir1, normal));
		float ln2 = abs(dot(ldir2, normal));
		float ln3 = abs(dot(ldir3, normal));


		// check for shadow:
		float shadowstart = 0.01;
		float shadowend = 6.;
		float shadowdt = 0.002;
		// shadow penumbra coefficient:
		float k = 1.5;
		color += color1 * ln1 * shadow(p, ldir1, shadowstart, shadowend, shadowdt, k);
		color += color2 * ln2 * shadow(p, ldir2, shadowstart, shadowend, shadowdt, k);
		color += color3 * ln3 * shadow(p, ldir3, shadowstart, shadowend, shadowdt, k);

		// apply ambient occlusion:
		color *= ao;
	}
	
	// render as depth:
	float depth = 1.-t/maxt;
	//color = vec3(depth, depth, depth);

	// render normal:
	//color = normal;
	
	// render gradient:
	// color = gradient / gradient_eps;

	// render iterations:
	float busy = 1. - 10./(10.+float(steps));
	//color = vec3(busy, busy, busy);
	
	// ao:
	//color = vec3(ao, ao, ao);

	gl_FragColor = vec4(color, 1);
}

]],},
--------------------------------------------------------------------------
}

]=]

local shader = Shader{
	ctx = ctx,
	source = glsl,
}

function win:key(e, k)
	if(e == "down") then
		if(k == 27) then
			self.fullscreen = not self.fullscreen
		end
	end
end

local frame = 0
function win:draw()
	frame = frame + 1
	sketch.enter_ortho()
	shader:param("param1", math.sin(frame * 0.007))
	shader:param("param2", math.cos(frame * 0.01))
	shader:param("param3", math.cos(frame * 0.03))
	shader:param("param4", math.sin(frame * 0.001))
	shader:bind()
	--tex:bind()
	gl.Color(0.5, 0.5, 1, 1)
	gl.Begin(GL.QUADS)
		sketch.quad()
	gl.End()
	--tex:unbind()
	shader:unbind()
	sketch.leave_ortho()
end