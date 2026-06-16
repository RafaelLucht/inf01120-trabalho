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
	        static std::array<char, 128> txtFilenameBuffer{};
	        static std::string txtFileErrorMessage{};
	        
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
				txtFilenameBuffer = {};
				ImGui::OpenPopup("Import txt");
			}
	        ImGui::EndDisabled();
			if (ImGui::BeginPopupModal("Import txt", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				
			    ImGui::Text("Enter the text filename to load:");
			    ImGui::InputTextWithHint("##filename", "fugue.txt", txtFilenameBuffer.data(), txtFilenameBuffer.size());
			    
			    ImGui::Spacing();
			    ImGui::Separator();
			    ImGui::Spacing();
			    
			    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			        ImGui::CloseCurrentPopup();
			    }
			    
			    ImGui::SameLine();
			    
			    if (ImGui::Button("OK", ImVec2(120, 0))) {
				    try {
						std::string fullPath = txtFilenameBuffer.data();
		                
		                if (fullPath.find(".txt") == std::string::npos)
		                    fullPath += ".txt";
						
				        std::string content = FileManager::loadText(fullPath);
				        
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
				        txtFileErrorMessage = e.what();
				        ImGui::OpenPopup("Could not open file");
				    }
				}
				ImGui::EndPopup();
			}
		    if (ImGui::BeginPopup("Could not open file")) {
		        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Error:");
		        ImGui::TextWrapped("%s", txtFileErrorMessage.c_str());
		        
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
		    ImGui::Begin("Voice Parameters", nullptr, PANEL_FLAGS);
		    
		    ImGui::Text("Fugue Composition");
		    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Total Voices: %d", score.getVoiceCount());
		    ImGui::Separator();
		    ImGui::Spacing();
		    
		    const std::vector<Voice> &voices = score.getVoices();
		    
		    // --- Lista vozes e parâmetros ---
		    for (auto i = 0; i < voices.size(); ++i) {
		        auto params = voices[i].getParams(); 
		        
		        std::string headerName = "Voice #" + std::to_string(i);
		        
		        if (ImGui::CollapsingHeader(headerName.c_str())) {
		            ImGui::Indent();
		            
		            auto instName = std::string(params.inst.getName());
		            ImGui::Text("Instrument: %s", instName.c_str());
		            ImGui::Text("BPM: %d", static_cast<int>(params.bpm.get()));
		            ImGui::Text("Octave: %s", params.oct.getName().c_str());
		            
		            ImGui::Spacing();
		            
		            short currentVol = params.vol.get(); 
			        auto minVol = params.vol.getMin();
			        auto maxVol = params.vol.getMax();
			
			        std::string sliderId = "##volume_slider_" + std::to_string(i);
		            
		            ImGui::Text("Volume Control:");
			        ImGui::SameLine();
			        ImGui::SetNextItemWidth(-1);
			        
			        if (ImGui::SliderScalar(sliderId.c_str(), ImGuiDataType_S16, &currentVol, &minVol, &maxVol, "%d")) {
			            score.updateVoiceVolume(i, currentVol);
			        }
		            
		            ImGui::Spacing();
		            ImGui::Separator();
		            ImGui::Spacing();
		            
					// --- Reset da voz ---
		            //~ std::string resetBtnId = "Reset voice parameters to default##" + std::to_string(i);
		            //~ if (ImGui::Button(resetBtnId.c_str())) {
		                //~ voices[i].getParams().resetToDefaultAll(); 
		            //~ }
		            
		            ImGui::Unindent();
		            ImGui::Spacing();
		        }
		    }
		    
		    if (voices.empty()) {
		        ImGui::SetCursorPos(ImVec2(width * 0.15f, height * 0.4f));
		        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No voices loaded.\nAwaiting score compilation");
		    }
		    ImGui::End();
		}
	
		// --- Seção Inferior ---
	    void RenderPlayerBar(float width, float height, float topOffset, FugueScore &score, AppState &state) {
			
	        ImGui::SetNextWindowPos(ImVec2(0, topOffset));
	        ImGui::SetNextWindowSize(ImVec2(width, height));
	        ImGui::Begin("Player", nullptr, PANEL_FLAGS);
	        
	        static std::array<char, 128> midFilenameBuffer{};
	        static std::string midFileErrorMessage{};
	        
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
				ImGui::OpenPopup("Export MIDI");
			}
	        ImGui::EndDisabled();
	        
	        if (ImGui::BeginPopupModal("Export MIDI", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		        ImGui::Text("Enter the filename to export:");
		        ImGui::InputTextWithHint("##mid_filename", "fugue.mid", midFilenameBuffer.data(), midFilenameBuffer.size());
		        
		        ImGui::Spacing();
		        ImGui::Separator();
		        ImGui::Spacing();
		        
		        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
		            ImGui::CloseCurrentPopup();
		        }
		        ImGui::SameLine();
		        if (ImGui::Button("Save", ImVec2(120, 0))) {
		            try {
		                std::string fullPath = midFilenameBuffer.data();
		                
		                if (fullPath.find(".mid") == std::string::npos) {
		                    fullPath += ".mid";
		                }
		                
		                MidiExporter::exportToFile(score, fullPath);
		                
		                ImGui::CloseCurrentPopup();
		            }
		            catch (const std::exception &e) {
		                midFileErrorMessage = e.what();
		                
		                ImGui::CloseCurrentPopup(); 
		                ImGui::OpenPopup("Export Error"); 
		            }
				}
		        ImGui::EndPopup();
		    }
		    if (ImGui::BeginPopup("Export Error")) {
		        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Could not export MIDI file!");
		        ImGui::TextWrapped("%s", midFileErrorMessage.c_str());
		        
		        ImGui::Spacing();
		        if (ImGui::Button("Dismiss", ImVec2(120, 0))) {
		            ImGui::CloseCurrentPopup();
		        }
		        
		        ImGui::EndPopup();
		    }
				    
	        
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
	    float topBarHeight = 40.0f;
	    float bottomBarHeight = 40.0f;
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
