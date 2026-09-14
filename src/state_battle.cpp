#include "states.hpp"
#include "st-ctx.hpp"

#include <cmath>

battle_state_t::battle_state_t(st_ctx_t& ctx) : battle(ctx.player, ctx.bt_info.foe){
	ctx.camera.camera = {
		.position = {0.4f, 0.8f, -2.0f},
		.target = {-0.3f, 0, 0},
		.up = {0, 1, 0},
		.fovy = 65,
		.projection = CAMERA_PERSPECTIVE,
	};
	battle.init_dialog({
			battle.get_dialog_intro(),
			battle.get_dialog_trainer_sent_out(),
			});
	ctx.formatter.feed("[FOE_TYPE]", ctx.formatter.format((!ctx.bt_info.prefix.empty()) ? ctx.bt_info.prefix : "[PKMN_SIGN] Trainer"));
	ctx.formatter.feed("[FOE]", ctx.bt_info.name);
	current_line = ctx.formatter.format(battle.step_dialog(writer, ctx.locale));
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
	writer.update();

	auto& party = (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
		(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
		ctx.bianca.get_party();
	
	Sound absent = ctx.assets.get_sfx("SEQ_SE_TB_KARA.wav");
	Sound present = ctx.assets.get_sfx("SEQ_SE_TB_KON.wav");
	bool play_foe_pokeball = battle.control_foe_pokeballs();
	if (play_foe_pokeball && (!IsSoundPlaying(absent) || !IsSoundPlaying(present))){
		PlaySound(party[battle.current_foe_pokeball() - 1].get_pokedex().id == MISSINGNO ? absent : present);
	}
}

// TODO: pokemon y should be managed;
// trainer should disappear after sending pokemon;

void battle_state_t::render(st_ctx_t& ctx){
	BeginMode3D(ctx.camera.camera);
	DrawModelEx(ctx.assets.get_model("batt_bg01.glb"), {0, 0, 0}, {0, 0, 0}, 0, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model("batt_bg01.glb"), {0, -1, 1}, {0, 1, 0}, bg_rotation, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model("batt_stage01.glb"), {-0.2f, 0.0f, -1.0f}, {0, 0, 0}, 0.0f, {0.15f, 0.15f, 0.15f}, WHITE);
	auto& party = (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
		(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
		ctx.bianca.get_party();
	battle.move_line_under_foe_pokeballs();
	party.data()[battle.current_foe_pokemon_idx()].render(ctx.assets, POKEMON_RENDER_TYPE::FRONT, {0, battle.pokemon_y(), 0}, {battle.pokemon_scale(), battle.pokemon_scale() * 0.75f}, &ctx.camera.camera);
	if (std::holds_alternative<trainer_t>(ctx.bt_info.foe.entry) || std::holds_alternative<gym_leader_t>(ctx.bt_info.foe.entry)){
		if (anim_trainer_render.draw(ctx.assets, ctx.bt_info.name, {-0.2f, 0.08f, -0.9f}, &ctx.camera.camera, {foe_alpha, foe_alpha, foe_alpha, 255}, foe_alpha == 255) && !battle.finished_line()){
			if (writer.finished){
				battle.finish_line(true);
			}
			if (!party.empty()){
				current_line = ctx.formatter.format(battle.step_dialog(writer, ctx.locale), party[battle.current_foe_pokemon_idx()].get_pokedex().id);
				battle.set_foe_sent_out_pokemon();
				writer.set_text(current_line);
			}
		}
	}
	if (battle.foe_sent_out_pokemon() && writer.finished){
		battle.inc_pokemon_scale();
	}
	EndMode3D();
	battle.write(ctx.assets, writer);
	if (!battle.foe_sent_out_pokemon()){
		DrawTextureEx(ctx.assets.get_tx("line-under-foe-pokeballs.png"), {battle.line_under_foe_pokeballs(), 300.0f}, 0.0f, 8.0f, WHITE);
		auto xs = battle.foe_pokeball_x();
		for (usz i = 0; i < 6; i++){
			DrawTexturePro(ctx.assets.get_tx("foe-pokeball.png"), {(party[i].get_pokedex().id == MISSINGNO) ? 0.0f : 13.0f, 0, 13, 14}, {xs[i], 200.0f, 70.0f, 75.0f}, {0, 0}, 0, WHITE);
		}
	}
}
