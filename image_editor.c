// Țucă Mihai-Laurențiu 314CA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define name_max 128
#define buffer_max 256
#define format_max 3

typedef struct {
	int r, g, b;
} pixel_t;

// Aloca dinamic o matrice de pixeli RGB
void rgb_matrix_alloc(pixel_t ***matrix, const int height, const int width)
{
	*matrix = (pixel_t **)malloc((height) * sizeof(pixel_t *));
	if (!(*matrix)) {
		fprintf(stderr, "malloc() failed");
		return;
	}
	int i;
	for (i = 0; i < height; i++) {
		(*matrix)[i] = (pixel_t *)calloc((width), sizeof(pixel_t));
		if (!(*matrix)) {
			fprintf(stderr, "malloc() failed");
			while (--i >= 0) {
				free((*matrix)[i]);
			}
			free(*matrix);
			*matrix = NULL;
			return;
		}
	}
}

// Elibereaza o matrice de tip pixeli RGB alocata dinamic cu height linii
void free_matrix_p(pixel_t ***matrix, const int height)
{
	int i;
	for (i = 0; i < height; i++) {
		free((*matrix)[i]);
		(*matrix)[i] = NULL;
	}
	free(*matrix);
	*matrix = NULL;
}

// Elibereaza o matrice de int-uri alocata dinamic cu height linii
void free_matrix_int(int ***matrix, const int height)
{
	int i;
	for (i = 0; i < height; i++) {
		free((*matrix)[i]);
		(*matrix)[i] = NULL;
	}
	free(*matrix);
	*matrix = NULL;
}

// Aloca dinamic o matrice de int-uri cu height linii si width coloane
void int_matrix_alloc(int ***matrix, const int height, const int width)
{
	*matrix = (int **)malloc((height) * sizeof(int *));
	if (!(*matrix)) {
		fprintf(stderr, "malloc() failed");
		return;
	}
	int i;
	for (i = 0; i < height; i++) {
		(*matrix)[i] = (int *)calloc((width), sizeof(int));
		if (!(*matrix)) {
			fprintf(stderr, "malloc() failed");
			while (--i >= 0) {
				free((*matrix)[i]);
			}
			free(*matrix);
			*matrix = NULL;
			return;
		}
	}
}

// Citeste tipul imaginii din antet si il salveaza in variabila type
void check_image_type(const char image_format[3], int *type)
{
	if (strcmp(image_format, "P1") == 0) {
		*type = 1;
	} else if (strcmp(image_format, "P2") == 0) {
		*type = 2;
	} else if (strcmp(image_format, "P3") == 0) {
		*type = 3;
	} else if (strcmp(image_format, "P4") == 0) {
		*type = 4;
	} else if (strcmp(image_format, "P5") == 0) {
		*type = 5;
	} else if (strcmp(image_format, "P6") == 0) {
		*type = 6;
	}
}

// Citeste o imagine rgb de tip text si aloca dinamic o matrice pentru ea
void load_rgb_image_text(FILE *image, pixel_t ***used_image, int *width,
						 int *height, int *max_intensity)
{
	fseek(image, 2, SEEK_SET);
	fscanf(image, "%d%d%d", width, height, max_intensity);
	rgb_matrix_alloc(used_image, *height, *width);

	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			fscanf(image, "%d%d%d", &(*used_image)[i][j].r,
				   &(*used_image)[i][j].g, &(*used_image)[i][j].b);
		}
	}
}

// Citeste o imagine alb-negru de tip text si aloca dinamic o matrice pentru ea
void load_image_text(FILE *image, int ***used_image, int *width,
					 int *height, int *max_intensity)
{
	fseek(image, 2, SEEK_SET);
	fscanf(image, "%d%d%d", width, height, max_intensity);
	int_matrix_alloc(used_image, *height, *width);

	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			fscanf(image, "%d", &(*used_image)[i][j]);
		}
	}
}

// Citeste o imagine alb-negru de tip binar si aloca dinamic o matrice
// pentru ea
void load_image_binary(FILE *image, int ***used_image, int *width,
					   int *height, int *max_intensity)
{
	fseek(image, 2, SEEK_SET);
	fscanf(image, "%d %d %d", width, height, max_intensity);
	fgetc(image);
	int_matrix_alloc(used_image, *height, *width);

	unsigned char **temp_binary = malloc(*height * sizeof(unsigned char *));
	for (int i = 0; i < *height; i++) {
		temp_binary[i] = calloc(*width, sizeof(unsigned char));
		fread(temp_binary[i], sizeof(unsigned char), *width, image);
	}

	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			(*used_image)[i][j] = (int)temp_binary[i][j];
		}
		free(temp_binary[i]);
	}
	free(temp_binary);
}

// Citeste o imagine rgb de tip binar si aloca dinamic o matrice pentru ea
void load_rgb_image_binary(FILE *image, pixel_t ***used_image,
						   int *width, int *height, int *max_intensity)
{
	fseek(image, 2, SEEK_SET);
	fscanf(image, "%d %d %d", width, height, max_intensity);
	fgetc(image);
	rgb_matrix_alloc(used_image, *height, *width);

	unsigned char **temp_binary = malloc(*height * sizeof(unsigned char *));
	for (int i = 0; i < *height; i++) {
		temp_binary[i] = calloc(3 * (*width), sizeof(unsigned char));
		fread(temp_binary[i], sizeof(unsigned char), 3 * (*width), image);
	}

	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			(*used_image)[i][j].r = (int)temp_binary[i][3 * j];
			(*used_image)[i][j].g = (int)temp_binary[i][3 * j + 1];
			(*used_image)[i][j].b = (int)temp_binary[i][3 * j + 2];
		}
		free(temp_binary[i]);
	}
	free(temp_binary);
}

// Rezolva comanda "LOAD" in functie de tipul imaginii
int handle_load_command(int *image_type, FILE **image, int *binary,
						int ***used_image, pixel_t ***rgb_used_image,
						int *loaded, int *height, int *width,
						int *max_intensity, int *startheight, int *startwidth,
						int *endheight, int *endwidth)
{
	if (*loaded == 1) {
		if (*used_image) {
			free_matrix_int(used_image, *height);
		}
		if (*rgb_used_image) {
			free_matrix_p(rgb_used_image, *height);
		}
	}
	char *filename = strtok(NULL, "\n ");
	if (*image) {
		fclose(*image);
	}
	*image = fopen(filename, "rt");
	if (!*image) {
		printf("Failed to load %s\n", filename);
		*loaded = 0;
		return -1;
	}
	char format[format_max];
	fseek(*image, 0, SEEK_SET);
	fgets(format, 3, *image);
	format[3] = '\0';
	check_image_type(format, image_type);
	if (*image_type == 1 || *image_type == 2 || *image_type == 3) {
		printf("Loaded %s\n", filename);
		*binary = 0;
	} else if (*image_type == 4 || *image_type == 5 || *image_type == 6) {
		fclose(*image);
		*image = fopen(filename, "rb");
		if (!*image) {
			printf("Failed to load %s\n", filename);
			return -1;
		}
		printf("Loaded %s\n", filename);
		*binary = 1;
	} else {
		printf("Failed to load %s\n", filename);
		return -1;
	}
	if (*binary == 0) {
		if (*image_type == 1 || *image_type == 2) {
			load_image_text(*image, used_image, width, height, max_intensity);
		} else {
			load_rgb_image_text(*image, rgb_used_image, width, height,
								max_intensity);
		}
	} else {
		if (*image_type == 4 || *image_type == 5) {
			load_image_binary(*image, used_image, width, height,
							  max_intensity);
		} else {
			load_rgb_image_binary(*image, rgb_used_image, width, height,
								  max_intensity);
		}
		*startheight = 0;
		*startwidth = 0;
		*endheight = *height;
		*endwidth = *width;
	}
	*loaded = 1;
	return 0;
}

