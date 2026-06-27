#include "../include/svg_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include "../include/quadra.h"
#include "../include/ponto.h"
#include "../include/grafo.h"

FILE *svg_init(const char* caminho_arquivo) {
	FILE *svg = fopen(caminho_arquivo, "w");
	if (svg == NULL) {
		fprintf(stderr, "Erro ao abrir o arquivo SVG. (svg_handler.c:%d)\n", __LINE__);
		return NULL;
	}

	fprintf(svg, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" "
				 "xmlns=\"http://www.w3.org/2000/svg\" "
				 "xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
				 "width=\"15000\" height=\"15000\">\n");

	fprintf(svg, "<g>\n");

	return svg;
}


void svg_quadra_insert(FILE *svg, const quadra_t *q) {
	fprintf(svg, "\t<rect id=\"%s\" x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" stroke=\"%s\" fill=\"%s\" stroke-width=\"%s\" opacity=\"%lf\" />\n",
		quadra_get_cep(q),
		quadra_get_x(q),
		quadra_get_y(q),
		quadra_get_width(q),
		quadra_get_height(q),
		quadra_get_cor_borda(q),
		quadra_get_cor_preenchimento(q),
		quadra_get_stroke_width(q), 0.5);

	fprintf(svg, "\t<text x=\"%lf\" y=\"%lf\" font-family=\"Arial\" font-size=\"12\" fill=\"black\">%s</text>\n",
		quadra_get_x(q),
		quadra_get_y(q) - 2.0,
		quadra_get_cep(q));
}

void svg_close(FILE *svg) {
	if (svg == NULL) return;

	fprintf(svg, "</g>\n");

	fprintf(svg, "</svg>\n");

	fclose(svg);
}

void svg_posicao_endereco(FILE *svg, double x, double y, char *id) {
	fprintf(svg, "<line x1=\"%.2lf\" y1=\"%.2lf\" x2=\"%.2lf\" y2=\"10\" stroke=\"red\" stroke-dasharray=\"5,5\" />\n", x, y, x);
	fprintf(svg, "<text x=\"%.2lf\" y=\"14\" fill=\"red\" font-size=\"16\">%s</text>\n", x, id);
}

void svg_rect_componente_conexo(FILE *svg, char *cor, double min_x, double min_y, double max_x, double max_y){
	double height =  max_y - min_y;
	double width = max_x - min_x;

	fprintf(svg, "<rect x=\"%.2lf\" y=\"%.2lf\" width=\"%.2lf\" height=\"%.2lf\" fill=\"%s\" fill-opacity=\"0.5\" stroke=\"%s\" stroke-width=\"2\" />\n",
			min_x, min_y, width, height, cor, cor);
}

void svg_linha_caminho(FILE *svg, const char *id_origem, const char *id_destino, graph_t *g, const char *cor) {
	vertex_t *src = graph_get_vertex(g, id_origem);
	vertex_t *dst = graph_get_vertex(g, id_destino);

	ponto_t *p_src = vertex_get_data(src);
	ponto_t *p_dst = vertex_get_data(dst);

	double x1 = ponto_get_x(p_src), y1 = ponto_get_y(p_src);
	double x2 = ponto_get_x(p_dst), y2 = ponto_get_y(p_dst);

	fprintf(svg, "<line x1=\"%.2lf\" y1=\"%.2lf\" x2=\"%.2lf\" y2=\"%.2lf\" stroke=\"%s\" stroke-width=\"6\" />\n",
		x1, y1, x2, y2, cor);
}

void svg_desenha_placas(FILE *svg, const char *id_src, const char *id_dst, graph_t *g) {
	vertex_t *src = graph_get_vertex(g, id_src);
	vertex_t *dst = graph_get_vertex(g, id_dst);

	ponto_t *p_src = vertex_get_data(src);
	ponto_t *p_dst = vertex_get_data(dst);

	double x1 = ponto_get_x(p_src), y1 = ponto_get_y(p_src);
	double x2 = ponto_get_x(p_dst), y2 = ponto_get_y(p_dst);

	fprintf(svg,
		"<circle cx=\"%.2lf\" cy=\"%.2lf\" r=\"12\" fill=\"white\" stroke=\"black\" stroke-width=\"2\" />\n"
		"<text x=\"%.2lf\" y=\"%.2lf\" font-family=\"Arial\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" text-anchor=\"middle\" dominant-baseline=\"central\">I</text>\n"

		"<circle cx=\"%.2lf\" cy=\"%.2lf\" r=\"12\" fill=\"white\" stroke=\"black\" stroke-width=\"2\" />\n"
		"<text x=\"%.2lf\" y=\"%.2lf\" font-family=\"Arial\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" text-anchor=\"middle\" dominant-baseline=\"central\">F</text>\n",
		x1, y1, x1, y1,
		x2, y2, x2, y2
	);
}

void svg_anima_caminho(FILE *svg, list_t *caminho, graph_t *g, const char *id_path, double velocidade, const char *cor) {
	if (list_size(caminho) < 2) return;

	// Path visível — faz o traço E serve de trilho para a animação
	fprintf(svg, "<path id=\"%s\" fill=\"none\" stroke=\"%s\" stroke-width=\"6\" d=\"", id_path, cor);
	list_node_t *no = list_node_front(caminho);
	bool primeiro = true;
	while (no != NULL) {
		const char *id = list_node_data(no);
		vertex_t *v = graph_get_vertex(g, id);
		ponto_t *p = vertex_get_data(v);
		fprintf(svg, "%s%.2lf,%.2lf",
				primeiro ? "M " : " L ",
				ponto_get_x(p), ponto_get_y(p));
		primeiro = false;
		no = list_node_next(no);
	}
	fprintf(svg, "\" />\n");

	fprintf(svg, "<image href=\"/home/pietro/Imagens/Capturas de tela/Captura de tela de 2026-05-28 15-30-39.png\" "
				 "x=\"-60\" y=\"-60\" width=\"120\" height=\"120\">\n");
	fprintf(svg, "    <animateMotion dur=\"%lf\" repeatCount=\"indefinite\">\n", velocidade);
	fprintf(svg, "        <mpath xlink:href=\"#%s\"/>\n", id_path);
	fprintf(svg, "    </animateMotion>\n");
	fprintf(svg, "</image>\n");
}

void svg_quadra_foreach_cb(void *record_data, void *context) {
	FILE *svg = (FILE *)context;
	quadra_t *q = *(quadra_t **)record_data;
	svg_quadra_insert(svg, q);
}