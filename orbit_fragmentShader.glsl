#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 new_center[9];

uniform float dist[9];

void main()
{
	vec4 color = vec4(0.f,0.f,0.f,0.f);
	float frag_dist = 0.f;
	float ref_dist = 0.f;
	if(int(FragPos.x * 10.f) %2 == 0 && int(FragPos.y * 10.f) %2 == 0)//grid effect
	{
		for(int i = 0; i < 9; i++)
		{
			frag_dist = distance(new_center[i],FragPos / 50);
			ref_dist = distance(vec3(dist[i],0,0), vec3(0,0,0));
			if(frag_dist > ref_dist - 0.003 && frag_dist < ref_dist + 0.003)
			{
				color = vec4(1.f,1.f,1.f,0.75f);//or 0.5 opacity without grid effect
			}
		}
	}
	FragColor = color * vec4(normalize(FragPos+ 50),1);
}