// Valideaza coordonatele din comanda "SELECT"
int validate_select(int *stselwidth, int *stselheight, int *endselwidth,
					int *endselheight, const int width, const int height)
{
	if (*stselheight > *endselheight) {
		int aux = *stselheight;
		*stselheight = *endselheight;
		*endselheight = aux;
	}
	if (*stselwidth > *endselwidth) {
		int aux = *stselwidth;
		*stselwidth = *endselwidth;
		*endselwidth = aux;
	}
	int valid = 1;
	if (*stselheight < 0 || *stselheight > height || *stselwidth < 0 ||
		*stselwidth > width || *endselheight < 0 ||
		*endselheight > height || *endselwidth < 0 ||
		*endselwidth > width || *stselheight == *endselheight ||
		*stselwidth == *endselwidth) {
		valid = 0;
	}

	return valid;
}

// Daca sunt bune coordonatele, le actualizeaza
void update_select(int stselwidth, int stselheight, int endselwidth,
				   int endselheight, int width, int height, int *startwidth,
				   int *startheight, int *endwidth, int *endheight,
				   int *sel_all)
{
	*startwidth = stselwidth;
	*startheight = stselheight;
	*endwidth = endselwidth;
	*endheight = endselheight;
	*sel_all = 0;

	if (*startheight == 0 && *startwidth == 0 && *endheight == height &&
		*endwidth == width) {
		*sel_all = 1;
	}
}

// Rezolva comanda "SELECT"
int handle_select(const int loaded, int *sel_all, int *startwidth,
				  int *startheight, int *endwidth, int *endheight, int width,
				  int height, int *stselheight, int *stselwidth,
				  int *endselheight, int *endselwidth)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return -1;
	}
	char *all = strtok(NULL, "\n ");
	if (strcmp(all, "ALL") == 0) {
		*sel_all = 1;
		*startheight = 0;
		*startwidth = 0;
		*endheight = height;
		*endwidth = width;
		printf("Selected ALL\n");
		return 0;
	}
	if (!all) {
		printf("Invalid command\n");
		return -1;
	}
	for (int i = 0; i < (int)strlen(all) - 1; i++) {
		if ((all[i] < '0' || all[i] > '9') && all[0] != '-') {
			printf("Invalid command\n");
			return -1;
		}
	}
	*stselwidth = atoi(all);
	all = strtok(NULL, "\n ");
	if (!all) {
		printf("Invalid command\n");
		return -1;
	}
	for (int i = 0; i < (int)strlen(all) - 1; i++) {
		if ((all[i] < '0' || all[i] > '9') && all[0] != '-') {
			printf("Invalid command\n");
			return -1;
		}
	}
	*stselheight = atoi(all);
	all = strtok(NULL, "\n ");
	if (!all) {
		printf("Invalid command\n");
		return -1;
	}
	for (int i = 0; i < (int)strlen(all) - 1; i++) {
		if ((all[i] < '0' || all[i] > '9') && all[0] != '-') {
			printf("Invalid command\n");
			return -1;
		}
	}
	*endselwidth = atoi(all);
	all = strtok(NULL, "\n ");
	if (!all) {
		printf("Invalid command\n");
		return -1;
	}
	for (int i = 0; i < (int)strlen(all) - 1; i++) {
		if ((all[i] < '0' || all[i] > '9') && all[0] != '-') {
			printf("Invalid command\n");
			return -1;
		}
	}
	*endselheight = atoi(all);
	if (validate_select(stselwidth, stselheight, endselwidth, endselheight,
						width, height) == 0) {
		printf("Invalid set of coordinates\n");
		return -1;
	}
	update_select(*stselwidth, *stselheight, *endselwidth,
				  *endselheight, width, height, startwidth,
				  startheight, endwidth, endheight, sel_all);
	printf("Selected %d %d %d %d\n", *stselwidth, *stselheight,
		   *endselwidth, *endselheight);
	return 0;
}

// Copiaza intr-o imagine destinatie o imagine sursa (ambele RGB)
void copy_rgb_image(pixel_t ***img_destination, pixel_t **img_source,
					const int start_h, const int start_w, const int height,
					const int width)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			(*img_destination)[i][j].r = img_source[start_h + i][start_w + j].r;
			(*img_destination)[i][j].g = img_source[start_h + i][start_w + j].g;
			(*img_destination)[i][j].b = img_source[start_h + i][start_w + j].b;
		}
	}
}

// Copiaaza intr-o imagine destinatie o imagine sursa (ambele alb-negru)
void copy_image(int ***img_destination, int **img_source,
				const int start_h, const int start_w, const int height,
				const int width)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			(*img_destination)[i][j] = img_source[start_h + i][start_w + j];
		}
	}
}

// Realoca o imagine RGB la apelul comenzii "CROP" cu coordonatele din "SELECT"
int crop_rgb_image(pixel_t ***image, int *height, int *width, int *start_h,
				   int *start_w, int *end_h, int *end_w)
{
	int new_height = *end_h - *start_h;
	int new_width = *end_w - *start_w;

	pixel_t **cropped_image = NULL;
	rgb_matrix_alloc(&cropped_image, new_height, new_width);
	if (!cropped_image) {
		return -1;
	}
	copy_rgb_image(&cropped_image, *image, *start_h, *start_w, new_height,
				   new_width);
	free_matrix_p(image, *height);
	rgb_matrix_alloc(image, new_height, new_width);
	if (!(*image)) {
		return -1;
	}
	copy_rgb_image(image, cropped_image, 0, 0, new_height, new_width);
	free_matrix_p(&cropped_image, new_height);
	*height = new_height;
	*width = new_width;
	*start_h = 0;
	*start_w = 0;
	*end_h = *height;
	*end_w = *width;
	return 0;
}

