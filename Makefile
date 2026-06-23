CC = mpicc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDLIBS = -lm
BUILD_DIR = build
SRC_DIR = src
SUMMA_OBJS = \
	$(BUILD_DIR)/summa.o \
	$(BUILD_DIR)/args.o \
	$(BUILD_DIR)/matrix.o \
	$(BUILD_DIR)/timing.o \
	$(BUILD_DIR)/output.o \
	$(BUILD_DIR)/summa_kernel.o

.PHONY: all clean

all: hello summa mpi_comm_test

hello: $(SRC_DIR)/hello.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

mpi_comm_test: $(SRC_DIR)/mpi_comm_test.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

summa: $(SUMMA_OBJS)
	$(CC) $(CFLAGS) $(SUMMA_OBJS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c include/*.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f hello summa mpi_comm_test $(SUMMA_OBJS)
