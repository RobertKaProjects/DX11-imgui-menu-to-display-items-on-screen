#include "ImGuiRender.h"
#include "../imgui.h"
#include "../imgui_impl_dx11.h"
#include "../imgui_impl_win32.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "../../testclass.h"
#include <Windows.h>


void RenderImGui(bool& draw, bool& menu) {
     

    static auto lastTime = std::chrono::high_resolution_clock::now();
    static const double refreshRate = 1.0 / 60.0;  

    auto currentTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime).count();

    if (elapsedTime < refreshRate) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((refreshRate - elapsedTime) * 1000)));
    }

    if (GetAsyncKeyState(VK_NUMPAD0) & 0x8000) {
        menu = !menu;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (menu) {
        if (ImGui::Begin("Main", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs)) {
            ImGui::Text("ESP Overlay Aktywne");
            ImGui::Checkbox("ESP", &draw);
            ImGui::End();
        }
    }

    if (draw) {
        ImVec2 window_size = ImGui::GetIO().DisplaySize;
        ImVec2 center(window_size.x * 0.5f, window_size.y * 0.5f);
        float radius = 5.0f; 
        ImU32 color = IM_COL32(255, 0, 0, 255); 
        ImGui::GetForegroundDrawList()->AddCircleFilled(center, radius, color);
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    lastTime = std::chrono::high_resolution_clock::now();
}