// Realoca o imagine alb-negru la apelul comenzii "CROP" cu coordonatele din
// "SELECT"
int crop_image(int ***image, int *height, int *width, int *start_h,
			   int *start_w, int *end_h, int *end_w)
{
	int new_height = *end_h - *start_h;
	int new_width = *end_w - *start_w;

	int **cropped_image = NULL;
	int_matrix_alloc(&cropped_image, new_height, new_width);
	if (!cropped_image) {
		return -1;
	}
	copy_image(&cropped_image, *image, *start_h, *start_w, new_height,
			   new_width);
	free_matrix_int(image, *height);
	int_matrix_alloc(image, new_height, new_width);
	if (!(*image)) {
		return -1;
	}
	copy_image(image, cropped_image, 0, 0, new_height, new_width);
	free_matrix_int(&cropped_image, new_height);
	*height = new_height;
	*width = new_width;
	*start_h = 0;
	*start_w = 0;
	*end_h = *height;
	*end_w = *width;
	return 0;
}

// Rezolva comanda "CROP" in functie de coordonatele din "SELECT"
int handle_crop(const int image_type, pixel_t ***rgb_image, int ***image,
				int *height, int *width, int *start_h, int *start_w,
				int *end_h, int *end_w, int *loaded, int *sel_all)
{
	if (*loaded == 0) {
		printf("No image loaded\n");
		return -1;
	}
	if (*sel_all == 1) {
		printf("Image cropped\n");
		return 0;
	}
	if (image_type == 3 || image_type == 6) {
		printf("Image cropped\n");
		*sel_all = 1;
		return crop_rgb_image(rgb_image, height, width, start_h, start_w,
							  end_h, end_w);
	} else {
		printf("Image cropped\n");
		*sel_all = 1;
		return crop_image(image, height, width, start_h, start_w, end_h,
						  end_w);
	}
}

// Salveaza o imagine daca avem prezent parametrul "ascii"
void save_image_ascii(FILE **output_image, int *image_type, const int width,
					  const int height, const int max_intensity,
					  pixel_t **rgb_image, int **image)
{
	if (*image_type > 3) {
		*image_type -= 3;
	}
	fprintf(*output_image, "P%d\n%d %d\n%d\n", *image_type, width, height,
			max_intensity);
	if (*image_type == 3) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				fprintf(*output_image, "%d %d %d ", rgb_image[i][j].r,
						rgb_image[i][j].g, rgb_image[i][j].b);
			}
			fprintf(*output_image, "\n");
		}
	} else {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				fprintf(*output_image, "%d ", image[i][j]);
			}
			fprintf(*output_image, "\n");
		}
	}
}

// Salveaza o imagine daca nu este prezent parametrul "ascii"
int save_image_binary(FILE **output_image, int *image_type, const int width,
					  const int height, const int max_intensity,
					  pixel_t **rgb_image, int **image)
{
	if (*image_type < 4) {
		*image_type += 3;
	}
	fprintf(*output_image, "P%d\n%d %d\n%d\n", *image_type, width, height,
			max_intensity);
	if (*image_type == 6) {
		unsigned char **output_bin = malloc(height * sizeof(unsigned char *));
		if (!output_bin) {
			perror("malloc() failed");
			return -1;
		}
		for (int i = 0; i < height; i++) {
			output_bin[i] = calloc(3 * width, sizeof(unsigned char));
			if (!output_bin[i]) {
				perror("calloc() failed");
				while (--i >= 0) {
					free(output_bin[i]);
				}
				free(output_bin);
				return -1;
			}
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				output_bin[i][3 * j] = rgb_image[i][j].r;
				output_bin[i][3 * j + 1] = rgb_image[i][j].g;
				output_bin[i][3 * j + 2] = rgb_image[i][j].b;
			}
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < 3 * width; j++)
				fwrite(&output_bin[i][j], sizeof(unsigned char), 1,
					   *output_image);
		}
		for (int i = 0; i < height; i++) {
			free(output_bin[i]);
		}
		free(output_bin);
	} else {
		unsigned char **output_bin = malloc(height * sizeof(unsigned char *));
		if (!output_bin) {
			perror("malloc() failed");
			return -1;
		}
		for (int i = 0; i < height; i++) {
			output_bin[i] = calloc(width, sizeof(unsigned char));
			if (!output_bin[i]) {
				perror("calloc() failed");
				while (--i >= 0) {
					free(output_bin[i]);
				}
				free(output_bin);
				return -1;
			}
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				output_bin[i][j] = image[i][j];
			}
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				fwrite(&output_bin[i][j], sizeof(unsigned char), 1,
					   *output_image);
		}
		for (int i = 0; i < height; i++) {
			free(output_bin[i]);
		}
		free(output_bin);
	}
	return 0;
}

// Rezolva comanda "SAVE" in functie de parametrii comenzii
int handle_save(int *image_type, const int width, const int height,
				const int max_intensity, pixel_t **rgb_image,
				int **image, const int loaded)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return -1;
	}
	int ret = 0;
	char *filename = strtok(NULL, "\n ");
	if (strtok(NULL, "\n ")) {
		FILE *output_image = fopen(filename, "wt");
		if (!output_image) {
			perror("Failed to open");
			return -1;
		}
		printf("Saved %s\n", filename);
		save_image_ascii(&output_image, image_type, width, height,
						 max_intensity, rgb_image, image);
		fclose(output_image);
	} else {
		FILE *output_image = fopen(filename, "wb");
		if (!output_image) {
			perror("Failed to open");
			return -1;
		}
		printf("Saved %s\n", filename);
		ret = save_image_binary(&output_image, image_type, width, height,
								max_intensity, rgb_image, image);
		fclose(output_image);
	}
	if (ret == -1) {
		return -1;
	}
	return 0;
}

// Calculeaza valoarea pixelului [i][j] la aplicarea filtrului EDGE
void edge_values(pixel_t ***applied_to, pixel_t ***filter_apply, int i, int j,
				 const int width, const int height)
{
	if (i - 1 >= 0 && j - 1 >= 0) {
		(*filter_apply)[i][j].r -= (*applied_to)[i - 1][j - 1].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i - 1][j - 1].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i - 1][j - 1].b;
	}
	if (i - 1 >= 0) {
		(*filter_apply)[i][j].r -= (*applied_to)[i - 1][j].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i - 1][j].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i - 1][j].b;
	}
	if (i - 1 >= 0 && j + 1 < width) {
		(*filter_apply)[i][j].r -= (*applied_to)[i - 1][j + 1].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i - 1][j + 1].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i - 1][j + 1].b;
	}
	if (j - 1 >= 0) {
		(*filter_apply)[i][j].r -= (*applied_to)[i][j - 1].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i][j - 1].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i][j - 1].b;
	}
	if (j + 1 < width) {
		(*filter_apply)[i][j].r -= (*applied_to)[i][j + 1].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i][j + 1].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i][j + 1].b;
	}
	if (i + 1 < height && j - 1 >= 0) {
		(*filter_apply)[i][j].r -= (*applied_to)[i + 1][j - 1].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i + 1][j - 1].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i + 1][j - 1].b;
	}
	if (i + 1 < height) {
		(*filter_apply)[i][j].r -= (*applied_to)[i + 1][j].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i + 1][j].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i + 1][j].b;
	}
	if (i + 1 < height && j + 1 < width) {
		(*filter_apply)[i][j].r -= (*applied_to)[i + 1][j + 1].r;
		(*filter_apply)[i][j].g -= (*applied_to)[i + 1][j + 1].g;
		(*filter_apply)[i][j].b -= (*applied_to)[i + 1][j + 1].b;
	}
}

