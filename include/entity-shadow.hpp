#pragma once

void draw_shadow(Texture2D texture, Rectangle src, Vector3 pos, Color tint){

	pos.y += 0.08f;

	rlDisableDepthMask();
	rlDisableBackfaceCulling();
	rlSetTexture(texture.id);

	float u0 = src.x / texture.width;
	float v0 = src.y / texture.height;
	float u1 = (src.x + src.width) / texture.width;
	float v1 = (src.y + src.height) / texture.height;

	rlBegin(RL_QUADS);
	rlColor4ub(tint.r, tint.g, tint.b, tint.a);
	rlTexCoord2f(u0, v0); rlVertex3f(pos.x, pos.y, pos.z + trainer_scale);
	rlTexCoord2f(u1, v0); rlVertex3f(pos.x + trainer_scale, pos.y, pos.z + trainer_scale);
	rlTexCoord2f(u1, v1); rlVertex3f(pos.x + trainer_scale, pos.y, pos.z);
	rlTexCoord2f(u0, v1); rlVertex3f(pos.x, pos.y, pos.z);
	rlEnd();

	rlSetTexture(0);
	rlEnableBackfaceCulling();
	rlEnableDepthMask();
}

