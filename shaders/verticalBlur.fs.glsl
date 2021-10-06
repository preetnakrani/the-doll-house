#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;
uniform int blur_state;

in vec2 texcoord[11];
in vec2 texcoord2[7];

layout(location = 0) out vec4 color;

//vec4 blur(vec2 xy) {

	
//}
//	vec2 uv = xy.xy;
//	float count = 1;

//	vec4 avg_color = texture(screen_texture, uv);
//	float right = uv.x + 0.01;
//	if(right <= 1 ){
//		avg_color = texture(screen_texture, vec2(right, uv.y));
//		count += 1;
//	}
//	float right_2 = uv.x + 0.02;
//	if(right_2 <= 1 ){
//		avg_color = 0.5*texture(screen_texture, vec2(right_2, uv.y));
//		count += 0.5;
//	}
//	float left = uv.x - 0.01;
//	if(left >= 0 ){
//		avg_color = texture(screen_texture, vec2(left, uv.y));
//		count += 1;
//	}
//	float left_2 = uv.x - 0.02;
//	if(left_2 >= 0 ){
//		avg_color = 0.5*texture(screen_texture, vec2(left_2, uv.y));
//		count += 0.5;
//	}
//	float up = uv.y + 0.01;
//	if(up <= 1 ){
//		avg_color = texture(screen_texture, vec2(uv.x, up));
//		count += 1;
//	}
//	float up_2 = uv.y + 0.02;
//	if(up_2 <= 1 ){
//		avg_color = 0.5*texture(screen_texture, vec2(uv.x, up_2));
//		count += 0.5;
//	}
//	float down = uv.y + 0.01;
//	if(down >= 0 ){
//		avg_color = texture(screen_texture, vec2(uv.x, down));
//		count += 1;
//	}
//	float down_2 = uv.y + 0.02;
//	if(down_2 >= 0 ){
//		avg_color = 0.5*texture(screen_texture, vec2(uv.x, down_2));
//		count += 0.5;
//}

	
//	avg_color = avg_color / count;
//	avg_color.a = 1;
//	return avg_color;

//}



void main()
{
	color = vec4(0.0);
	if (blur_state == 2){
		color += texture(screen_texture, texcoord2[0]) * 0.0713;
		color += texture(screen_texture, texcoord2[1]) * 0.1315;
		color += texture(screen_texture, texcoord2[2]) * 0.1898;
		color += texture(screen_texture, texcoord2[3]) * 0.2146;
		color += texture(screen_texture, texcoord2[4]) * 0.1898;
		color += texture(screen_texture, texcoord2[5]) * 0.1315;
		color += texture(screen_texture, texcoord2[6]) * 0.0713;
	} else if(blur_state == 1) {
		color += texture(screen_texture, texcoord[0]) * 0.0093;
		color += texture(screen_texture, texcoord[1]) * 0.0280;
		color += texture(screen_texture, texcoord[2]) * 0.0659;
		color += texture(screen_texture, texcoord[3]) * 0.1217;
		color += texture(screen_texture, texcoord[4]) * 0.1757;
		color += texture(screen_texture, texcoord[5]) * 0.1985;
		color += texture(screen_texture, texcoord[6]) * 0.1757;
		color += texture(screen_texture, texcoord[7]) * 0.1217;
		color += texture(screen_texture, texcoord[8]) * 0.0659;
		color += texture(screen_texture, texcoord[9]) * 0.0280;
		color += texture(screen_texture, texcoord[10]) * 0.0093;
	} else {
		color = texture(screen_texture, texcoord[5]);
}
	

	
	
}
// Guassian Blur distribution from https://observablehq.com/@jobleonard/gaussian-kernel-calculater
//  0: 0.009300040045324049
// 1: 0.028001560233780885
//2: 0.06598396774984912
//3: 0.12170274650962626
//4: 0.17571363439579307
//5: 0.19859610213125314
//6: 0.17571363439579307
//7: 0.12170274650962626
//8: 0.06598396774984912
//9: 0.028001560233780885
//10: 0.009300040045324049