void check_border(int *i, int *j)
{
	if (*j == 0) {
		(*j)++;
	}
	if (*i == 0) {
		(*i)++;
	}
}

// Rezolva "APPLY EDGE"
int apply_edge(pixel_t ***applied_to, const int start_w, const int start_h,
			   const int end_w, const int end_h, const int height,
			   const int width)
{
	pixel_t **filter_apply = NULL;
	rgb_matrix_alloc(&filter_apply, height, width);
	if (!filter_apply) {
		return -1;
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			edge_values(applied_to, &filter_apply, i, j, width, height);
			filter_apply[i][j].r += 8 * (*applied_to)[i][j].r;
			filter_apply[i][j].g += 8 * (*applied_to)[i][j].g;
			filter_apply[i][j].b += 8 * (*applied_to)[i][j].b;
			filter_apply[i][j].r = filter_apply[i][j].r < 0 ?
								   0 : filter_apply[i][j].r;
			filter_apply[i][j].r = filter_apply[i][j].r > 255 ?
								   255 : filter_apply[i][j].r;
			filter_apply[i][j].g = filter_apply[i][j].g < 0 ?
								   0 : filter_apply[i][j].g;
			filter_apply[i][j].g = filter_apply[i][j].g > 255 ?
								   255 : filter_apply[i][j].g;
			filter_apply[i][j].b = filter_apply[i][j].b < 0 ?
								   0 : filter_apply[i][j].b;
			filter_apply[i][j].b = filter_apply[i][j].b > 255 ?
								   255 : filter_apply[i][j].b;
		}
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			(*applied_to)[i][j].r = filter_apply[i][j].r;
			(*applied_to)[i][j].g = filter_apply[i][j].g;
			(*applied_to)[i][j].b = filter_apply[i][j].b;
		}
	}
	free_matrix_p(&filter_apply, height);
	printf("APPLY EDGE done\n");
	return 0;
}

// Rezolva "APPLY SHARPEN"
int apply_sharpen(pixel_t ***applied_to, const int start_w, const int start_h,
				  const int end_w, const int end_h, const int height,
				  const int width)
{
	pixel_t **filter_apply = NULL;
	rgb_matrix_alloc(&filter_apply, height, width);
	if (!filter_apply) {
		return -1;
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			if (i - 1 >= 0) {
				filter_apply[i][j].r -= (*applied_to)[i - 1][j].r;
				filter_apply[i][j].g -= (*applied_to)[i - 1][j].g;
				filter_apply[i][j].b -= (*applied_to)[i - 1][j].b;
			}
			if (j - 1 >= 0) {
				filter_apply[i][j].r -= (*applied_to)[i][j - 1].r;
				filter_apply[i][j].g -= (*applied_to)[i][j - 1].g;
				filter_apply[i][j].b -= (*applied_to)[i][j - 1].b;
			}
			if (j + 1 < width) {
				filter_apply[i][j].r -= (*applied_to)[i][j + 1].r;
				filter_apply[i][j].g -= (*applied_to)[i][j + 1].g;
				filter_apply[i][j].b -= (*applied_to)[i][j + 1].b;
			}
			if (i + 1 < height) {
				filter_apply[i][j].r -= (*applied_to)[i + 1][j].r;
				filter_apply[i][j].g -= (*applied_to)[i + 1][j].g;
				filter_apply[i][j].b -= (*applied_to)[i + 1][j].b;
			}
			filter_apply[i][j].r += 5 * (*applied_to)[i][j].r;
			filter_apply[i][j].g += 5 * (*applied_to)[i][j].g;
			filter_apply[i][j].b += 5 * (*applied_to)[i][j].b;
			filter_apply[i][j].r = filter_apply[i][j].r < 0 ?
								   0 : filter_apply[i][j].r;
			filter_apply[i][j].r = filter_apply[i][j].r > 255 ?
								   255 : filter_apply[i][j].r;
			filter_apply[i][j].g = filter_apply[i][j].g < 0 ?
								   0 : filter_apply[i][j].g;
			filter_apply[i][j].g = filter_apply[i][j].g > 255 ?
								   255 : filter_apply[i][j].g;
			filter_apply[i][j].b = filter_apply[i][j].b < 0 ?
								   0 : filter_apply[i][j].b;
			filter_apply[i][j].b = filter_apply[i][j].b > 255 ?
								   255 : filter_apply[i][j].b;
		}
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			(*applied_to)[i][j].r = filter_apply[i][j].r;
			(*applied_to)[i][j].g = filter_apply[i][j].g;
			(*applied_to)[i][j].b = filter_apply[i][j].b;
		}
	}
	free_matrix_p(&filter_apply, height);
	printf("APPLY SHARPEN done\n");
	return 0;
}

// Calculeaza valoare pixelului la apelul "APPLY BLUR"
void blur_values(double *red, double *green, double *blue, int i, int j,
				 const int height, const int width, pixel_t ***applied_to)
{
	if (i - 1 >= 0 && j - 1 >= 0) {
		*red += (*applied_to)[i - 1][j - 1].r;
		*green += (*applied_to)[i - 1][j - 1].g;
		*blue += (*applied_to)[i - 1][j - 1].b;
	}
	if (i - 1 >= 0) {
		*red += (*applied_to)[i - 1][j].r;
		*green += (*applied_to)[i - 1][j].g;
		*blue += (*applied_to)[i - 1][j].b;
	}
	if (i - 1 >= 0 && j + 1 < width) {
		*red += (*applied_to)[i - 1][j + 1].r;
		*green += (*applied_to)[i - 1][j + 1].g;
		*blue += (*applied_to)[i - 1][j + 1].b;
	}
	if (j - 1 >= 0) {
		*red += (*applied_to)[i][j - 1].r;
		*green += (*applied_to)[i][j - 1].g;
		*blue += (*applied_to)[i][j - 1].b;
	}
	if (j + 1 < width) {
		*red += (*applied_to)[i][j + 1].r;
		*green += (*applied_to)[i][j + 1].g;
		*blue += (*applied_to)[i][j + 1].b;
	}
	if (i + 1 < height && j - 1 >= 0) {
		*red += (*applied_to)[i + 1][j - 1].r;
		*green += (*applied_to)[i + 1][j - 1].g;
		*blue += (*applied_to)[i + 1][j - 1].b;
	}
	if (i + 1 < height) {
		*red += (*applied_to)[i + 1][j].r;
		*green += (*applied_to)[i + 1][j].g;
		*blue += (*applied_to)[i + 1][j].b;
	}
	if (i + 1 < height && j + 1 < width) {
		*red += (*applied_to)[i + 1][j + 1].r;
		*green += (*applied_to)[i + 1][j + 1].g;
		*blue += (*applied_to)[i + 1][j + 1].b;
	}
}

