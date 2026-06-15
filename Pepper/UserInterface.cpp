#include "UserInterface.h"
#include "AudioSynth.h"
#include "AudioParameters.h"
#include "FileHandlers.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <string>
#include <array>
#include <algorithm>
#include <thread>
#include <atomic>

namespace UI {
	
	// --- Configurações do app ---
	namespace	{
		
		static GLFWwindow* g_window = nullptr;
		
		const int WINDOW_WIDTH = 1280;
        const int WINDOW_HEIGHT = 720;
        const char* APP_TITLE = "Pepper Sound";
		
		enum class AppState	{
			AWAITING_INPUT,
			INPUT_COMPILED,
			PLAYING_MUSIC
		};
		
	    // --- Fixa as janelas ---
	    const ImGuiWindowFlags PANEL_FLAGS = (
			ImGuiWindowFlags_NoMove | 
			ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_NoCollapse | 
			ImGuiWindowFlags_NoTitleBar
		);
		
	}
	
	// --- Renderização ---
	namespace	{
		
	
		// --- Seção Superior: Título; “Ajuda” (independe do estado) ---
	    void RenderTopBar(float width, float height) {
	        ImGui::SetNextWindowPos(ImVec2(0, 0));
	        ImGui::SetNextWindowSize(ImVec2(width, height));
	        ImGui::Begin("Top Bar", nullptr, PANEL_FLAGS);
	        
	        ImGui::Text("Music Generator");
	        
	        float buttonWidth = 80.0f;
	        ImGui::SameLine(width - buttonWidth - 20.0f, 0.0f);
	        if(ImGui::Button("Help",ImVec2(buttonWidth, 0)))	{
				// Popup chave->evento (deixar por último)
			}
	        ImGui::End();
	    }
	
