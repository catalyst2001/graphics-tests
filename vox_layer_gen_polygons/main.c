#include <stdio.h>
#include <stdbool.h>

int layer_array[][10] = {
	{ 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 0, 0, 0, 1, 1 },
	{ 1, 0, 1, 1, 0, 0, 0, 0, 1, 1 },
	{ 0, 0, 0, 1, 0, 0, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
	{ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
	{ 0, 1, 1, 0, 1, 1, 0, 0, 0, 0 }
};

void print_layer()
{
	for (int i = 0; i < sizeof(layer_array) / sizeof(layer_array[0]); i++) {
		for (int j = 0; j < sizeof(layer_array[0]) / sizeof(layer_array[0][0]); j++)
			printf("%c ", layer_array[i][j] ? '#' : '.');

		printf("\n");
	}
}

void count_regions()
{
	int num_of_regions = 0;
	bool voxs_closely_x = true;
	bool voxs_closely_y = true;

	/* find min coord */
	int xmin, ymin;
	for (int y = 0; y < sizeof(layer_array) / sizeof(layer_array[0]); y++) {
		for (int x = 0; x < sizeof(layer_array[0]) / sizeof(layer_array[0][0]); x++) {
			if (layer_array[x][y]) {
				xmin = x;
				ymin = y;
				break;
			}
		}
	}

	/* traver from min coords to max */
	for (int y = ymin; y < sizeof(layer_array) / sizeof(layer_array[0]); y++) {
		for (int x = xmin; x < sizeof(layer_array[0]) / sizeof(layer_array[0][0]); x++) {
			if (layer_array[x][y]) {

			}
		}
	}
	printf("Number of regions in layer: %d\n", num_of_regions);
}

int main()
{
	print_layer();
	return 0;
}