// Rezolva "APPLY BLUR"
int apply_blur(pixel_t ***applied_to, const int start_w, const int start_h,
			   const int end_w, const int end_h, const int height,
			   const int width)
{
	pixel_t **filter_apply = NULL;
	rgb_matrix_alloc(&filter_apply, height, width);
	if (!filter_apply) {
		return -1;
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			double red = 0, blue = 0, green = 0;
			blur_values(&red, &green, &blue, i, j, height, width, applied_to);
			red += (*applied_to)[i][j].r;
			green += (*applied_to)[i][j].g;
			blue += (*applied_to)[i][j].b;
			red /= 9;
			green /= 9;
			blue /= 9;
			filter_apply[i][j].r /= 9;
			filter_apply[i][j].g /= 9;
			filter_apply[i][j].b /= 9;
			filter_apply[i][j].r = (int)red;
			filter_apply[i][j].g = (int)green;
			filter_apply[i][j].b = (int)blue;
			filter_apply[i][j].r = filter_apply[i][j].r < 0 ? 0 : (int)red;
			filter_apply[i][j].r = filter_apply[i][j].r > 255 ? 255 : (int)red;
			filter_apply[i][j].g = filter_apply[i][j].g < 0 ?
								   0 : (int)green;
			filter_apply[i][j].g = filter_apply[i][j].g > 255 ?
								   255 : (int)green;
			filter_apply[i][j].b = filter_apply[i][j].b < 0 ? 0 : (int)blue;
			filter_apply[i][j].b = filter_apply[i][j].b > 255 ?
								   255 : (int)blue;
		}
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			(*applied_to)[i][j].r = filter_apply[i][j].r;
			(*applied_to)[i][j].g = filter_apply[i][j].g;
			(*applied_to)[i][j].b = filter_apply[i][j].b;
		}
	}
	free_matrix_p(&filter_apply, height);
	printf("APPLY BLUR done\n");
	return 0;
}

// Calculeaza valoarea pixelului la apelul "APPLY GAUSSIAN_BLUR"
void gaussian_blur_values(double *red, double *green, double *blue, int i,
						  int j, const int height, const int width,
						  pixel_t ***applied_to)
{
	if (i - 1 >= 0 && j - 1 >= 0) {
		*red += (*applied_to)[i - 1][j - 1].r;
		*green += (*applied_to)[i - 1][j - 1].g;
		*blue += (*applied_to)[i - 1][j - 1].b;
	}
	if (i - 1 >= 0) {
		*red += 2 * (*applied_to)[i - 1][j].r;
		*green += 2 * (*applied_to)[i - 1][j].g;
		*blue += 2 * (*applied_to)[i - 1][j].b;
	}
	if (i - 1 >= 0 && j + 1 < width) {
		*red += (*applied_to)[i - 1][j + 1].r;
		*green += (*applied_to)[i - 1][j + 1].g;
		*blue += (*applied_to)[i - 1][j + 1].b;
	}
	if (j - 1 >= 0) {
		*red += 2 * (*applied_to)[i][j - 1].r;
		*green += 2 * (*applied_to)[i][j - 1].g;
		*blue += 2 * (*applied_to)[i][j - 1].b;
	}
	if (j + 1 < width) {
		*red += 2 * (*applied_to)[i][j + 1].r;
		*green += 2 * (*applied_to)[i][j + 1].g;
		*blue += 2 * (*applied_to)[i][j + 1].b;
	}
	if (i + 1 < height && j - 1 >= 0) {
		*red += (*applied_to)[i + 1][j - 1].r;
		*green += (*applied_to)[i + 1][j - 1].g;
		*blue += (*applied_to)[i + 1][j - 1].b;
	}
	if (i + 1 < height) {
		*red += 2 * (*applied_to)[i + 1][j].r;
		*green += 2 * (*applied_to)[i + 1][j].g;
		*blue += 2 * (*applied_to)[i + 1][j].b;
	}
	if (i + 1 < height && j + 1 < width) {
		*red += (*applied_to)[i + 1][j + 1].r;
		*green += (*applied_to)[i + 1][j + 1].g;
		*blue += (*applied_to)[i + 1][j + 1].b;
	}
}

// Rezolva "APPLY GAUSSIAN_BLUR"
int apply_gaussian_blur(pixel_t ***applied_to, const int start_w,
						const int start_h, const int end_w, const int end_h,
						const int height, const int width)
{
	pixel_t **filter_apply = NULL;
	rgb_matrix_alloc(&filter_apply, height, width);
	if (!filter_apply) {
		return -1;
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			double red = 0, green = 0, blue = 0;
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			gaussian_blur_values(&red, &green, &blue, i, j, height,
								 width, applied_to);
			red += 4 * (*applied_to)[i][j].r;
			green += 4 * (*applied_to)[i][j].g;
			blue += 4 * (*applied_to)[i][j].b;
			red /= 16;
			green /= 16;
			blue /= 16;
			filter_apply[i][j].r = (int)red;
			filter_apply[i][j].g = (int)green;
			filter_apply[i][j].b = (int)blue;
			filter_apply[i][j].r = filter_apply[i][j].r < 0 ?
								   0 : filter_apply[i][j].r;
			filter_apply[i][j].r = filter_apply[i][j].r > 255 ?
								   255 : filter_apply[i][j].r;
			filter_apply[i][j].g = filter_apply[i][j].g < 0 ?
								   0 : filter_apply[i][j].g;
			filter_apply[i][j].g = filter_apply[i][j].g > 255 ?
								   255 : filter_apply[i][j].g;
			filter_apply[i][j].b = filter_apply[i][j].b < 0 ?
								   0 : filter_apply[i][j].b;
			filter_apply[i][j].b = filter_apply[i][j].b > 255 ?
								   255 : filter_apply[i][j].b;
		}
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			check_border(&i, &j);
			if (i == height - 1) {
				break;
			}
			if (j == width - 1) {
				break;
			}
			(*applied_to)[i][j].r = filter_apply[i][j].r;
			(*applied_to)[i][j].g = filter_apply[i][j].g;
			(*applied_to)[i][j].b = filter_apply[i][j].b;
		}
	}
	free_matrix_p(&filter_apply, height);
	printf("APPLY GAUSSIAN_BLUR done\n");
	return 0;
}

