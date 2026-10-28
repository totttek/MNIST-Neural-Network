#ifndef IDX_READER_H
# define IDX_READER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct	s_mnist
{
	unsigned char	*images;
	unsigned char	*labels;
	size_t			ppi;
	size_t			image_count;
	size_t			rows;
	size_t			cols;
}				t_mnist;

void	mnist_free(t_mnist *mnist);
t_mnist	*mnist_load(char *images_path, char *labels_path);
int		mnist_get_image(t_mnist *mnist, size_t index, double *out);
void	mnist_print_image(t_mnist *mnist, size_t index);

#endif
