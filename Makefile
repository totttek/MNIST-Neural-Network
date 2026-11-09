SAVE_FILE = mnist.model

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -O2 -g
LDLIBS  = -lm

TRN_DIR   = training_src
MNIST_DIR = mnist_src
LIB_DIR   = libmodel
DATA_DIR  = data

TRN     = trainer
LIB     = $(LIB_DIR)/libmodel.a
LIB_HDR = $(LIB_DIR)/model.h $(LIB_DIR)/activation.h $(LIB_DIR)/model_trainer.h
HDR     = $(MNIST_DIR)/idx_reader.h $(LIB_HDR)
INC     = -I$(MNIST_DIR) -I$(LIB_DIR)

TRN_OBJ = $(TRN_DIR)/training.o $(MNIST_DIR)/idx_reader.o

# Argument order the trainer expects: train images, train labels, test images, test labels
DATA    = $(DATA_DIR)/train-images-idx3-ubyte $(DATA_DIR)/train-labels-idx1-ubyte \
          $(DATA_DIR)/t10k-images-idx3-ubyte $(DATA_DIR)/t10k-labels-idx1-ubyte

all: $(TRN)

$(TRN): $(TRN_OBJ) $(LIB)
	$(CC) $(CFLAGS) $(TRN_OBJ) -L$(LIB_DIR) -lmodel -o $@ $(LDLIBS)

# The library has its own Makefile; let it decide what needs rebuilding.
$(LIB): FORCE
	$(MAKE) -C $(LIB_DIR)

FORCE:

$(TRN_DIR)/%.o: $(TRN_DIR)/%.c $(HDR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(MNIST_DIR)/%.o: $(MNIST_DIR)/%.c $(MNIST_DIR)/idx_reader.h
	$(CC) $(CFLAGS) -I$(MNIST_DIR) -c $< -o $@

# Builds the trainer and runs it on the MNIST files in data/
run: $(TRN)
	./$(TRN) $(DATA)

save_weights: $(TRN)
	./$(TRN) $(DATA) $(SAVE_FILE)

clean:
	$(MAKE) -C $(LIB_DIR) clean
	rm -rf $(TRN_OBJ)

fclean: clean
	$(MAKE) -C $(LIB_DIR) fclean
	rm -rf $(TRN)

re: fclean all

.PHONY: all run clean fclean re FORCE save_weights