// Rezolva comanda "APPLY" in functie de ce parametrul primeste
int handle_apply(pixel_t ***applied_to, const int image_type,
				 const int start_w, const int start_h, const int end_w,
				 const int end_h, const int height, const int width,
				 const int loaded)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return -1;
	}
	char *filter = strtok(NULL, "\n ");
	if (!filter) {
		printf("Invalid command\n");
		return -1;
	}
	int good_alloc = 0;
	if (image_type == 3 || image_type == 6) {
		if (strcmp(filter, "EDGE") == 0) {
			good_alloc = apply_edge(applied_to, start_w, start_h, end_w,
									end_h, height, width);
		} else if (strcmp(filter, "SHARPEN") == 0) {
			good_alloc = apply_sharpen(applied_to, start_w, start_h, end_w,
									   end_h, height, width);
		} else if (strcmp(filter, "BLUR") == 0) {
			good_alloc = apply_blur(applied_to, start_w, start_h, end_w,
									end_h, height, width);
		} else if (strcmp(filter, "GAUSSIAN_BLUR") == 0) {
			good_alloc = apply_gaussian_blur(applied_to, start_w, start_h,
											 end_w, end_h, height, width);
		} else {
			printf("APPLY parameter invalid\n");
			return -1;
		}
	} else {
		printf("Easy, Charlie Chaplin\n");
		return -1;
	}
	return good_alloc;
}

// Roteste patratul cu coordonatele date de "SELECT" la 90 de grade
int rotate_90_rgb_square(pixel_t ***applied, int *start_w, int *start_h,
						 int *end_w, int *end_h)
{
	pixel_t **rotated_image = NULL;
	rgb_matrix_alloc(&rotated_image, (*end_w - *start_w), (*end_h - *start_h));
	if (!rotated_image) {
		return -1;
	}
	for (int i = *start_h; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			rotated_image[j - *start_w][*end_h - i - 1].r = (*applied)[i][j].r;
			rotated_image[j - *start_w][*end_h - i - 1].g = (*applied)[i][j].g;
			rotated_image[j - *start_w][*end_h - i - 1].b = (*applied)[i][j].b;
		}
	}
	for (int i = *start_h; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			(*applied)[i][j].r = rotated_image[i - *start_h][j - *start_w].r;
			(*applied)[i][j].g = rotated_image[i - *start_h][j - *start_w].g;
			(*applied)[i][j].b = rotated_image[i - *start_h][j - *start_w].b;
		}
	}
	free_matrix_p(&rotated_image, (*end_w - *start_w));
	return 0;
}

int rotate_90_square(int ***applied_to, int *start_w, int *start_h,
					 int *end_w, int *end_h)
{
	int **rotated_image = NULL;
	int_matrix_alloc(&rotated_image, (*end_w - *start_w), (*end_h - *start_h));
	if (!rotated_image) {
		return -1;
	}
	for (int i = *start_h; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			rotated_image[j - *start_w][*end_h - i - 1] = (*applied_to)[i][j];
		}
	}
	for (int i = *start_h; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			(*applied_to)[i][j] = rotated_image[i - *start_h][j - *start_w];
		}
	}
	free_matrix_int(&rotated_image, (*end_w - *start_w));
	return 0;
}

// Roteste toata imaginea la 90 de grade, realoca imaginea datorita
// inversiunii dintre inaltime si latime
int rotate_90_all(int ***applied_to, int *width, int *height)
{
	int **rotated_image = NULL;
	int_matrix_alloc(&rotated_image, *width, *height);
	if (!rotated_image) {
		return -1;
	}
	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			rotated_image[j][*height - i - 1] = (*applied_to)[i][j];
		}
	}
	free_matrix_int(applied_to, *height);
	int_matrix_alloc(applied_to, *width, *height);
	if (!(*applied_to)) {
		return -1;
	}
	for (int i = 0; i < *width; i++) {
		for (int j = 0; j < *height; j++) {
			(*applied_to)[i][j] = rotated_image[i][j];
		}
	}
	free_matrix_int(&rotated_image, *width);
	int aux = *height;
	*height = *width;
	*width = aux;
	return 0;
}

int rotate_90_rgb_all(pixel_t ***applied_to, int *width, int *height)
{
	pixel_t **rotated_image = NULL;
	rgb_matrix_alloc(&rotated_image, *width, *height);
	if (!rotated_image) {
		return -1;
	}
	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			rotated_image[j][*height - i - 1].r = (*applied_to)[i][j].r;
			rotated_image[j][*height - i - 1].g = (*applied_to)[i][j].g;
			rotated_image[j][*height - i - 1].b = (*applied_to)[i][j].b;
		}
	}
	free_matrix_p(applied_to, *height);
	rgb_matrix_alloc(applied_to, *width, *height);
	if (!(*applied_to)) {
		return -1;
	}
	for (int i = 0; i < *width; i++) {
		for (int j = 0; j < *height; j++) {
			(*applied_to)[i][j].r = rotated_image[i][j].r;
			(*applied_to)[i][j].g = rotated_image[i][j].g;
			(*applied_to)[i][j].b = rotated_image[i][j].b;
		}
	}
	free_matrix_p(&rotated_image, *width);
	int aux = *height;
	*height = *width;
	*width = aux;
	return 0;
}

// Roteste la 180 de grade o imagine
int rotate_180_rgb(pixel_t ***applied, int start_w, int start_h, int end_w,
				   int end_h)
{
	pixel_t **rotated_image = NULL;
	rgb_matrix_alloc(&rotated_image, (end_h - start_h), (end_w - start_w));
	if (!rotated_image) {
		return -1;
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			rotated_image[end_h - i - 1][end_w - j - 1].r = (*applied)[i][j].r;
			rotated_image[end_h - i - 1][end_w - j - 1].g = (*applied)[i][j].g;
			rotated_image[end_h - i - 1][end_w - j - 1].b = (*applied)[i][j].b;
		}
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			(*applied)[i][j].r = rotated_image[i - start_h][j - start_w].r;
			(*applied)[i][j].g = rotated_image[i - start_h][j - start_w].g;
			(*applied)[i][j].b = rotated_image[i - start_h][j - start_w].b;
		}
	}
	free_matrix_p(&rotated_image, (end_h - start_h));
	return 0;
}

int rotate_180(int ***applied_to, int start_w, int start_h, int end_w,
			   int end_h)
{
	int **rotated_image = NULL;
	int_matrix_alloc(&rotated_image, (end_h - start_h), (end_w - start_w));
	if (!rotated_image) {
		return -1;
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			rotated_image[end_h - i - 1][end_w - j - 1] = (*applied_to)[i][j];
		}
	}
	for (int i = start_h; i < end_h; i++) {
		for (int j = start_w; j < end_w; j++) {
			(*applied_to)[i][j] = rotated_image[i - start_h][j - start_w];
		}
	}
	free_matrix_int(&rotated_image, (end_h - start_h));
	return 0;
}

