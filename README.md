<div align="center">

# Projeto 02 - Estrutura de Dados II (EDII)

![Autor](https://img.shields.io/badge/Autor-Pietro%20Fernando%20Pelizon-maroon)
![C99](https://img.shields.io/badge/Language-C99-blue)
![Build](https://img.shields.io/badge/Makefile-GCC-green)
![Data Structures](https://img.shields.io/badge/ED-Graph%20%7C%20List%20%7C%20Hashmap%20%7C%20Pqueue-orange)

</div>

# Estrutura do Repositório
```text
.
├── docs            # Arquivos (.pdf) com as especificações do projeto
├── include         # Arquivos de cabeçalho (.h)
├── src             # Implementação do projeto (.c)
├── testes          # Arquivos de comando (.geo, .qry e .via)
├── unit_test       # Testes unitários feitos utilizando o Framework Unity (test_*.c)
├── unity           # Framework Unity para testes em C
└── README.md       # Documentação do repositório
```
***
## Sobre o Projeto

Este projeto desenvolve um Sistema de Informações Geográficas (SIG) simulado para a cidade de Bitnópolis, focando na implementação e uso de Estruturas de Dados avançadas (Grafo, Lista, Hashmap e Fila de Prioridade) com a intenção de otimizar e analisar o seu complexo fluxo viário.

O sistema processa mapas baseados em quadras, constrói a rede viária e resolve problemas de roteamento (caminhos mais curtos e mais rápidos) gerando saídas textuais detalhadas e renderizações visuais animadas em `.svg`

## Principais Funcionalidades
* **Estruturas de Dados Genéricas:** Implementação modular de estruturas para lidar com os dados da cidade, garantindo flexibilidade e reaproveitamento de código.
  * **Hashmap:** Mapeamento e busca rápida de quadras através de seus CEPs alfanuméricos.
  * **Grafo (Lista de Adjacência):** Representação do mapa viário (esquinas como vértices, ruas como arestas com pesos de distância e velocidade).
  * **Fila de Prioridade (PQueue):** Estrutura base para otimização de algoritmos de busca. 
* **Algoritmos de Roteamento e Análise:**
  * **Algoritmo de Dijkstra:** Cálculo eficiente do melhor trajeto (mais curto em distância ou mais rápido em tempo).
  * **Árvore Geradora Mínima (MST):** Identificação de trechos que necessitam de ampliação para melhorar a interconexão urbana.
  * **Componentes Conexos:** Análise de "ilhas" de fluidez viária.

* **Saída Gráfica e Animação:** Geração de arquivos `.svg` interativos, mostrando o mapa da cidade, bounding boxes, trechos destacados e animações (usando \<animateMotion>) dos percursos encontrados.

## Arquivos de Entrada
O sistema opera com três tipos principais de arquivos de entrada:
- Arquivo `.geo`: Descreve a geometria da cidade (quadras retangulares, CEPs, cores).
- Arquivo `.via`: Descreve o sistema viário (vértices representando interseções e arestas direcionadas representando as ruas, contendo propriedades como velocidade média e comprimento).
- Arquivo `.qry`: Contém as consultas geográficas e de roteamento que modificarão o mapa ou solicitarão dados.

### Arquivo (`.geo`)
Define a aparência, tamanho e localização das quadras da cidade.

| **Comando** | **Parâmetros**          | **Descrição**                                                                                      |
|-------------|-------------------------|----------------------------------------------------------------------------------------------------|
| `q`         | `cep x y w h`           | Insere uma quadra com os atributos especificados.                                                  |
| `cq`        | `sw cor_fill cor_borda` | Define a cor de preenchimento, a espessura e cor da borda das quadras subsequentes a esse comando. |

### Arquivo (`.via`)
Define todo o sistema viário da cidade (ruas e esquinas)

| **Comando**  | **Parâmetros**              | **Descrição**                                                                                                                                                    |
|--------------|-----------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `nv`         | `n`                         | A primeira linha do arquivo define previamente a quantidade de vértices presentes no grafo viário.                                                               |
| `v`          | `id x y`                    | Cria o vértice de identificador `id` e o posiciona na coordenada (`x, y`).                                                                                       |                                                                                                |
| `e`          | `i j ldir lesq cmp vm nome` | Cria a aresta (`i, j`) e associa as outras informações a aresta. Caso a aresta não possua quadras em algum de seus lados, esta ausência é indicada por um '`#`'. |

### Comandos de Consulta (.qry)
| **Comando** | **Parâmetros**     | **Descrição**                                                                                                                                              |
|-------------|--------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `@o?`       | `reg cep face num` | Armazena a posição geográfica do endereço no registrador `reg`. Marca o ponto no `.svg` e reporta no `.txt`.                                               |
| `mvm`       | `v x y w h`        | Atualiza a velocidade média das arestas (ruas) dentro da região definida para `v`.                                                                         | 
| `regs`      | `v`                | Identifica componentes conexos de ilhas de fluidez (ruas com velocidade $\ge$ `v1`). Desenha bounding boxes translúcidos no (`.svg`).                      |
| `exp`       | `v1`               | Calcula a Árvore Geradora Mínima (MST) das vias lentas (< `v1`), aumentando a velocidade dessas vias em 50%. Destaca no `.svg` em vermelho.                |
| `p?`        | `reg1 reg2 cc cr`  | Calcula o melhor trajeto entre a origem (`reg1`) e o destino (`reg2`). Traça e anima o caminho mais curto (cor `cc`) e o mais rápido (cor `cr`) no `.svg`. |


## Pré-requisitos
Certifique-se de ter as seguintes ferramentas instaladas no seu ambiente:
* Compilador **GCC** (com suporte ao padrão C99)
* **Make** (para automação da build)
* Ambiente **Linux** ou **WSL** (Windows Subsystem for Linux)

## Compilação e Execução
O projeto inclui um Makefile na pasta `src` para facilitar a compilação do executável principal e das suítes de teste.

- Para compilar o projeto principal, execute:
```bash
cd src && make
```

## Testes Unitários (Unity Framework)
Para garantir a integridade de cada **TAD** (Tipo Abstrato de Dado) e módulo funcional, o projeto utiliza o framework **Unity**.

Os arquivos de teste estão localizados na pasta `unit_test/` com o prefixo `test_*.c`. O `Makefile` está configurado para compilar e executar isoladamente cada suíte de testes.

- Para rodar um teste específico, de dentro da pasta `src/`, utilize o prefixo `test_` seguido do nome do módulo. Por exemplo:

```bash
make test_grafo
make test_pqueue
make test_hashmap
```

- Para limpar os binários gerados pela compilação e testes:
```Bash
make clean
```

## Parâmetros de Execução
O programa suporta até cinco parâmetros de linha de comando:
```Bash
./ted -e [path] -f [arq.geo] -q [consulta.qry] -v [arqvias.via] -o [dir_saida]
```


| **Parâmetro** | **Obrigatório** | **Descrição**                                                                  |
|---------------|-----------------|--------------------------------------------------------------------------------|
| `-e path`     | Não             | Diretório-base de entrada (`BED`). Se omitido, usa o diretório atual.          |
| `-f arq.geo`  | Sim             | Arquivo principal com a descrição da cidade.                                   |
| `-v arq.via`  | Não             | Arquivo de vias para a construção do grafo de tráfego.                         |
| `-q arq.qry`  | Não             | Arquivo de consultas e comandos.                                               |
| `-o path`     | Sim             | Diretório-base de saída (`BSD`) onde os arquivos `.svg` e `.txt` serão salvos. |

**Automação da execução:**

Pode se utilizar o script `(.sh)` provido no repositório para executar todos os testes de uma vez.

O seguinte comando deve ser executado após a compilação:

```bash
chmod +x run.sh && ./run.sh
```

Isso retira o processo de citar arquivo por arquivo corretamente. O único pré-requisito é que a pasta `testes` esteja presente na raiz do projeto, contendo todos os arquivos `(.geo), (.qry) e (.via)`.




## Estrutura do Código (Módulos)

* **`main`**: Ponto de entrada do programa, coordena a execução e os parâmetros.
* **`geo_handler`**: Lê o arquivo `.geo` e inicializa a geometria da cidade (quadras).
* **`via_handler`**: Lê o arquivo `.via` e constrói o grafo do sistema viário.
* **`qry_handler`**: Processa as consultas do `.qry` (roteamento, expansão, bounding boxes).
* **`svg_handler`**: Responsável pela geração e formatação do arquivo visual `.svg`.
* **`grafo`**: Implementação do grafo direcionado utilizando listas de adjacência.
* **`exhash`**: Tabela Hash Extensível para indexação e busca ultra-rápida de CEPs.
* **`priority_queue`**: Fila de prioridade (Min-Heap) otimizada para o algoritmo de Dijkstra.
* **`lista`**: Lista dinâmica genérica, base para o hash e vértices do grafo.
* **`quadra`**: Estrutura e manipulação dos dados das quadras urbanas.
* **`rua`**: Representação das vias e seus atributos (arestas do grafo).
* **`ponto`**: Estrutura auxiliar para coordenadas geográficas (x, y).
* **`utils`**: Funções utilitárias de uso geral (tratamento de strings, etc.).
