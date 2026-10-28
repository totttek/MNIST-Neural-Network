#include "idx_reader.h"

static int	read_be_u32(FILE *file, uint32_t *out)
{
	unsigned char	b[4];

	if (fread(b, 1, 4, file) != 4)
		return (-1);

	*out = (uint32_t)b[0] << 24 | (uint32_t)b[1] << 16 | (uint32_t)b[2] << 8 | (uint32_t)b[3];
	return (0);
}

static int	read_images(char *filename, t_mnist *mnist)
{
	uint32_t	header[4];

	FILE *file = fopen(filename, "rb");
	if (!file)
		return (-1);

	if (read_be_u32(file, &header[0]) != 0 ||
			read_be_u32(file, &header[1]) != 0 ||
			read_be_u32(file, &header[2]) != 0 ||
			read_be_u32(file, &header[3]) != 0)
		return(fclose(file), -1);

	if (header[0] != 2051)
		return (fclose(file), -1);
	
	mnist->image_count = header[1];
	mnist->rows = header[2];
	mnist->cols = header[3];
	mnist->ppi = header[2] * header[3];
	mnist->images = calloc(mnist->image_count * mnist->ppi, sizeof(unsigned char));
	if (!mnist->images)
		return (fclose(file), -1);
	
	if (fread(mnist->images, mnist->ppi, mnist->image_count, file) != mnist->image_count)
		return (fclose(file), -1);

	return (fclose(file), 0);
}

static int	read_lables(char *filename, t_mnist *mnist)
{
	uint32_t	header[2];

	FILE *file = fopen(filename, "rb");
	if (!file)
		return (-1);

	if (read_be_u32(file, &header[0]) != 0 ||
			read_be_u32(file, &header[1]) != 0)
		return(fclose(file), -1);
	
	if (header[0] != 2049)
		return (fclose(file), -1);

	if (header[1] != mnist->image_count)
		return (fclose(file), -1);

	mnist->labels = calloc(mnist->image_count, sizeof(unsigned char));
	if (!mnist->labels)
		return (fclose(file), -1);

	if (fread(mnist->labels, 1, mnist->image_count, file) != mnist->image_count)
		return (fclose(file), -1);

	for (size_t i = 0; i < mnist->image_count; i++)
		if (mnist->labels[i] > 9)
			return (fclose(file), -1);
	
	return (fclose(file), 0);
}

void	mnist_free(t_mnist *mnist)
{
	if (!mnist)
		return ;
	free(mnist->images);
	free(mnist->labels);
	free(mnist);
}

t_mnist	*mnist_load(char *images_path, char *labels_path)
{
	t_mnist	*mnist = calloc(1, sizeof(t_mnist));

	if(!mnist)
		return (NULL);

	if (read_images(images_path, mnist) != 0)
		return (mnist_free(mnist), NULL);

	if (read_lables(labels_path, mnist) != 0)
		return (mnist_free(mnist), NULL);
	
	return (mnist);
}

int	mnist_get_image(t_mnist *mnist, size_t index, double *out)
{
	if (index >= mnist->image_count)
		return (-1);

	for (size_t i = 0; i < mnist->ppi; i++)
	{
		out[i] = (double)mnist->images[index * mnist->ppi + i] / 255.0;
	}

	return(0);
}

void	mnist_print_image(t_mnist *mnist, size_t index)
{
	char	map[] = " .:-=+*#%@";

	if (index >= mnist->image_count)
		return ;

	for (size_t i = 0; i < mnist->rows; i++)
	{
		for (size_t j = 0; j < mnist->cols; j++) 
		{
			printf("%c", map[mnist->images[index * mnist->ppi + i * mnist->cols + j] * 9 / 255]);
		}
		printf("\n");
	}
}

int	main(int argc, char **argv)
{
	(void)argc;
	t_mnist *mnist = NULL;

	mnist = mnist_load(argv[1], argv[2]);
	mnist_print_image(mnist, 0);
}
