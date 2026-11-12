# include "model_trainer.h"
# include "idx_reader.h"

static size_t	argmax(double *array, size_t size)
{
	size_t	index = 0;

	for (size_t i = 0; i < size; i++)
	{
		if (array[index] < array[i])
			index = i;
	}
	return (index);
}

double	test_model(t_model *model, t_mnist *testset)
{
	int		correct = 0;
	double	accuracy = 0;
	double	*image = NULL;
	double	*result = NULL;

	image = calloc(testset->ppi, sizeof(double));
	if (!image)
		return (-1);
	for (size_t i = 0; i < testset->image_count; i++)
	{
		if (mnist_get_image(testset, i, image) != 0)
			return (free(image), -1);
		result = model_forward(model, image);
		if (!result)
			return (free(image), -1);

		if (argmax(result, 10) == testset->labels[i])
			correct++;

		free(result);
	}
	free (image);
	accuracy = (double)correct/(double)testset->image_count	;
	printf("Test: accuracy - %lf\n", accuracy * 100.0);
	return (accuracy);
}

size_t	*generate_indexes(t_mnist *dataset)
{
	size_t	*elements = NULL;
	size_t	j = 0;
	size_t	tmp = 0;

	if (dataset->image_count == 0)
		return (NULL);

	elements = calloc(dataset->image_count, sizeof(size_t));
	if (!elements)
		return (NULL);

	for (size_t i = 0; i < dataset->image_count; i++)
		elements[i] = i;

	for (size_t i = dataset->image_count - 1; i > 0; --i)
	{
		j = rand() % (i + 1);
		tmp = elements[i];
		elements[i] = elements[j];
		elements[j] = tmp;
	}
	return (elements);
}

double	learn_epoch(t_trainer *trainer, t_mnist *dataset, double lr)
{
	double	total_loss = 0;
	double	*image = NULL;
	double	*target = NULL;
	size_t	*indexes = NULL;

	image = calloc(dataset->ppi, sizeof(double));
	if (!image)
		return (-1);
	target = calloc(trainer->model->layers[trainer->model->layer_count - 1].l_neurons, sizeof(double));
	if (!target)
		return(free(image), -1);
	indexes = generate_indexes(dataset);
	if (!indexes)
		return(free(target), free(image), -1);
	for (size_t i = 0; i < dataset->image_count; i++)
	{
		if (mnist_get_image(dataset, indexes[i], image) != 0)
			return(free(target), free(image), free(indexes), -1);
		if (trainer_forward(trainer, image) != 0)
			return(free(target), free(image), free(indexes), -1);
		target[dataset->labels[indexes[i]]] = 1;
		total_loss += trainer_loss(trainer, target);
		if (trainer_backward(trainer, target))
			return(free(target), free(image), free(indexes), -1);
		trainer_update(trainer, lr);
		target[dataset->labels[indexes[i]]] = 0;
	}
	return (free(target), free(image), free(indexes), total_loss/dataset->image_count);
}

int	train_model(t_trainer *trainer, t_mnist *dataset, t_mnist *testset,
	int epochs, double lr)
{
	double	avrage_loss = 0;

	for	(int i = 1; i <= epochs; i++)
	{
		avrage_loss = learn_epoch(trainer, dataset, lr);
		if (avrage_loss < 0)
			return (-1);
		printf("Epoch %d: avrage loss - %lf\n", i, avrage_loss);
		if (i % 1 == 0)
		{
			test_model(trainer->model, testset);
		}
		lr *= 0.96;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_mnist		*dataset = NULL;
	t_mnist		*testset = NULL;
	t_model		*model = NULL;
	t_trainer	*trainer = NULL;
	unsigned int	seed = 1234567890;
	int				epochs = 10;
	double			lr = 0.05;

	srand(seed);
	if (argc != 5 && argc != 6)
	{
		printf("Usage: ./trainer <dataset-images> <dataset-lables> <testset-images> <dataset-lables> [model filename]");
		return (-1);
	}
	testset = mnist_load(argv[3], argv[4]);
	if (!testset || testset->image_count == 0)
		return (-1);
	dataset = mnist_load(argv[1], argv[2]);
	if (!dataset || dataset->image_count == 0)
		return (mnist_free(testset), -1);
	model = model_create((size_t[]){784, 128, 10}, 3);
	if (!model)
		return (mnist_free(testset), mnist_free(dataset), -1);
	model_apply_activation(model, 0, ACT_SIGMOID);
	model_apply_activation(model, 1, ACT_SOFTMAX);

	model_init_random(model);

	trainer = trainer_create(model, LOSS_CROSS_ENTROPY);
	if(!trainer)
		return (model_free(model), mnist_free(testset), mnist_free(dataset), -1);

	printf("Run Stats: seed - %u, epochs - %d, lr - %f\n", seed, epochs, lr);
	if (train_model(trainer, dataset, testset, epochs, lr) != 0)
		return (trainer_free(trainer), model_free(model), mnist_free(testset), mnist_free(dataset), -1);
	if (argc == 6)
		model_save(model, argv[5]);
	trainer_free(trainer);
	model_free(model);
	mnist_free(testset);
	mnist_free(dataset);
	return (0);
}
