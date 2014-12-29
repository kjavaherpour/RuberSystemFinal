# version 330 core

uniform mat3 NormalMatrix;
uniform mat4 ModelView;
uniform mat4 ModelViewProjection;
uniform bool IsTexture;

in vec4 vPosition;
in vec3 vNormal;
in vec4 vColor;
in vec2 vTexCoord;

out vec3 Position;
out vec3 Normal;
out vec4 Color;
out vec2 vs_texCoord;

void main() {
	gl_Position = ModelViewProjection * vPosition;
	Position = (ModelView * vPosition).xyz;
	Normal = normalize(NormalMatrix * vNormal);
	if(IsTexture){
		vs_texCoord = vTexCoord;
		Color = vec4(0,0,0,0);
	}else{
		vs_texCoord = vec2(0,0);
		Color = vColor;
	}
  }