#include "../include/svg_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include "../include/quadra.h"

FILE *svg_init(const char* caminho_arquivo) {
	FILE *svg = fopen(caminho_arquivo, "w");
	if (svg == NULL) {
		perror("ERRO ao abrir o arquivo SVG");
		return NULL;
	}

	fprintf(svg, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	fprintf(svg, "<svg xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns=\"http://www.w3.org/2000/svg\" width=\"15000\" height=\"15000\">\n");

	fprintf(svg, "<g>\n");

	return svg;
}


void svg_quadra_insert(FILE *svg, const quadra_t *q) {
	fprintf(svg, "\t<rect id=\"%s\" x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" stroke=\"%s\" fill=\"%s\" stroke-width=\"%s\" opacity=\"%lf\" />\n",
		quadra_get_cep(q),
		quadra_get_x(q),
		quadra_get_y(q),
		quadra_get_w(q),
		quadra_get_h(q),
		quadra_get_corb(q),
		quadra_get_corp(q),
		quadra_get_sw(q), 0.5);

	fprintf(svg, "\t<text x=\"%lf\" y=\"%lf\" font-family=\"Arial\" font-size=\"12\" fill=\"black\">%s</text>\n",
		quadra_get_x(q),
		quadra_get_y(q) - 2.0,
		quadra_get_cep(q));
}

void fecha_svg(FILE *svg) {
	if (svg == NULL) return;

	fprintf(svg, "</g>\n");

	fprintf(svg, "</svg>\n");

	fclose(svg);
}

void pos_endereco(FILE *svg, double x, double y, char *id) {
	fprintf(svg, "<line x1=\"%.2lf\" y1=\"%.2lf\" x2=\"%.2lf\" y2=\"10\" stroke=\"red\" stroke-dasharray=\"5,5\" />\n", x, y, x);
	fprintf(svg, "<text x=\"%.2lf\" y=\"10\" fill=\"red\" font-size=\"12\">%s</text>\n", x, id);
}

void rect_componente_conexo(FILE *svg, char *cor, double min_x, double min_y, double max_x, double max_y){
	double height = max_x - min_x;
	double width = max_y - min_y;

	fprintf(svg, "<rect x=\"%.2lf\" y=\"%.2lf\" width=\"%.2lf\" height=\"%.2lf\" fill=\"%s\" fill-opacity=\"0.5\" stroke=\"%s\" stroke-width=\"2\" />\n",
			min_x, min_y, width, height, cor, cor);
}