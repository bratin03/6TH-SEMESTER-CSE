# Directory Structure

- `DATASET`: Contains the dataset files for the assignment.
- `SOURCE_CODE`: Contains the source code files for the assignment.

## DATASET

- `song.csv`: The dataset for the assignment.
- `train.csv`: The training dataset.
- `test.csv`: The test dataset.

## SOURCE_CODE

- `Preprocess.py`: The script is used to preprocess the dataset and split it into training and test datasets and save them in the DATASET directory.
- `Model.py`: The script contains the model implementation for the Neural Network along with the different modules as mentioned in the assignment.
- `main.py`: The main script for the assignment.
- `My_Model.py`: The script contains the model implementation for the Neural Network along with the different modules with some improvements over the original model.
- `scikit_learn.py`: The script contains the implementation of the model using scikit-learn.
- `makefile`: The makefile for running the project.
- `requirements.txt`: The file contains the required packages for the project.
# Usage

To run the project, execute the following command from terminal from SOURCE_CODE directory:

```bash
make model
```
To run the project using scikit-learn, execute the following command from terminal from SOURCE_CODE directory:

```bash
make test_scikit_learn
```

To run the project using the improved model, execute the following command from terminal from SOURCE_CODE directory:

```bash
make mymodel
```

To clean the project, execute the following command from terminal from SOURCE_CODE directory:

```bash
make clean
```

# Setup Instructions

1. First Download the dataset and place it in the DATASET directory. Name the dataset as 'song.csv'.

2. Install the required packages using the following command:

```bash
pip install -r requirements.txt
``` 
3. Process the dataset using `Preprocess.py` script using the following command:
```bash
python3 Preprocess.py
```

4. Run the project using the following command:

```bash
make model
```
# Outputs
A new directory named `OUTPUT` will be created in the main directory. The plot for training accuracy and test accuracy vs epoch will be saved in the `OUTPUT` directory.