// Roteste la 270 de grade patratul dat de coordonatele din apelul "SELECT"
int rotate_270_rgb_square(pixel_t ***applied, int *start_w, int *start_h,
						  int *end_w, int *end_h)
{
	pixel_t **rotated_image = NULL;
	rgb_matrix_alloc(&rotated_image, (*end_w - *start_w), (*end_h - *start_h));
	if (!rotated_image) {
		return -1;
	}
	for (int i = *start_h; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			rotated_image[*end_w - j - 1][i - *start_h].r = (*applied)[i][j].r;
			rotated_image[*end_w - j - 1][i - *start_h].g = (*applied)[i][j].g;
			rotated_image[*end_w - j - 1][i - *start_h].b = (*applied)[i][j].b;
		}
	}
	for (int i = *start_h; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			(*applied)[i][j].r = rotated_image[i - *start_h][j - *start_w].r;
			(*applied)[i][j].g = rotated_image[i - *start_h][j - *start_w].g;
			(*applied)[i][j].b = rotated_image[i - *start_h][j - *start_w].b;
		}
	}
	free_matrix_p(&rotated_image, (*end_w - *start_w));
	return 0;
}

int rotate_270_square(int ***applied_to, int *start_w, int *start_h,
					  int *end_w, int *end_h)
{
	int **rotated_image = NULL;
	int_matrix_alloc(&rotated_image, (*end_w - *start_w), (*end_h - *start_h));
	if (!rotated_image) {
		return -1;
	}
	for (int i = *start_h ; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			rotated_image[*end_w - j - 1][i - *start_h] = (*applied_to)[i][j];
		}
	}
	for (int i = *start_h; i < *end_h; i++) {
		for (int j = *start_w; j < *end_w; j++) {
			(*applied_to)[i][j] = rotated_image[i - *start_h][j - *start_w];
		}
	}
	free_matrix_int(&rotated_image, (*end_w - *start_w));
	return 0;
}

// Roteste la 270 de grade toata imaginea, realoca imaginea
int rotate_270_rgb_all(pixel_t ***applied_to, int *width, int *height)
{
	pixel_t **rotated_image = NULL;
	rgb_matrix_alloc(&rotated_image, *width, *height);
	if (!rotated_image) {
		return -1;
	}
	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			rotated_image[*width - j - 1][i].r = (*applied_to)[i][j].r;
			rotated_image[*width - j - 1][i].g = (*applied_to)[i][j].g;
			rotated_image[*width - j - 1][i].b = (*applied_to)[i][j].b;
		}
	}
	free_matrix_p(applied_to, *height);
	rgb_matrix_alloc(applied_to, *width, *height);
	for (int i = 0; i < *width; i++) {
		for (int j = 0; j < *height; j++) {
			(*applied_to)[i][j].r = rotated_image[i][j].r;
			(*applied_to)[i][j].g = rotated_image[i][j].g;
			(*applied_to)[i][j].b = rotated_image[i][j].b;
		}
	}
	free_matrix_p(&rotated_image, *width);
	int aux = *height;
	*height = *width;
	*width = aux;
	return 0;
}

int rotate_270_all(int ***applied_to, int *width, int *height)
{
	int **rotated_image = NULL;
	int_matrix_alloc(&rotated_image, *width, *height);
	if (!rotated_image) {
		return -1;
	}
	for (int i = 0; i < *height; i++) {
		for (int j = 0; j < *width; j++) {
			rotated_image[*width - j - 1][i] = (*applied_to)[i][j];
		}
	}
	free_matrix_int(applied_to, *height);
	int_matrix_alloc(applied_to, *width, *height);
	for (int i = 0; i < *width; i++) {
		for (int j = 0; j < *height; j++) {
			(*applied_to)[i][j] = rotated_image[i][j];
		}
	}
	free_matrix_int(&rotated_image, *width);
	int aux = *height;
	*height = *width;
	*width = aux;
	return 0;
}

// Rezolva comanda "ROTATE" in functie de parametrii primiti
int handle_rotate(pixel_t ***rgb_applied_to, int ***applied_to, int *height,
				  int *width, int *start_w, int *start_h, int *end_w,
				  int *end_h, int loaded, int sel_all, int image_type)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return -1;
	}
	if ((*end_h - *start_h != *end_w - *start_w) && sel_all == 0) {
		printf("The selection must be square\n");
		return -1;
	}
	char *angle = strtok(NULL, "\n ");
	int rotation_angle = atoi(angle);
	int original_angle = rotation_angle;
	int good_alloc = 0;
	if (rotation_angle < 0) {
		rotation_angle = (rotation_angle % 360 + 360) % 360;
	}
	if (original_angle % 90 != 0 || original_angle > 360 ||
		original_angle < -360) {
		printf("Unsupported rotation angle\n");
		return -1;
	}
	if (sel_all == 0) {
		if (image_type == 3 || image_type == 6) {
			if (rotation_angle == 90) {
				good_alloc = rotate_90_rgb_square(rgb_applied_to, start_w,
												  start_h, end_w, end_h);
			} else if (rotation_angle == 180) {
				good_alloc = rotate_180_rgb(rgb_applied_to, *start_w,
											*start_h, *end_w, *end_h);
			} else if (rotation_angle == 270) {
				good_alloc = rotate_270_rgb_square(rgb_applied_to, start_w,
												   start_h, end_w, end_h);
			}
		} else {
			if (rotation_angle == 90) {
				good_alloc = rotate_90_square(applied_to, start_w, start_h,
											  end_w, end_h);
			} else if (rotation_angle == 180) {
				good_alloc = rotate_180(applied_to, *start_w, *start_h,
										*end_w, *end_h);
			} else if (rotation_angle == 270) {
				good_alloc = rotate_270_square(applied_to, start_w,
											   start_h, end_w, end_h);
			}
		}
	} else {
		if (image_type == 3 || image_type == 6) {
			if (rotation_angle == 90) {
				good_alloc = rotate_90_rgb_all(rgb_applied_to, width,
											   height);
			} else if (rotation_angle == 180) {
				good_alloc = rotate_180_rgb(rgb_applied_to, 0, 0, *width,
											*height);
			} else if (rotation_angle == 270) {
				good_alloc = rotate_270_rgb_all(rgb_applied_to, width,
												height);
			}
		} else {
			if (rotation_angle == 90) {
				good_alloc = rotate_90_all(applied_to, width, height);
			} else if (rotation_angle == 180) {
				good_alloc = rotate_180(applied_to, 0, 0, *width,
										*height);
			} else if (rotation_angle == 270) {
				good_alloc = rotate_270_all(applied_to, width, height);
			}
		}
		int aux = *start_h;
		*start_h = *start_w;
		*start_w = aux;
		aux = *end_h;
		*end_h = *end_w;
		*end_w = aux;
	}
	printf("Rotated %d\n", original_angle);
	return good_alloc;
}

int check_power_two(int number)
{
	while (number % 2 == 0) {
		number /= 2;
	}
	return number;
}

