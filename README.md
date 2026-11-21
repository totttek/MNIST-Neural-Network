# MNIST Neural Network

A handwritten digit classifier trained from scratch in C, on the MNIST dataset, using my own neural network library [libmodel](https://github.com/totttek/libmodel) (included as a submodule). No ML frameworks: the IDX loader, the training loop, backpropagation, softmax and cross-entropy are all plain C.

**Best result: 98.18% test accuracy** with a 784 → 128 → 10 network after 10 epochs (about 6 seconds per epoch on a laptop CPU).

## Model and training

- **Network:** 784 inputs (28×28 pixels scaled to 0–1) → 128 sigmoid neurons → 10 softmax outputs, one per digit
- **Loss:** categorical cross-entropy against a one-hot target
- **Init:** Xavier (Glorot), computed per layer
- **Optimiser:** plain stochastic gradient descent, one image at a time, in a freshly shuffled order every epoch (Fisher–Yates over an index array, so images and labels can't drift apart)
- **Learning rate:** multiplied by 0.96 after every epoch
- **Prediction:** the index of the largest output (argmax)

## Results

| Output + loss | Seed | Learning rate | Epochs | Test accuracy |
|---|---|---|---|---|
| sigmoid + binary cross-entropy | 1234567890 | 0.1 | 5 | 97.74% |
| softmax + cross-entropy | 1234567890 | 0.1 | 5 | 97.49% |
| softmax + cross-entropy | 1234 | 0.05 | 10 | **98.18%** |
| softmax + cross-entropy | 1234 | 0.1 | 10 | 98.17% |
| softmax + cross-entropy | 1234567890 | 0.05 | 10 | 97.97% |

What these runs showed:

- At 10 epochs the learning rate (0.05 vs 0.1) barely matters; changing the seed moves the result by about 0.2%. Differences smaller than that between single runs are noise, so the sigmoid vs softmax comparison above is inconclusive.
- After about 5 epochs the training loss keeps falling while test accuracy levels off around 98%: the start of overfitting.
- Test accuracy is printed after every epoch. A separate validation split (the last 10k training images) is the next step, so that settings can be tuned without looking at the test set.

The trained models from these runs are included as `mnist_<output>_<epochs>_<seed>_<lr>.model` (for example `mnist_softmax_10_1234_005.model`), in libmodel's text model format.

## Build and run

```sh
git clone --recurse-submodules https://github.com/totttek/MNISTAI.git
cd MNISTAI
make
```

The MNIST files are not included. Put the four original IDX files in `data/`:

```
data/train-images-idx3-ubyte
data/train-labels-idx1-ubyte
data/t10k-images-idx3-ubyte
data/t10k-labels-idx1-ubyte
```

Then:

```sh
make run            # train and print loss and test accuracy per epoch
make save_weights   # same, then save the trained model to mnist.model
```

or directly:

```sh
./trainer <train-images> <train-labels> <test-images> <test-labels> [model file]
```

The seed, number of epochs and learning rate are set at the top of `main` in `training_src/training.c`, and printed at the start of every run.

## Files

| Path | Contents |
|---|---|
| `mnist_src/idx_reader.c` / `.h` | IDX file loader: checks the magic numbers (2051 for images, 2049 for labels), reads the big-endian headers, keeps pixels as bytes and converts one image at a time to doubles in 0–1 |
| `training_src/training.c` | the trainer: shuffling, one epoch of training, test accuracy, and `main` |
| `libmodel/` | the neural network library (submodule) |

## AI use

The C code (the IDX loader and the trainer) is my own. I used Claude (Anthropic's AI assistant) for:

- Explanations (softmax, cross-entropy, weight initialisation, validation sets) and code reviews that pointed me toward bugs instead of fixing them
- Writing this README and the Makefile
