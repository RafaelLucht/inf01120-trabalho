#include "MidiExporter.h"
#include "FileManager.h"
#include <iostream>
#include <algorithm>

int main() {
    std::cout << "=== Gerador Musical ===" << std::endl;
    std::cout << "Notas: A=La B=Si C=Do D=Re E=Mi F=Fa G=Sol H=SiBemol Mb=MiBemol" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "1 - Digitar texto" << std::endl;
    std::cout << "2 - Carregar arquivo .txt" << std::endl;
    std::cout << "Opcao: ";

    int opcao;
    std::cin >> opcao;
    std::cin.ignore();

    std::string texto = "";

    if (opcao == 1) {
        std::cout << "Digite o texto (linha vazia para terminar):" << std::endl;
        std::cout << "Use [n] no inicio de cada linha para delay." << std::endl;
        std::string linha;
        while (true) {
            std::getline(std::cin, linha);
            if (linha.empty()) break;
            texto += linha + "\n";
        }
    } else if (opcao == 2) {
        std::cout << "Nome do arquivo (com extensao .txt): ";
        std::string filename;
        std::getline(std::cin, filename);
        try {
            texto = FileManager::loadText(filename);
            std::cout << "Arquivo carregado!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Erro: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cout << "Opcao invalida." << std::endl;
        return 1;
    }

    if (texto.empty()) {
        std::cout << "Nenhum texto inserido." << std::endl;
        return 0;
    }

    std::cout << "\nProcessando " << std::count(texto.begin(), texto.end(), '\n') << " vozes..." << std::endl;

    FugueScore score = FugueScore::fromText(texto);

    std::cout << "Tocando..." << std::endl;
    score.play();

    while (true) {
        std::cout << "\n1 - Escutar novamente" << std::endl;
        std::cout << "2 - Salvar MIDI" << std::endl;
        std::cout << "3 - Salvar texto" << std::endl;
        std::cout << "4 - Salvar ambos" << std::endl;
        std::cout << "5 - Sair" << std::endl;
        std::cout << "Opcao: ";

        int opcaoSalvar;
        std::cin >> opcaoSalvar;
        std::cin.ignore();

        if (opcaoSalvar == 1) {
            std::cout << "Tocando novamente..." << std::endl;
            score.play();
        } else if (opcaoSalvar == 2) {
            std::cout << "Nome do arquivo MIDI (sem extensao): ";
            std::string nome;
            std::getline(std::cin, nome);
            MidiExporter::exportToFile(score, nome + ".mid");
            std::cout << "Arquivo " << nome << ".mid salvo!" << std::endl;
        } else if (opcaoSalvar == 3) {
            std::cout << "Nome do arquivo texto (sem extensao): ";
            std::string nome;
            std::getline(std::cin, nome);
            FileManager::saveText(nome + ".txt", texto);
            std::cout << "Arquivo " << nome << ".txt salvo!" << std::endl;
        } else if (opcaoSalvar == 4) {
            std::cout << "Nome do arquivo MIDI (sem extensao): ";
            std::string nomeMidi;
            std::getline(std::cin, nomeMidi);
            MidiExporter::exportToFile(score, nomeMidi + ".mid");
            std::cout << "Nome do arquivo texto (sem extensao): ";
            std::string nomeTxt;
            std::getline(std::cin, nomeTxt);
            FileManager::saveText(nomeTxt + ".txt", texto);
            std::cout << "Arquivos salvos!" << std::endl;
        } else if (opcaoSalvar == 5) {
            break;
        } else {
            std::cout << "Opcao invalida." << std::endl;
        }
    }

    std::cout << "Fim." << std::endl;
    return 0;
}