#include "states.hpp"
#include "st-ctx.hpp"

#include <cmath>

battle_state_t::battle_state_t(st_ctx_t& ctx) : battle(ctx.player, ctx.bt_info.foe){
	ctx.camera.camera = {
		.position = {0.4f, 0.8f, -2.0f},
		.target = {0, 0, 0},
		.up = {0, 1, 0},
		.fovy = 65,
		.projection = CAMERA_PERSPECTIVE,
	};
	ctx.formatter.feed("[FOE_TYPE]", ctx.formatter.format((!ctx.bt_info.prefix.empty()) ? ctx.bt_info.prefix : "[PKMN_SIGN] Trainer"));
	ctx.formatter.feed("[FOE]", ctx.bt_info.name);
	current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, battle.get_battle_intro()));
	writer.set_text(current_line);
}

void battle_state_t::update(st_ctx_t& ctx){
	ctx.camera.set_camera_route({
		{0.4f, 0.8f, -2.0f},
		{0.4f, 0.8f, 2.0f},
	});
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	if (ctx.set_state_meta){
		if (foe_alpha < 255){
			foe_alpha += 15;
		}
	}
	bg_rotation = std::sinf(GetTime()) * 15.0f;
	Sound kara = ctx.assets.get_sfx("SEQ_SE_TB_KARA.wav"); 
	Sound kon = ctx.assets.get_sfx("SEQ_SE_TB_KON.wav");
	writer.update(&kara, &kon);
}
void battle_state_t::render(st_ctx_t& ctx){
	BeginMode3D(ctx.camera.camera);
	DrawModelEx(ctx.assets.get_model("batt_bg01.glb"), {0, 0, 0}, {0, 0, 0}, 0, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model("batt_bg01.glb"), {0, -1, 1}, {0, 1, 0}, bg_rotation, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model("batt_stage01.glb"), {-0.2f, 0.0f, -1.0f}, {0, 0, 0}, 0.0f, {0.15f, 0.15f, 0.15f}, WHITE);
	auto& party = (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
		(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
		ctx.bianca.get_party();
	// TODO: fix floating, add shadow
	if (anim_trainer_render.draw(ctx.assets, ctx.bt_info.name, {-0.2f, 0.05f, -1.0f}, &ctx.camera.camera, {foe_alpha, foe_alpha, foe_alpha, 255}, foe_alpha == 255) && !battle.finished_line()){
		if (writer.finished){
			battle.finish_line(true);
		}
		if (std::holds_alternative<trainer_t>(ctx.bt_info.foe.entry) && !party.empty()){
			current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "TRAINER_SENT_OUT"), party.data()[0].get_pokedex().id);
			writer.set_text(current_line);
		}
	}
	EndMode3D();
	battle.write(ctx.assets, writer);
}
