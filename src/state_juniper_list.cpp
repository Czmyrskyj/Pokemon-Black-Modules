#include "state_juniper_list.hpp"
#include "st-ctx.hpp"

namespace JuniperList{
	void update(st_ctx_t& ctx){
		 if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			 ctx.resume_state_alpha(ctx.transition_alpha);
		 }
		 if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			 ctx.current_music = ctx.assets.get_mus("Professor Juniper.mp3");
			 ctx.set_state_meta = true;
		 }
	}
	void render(st_ctx_t& ctx){

	}
};
