#ifndef PROJETO_01_EDII_QUADRA_H
#define PROJETO_01_EDII_QUADRA_H
/*
 * A estrutura quadra_t encapsula os dados espaciais fundamentais da cidade,
 * incluindo seu identificador único (CEP), suas coordenadas de ancoragem (x, y),
 * suas dimensões (largura e altura) e suas propriedades visuais para
 * renderização em SVG (cores de preenchimento, borda e espessura).
 * * O módulo foi projetado utilizando a técnica de Ponteiros Opacos (Opaque Pointers)
 * com alocação dinâmica (Heap). Isso garante o encapsulamento total dos dados;
 *
 * Funcionalidades principais:
 * - Criação segura: aloca quadras na memória dinâmica (Heap).
 * - Encapsulamento total: uso rigoroso de Getters e Setters para proteger
 * as propriedades visuais e espaciais contra alterações indevidas.
 * - Atualização em lote: suporte nativo para o comando 'cq', permitindo
 * alterar múltiplas propriedades visuais (cores e espessura) com uma única chamada.
 */

typedef struct stQuadra quadra_t;

/// @brief Inicializa uma nova quadra com CEP, coordenadas, altura e largura.
/// @param cep CEP identificador da quadra
/// @param x Coordenada X do ponto de ancoragem da quadra.
/// @param y Coordenada Y do ponto de ancoragem da quadra.
/// @param w Largura (width) da quadra.
/// @param h Altura (height) da quadra.
/// @return Retorna um ponteiro para a quadra alocada dinamicamente (Heap). O usuário DEVE chamar quadra_destroy() após o uso.
quadra_t *quadra_init(const char *cep, double x, double y, double w, double h);

/// @brief Destrói a quadra, liberando a memória alocada.
/// @param q Ponteiro para a quadra a ser destruída.
void quadra_destroy(quadra_t *q);

/// @brief Altera o identificador (CEP) da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param novo_cep String contendo o novo CEP.
void quadra_set_cep(quadra_t *q, const char *novo_cep);

/// @brief Define a coordenada X do ponto de ancoragem da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param novo_x Novo valor da coordenada X no eixo horizontal.
void quadra_set_x(quadra_t *q, double novo_x);

/// @brief Define a coordenada Y do ponto de ancoragem da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param novo_y Novo valor da coordenada Y no eixo vertical.
void quadra_set_y(quadra_t *q, double novo_y);

/// @brief Define a largura (width) do retângulo da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param novo_w Novo valor numérico da largura.
void quadra_set_w(quadra_t *q, double novo_w);

/// @brief Define a altura (height) do retângulo da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param novo_h Novo valor numérico da altura.
void quadra_set_h(quadra_t *q, double novo_h);

/// @brief Define a cor de preenchimento (cfill) do retângulo da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param nova_corp String contendo a cor (ex: "red", "#FF0000").
void quadra_set_corp(quadra_t *q, const char *nova_corp);

/// @brief Define a cor da borda (cstrk) do retângulo da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param nova_corb String contendo a cor da borda.
void quadra_set_corb(quadra_t *q, const char *nova_corb);

/// @brief Define a espessura da borda (stroke width) do retângulo da quadra.
/// @param q Ponteiro para a quadra que será alterada.
/// @param novo_sw Novo valor numérico da espessura.
void quadra_set_sw(quadra_t *q, const char *novo_sw);

/// @brief Obtém a cor de preenchimento atual da quadra.
/// @param q Ponteiro para a quadra.
/// @return Retorna a string inalterável contendo a cor de preenchimento.
const char *quadra_get_corp(const quadra_t *q);

/// @brief Obtém a cor da borda atual da quadra.
/// @param q Ponteiro para a quadra.
/// @return Retorna a string inalterável contendo a cor da borda.
const char *quadra_get_corb(const quadra_t *q);

/// @brief Obtém a espessura da borda (stroke width) atual da quadra.
/// @param q Ponteiro para a quadra.
/// @return Retorna o valor numérico da espessura.
const char *quadra_get_sw(const quadra_t *q);

/// @brief Obtém o CEP (identificador único) da quadra.
/// @param q Ponteiro para a quadra.
/// @return Retorna a string inalterável contendo o CEP.
const char *quadra_get_cep(const quadra_t *q);

/// @brief Obtém a coordenada X do ponto de ancoragem da quadra.
/// @param q Ponteiro para a quadra.
/// @return Valor da coordenada X.
double quadra_get_x(const quadra_t *q);

/// @brief Obtém a coordenada Y do ponto de ancoragem da quadra.
/// @param q Ponteiro para a quadra.
/// @return Valor da coordenada Y.
double quadra_get_y(const quadra_t *q);

/// @brief Obtém a largura atual (width) da quadra.
/// @param q Ponteiro para a quadra.
/// @return Valor da largura.
double quadra_get_w(const quadra_t *q);

/// @brief Obtém a altura atual (height) da quadra.
/// @param q Ponteiro para a quadra.
/// @return Valor da altura.
double quadra_get_h(const quadra_t *q);

/// @brief Aplica em lote as propriedades visuais da quadra (Equivalente ao comando 'cq').
/// @param q Ponteiro para a quadra que será alterada.
/// @param sw Espessura da borda (stroke width).
/// @param corb Cor da borda (cstrk).
/// @param corp Cor de preenchimento (cfill).
void quadra_set_cq(quadra_t *q, char *sw, const char *corp,  const char *corb);

/// @brief Faz o cálculos necessários para achar determinado ponto a partir da quadra
/// @param face Face desejada
/// @param x Coordenada x que será "retornada"
/// @param y Coordenada y que será "retornada"
/// @param quadra Ponteiro para a quadra
/// @param numero Número da "casa"
void quadra_get_coord(char face, double *x, double *y, quadra_t *quadra, double numero);

#endif //PROJETO_01_EDII_QUADRA_H