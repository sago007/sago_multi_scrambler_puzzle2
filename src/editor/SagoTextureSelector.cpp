#include "SagoTextureSelector.hpp"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <SDL_image.h>
#include "../../sago/SagoMisc.hpp"
#include "../globals.hpp"


static void addLinesToCanvas(SDL_Renderer* renderer, SDL_Texture* texture, int xstep = 32, int ystep = 32, int xoffset = 0, int yoffset = 0) {
	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	if (xstep > 0) {
		for (int i = xoffset; i < width; i += xstep) {
			SDL_RenderDrawLine(renderer, i, 0, i, height);
		}
	}
	if (ystep > 0) {
		for (int i = yoffset; i < height; i += ystep) {
			SDL_RenderDrawLine(renderer, 0, i, width, i);
		}
	}
}

static void addFolderToList(const std::string& folder, std::vector<std::string>& list, const std::string& filter = "") {
	std::vector<std::string> textures = sago::GetFileList(folder.c_str());
	for (const auto& texture : textures) {
		std::cout << "Texture: " << texture << "\n";
		if (filter.empty() || texture.find(filter) != std::string::npos) {
			list.push_back(texture);
		}
	}
}

static std::vector<std::string> populateTree(const std::string& filter = "") {
	std::vector<std::string> textures;
	addFolderToList("textures", textures, filter);
	return textures;
}

std::string remove_file_extension(const std::string& filename) {
	size_t lastindex = filename.find_last_of(".");
	return filename.substr(0, lastindex);
}

void SagoTextureSelector::runTextureSelectorFrame(SDL_Renderer* target) {
	ImGui::Begin("TextureList", nullptr, ImGuiWindowFlags_NoCollapse);
	static char filter[256] = "";
	ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));
	ImGui::Separator();
	for (const auto& texture : textures) {
		if (filter[0] == '\0' || texture.find(filter) != std::string::npos) {
			if (ImGui::Selectable(texture.c_str(), selected_texture == texture)) {
				selected_texture = texture;
			}
		}
	}

	ImGui::End();

	ImGui::Begin("Canvas");
	if (selected_texture.length()) {
		int tex_w, tex_h;
		SDL_Texture* current_texture = globalData.dataHolder->getTexturePtr(remove_file_extension(selected_texture));
		SDL_QueryTexture(current_texture, nullptr, nullptr, &tex_w, &tex_h);
		ImGui::Text("Size: %d x %d", tex_w, tex_h);
		ImGui::Image(current_texture, ImVec2((float)tex_w, (float)tex_h));
		addLinesToCanvas(target, current_texture);
	}
	ImGui::End();

}



SagoTextureSelector::SagoTextureSelector() {
}

SagoTextureSelector::~SagoTextureSelector() {
}

bool SagoTextureSelector::IsActive() {
	return isActive;
}

void SagoTextureSelector::ProcessInput(const SDL_Event& event, bool &processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void SagoTextureSelector::Draw(SDL_Renderer* target) {
	runTextureSelectorFrame(target);
}

void SagoTextureSelector::Update() {
}

void SagoTextureSelector::Init() {
	textures = populateTree();
}