		// --- Seção Sestra: Entrada de dados (texto, arquivo) ---
	    void RenderInputSection(float width, float height, float topOffset, FugueScore &score, AppState &state) {
			
	        // --- Variáveis de texto ---
	        static std::array<char, 5000> textBuffer{};
	        static std::array<char, 128> filenameBuffer{};
	        static std::string errorMessage{};
	        
			// --- Inicializa janela ---
			ImGui::PushID(&textBuffer);
	        ImGui::SetNextWindowPos(ImVec2(0, topOffset));
	        ImGui::SetNextWindowSize(ImVec2(width, height));
	        ImGui::Begin("Text Input", nullptr, PANEL_FLAGS);
	        
	        
	        ImGui::Text("Input text here");
	        ImGui::SameLine();
	        
	        // --- Import .txt ---
	        ImGui::BeginDisabled(state == AppState::PLAYING_MUSIC);
	        if (ImGui::Button("Import from .txt"))	{
				filenameBuffer = {};
				ImGui::OpenPopup("Import txt");
			}
	        ImGui::EndDisabled();
			if (ImGui::BeginPopupModal("Import txt", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				
			    ImGui::Text("Enter the filename to load (include \".txt\"):");
			    ImGui::InputTextWithHint("##filename", "fugue.txt", filenameBuffer.data(), filenameBuffer.size());
			    
			    ImGui::Spacing();
			    ImGui::Separator();
			    ImGui::Spacing();
			    
			    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			        ImGui::CloseCurrentPopup();
			    }
			    
			    ImGui::SameLine();
			    
			    if (ImGui::Button("OK", ImVec2(120, 0))) {
				    try {
						
				        std::string content = FileManager::loadText(filenameBuffer.data());
				        
				        // --- Trunca string se for maior que o buffer ---
				        if (content.size() >= textBuffer.size()) {
				            content.resize(textBuffer.size() - 1);
				        }
				        std::copy(content.begin(), content.end(), textBuffer.begin());
				        textBuffer[content.size()] = '\0'; // Tira lixo da entrada anterior
				        
				        ImGui::CloseCurrentPopup();
				    }
				    catch (const std::exception &e) {
						ImGui::CloseCurrentPopup();
				        errorMessage = e.what();
				        ImGui::OpenPopup("Could not open file");
				    }
				}
				ImGui::EndPopup();
			}
		    if (ImGui::BeginPopupModal("Could not open file", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Error:");
		        ImGui::TextWrapped("%s", errorMessage.c_str());
		        
		        ImGui::Spacing();
		        ImGui::Separator();
		        ImGui::Spacing();
		        
		        if (ImGui::Button("Close", ImVec2(120, 0))) {
		            ImGui::CloseCurrentPopup();
		        }
		        
		        ImGui::EndPopup();
		    }
			
			ImGui::SameLine();
			
			ImGui::BeginDisabled(state == AppState::PLAYING_MUSIC || textBuffer[0] == '\0');
			
			if (ImGui::Button("Compile score"))	{
				score = FugueScore::fromText(textBuffer.data());
				state = AppState::INPUT_COMPILED;
				
			}
			
			ImGui::EndDisabled();
	
	        ImGui::InputTextMultiline("##text", textBuffer.data(), textBuffer.size(), ImVec2(-1, -1));
	        
	        ImGui::End();
	        ImGui::PopID();
	    }
	
		// --- Seção Destra: Parâmetros das vozes (fazer MVP funcionar antes) ---
	    void RenderParameters(float width, float height, float leftOffset, float topOffset, FugueScore &score) {
	        ImGui::SetNextWindowPos(ImVec2(leftOffset, topOffset));
	        ImGui::SetNextWindowSize(ImVec2(width, height));
	        ImGui::Begin("Parameters", nullptr, PANEL_FLAGS);
	        
	        ImGui::Text("Score Parameters");
	        ImGui::Separator();
	        
	        // --- Exemplos estáticos temporários ---
	        static int instrumentIndex = 0;
	        const char* instruments[] = { "Sine Wave", "Square Wave", "Sawtooth", "Custom Sampler" };
	        ImGui::SetNextItemWidth(-1);
	        ImGui::Combo("##Instrument", &instrumentIndex, instruments, IM_ARRAYSIZE(instruments));
	        
	        static float volume = 0.5f;
	        ImGui::Text("Volume");
	        ImGui::SetNextItemWidth(-1);
	        ImGui::SliderFloat("##Volume", &volume, 0.0f, 1.0f);
	        
	        ImGui::End();
	    }
	
		// --- Seção Inferior ---
	    void RenderPlayerBar(float width, float height, float topOffset, FugueScore &score, AppState &state) {
			
	        ImGui::SetNextWindowPos(ImVec2(0, topOffset));
	        ImGui::SetNextWindowSize(ImVec2(width, height));
	        ImGui::Begin("Player", nullptr, PANEL_FLAGS);
	        
	        // --- Flag para thread de áudio ---
	        static std::atomic<bool> musicThreadOn = false;
	        
	        if (state == AppState::PLAYING_MUSIC && !musicThreadOn)	{
				state = AppState::INPUT_COMPILED;
			}
	        
	        ImGui::BeginDisabled(state != AppState::INPUT_COMPILED);
	        if (ImGui::Button("Play"))	{
				state = AppState::PLAYING_MUSIC;
		        musicThreadOn = true;
		        
		        // --- Dispara score.play() em thread paralela à interface ---
		        std::thread audioThread([&score]() {
		            score.play();         
		            musicThreadOn = false;
		        });
		        
		        audioThread.detach(); // Dá independência à thread de áudio
				
			}
			ImGui::EndDisabled();
			
			ImGui::SameLine();
			
			ImGui::BeginDisabled(state != AppState::INPUT_COMPILED);
			if (ImGui::Button("Export .mid"))	{
				// Ignorar por enquanto
			}
	        ImGui::EndDisabled();
	        
	        ImGui::End();
	    }
	}
	//--------------------------------------------------------------------------------
	// Funções que a main vê
	//--------------------------------------------------------------------------------
	bool init() {
        // --- Inicializa o GLFW ---
        if (!glfwInit())
			return false;
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        
        g_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_TITLE, nullptr, nullptr);
        if (g_window == nullptr) {
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(g_window);
        glfwSwapInterval(1); // Ativa VSync
        
        IMGUI_CHECKVERSION();
        if (!ImGui::CreateContext())
			return false;
        
        ImGui::StyleColorsDark();
        
        // --- Backends ---
        if (!ImGui_ImplGlfw_InitForOpenGL(g_window, true))
			return false;
        if (!ImGui_ImplOpenGL3_Init("#version 130"))
			return false;
        
        return true;
    }
    
    bool shouldRender() {
        if (glfwWindowShouldClose(g_window))
            return false;
        
        glfwPollEvents();
        
        int display_w, display_h;
        glfwGetFramebufferSize(g_window, &display_w, &display_h);
        
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        return true;
    }
    
    void cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        if (g_window) {
            glfwDestroyWindow(g_window);
        }
        glfwTerminate();
    }
	
	void renderApp(FugueScore &score) {
		
		// --- Pega dimensões da janela ---
	    int display_w, display_h;
	    glfwGetFramebufferSize(g_window, &display_w, &display_h);
	    float currentWidth = (float)display_w;
	    float currentHeight = (float)display_h;
	
	    
	    if (currentWidth <= 0 || currentHeight <= 0) {
	        ImGui::EndFrame();
	        return;
	    }
	        
	    // --- Cálculos usando o tamanho dinâmico da janela ---
	    float topBarHeight = 60.0f;
	    float bottomBarHeight = 100.0f;
	    float middleHeight = currentHeight - topBarHeight - bottomBarHeight;
	    float leftWidth = currentWidth * 0.60f;
	    float rightWidth = currentWidth - leftWidth;
	        
	    static AppState state = AppState::AWAITING_INPUT;
	    
	    // --- Renderiza seções ---
	    RenderTopBar(currentWidth, topBarHeight);
	    RenderInputSection(leftWidth, middleHeight, topBarHeight, score, state);
	    RenderParameters(rightWidth, middleHeight, leftWidth, topBarHeight, score);
	    RenderPlayerBar(currentWidth, bottomBarHeight, topBarHeight + middleHeight, score, state);
	    
	    ImGui::Render();
	    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	    glfwSwapBuffers(g_window);
	}
}
