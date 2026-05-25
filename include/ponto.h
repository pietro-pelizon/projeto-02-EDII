#ifndef PROJETO_02_EDII_PONTO_H
#define PROJETO_02_EDII_PONTO_H

typedef struct stPonto ponto_t;

/// @brief Inicializa um ponto com coordenadas (x, y)
/// @param x Coordenada x do ponto
/// @param y Coordenada y do ponto
/// @return Retorna um ponto criado
ponto_t *ponto_init(double x, double y);

/// @brief Pega a coordenada x do ponto
/// @param p Ponteiro para o ponto
/// @return Coordenada x do ponto
double ponto_get_x(ponto_t *p);

/// @brief Pega a coordenada y do ponto
/// @param p Ponteiro para o ponto
/// @return Coordenada y do ponto
double ponto_get_y(ponto_t *p);

/// @brief Define uma nova coordenada x para o ponto
/// @param p Ponteiro para o ponto
/// @param x Nova coordenada x
void ponto_set_x(ponto_t *p, double x);

/// @brief Define uma nova coordenada y para o ponto
/// @param p Ponteiro para o ponto
/// @param y Nova coordenada y
void ponto_set_y(ponto_t *p, double y);

/// @brief Faz uma cópia do ponto
/// @param p Ponteiro para o ponto que será copiado
/// @return Cópia do ponto
ponto_t *ponto_copia(ponto_t *p);

/// @brief Libera a estrutura ponto
/// @param p Ponteiro para o ponto a ser liberado
void ponto_destroy(ponto_t *p);

#endif //PROJETO_02_EDII_PONTO_H
