#include <iostream>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include <thread>
#include "ImGui/imgui/ImGuiRender.h"
#include "ImGui/imgui/ImGuiSetup.h"
#include "ImGui/imgui/Graphics.h"
#include "ImGui/imgui/Menu.h"
#include "testclass.h"
#include <Windows.h>


// Funkcja obs�ugi okna / Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    AllocConsole(); // Tworzenie konsoli / Create console
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout); // Przekierowanie wyj�cia stdout do konsoli / Redirect stdout to console
    freopen_s(&f, "CONOUT$", "w", stderr); // Przekierowanie wyj�cia stderr do konsoli / Redirect stderr to console

    const wchar_t CLASS_NAME[] = L"OverlayWindow"; // Nazwa klasy okna / Window class name

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc; // Wska�nik do funkcji obs�ugi okna / Pointer to window procedure
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW; // Styl okna / Window style
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Ustawienie kursora / Set cursor

    if (!RegisterClass(&wc)) {
        std::cerr << "Nie uda�o si� zarejestrowa� klasy okna.\n"; // B��d rejestracji / Failed to register window class
        return 1;
    }

    HWND hwnd = CreateOverlayWindow(hInstance, CLASS_NAME, nCmdShow); // Tworzenie okna / Create window
    if (!hwnd) {
        std::cerr << "Nie uda�o si� stworzy� okna.\n"; // B��d tworzenia okna / Failed to create window
        return 1;
    }

    if (!InitializeDirectX(hwnd)) {
        std::cerr << "Nie uda�o si� zainicjowa� DirectX.\n"; // B��d inicjalizacji DirectX / Failed to initialize DirectX
        return 1;
    }

    std::cout << "DirectX zainicjowane pomy�lnie.\n"; // DirectX zainicjowane / DirectX initialized

    InitializeImGui(hwnd); // Inicjalizacja ImGui / Initialize ImGui
    std::cout << "ImGui zainicjowane pomy�lnie.\n"; // ImGui zainicjowane / ImGui initialized

    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // Kolor t�a / Background color
    bool draw = false; // Flaga ESP / ESP flag
    bool menuVisible = true; // Widoczno�� menu / Menu visibility
    bool prevEspKeyState = false; // Poprzedni stan klawisza ESP / Previous ESP key state
    bool prevMenuKeyState = false; // Poprzedni stan klawisza menu / Previous menu key state

    Testclass1* test = nullptr;

    try {
        // Umie�� tutaj kod / Put your code here
        test = new Testclass1();
    }
    catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl; // Obs�uga wyj�tk�w / Exception handling
        std::cin.get();
    }
    
    MSG msg = {};
    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                break; // Wyj�cie z p�tli przy zamkni�ciu / Exit loop on quit
        }

        // Obs�uga klawiszy tylko je�li ich stan si� zmieni�
        // Handle keys only if their state has changed
        bool currentEspKeyState = GetAsyncKeyState(VK_NUMPAD2) & 0x8000;
        if (currentEspKeyState && !prevEspKeyState) {
            draw = !draw; 
        }
        prevEspKeyState = currentEspKeyState;

        bool currentMenuKeyState = GetAsyncKeyState(VK_NUMPAD0) & 0x8000;
        if (currentMenuKeyState && !prevMenuKeyState) {
            menuVisible = !menuVisible; // Prze��cz widoczno�� menu / Toggle menu visibility
        }
        prevMenuKeyState = currentMenuKeyState;

        if (test) { // <- ptr to class 
            RenderImGui(draw, menuVisible);  // Poprawne przekazywanie wska�nika 'uma' / Correctly passing the 'uma' pointer
        }
     

        const float clear_color_with_alpha[4] = { clear_color.x, clear_color.y, clear_color.z, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL); // Ustawianie render targetu / Set render target
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha); // Czyszczenie t�a / Clear background
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Renderowanie ImGui / Render ImGui
        g_pSwapChain->Present(1, 0); // Prezentacja klatki / Present frame

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Odpoczynek dla CPU / Sleep for CPU
    }

    if (test) {
        delete test; // Zwolnienie pami�ci UMA / Free UMA memory
    }

    // Czyszczenie zasob�w / Resource cleanup
    CleanupImGui();
    CleanupDirectX();

    return 0;
}

// Funkcja obs�ugi zdarze� okna / Window message handler function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0); // Zako�czenie aplikacji / Quit application
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam); // Domy�lna obs�uga / Default handling
    }
    return 0;
}
