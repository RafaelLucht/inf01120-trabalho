# Gerador Musical a partir de Texto

Trabalho Prático — INF01120 Desenvolvimento de Software  
Johann Hardt & Rafael Lucht — UFRGS 2026

## Descrição

Programa que converte texto em música MIDI. Cada caractere do texto é mapeado para um evento musical (nota, mudança de instrumento, oitava, BPM, etc.). O texto pode ter múltiplas linhas, cada uma representando uma voz independente no estilo de fuga de Bach.

## Mapeamento de caracteres

| Caractere | Evento |
|-----------|--------|
| A | Nota Lá |
| B | Nota Si |
| C | Nota Dó |
| D | Nota Ré |
| E | Nota Mi |
| F | Nota Fá |
| G | Nota Sol |
| H | Nota Si bemol |
| Mb | Nota Mi bemol |
| a-h minúsculas | Pausa |
| Espaço | Dobra o volume |
| ? | Sobe uma oitava |
| V | Desce uma oitava |
| > | Aumenta BPM em 10 |
| < | Diminui BPM em 10 |
| ! | Muda para Harmonica (GM 22) |
| ; | Muda para Tubular Bells (GM 15) |
| , | Muda para Church Organ (GM 20) |
| o, i, u | Muda para Gaita de Foles (GM 110) |
| Dígito par | Muda instrumento relativamente |
| [n] | Delay de n beats no início da linha |
| Outras letras | Repete última nota (ou pausa) |

## Parâmetros iniciais por voz

| Voz | Oitava | Volume | Instrumento |
|-----|--------|--------|-------------|
| V0 | 6 | 100 | Cravo (GM 6) |
| V1 | 5 | 80 | Órgão (GM 20) |
| V2 | 4 | 60 | Piano (GM 0) |
| V3 | 3 | 40 | Fagote (GM 70) |

## Exemplo de texto

[0] G A H C
[4] D E F G

Tema da Fuga em Sol menor BWV 578 de Bach.

## Compilação

Requisitos: g++ 13+, Windows com WinMM.

```bash
cd src/audio
g++ -std=c++17 chili_pepper.cpp RtMidi.cpp -o chili_pepper -D__WINDOWS_MM__ -DRTMIDI_DO_NOT_ENSURE_UNIQUE_PORTNAMES -lwinmm
```

## Execução

```bash
./chili_pepper
```

O programa oferece um menu interativo:
1. Digitar texto diretamente
2. Carregar arquivo .txt

Após a reprodução, é possível escutar novamente, salvar o MIDI ou salvar o texto.

## GUI demonstrativa

Abrir `gui/index.html` no navegador para visualizar a interface gráfica planejada.

## Estrutura do projeto

inf01120-trabalho/
├── src/audio/          # Código fonte principal
│   ├── chili_pepper.cpp
│   ├── RtMidi.h
│   └── RtMidi.cpp
├── gui/                # Interface gráfica demonstrativa (HTML)
├── docs/               # Documentação
└── tests/              # Arquivos de teste