// Calculeaza si afiseaza histograma unei imagini alb-negru
int handle_histogram(int **image_in_use, const int height,
					 const int width, const int loaded, const int image_type)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return -1;
	}
	char *values = strtok(NULL, "\n ");
	if (!values) {
		printf("Invalid command\n");
		return -1;
	}
	int asterisk_number = atoi(values);
	values = strtok(NULL, "\n ");
	if (!values) {
		printf("Invalid command\n");
		return -1;
	}
	int bin_number = atoi(values);
	values = strtok(NULL, "\n ");
	if (values) {
		printf("Invalid command\n");
		return -1;
	}
	if (image_type == 3 || image_type == 6) {
		printf("Black and white image needed\n");
		return -1;
	}
	if (asterisk_number == 0) {
		printf("Invalid set of parameters\n");
		return -1;
	}
	if (bin_number > 256 || bin_number < 2) {
		printf("Invalid set of parameters\n");
		return -1;
	}
	if (check_power_two(bin_number) != 1) {
		printf("Invalid set of parameters\n");
		return -1;
	}
	int *freq_histogram = calloc(256, sizeof(int));
	int max_freq = 0;
	int *bin_asterisk_count = calloc(bin_number, sizeof(int));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			freq_histogram[image_in_use[i][j]]++;
		}
	}
	int index = 0;
	for (int i = 0; i < 256; i += (256 / bin_number)) {
		for (int j = i; j < i + (256 / bin_number); j++) {
			bin_asterisk_count[index] += freq_histogram[j];
		}
		index++;
	}
	for (int i = 0; i < bin_number; i++) {
		if (bin_asterisk_count[i] > max_freq) {
			max_freq = bin_asterisk_count[i];
		}
	}
	for (int i = 0; i < bin_number; i++) {
		double asterisk_count = bin_asterisk_count[i] * asterisk_number /
								max_freq;
		asterisk_count = floor(asterisk_count);
		bin_asterisk_count[i] = (int)asterisk_count;
	}
	for (int i = 0; i < bin_number; i++) {
		printf("%d\t|\t", bin_asterisk_count[i]);
		for (int j = 0; j < bin_asterisk_count[i]; j++) {
			printf("*");
		}
		printf("\n");
	}
	free(bin_asterisk_count);
	free(freq_histogram);
	return 0;
}

// Rezolva comanda "EQUALIZE" pentru o imagine alb-negru
int handle_equalize(const int loaded, const int image_type, const int height,
					const int width, int ***image_in_use)
{
	if (loaded == 0) {
		printf("No image loaded\n");
		return -1;
	}
	if (image_type == 3 || image_type == 6) {
		printf("Black and white image needed\n");
		return -1;
	}
	int *freq_histogram = calloc(256, sizeof(int));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			freq_histogram[(*image_in_use)[i][j]]++;
		}
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			double new_pixel_val = 0;
			for (int k = 0; k <= (*image_in_use)[i][j]; k++) {
				new_pixel_val += freq_histogram[k];
			}
			new_pixel_val /= (height * width);
			new_pixel_val *= 255;
			new_pixel_val = new_pixel_val < 0 ? 0 : new_pixel_val;
			new_pixel_val = new_pixel_val > 255 ? 255 : new_pixel_val;
			new_pixel_val = round(new_pixel_val);
			(*image_in_use)[i][j] = (int)new_pixel_val;
		}
	}
	free(freq_histogram);
	printf("Equalize done\n");
	return 0;
}

// Rezolva comanda "EXIT", elibereaza memoria alocata
void handle_exit(const int loaded, FILE **image, int ***image_in_use,
				 pixel_t ***rgb_image_in_use, const int height)
{
	if (loaded == 0) {
		printf("No image loaded\n");
	}
	if (*image) {
		fclose(*image);
		*image = NULL;
	}
	if (*image_in_use) {
		for (int i = 0; i < height; i++) {
			free((*image_in_use)[i]);
			(*image_in_use)[i] = NULL;
		}
		free(*image_in_use);
		*image_in_use = NULL;
	}
	if (*rgb_image_in_use) {
		for (int i = 0; i < height; i++) {
			free((*rgb_image_in_use)[i]);
			(*rgb_image_in_use)[i] = NULL;
		}
		free(*rgb_image_in_use);
		*rgb_image_in_use = NULL;
	}
}

int main(void)
{
	char command[name_max], buffer[buffer_max];
	FILE *image = NULL;
	int height = 0, width = 0, binary = 0, image_type = 0, startheight = 0;
	int startwidth = 0, endheight = 0, endwidth = 0, max_intensity = 0;
	int stselheight, stselwidth, endselheight, endselwidth, loaded = 0;
	int **image_in_use = NULL, sel_all = 1;
	pixel_t **rgb_image_in_use = NULL;

	while (1) {
		fgets(buffer, 256, stdin);
		char *token = strtok(buffer, "\n ");
		if (!token) {
			return 0;
		}
		strcpy(command, token);
		if (strcmp(command, "LOAD") == 0) {
			if (handle_load_command(&image_type, &image, &binary,
									&image_in_use, &rgb_image_in_use,
									&loaded, &height, &width,
									&max_intensity, &startheight,
									&startwidth, &endheight, &endwidth) == -1) {
				continue;
			}
		} else if (strcmp(command, "SELECT") == 0) {
			if (handle_select(loaded, &sel_all, &startwidth,
							  &startheight, &endwidth, &endheight,
							  width, height, &stselheight, &stselwidth,
							  &endselheight, &endselwidth) == -1) {
				continue;
			}
		} else if (strcmp(command, "CROP") == 0) {
			if (handle_crop(image_type, &rgb_image_in_use,
							&image_in_use, &height, &width,
							&startheight, &startwidth, &endheight,
							&endwidth, &loaded, &sel_all) == -1) {
				continue;
		    }
		} else if (strcmp(command, "EXIT") == 0) {
			handle_exit(loaded, &image, &image_in_use, &rgb_image_in_use,
						height);
			return 0;
		} else if (strcmp(command, "SAVE") == 0) {
			if (handle_save(&image_type, width, height, max_intensity,
							rgb_image_in_use, image_in_use, loaded) == -1) {
				continue;
			}
		} else if (strcmp(command, "APPLY") == 0) {
			if (handle_apply(&rgb_image_in_use, image_type, startwidth,
							 startheight, endwidth, endheight, height,
							 width, loaded) == -1) {
				continue;
			}
		} else if (strcmp(command, "HISTOGRAM") == 0) {
			if (handle_histogram(image_in_use, height, width,
								 loaded, image_type) == -1) {
				continue;
			}
		} else if (strcmp(command, "EQUALIZE") == 0) {
			if (handle_equalize(loaded, image_type, height, width,
								&image_in_use) == -1) {
				continue;
			}
		} else if (strcmp(command, "ROTATE") == 0) {
			if (handle_rotate(&rgb_image_in_use, &image_in_use, &height,
							  &width, &startwidth, &startheight,
							  &endwidth, &endheight, loaded, sel_all,
							  image_type) == -1) {
				continue;
			}
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
